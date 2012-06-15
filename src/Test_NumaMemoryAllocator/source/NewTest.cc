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
#include <iostream>
#include "MmapAllocator.h"
#include "Numa.h"
#include<list>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <cassert>

using namespace std;
//#define NUM_NUMA 10
static int NUM_NUMA = 0;
#define NUM_THREADS 1
//#define SANITY_CHECK_OF_DATA 1

// After these many pages, start the thread to free memory in parallel
#define START_FREEING_AFTER_PAGES 5000

static list<void*> listPtr;
static list<int> listSize;
static list<int> listValue;
static pthread_mutex_t mutex;
static bool exitMain = false;

static int page_count1 = 0;;
static int page_count2 = 0;;

static int total_page_count = 0;
// This allocator does random chunk size allocation in random NUMA number in separate thread
// Chunks are freed in separate thread.
// There is no particular order of allocation or freeing, we keep doing it for 1 minute and than exit


void* alloc_m (void* arg)
{
    while(!exitMain)
    {
        pthread_mutex_lock(&mutex);
				int max_pages = 10;
				int least_page = 1;
				if ((long long)arg == 0) {
					max_pages = 10;
					least_page = 1;
					int incrementBy = rand() % 5 + 1;
					page_count1 += incrementBy;
					page_count2 += incrementBy;
				}
				else if ((long long)arg == 1 && page_count1 > 50) {
					max_pages = 64;
					least_page = 17;
					page_count1 = 0;
				}
				else if ((long long)arg == 2 && page_count2 > 100) {
					max_pages = 250;
					least_page = 150;
					page_count2 = 0;
				}

				// calculate number of pages randomnly, but not so random
				int pages = ((size_t)((rand()%(max_pages)) + least_page));
				total_page_count += pages;
        int size = PAGES_TO_BYTES(pages);

				// find numa node randomly
        int numa = rand()%NUM_NUMA;

				// allocate memory
        void *p = mmap_alloc(size, numa);

				// Do some sanity check of data
        int* pInt = (int*)p;
        int val = rand();
//#ifdef SANITY_CHECK_OF_DATA
        //for (int i = 0; i < size/(sizeof(int)); i++)
				if (BYTES_TO_PAGES(size) < 17) {
        	for (int i = 0; i < size/((sizeof(int))); i++)
          	pInt[i] = val;
				}
				else {
        	for (int i = 0; i < 512; i++)
          	pInt[i] = val;
				}
//#endif
        cout << "Allocated ptr " << p << " with size " << BYTES_TO_PAGES(size) << " and numa = " << numa << " from thread " << (long long)arg << " value " << val << endl;
        listPtr.push_back(p);
        assert(listPtr.size() < 20000);
        listSize.push_back(size);
        listValue.push_back(val);
        pthread_mutex_unlock(&mutex);
    }
}

void* free_m (void* arg)
{
    while(!exitMain)
    {
        pthread_mutex_lock(&mutex);
        assert(listPtr.size() < 2000);
        if (!listPtr.empty())
        {
            void *p = listPtr.front();
            int size = listSize.front();
            int val = listValue.front();
            int* pInt = (int*)p;
            cout << "Freed ptr " << p << " from thread " << (long long)arg << " value " << val << endl;
#ifdef SANITY_CHECK_OF_DATA
            for (int i = 0; i < size/(sizeof(int)); i++)
              assert(pInt[i] == val);
#endif
            mmap_free(p);
            listPtr.pop_front();
            listSize.pop_front();
            listValue.pop_front();
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    NUM_NUMA = numaNodeCount();
    cout << "Max NUMA nodes = " << NUM_NUMA;

    vector<pthread_t*> vec;
    for (int i = 0; i < NUM_THREADS; i++)
    {
				for (int j = 0 ; j < 3 ; j++) {
        	pthread_t* threadPtr = new pthread_t;
        	pthread_attr_t t_attr;
        	pthread_attr_init(&t_attr);
        	pthread_create(threadPtr, &t_attr, alloc_m, (void*)j);
        	vec.push_back(threadPtr);
					cout << endl << "Allocation thread " << j << " active";
				}

				while (total_page_count < START_FREEING_AFTER_PAGES);

				for (int j = 0 ; j < 1 ; j++) {
       		pthread_t* threadPtr1 =  new pthread_t;
       		pthread_attr_t t_attr1;
       		pthread_attr_init(&t_attr1);
       		pthread_create(threadPtr1, &t_attr1, free_m, (void*)j);
       		vec.push_back(threadPtr1);
					cout << endl << "Deallocation thread " << j << " active";
				}

    }

    sleep(600);
    for (int j = 0; j < vec.size(); j++)
    {
        delete vec[j];
    }
    exitMain = true;
    cout << "Exiting main()\n";
    sleep(1);
    return 0;

}
