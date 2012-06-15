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
dnl # // M4 PREAMBLE CODE
include(SQLite.m4)
dnl # END OF M4 CODE


#include <iostream>
#include "DiskMemoryAllocator.h"
#include "MetadataDB.h"
#include <list>
#include <vector>
#include <set>
#include <pthread.h>
#include <cstdlib>
#include <cassert>
#include "MmapAllocator.h"

using namespace std;
#define MAX_PAGES 100
#define NUM_THREADS 5
#define MAX_RELATION 10

static  pthread_mutex_t mutex;
static int count = 0; // ensure allocation is not too fast than free
static bool exitMain = false;
static map<int, off_t> relToSize;
static DiskMemoryAllocator* dm = NULL;

// This allocator does random chunk size allocation in separate thread
// Chunks are freed in separate thread.
// There is no particular order of allocation or freeing, we keep doing it for 1 minute and than exit
// This is testing disk metadata testing, not actual memory pointer allocation


void* alloc_m (void* arg)
{
    while(!exitMain)
    {
        pthread_mutex_lock(&mutex);
				// Get the page size to be allocated, make sure it is not zero
        off_t size = ((off_t)(rand()%MAX_PAGES)) + 1;
				// Get the relation ID
				int rel = rand() % MAX_RELATION;
				// Allocate
        off_t pageNo = dm->DiskAlloc(size, rel);
				// Update total size for given relation, to be used in DiskFree
				off_t totalSize = relToSize[rel] + size;
				relToSize[rel] = totalSize;
				// print the details
        cout << "Allocated size " << size << " relation no = " << rel << " received page no = " << pageNo << " from thread " << (long long)arg  << endl;
        count++;
        pthread_mutex_unlock(&mutex);
    }
}


void* free_m (void* arg)
{
    while(!exitMain)
    {
        pthread_mutex_lock(&mutex);
        if (count > 10000)
        {
						int rel = 1 + rand() % MAX_RELATION;
						map<int, off_t>::iterator it = relToSize.find(rel);
						if (it != relToSize.end() && it->second > 100000) { // delete only if relation exists and it contains at least 100000 page size
            	cout << "Freed rel ID " << rel << " from thread " << (long long)arg << endl;
							// erase the entire relation
            	dm->DiskFree(rel);
							// erase from our map
							relToSize.erase(rel);
            	count--;
						}
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
		dm = new DiskMemoryAllocator(1);

    vector<pthread_t*> vec;
    for (int i = 0; i < NUM_THREADS; i++)
    {

        pthread_t* threadPtr1 =  new pthread_t;
        pthread_attr_t t_attr1;
        pthread_attr_init(&t_attr1);
        pthread_create(threadPtr1, &t_attr1, free_m, (void*)i);
        vec.push_back(threadPtr1);

        pthread_t* threadPtr = new pthread_t;
        pthread_attr_t t_attr;
        pthread_attr_init(&t_attr);
        pthread_create(threadPtr, &t_attr, alloc_m, (void*)i);
        vec.push_back(threadPtr);
    }

    sleep(3);
    for (int j = 0; j < vec.size(); j++)
    {
        delete vec[j];
    }
    exitMain = true;
		SQL_OPEN_DATABASE( GetMetadataDB() );

		dm->Flush(SQL_DATABASE_OBJECT);

		SQL_CLOSE_DATABASE;

    cout << "Exiting main()\n";
    sleep(1);
    return 0;

}
