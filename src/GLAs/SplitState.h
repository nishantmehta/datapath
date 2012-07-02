//
//  Copyright 2012 Christopher Dudley
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

#ifndef _SPLIT_STATE_H_
#define _SPLIT_STATE_H_

#include <pthread.h>
#include <iostream>

using namespace std;

/*
 * This class provides a facility to handle several parts of a GLA's state
 * and access them in a thread-safe manner.
 */
template<typename G>
class SplitState {

    typedef G StateType;

    // Array of states
    StateType ** stateArray;

    // Mutex to protect states
    pthread_mutex_t * myMutex;

    // Condition variable to wake up threads blocked on acquiring a state.
    pthread_cond_t * signalVar;

    // number of states
    size_t numStates;

    // Keeps track of which states are available to be checked out.
    bool * writeLocked;

public:

    // Constructor
    SplitState( size_t numStates ) : numStates( numStates ) {
        stateArray = new StateType *[numStates];
        writeLocked = new bool[numStates];

        for( size_t i = 0; i < numStates; ++i ) {
            stateArray[i] = NULL;
            writeLocked[i] = false;
        }

        myMutex = new pthread_mutex_t;
        pthread_mutex_init(myMutex, NULL);

        signalVar = new pthread_cond_t;
        pthread_cond_init(signalVar, NULL);

    }

    // Destructor
    ~SplitState() {
        for( size_t i = 0; i < numStates; ++i ) {
            if( stateArray[i] != NULL )
                delete stateArray[i];
        }

        delete [] stateArray;
        delete [] writeLocked;

        pthread_mutex_destroy( myMutex );
        delete myMutex;

        pthread_cond_destroy( signalVar );
        delete signalVar;
    }

    // Methods

    int CheckOutOne( int *theseAreOK, StateType *& checkMeOut ) {
        // first, figure out all of the OK segments
        int numWanted = 0;
        int goodOnes[numStates];
        for (int i = 0; i < numStates; i++) {
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
                if (!writeLocked[whichToChoose]) {

                    // he is open, so write lock him
                    writeLocked[whichToChoose] = true;

                    // and return him
                    checkMeOut = stateArray[whichToChoose];
                    stateArray[whichToChoose] = NULL;
                    pthread_mutex_unlock (myMutex);
                    return whichToChoose;
                }
            }

            // if we got here, then every one that we want is write locked.  So
            // we will go to sleep until one of them is unlocked, at which point
            // we will wake up and try again...
            pthread_cond_wait (signalVar, myMutex);
        }
    }

    void CheckIn( int whichEntry, StateType *& checkMeIn ) {
        // just note that no one is writing this one, then signal all potential writers
        pthread_mutex_lock (myMutex);
        writeLocked[whichEntry] = false;

        stateArray[whichEntry] = checkMeIn;
        checkMeIn = NULL;

        pthread_cond_broadcast (signalVar);
        pthread_mutex_unlock (myMutex);
    }

    StateType * Peek( int whichEntry ) {
        return stateArray[ whichEntry ];
    }
};

#endif // _SPLIT_STATE_H_
