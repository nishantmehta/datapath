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
#include "GlobalGLAState.h"

int GlobalGLAState :: CheckOutOne (int *theseAreOK, GLAState& checkMeOut) {
  // Can't check out a state if we're finished
  FATALIF( isFinished, "Tried to check out a segment in a global state marked as finished!" );

  // first, figure out all of the OK segments
  int numWanted = 0;
  int goodOnes[NUM_SEGS];
  for (int i = 0; i < NUM_SEGS; i++) {
    if (theseAreOK[i] == 1) {
      goodOnes[numWanted] = i;
      numWanted++;
    }
  }

  // now, try them one-at-a-time, in random order
  pthread_mutex_lock (myMutex);
  while (1) {

    // try each of the desired hash table segments, in random order
    for (int i = 0; i < numWanted; i++) {

      // randomly pick one of the guys in the list
      int rangeSize = numWanted - i;
      int whichIndex = i + (lrand48() % rangeSize);

      // move him into the current slot
      int whichToChoose = goodOnes[whichIndex];
      goodOnes[whichIndex] = goodOnes[i];
      goodOnes[i] = whichToChoose;

      // try him
      if (!(*writeLocked)[whichToChoose]) {

    // he is open, so write lock him
    (*writeLocked)[whichToChoose] = true;

    // and return him
    GLAState& item = (*mapSt)[whichToChoose];
    checkMeOut.swap(item);
    pthread_mutex_unlock (myMutex);
    return whichToChoose;
      }
    }

    // if we got here, then every one that we want is write locked.  So
    // we will go to sleep until one of them is unlocked, at which point
    // we will wake up and try again...
    pthread_cond_wait (signalWriters, myMutex);
  }
}

void GlobalGLAState :: CheckIn (int whichEntry,  GLAState& checkIn) {
  // Can't check out a state if we're finished
  FATALIF( isFinished, "Tried to check in a segment in a global state marked as finished!" );

  // just note that no one is writing this one, then signal all potential writers
  pthread_mutex_lock (myMutex);
  (*writeLocked)[whichEntry] = false;

  GLAState& item = (*mapSt)[whichEntry];
  checkIn.swap(item);

  pthread_cond_broadcast (signalWriters);
  pthread_mutex_unlock (myMutex);

}

GLAState & GlobalGLAState :: Peek( int whichEntry ) {
  // Can only peek directly at elements if the state has finished building
  FATALIF( !isFinished, "Tried to peek at a segment in a global state not marked as finished!" );
  FATALIF( whichEntry < 0 || whichEntry >= K, "No segment in global state pertaining to that index.");

  return (*mapSt)[whichEntry];
}

void GlobalGLAState :: GetFragmentCount( vector<int> & fCount ) {
  FATALIF( !isFinished, "Tried to get the fragment counts for a global state that is not marked as finished.");
  for( vector<int>::iterator it = fragCount->begin(); it != fragCount->end(); ++it ) {
    fCount.push_back( *it );
  }
}

void GlobalGLAState :: SetFragmentCount( int segment, int fCount ) {
  FATALIF( !isFinished, "Tried to set the fragment count for a global state that is not marked as finished.");
  fragCount->at(segment) = fCount;
}

bool GlobalGLAState :: FragmentCountsValid() {
  for( vector<int>::iterator it = fragCount->begin(); it != fragCount->end(); ++it ) {
    if( *it == 0 )
      return false;
  }

  return true;
}

void GlobalGLAState :: Clone( GlobalGLAState &fromMe ) {
  // Deallocate myself if needed. Note: the mutex can be null if the object
  // was constructed using the default constructor, and thuse does not need
  // to be destructed itself as none of the internal data structures will
  // have been allocated.
  if( myMutex != NULL ) {
    pthread_mutex_lock( myMutex );

    (*numCopies) -= 1;

    if( *numCopies > 0 ) {
      // There are still copies left, exit normally
      pthread_mutex_unlock( myMutex );
    }
    else {
      pthread_mutex_unlock( myMutex );
      deallocateInternals();
    }
  }

  myMutex = fromMe.myMutex;
  signalWriters = fromMe.signalWriters;
  writeLocked = fromMe.writeLocked;
  mapSt = fromMe.mapSt;
  numCopies = fromMe.numCopies;
  fragCount = fromMe.fragCount;

  K = fromMe.K;
  isFinished = fromMe.isFinished;

  // Need to update the number of copies.
  if( myMutex != NULL ) {
    pthread_mutex_lock( myMutex );

    (*numCopies) += 1;

    pthread_mutex_unlock( myMutex );
  }
}

