dnl #
dnl #  Copyright 2012 Alin Dobra and Christopher Jermaine
dnl #
dnl #  Licensed under the Apache License, Version 2.0 (the "License");
dnl #  you may not use this file except in compliance with the License.
dnl #  You may obtain a copy of the License at
dnl #
dnl #      http://www.apache.org/licenses/LICENSE-2.0
dnl #
dnl #  Unless required by applicable law or agreed to in writing, software
dnl #  distributed under the License is distributed on an "AS IS" BASIS,
dnl #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
dnl #  See the License for the specific language governing permissions and
dnl #  limitations under the License.
dnl #
/** The methods in DiskArrayImp that are partially implemented in SQL

        The DiskArray metadata is assumed already instantiated (no code to
        create it here). A tool, called diskArrayInit can create an
        initial state of the data.
 */

dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE

#include "DiskArrayImp.h"
#include "Errors.h"
#include "MetadataDB.h"
#include "Constants.h"
#include "MmapAllocator.h"

#include <iostream>
#include <pthread.h>

DiskArrayImp::DiskArrayImp(bool isReadOnly)
#ifdef  DEBUG_EVPROC
    :EventProcessorImp(true, "DiskArrayImp")
#endif
{
    //initialize the mutex
    pthread_mutex_init(&lock, NULL);

    SQL_OPEN_DATABASE( GetMetadataDB() );

    // Just in case the disk arrays do not exist
    SQL_STATEMENTS_NOREZ(</"
      CREATE TABLE  IF NOT EXISTS DiskArrays (
          /* name must be null for the default array */
           name                                 TEXT,
           arrayID                              INTEGER                   PRIMARY KEY,
           pageMultExp                          INTEGER,
           stripeParam1                         INTEGER                   NOT NULL,
           stripeParam2                         INTEGER                   NOT NULL,
           numberOfPages                        INTEGER                   NOT NULL
      );

      CREATE TABLE IF NOT EXISTS Stripes (
          /* diskID is INT not INTEGER since it cannot be 0 */
          /* arrayID is a foreign key to DiskArrays table */
           diskID                        INT,
           arrayID                       INTEGER,
           fileName                      TEXT
      );
      "/>);

    meta.arrayID = -1; // invalid. If the next piece code does not set it to valid
    // latter code will fail

    // read the info from DiskArray
    SQL_STATEMENT_TABLE(</"
        SELECT arrayID, pageMultExp, stripeParam1, stripeParam2, numberOfPages
        FROM DiskArrays
        WHERE name='Default';
    "/>,</(arrayID, int), (pageMultExp,int), (stripeParam1,int), (stripeParam2,int), (numberOfPages,int)/>){
        meta.arrayID = arrayID;
        meta.pageMultExp = pageMultExp;
        meta.stripeParam1 = stripeParam1;
        meta.stripeParam2 = stripeParam2;
        meta.numberOfPages = numberOfPages;
    }SQL_END_STATEMENT_TABLE;

    if (meta.arrayID == -1){
        // no disk array defined yet. Must interact with the user
        printf("No Disk Array defined yet. Please answer the following quesions so we can set one up\n");
        meta.arrayID = 1; // default
        printf("What is the page muliplier exponent of the array? The disk requests will be pmultExp^2*%d\n", MMAP_PAGE_SIZE);
    cin >> meta.pageMultExp;
        FATALIF(meta.pageMultExp > 8, "Now, now, is that not excessive?");
        meta.stripeParam1 = lrand48();
        meta.stripeParam2 = lrand48();
        meta.numberOfPages = 0;

        SQL_STATEMENTS_NOREZ(</"
          INSERT INTO DiskArrays(name, arrayID, pageMultExp, stripeParam1, stripeParam2, numberOfPages)
          VALUES ('Default', %d, %d, %ld, %ld, %d);
      "/>,</meta.arrayID, meta.pageMultExp, meta.stripeParam1, meta.stripeParam2, meta.numberOfPages/>);

        printf("The disk array can use multiple stripes. How many stripes should we use?\n");
        cin >> meta.HDNo;
        FATALIF( meta.HDNo<1 || meta.HDNo > 1000, "The number of disks is unacceptable");

        printf("Pattern for the stripes fies. Use %%d for the position of the numbers. The numbering starts at 1\n");
        string pattern;
        cin >> pattern;

        SQL_STATEMENT_PARAMETRIC_NOREZ(</"
          INSERT INTO Stripes(diskID, arrayID, fileName) VALUES (?1, ?2, ?3);
          "/>, </int, int, text/>);
        for (int i=0; i<meta.HDNo; i++){
            char fileName[1000];
            sprintf(fileName, pattern.c_str(), i+1);
            SQL_INSTANTIATE_PARAMETERS(i, meta.arrayID, fileName);
        }
        SQL_PARAMETRIC_END;

    } else { // disk array is valid
#if 0
        printf("Opening Disk Array %ld: pageMultExp:%ld,\tnumberOfPages=%ld\n", meta.arrayID, meta.pageMultExp, meta.numberOfPages);
#endif
        // get the number of drives
        SQL_STATEMENT_SCALAR(</"
          SELECT COUNT(diskID)
          FROM Stripes
          WHERE arrayID=%d;
      "/>, _cnt, int, </meta.arrayID/>);
        meta.HDNo = _cnt;
    }

    totalPages = 0;
    hds = new EventProcessor[meta.HDNo];

    // read the stripes from Stripes and start the HD threads
    SQL_STATEMENT_TABLE(</"
        SELECT fileName, diskID
        FROM Stripes
        WHERE arrayID=%d;
    "/>,</(fileName, text), (diskID, int)/>,
        </meta.arrayID/>){
        HDThread hd(fileName, diskID, myInterface, DISK_OPERATION_STATISTICS_INTERVAL, isReadOnly);
        hd.ForkAndSpin();
        hds[diskID].swap(hd); // put the hd in the vector

        //free the memory
        free(fileName);
    }SQL_END_STATEMENT_TABLE;

    // now we let the space manager initialize itself
    diskSpaceMng.SetArrayID(meta.arrayID);
    diskSpaceMng.Load(SQL_DATABASE_OBJECT);
    SQL_CLOSE_DATABASE;

    //priority for processing read chunks is higher than accepting new chunks
    RegisterMessageProcessor(DiskStatistics::type, &ProcessDiskStatistics, 2);
    RegisterMessageProcessor(DiskOperation::type, &DoDiskOperation, 1);
}

void DiskArrayImp::Flush(void) {
    // the Array did not change, just the space manager
    SQL_OPEN_DATABASE( GetMetadataDB() );
    printf("\nDiskArrayImp::Flush We open the DB");
    diskSpaceMng.Flush(SQL_DATABASE_OBJECT);
    printf("\nDiskArrayImp::Flush We close the DB");
    SQL_CLOSE_DATABASE;
}
