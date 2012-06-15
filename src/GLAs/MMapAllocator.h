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
#ifndef _MMAP_STL_ALLOCATOR_H_
#define  _MMAP_STL_ALLOCATOR_H_

#include <boost/pool/pool.hpp>

#include "MmapAllocator.h"
#include "DistributedCounter.h"
#include <iostream>

/* This file defines an allocator compatible with std::allocator that
	 can be used in conjuntion to stl containers. The main feature of
	 the allocator is the fact that it allocates memory using
	 mmap_free/delete thus can make use of the huge pages.

	 We use pool to manage the pool of objects that leave in the memory
	 allocated with mmap_alloc.

*/

struct mmap_allocator
{
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  static char * malloc(const size_type bytes) {
	  void* ret = mmap_alloc(bytes,1);
	//cout << "ALLOCATED: " << ret << " size=" << bytes << endl;
	return reinterpret_cast<char *>(ret);
  }
  static void free(char * const block) {
	mmap_free(block);
  }
};



template<typename T>
class MMapStlAllocator {
protected:
	boost::pool<mmap_allocator>* myPool; // does the memory management
	DistributedCounter* refCnt;

	inline void Kill(){
		if (refCnt != NULL && refCnt->Decrement(1) == 0) {
			/* wipe everything if valid and last copy*/
			delete refCnt;
			myPool->purge_memory(); 
			delete myPool;  
		}
	}

	inline void CopyFrom(MMapStlAllocator const& aux) {
		Kill();
		refCnt = aux.refCnt;
		refCnt->Increment(1);
		myPool = aux.myPool;

		//		cout << "COPY constr with " << sizeof(T) << endl;
	}

	/** This is definitely a hack but things are getting too complicated to avoid it.
	    It should be fine, though **/

	template<typename U>
	friend class MMapStlAllocator;

	template<typename U>
	inline void CopyFrom2(MMapStlAllocator<U> const& aux) {
		// the other allocator is of a different type,  ignore it
		Kill();
		refCnt = aux.refCnt;
		refCnt->Increment(1);
		myPool = aux.myPool;

		// refCnt = new DistributedCounter(1);
		// // for some reason, on rebind we do not get the larger size here but the original size
		// myPool = new boost::pool<mmap_allocator>(sizeof(T)+8, MMAP_PAGE_SIZE/(sizeof(T))-5);
		
		// cout << "STARTED WITH: " << sizeof(T) << " vs " << sizeof(U) << endl;
	}

public : 
	// typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	// convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef MMapStlAllocator<U> other;
	};

	inline explicit MMapStlAllocator() {
		refCnt = new DistributedCounter(1);
		// for some reason, on rebind we do not get the larger size here but the original size
		myPool = new boost::pool<mmap_allocator>(sizeof(T)+8, MMAP_PAGE_SIZE/(sizeof(T))-5);

		//		cout << "CREATED ALLOCATOR FOR SIZE:" << sizeof(T) << endl;
	}

	inline ~MMapStlAllocator() {
		Kill();
	}

	// dangerous function. make sure the map is not used anymore
	inline void purge(void){
		myPool->purge_memory(); 
	}

	inline  MMapStlAllocator(MMapStlAllocator const& aux):myPool(NULL), refCnt(NULL) {
		CopyFrom(aux);
	}

	inline  MMapStlAllocator(MMapStlAllocator& aux) { CopyFrom(aux); }

	template<typename U>
	inline explicit MMapStlAllocator(MMapStlAllocator<U> const& aux) :
		myPool(NULL), refCnt(NULL) {
		CopyFrom2(aux);
	}

	// give outside world access to pool
	boost::pool<mmap_allocator>* GetPool(void) const { return myPool; }
	
	// address
	inline pointer address(reference r) { return &r; }
	inline const_pointer address(const_reference r) { return &r; }

	// memory allocation
	inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) { 
		void* ret = myPool->ordered_malloc(cnt);
		// cout << "MyMmap: allocated " << cnt << " objects at: " << ret << " of size " 
		// 		 << myPool->get_requested_size() << endl;
		return reinterpret_cast<pointer>(ret); 
	}

	inline void deallocate(pointer p, size_type cnt) { 
		myPool->ordered_free(p, cnt);
	}

	// size
	inline size_type max_size() const { 
		return MMAP_PAGE_SIZE * 8192 / sizeof(T);
	}

	// construction/destruction

	inline void construct(pointer p, const T& t) { new(p) T(t); }
	inline void destroy(pointer p) { p->~T(); }

	inline bool operator==(MMapStlAllocator const& other) { return (this == &other); }
	inline bool operator!=(MMapStlAllocator const& a) { return !operator==(a); }
}; // end of class MMapStlAllocator 


#endif //  _MMAP_STL_ALLOCATOR_H_