GlobalGLAState :: GlobalGLAState( int k ) : K(k), isFinished(false) {
  myMutex = new pthread_mutex_t;
  signalWriters = new pthread_cond_t;

  pthread_mutex_init( myMutex, NULL );
  pthread_cond_init( signalWriters, NULL );

  numCopies = new int;
  *numCopies = 1;

  mapSt = new vector<GLAState>( k, GLAState() );

  fragCount = new vector<int>( k, 0 );

  writeLocked = new vector<bool>( k, false );
}

GlobalGLAState :: ~GlobalGLAState() {
  if( myMutex != NULL ) {
    pthread_mutex_lock( myMutex );

    (*numCopies) -= 1;

    pthread_mutex_unlock( myMutex );

    if( *numCopies == 0 )
      deallocateInternals();
  }
}

void GlobalGLAState :: swap( GlobalGLAState &fromMe ) {
  // Make temporary copies of our state
  bool _isFinished = isFinished;
  pthread_mutex_t * _myMutex = myMutex;
  pthread_cond_t * _signalWriters = signalWriters;
  vector<bool> * _writeLocked = writeLocked;
  int * _numCopies = numCopies;
  int _K = K;
  vector<GLAState> * _mapSt = mapSt;
  vector<int> * _fragCount = fragCount;

  // Copy the state from fromMe into ourselves.
  isFinished = fromMe.isFinished;
  myMutex = fromMe.myMutex;
  signalWriters = fromMe.signalWriters;
  writeLocked = fromMe.writeLocked;
  numCopies = fromMe.numCopies;
  K = fromMe.K;
  mapSt = fromMe.mapSt;
  fragCount = fromMe.fragCount;

  // Copy the information we saved earlier into fromMe
  fromMe.isFinished = _isFinished;
  fromMe.myMutex = _myMutex;
  fromMe.signalWriters = _signalWriters;
  fromMe.writeLocked = _writeLocked;
  fromMe.numCopies = _numCopies;
  fromMe.K = _K;
  fromMe.mapSt = _mapSt;
  fromMe.fragCount = _fragCount;
}

void GlobalGLAState :: deallocateInternals() {
  if( myMutex != NULL ) {
    pthread_mutex_destroy( myMutex );
    delete myMutex;
  }

  if( signalWriters != NULL ) {
    pthread_cond_destroy( signalWriters );
    delete signalWriters;
  }

  if( writeLocked != NULL )
    delete writeLocked;

  if( numCopies != NULL )
    delete numCopies;

  if( mapSt != NULL )
    delete mapSt;

  if( fragCount != NULL )
    delete fragCount;
}

// GlobalGLAPtr definitions

GlobalGLAPtr :: GlobalGLAPtr() : glaPtr(NULL) { }

GlobalGLAPtr :: GlobalGLAPtr( GlobalGLAState & state ) {
  glaPtr = &state;
}

GlobalGLAPtr :: ~GlobalGLAPtr() { }

void GlobalGLAPtr :: swap( GlobalGLAPtr &fromMe ) {
  GlobalGLAState * _glaPtr = glaPtr;

  glaPtr = fromMe.glaPtr;

  fromMe.glaPtr = _glaPtr;
}

void GlobalGLAPtr :: copy( GlobalGLAPtr & fromMe ) {
  glaPtr = fromMe.glaPtr;
}

GlobalGLAState * GlobalGLAPtr :: get_glaPtr() {
  return glaPtr;
}
