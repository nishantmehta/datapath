//
//  Copyright 2012 Alin Dobra and Christopher Jermain
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
#ifndef _GLOBAL_GLA_STATE_H_
#define _GLOBAL_GLA_STATE_H_


#include <pthread.h>
#include "EfficientMap.h"
#include "GLAData.h"
#include <vector>

// Prototype

class GlobalGLAState;

class GlobalGLAPtr;

/** Useful datatypes for large GLAs */

typedef EfficientMap<QueryID, GlobalGLAState> QueryToGlobalGLAStateMap;

// Used in work descriptions to reference the global states.
typedef EfficientMap<QueryID, GlobalGLAPtr> QueryToGlobalGLAPtrMap;

/** This class allows for the manipulation of partitioned GLA States

 */

class GlobalGLAState {
private:
  // set to true if the state is fully built
  // when true, no update is accepted
  bool isFinished;

  // mutex that protects the global state
  pthread_mutex_t *myMutex;

  // condition variable used to signal those who are now blocked waiting for a
  // segment
  pthread_cond_t *signalWriters;

  // Keeps track of which segments are currently write locked
  std::vector<bool> * writeLocked;

  // tells how many copies of this object are out there
  int *numCopies;

  // the substates
  int K; // number of substates

  // vector of GLA states
  std::vector<GLAState> * mapSt; // vector of states

  // keeps track of the number of fragments per segment
  std::vector<int> * fragCount;

public:

  // this function is called when a part of the state needs updating
  // this part will be unavailable to other threads
  // Only available if the state has not yet finished building
  int CheckOutOne (int *theseAreOK, GLAState& checkMeOut);

  // simply releases a write lock on the segment
  // Only available if the state has not yet finished building
  void CheckIn (int whichEntry, GLAState& checkIn);

  // makes a shallow copy of the state
  void Clone (GlobalGLAState &fromMe);

  // synonym for clone
  void copy (GlobalGLAState &other) { Clone(other); }

  // Sets the global state as finished building, disallowing checking in and out
  // of segments.
  void Finalize() { isFinished = true; }

  // Peeks at a particular state segment.
  // Only available when the state is marked as finished.
  GLAState & Peek( int whichEntry );

  // Populates the vector fCount with the number of fragments that each
  // segment will produce. Only available when the state is marked as finished
  void GetFragmentCount( std::vector<int> & fCount );

  // Sets the number of fragments that a given segment has
  // Only availble when the state is marked as finished
  void SetFragmentCount( int segment, int fCount );

  // Checks to see if all of the fragment counts are valid, and thus
  // GetFragmentCount can be called
  bool FragmentCountsValid();

  // creates an empty hash table
  GlobalGLAState ():isFinished(false), K(0), myMutex(NULL){}

  // creates a hash table with k fragments
  GlobalGLAState (int k);

  // Decrements numCopies and deallocates resources if it's the last one.
  virtual ~GlobalGLAState();

  int GetNumSegments(void){ return K; }

  void swap(GlobalGLAState &fromMe);

private:

  // Deallocates all of the internal data structures, used when the
  // last copy of the state is destroyed.
  void deallocateInternals();

};

/**
 * An in-memory pointer to a global GLA state.
 *
 * Used to allow multiple threads to access the global state at once without
 * making a ton of copies of it.
 */
class GlobalGLAPtr {

  GlobalGLAState * glaPtr;

public:

  /**
   * Default constructor, null pointer.
   */
  GlobalGLAPtr();

  /**
   * Creates a pointer referencing the specified GlobalGLAState
   */
  GlobalGLAPtr( GlobalGLAState & state );

  virtual ~GlobalGLAPtr();

  /**
   * Swaps the values of this pointer and fromMe.
   */
  void swap( GlobalGLAPtr &fromMe );

  /**
   * Copies the data from the given pointer without destroying it
   */
  void copy( GlobalGLAPtr &fromMe );

  /**
   * Returns the pointer to the GlobalGLAState that this pointer references.
   */
  GlobalGLAState * get_glaPtr();
};

#endif //  _GLOBAL_GLA_STATE_H_
