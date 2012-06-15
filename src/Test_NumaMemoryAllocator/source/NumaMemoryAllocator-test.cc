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
#define MAX_PAGES 10
#define NUM_THREADS 2

static list<void*> listPtr;
static list<int> listSize;
static list<int> listValue;
static  pthread_mutex_t mutex;
static int count = 0; // ensure allocation is not too fast than free
static bool exitMain = false;

// This allocator does random chunk size allocation in random NUMA number in separate thread
// Chunks are freed in separate thread.
// There is no particular order of allocation or freeing, we keep doing it for 1 minute and than exit


void* alloc_m (void* arg)
{
    while(!exitMain)
    {
      if (count < 100 ) {
        pthread_mutex_lock(&mutex);
        int size = ((size_t)(rand()%MAX_PAGES)) << ALLOC_PAGE_SIZE_EXPONENT;
        size = size + size / (rand()%MAX_PAGES + 1) + 1;
        int numa = rand()%NUM_NUMA;
        void *p = mmap_alloc(size, numa);
        int* pInt = (int*)p;
        int val = rand();
        for (int i = 0; i < size/(sizeof(int)); i++)
          pInt[i] = val;
        cout << "Allocated ptr " << p << " with size " << size << " and numa = " << numa << " from thread " << (long long)arg << " value " << val << endl;
        listPtr.push_back(p);
        listSize.push_back(size);
        listValue.push_back(val);
        count++;
        pthread_mutex_unlock(&mutex);
      }
    }
}

void* free_m (void* arg)
{
    while(!exitMain)
    {
        pthread_mutex_lock(&mutex);
        assert(listPtr.size() < 500);
        if (!listPtr.empty())
        {
            void *p = listPtr.front();
            int size = listSize.front();
            int val = listValue.front();
            int* pInt = (int*)p;
            cout << "Freed ptr " << p << " from thread " << (long long)arg << " value " << val << endl;
            for (int i = 0; i < size/(sizeof(int)); i++)
              assert(pInt[i] == val);
            mmap_free(p);
            listPtr.pop_front();
            listSize.pop_front();
            listValue.pop_front();
            count--;
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

    sleep(60);
    for (int j = 0; j < vec.size(); j++)
    {
        delete vec[j];
    }
    exitMain = true;
    cout << "Exiting main()\n";
    sleep(1);
    return 0;

}
