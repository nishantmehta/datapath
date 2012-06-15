//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#include "SqliteSelector.h"

#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

SqliteSelector::SqliteSelector(){

}

void SqliteSelector::DataSelector(string sender, string receiver, string msgtype, double tsf, double tst, double dless, double dmore, int maxduration, int minduration, bool screen, bool all){

	int ifsender = 1, ifreceiver = 1, ifmsgtype = 1, iftst = 1, iftsf = 1, ifdless=1, ifdmore=1;

	if(sender != "") ifsender = 0;
	if(receiver != "") ifreceiver = 0;
	if(msgtype != "") ifmsgtype = 0;
	if(tst != -1.0) iftst = 0;
	if(tsf != -1.0) iftsf = 0;
	if(dless != -1.0) ifdless = 0;
	if(dmore != -1.0) ifdmore = 0;

	int rc = sqlite3_open("log.sqlite", &logDB);
	char *zErrMsg = 0; //error message is passed via this


	if(rc){ //in case of an error
		cerr<< "Can't open database: " <<sqlite3_errmsg(logDB) << endl;
		sqlite3_close(logDB);
		exit(1);
	}

	rc = sqlite3_exec(logDB, "PRAGMA journal_mode=WAL", NULL, NULL, &zErrMsg);
	if(rc){ //in case of an error
		cerr<< "PRAGMA Error: " <<sqlite3_errmsg(logDB) << endl;
		sqlite3_close(logDB);
		exit(1);
	}

	if(all){

		zSQL_SEL =  "SELECT SENDER, RECEIVER, MSG_TYPE FROM LOG_RECORDS ORDER BY TIMESTAMP;";

		rc = sqlite3_prepare_v2(logDB, zSQL_SEL, strlen(zSQL_SEL), &preStmtSEL, NULL);

		if( rc ){
			cerr << "SQL PreparedStatement Error: " << sqlite3_errmsg(logDB) <<endl;
		}else{

			dfGen.CreateDotFile("test");	//Creates a new dotfile with given name (without extension)

			// execute the SELECT statement
			int count=1;
	          do{
	        	  rc = sqlite3_step(preStmtSEL);
	        	  switch( rc ){
	        	  case SQLITE_DONE:
	        		  break;
	              case SQLITE_ROW:
	            	  //dfGen.AddNode(sender, receiver, msgtype, edgeCount)
	              	  { const char* s = (const char*)sqlite3_column_text(preStmtSEL, 0);
	              	  const char* r = (const char*)sqlite3_column_text(preStmtSEL, 1);
	              	  const char* mt = (const char*)sqlite3_column_text(preStmtSEL, 2);
	              	  int edgeC = 1;//sqlite3_column_int(preStmtSEL, 3);
	              	  dfGen.AddNode(s, r, mt, edgeC, count);
	              	  if(screen){
	              		  cout<<"Sender: "<<s<<" | Receiver: "<<r<<" | MsgType: "<<mt<<" | Count: "<<edgeC<<endl;
	              	  }
	              	  count++;

	              }break;
	              default:
	            	  cerr << "SQL SELECT ERROR: " << sqlite3_errmsg(logDB) <<endl;
	                break;
	            }
	          }while( rc==SQLITE_ROW );

	          // finalize the statement to release resources
	          sqlite3_finalize(preStmtSEL);
		}
	    dfGen.CloseDotFile();


	}else if(minduration != 0 || maxduration !=0){

		int limit = minduration;
		zSQL_SEL =  "SELECT * FROM LOG_RECORDS WHERE DURATION <> -1.0 ORDER BY DURATION ASC LIMIT @LIM;";

		if(maxduration !=0){
			limit = maxduration;
			zSQL_SEL =  "SELECT * FROM LOG_RECORDS ORDER BY DURATION DESC LIMIT @LIM;";
		}

		rc = sqlite3_prepare_v2(logDB, zSQL_SEL, strlen(zSQL_SEL), &preStmtSEL, NULL);

		if( rc ){
			cerr << "SQL PreparedStatement Error: " << sqlite3_errmsg(logDB) <<endl;
		}else{
			//bind values to the preparedStatement

			sqlite3_bind_int (preStmtSEL, 1, limit);

			// execute the SELECT statement
	          do{
	        	  rc = sqlite3_step(preStmtSEL);
	        	  switch( rc ){
	        	  case SQLITE_DONE:
	        		  break;
	              case SQLITE_ROW:
	            	  cout<<"LOG_ID: "<<sqlite3_column_int(preStmtSEL, 0)<<" | Sender: "<<(const char*)sqlite3_column_text(preStmtSEL, 1)<<" | Receiver: "<<(const char*)sqlite3_column_text(preStmtSEL, 2)<<" | MsgType: "<<(const char*)sqlite3_column_text(preStmtSEL, 3)<<" | Timestamp: "<<sqlite3_column_text(preStmtSEL, 4)<<" | Execution Time: "<<sqlite3_column_text(preStmtSEL, 5)<<endl;
	              break;
	              default:
	            	  cerr << "SQL SELECT ERROR: " << sqlite3_errmsg(logDB) <<endl;
	                break;
	            }
	          }while( rc==SQLITE_ROW );

	          // finalize the statement to release resources
	          sqlite3_finalize(preStmtSEL);
		}


	}
	else{
	zSQL_SEL =  "SELECT SENDER, RECEIVER, MSG_TYPE, COUNT(*) FROM LOG_RECORDS WHERE (@IFSENDER OR SENDER = @SENDER) AND (@IFRECEIVER OR RECEIVER = @RECEIVER) AND (@IFMSGTYPE OR MSG_TYPE = @MSGTYPE) AND (@IFTSF OR TIMESTAMP >= @TS1) AND (@IFTST OR TIMESTAMP <= @TS2) AND (@IFDLESS OR (DURATION <= @D1 AND DURATION <> -1.0)) AND (@IFDMORE OR DURATION >= @D2) GROUP BY SENDER, RECEIVER, MSG_TYPE;";


	rc = sqlite3_prepare_v2(logDB, zSQL_SEL, strlen(zSQL_SEL), &preStmtSEL, NULL);

	if( rc ){
		cerr << "SQL PreparedStatement Error: " << sqlite3_errmsg(logDB) <<endl;
	}else{

		dfGen.CreateDotFile("test");	//Creates a new dotfile with given name (without extension)

		//bind values to the preparedStatement
		sqlite3_bind_int (preStmtSEL, 1, ifsender);
		sqlite3_bind_text(preStmtSEL, 2, sender.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int (preStmtSEL, 3, ifreceiver);
		sqlite3_bind_text(preStmtSEL, 4, receiver.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int (preStmtSEL, 5, ifmsgtype);
		sqlite3_bind_text(preStmtSEL, 6, msgtype.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int (preStmtSEL, 7, iftsf);
		sqlite3_bind_double(preStmtSEL, 8, tsf);
		sqlite3_bind_int (preStmtSEL, 9, iftst);
		sqlite3_bind_double(preStmtSEL, 10, tst);
		sqlite3_bind_int (preStmtSEL, 11, ifdless);
		sqlite3_bind_double(preStmtSEL, 12, dless);
		sqlite3_bind_int (preStmtSEL, 13, ifdmore);
		sqlite3_bind_double(preStmtSEL, 14, dmore);

		int count=1;
		// execute the SELECT statement
          do{
        	  rc = sqlite3_step(preStmtSEL);
        	  switch( rc ){
        	  case SQLITE_DONE:
        		  break;
              case SQLITE_ROW:
            	  //dfGen.AddNode(sender, receiver, msgtype, edgeCount)
              { const char* s = (const char*)sqlite3_column_text(preStmtSEL, 0);
              	  const char* r = (const char*)sqlite3_column_text(preStmtSEL, 1);
              	  const char* mt = (const char*)sqlite3_column_text(preStmtSEL, 2);
              	  int edgeC = sqlite3_column_int(preStmtSEL, 3);
              	  dfGen.AddNode(s, r, mt, edgeC, count);
              	  if(screen){
              		  cout<<"Sender: "<<s<<" | Receiver: "<<r<<" | MsgType: "<<mt<<" | Count: "<<edgeC<<endl;
              	  }
              	  count++;

              }break;
              default:
            	  cerr << "SQL SELECT ERROR: " << sqlite3_errmsg(logDB) <<endl;
                break;
            }
          }while( rc==SQLITE_ROW );

          // finalize the statement to release resources
          sqlite3_finalize(preStmtSEL);
	}
    dfGen.CloseDotFile();
	}

	sqlite3_close(logDB);
}
