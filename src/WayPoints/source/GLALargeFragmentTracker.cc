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
#include "GLALargeFragmentTracker.h"
#include "Errors.h"

//#include <iostream>

using namespace std;

GLALargeFragmentTracker :: GLALargeFragmentTracker( const vector<int> & fragPerSeg ) {
  numAvailable = 0;

  fragList = vector< vector<bool> >();
  ackedFragments = vector< vector<bool> >();
  acksRemaining = vector< int >();
  segsDeallocated = vector< bool >( fragPerSeg.size(), false );

  //cerr << "Creating fragment tracker:" << endl;
  //for( int i = 0; i < fragPerSeg.size(); ++i )
  //  cout << "[" << i << "]: " << fragPerSeg[i] << endl;

  for( vector<int>::const_iterator iter = fragPerSeg.begin(); iter != fragPerSeg.end(); ++iter ) {
    numAvailable += *iter;
    if( *iter > 0 ) {
        fragList.push_back( vector<bool>( *iter, true ) );
        ackedFragments.push_back( vector<bool>( *iter, false ) );
    } else {
        fragList.push_back( vector<bool>( 1, false ) );
        ackedFragments.push_back( vector<bool>( 1, true ) );
    }
    acksRemaining.push_back( *iter );
  }

  toBeAcked = numAvailable;

  segIter = 0;
  fragIter = 0;
}

GLALargeFragmentTracker :: ~GLALargeFragmentTracker() {
}

bool GLALargeFragmentTracker :: getNext( int & segNum, int & fragNum ) {
  if( numAvailable == 0 )
    return false;

  segNum_t prevSeg = segIter;
  fragNum_t prevFrag = fragIter;

  // Finish off current segment
  for(; fragIter < fragList[segIter].size(); ++fragIter ) {
    if( fragList[segIter][fragIter] ) {
      segNum = segIter;
      fragNum = fragIter;
      fragList[segIter][fragIter] = false;
      --numAvailable;
      return true;
    }
  }

  ++segIter;

  // Search to end of list
  for(; segIter < fragList.size(); ++segIter ) {
    for(fragIter = 0; fragIter < fragList[segIter].size(); ++fragIter ) {
      if( fragList[segIter][fragIter] ) {
    segNum = segIter;
    fragNum = fragIter;
    fragList[segIter][fragIter] = false;
    --numAvailable;
    return true;
      }
    }
  }

  // Search from beginning of list
  for( segIter = 0; segIter <= prevSeg && segIter < fragList.size(); ++segIter ) {
    for( fragIter = 0; (segIter != prevSeg && fragIter < fragList[segIter].size()) ||
       (fragIter < prevFrag); ++fragIter ) {
      if( fragList[segIter][fragIter] ) {
    segNum = segIter;
    fragNum = fragIter;
    fragList[segIter][fragIter] = false;
    --numAvailable;
    return true;
      }
    }
  }

  FATAL( "We should have had a fragment available but we didn't." );
}

bool GLALargeFragmentTracker :: isFinished() {
  // We are only finished if every single fragment has been acknowledged.
  return (toBeAcked == 0);
}

void GLALargeFragmentTracker :: fragAcked( const int segNum, const int fragNum ) {
  FATALIF( segNum < 0 || segNum >= fragList.size(),
       "Tried to ack a fragment for a segment that doesn't exist. (%d, %d)",
       segNum, fragNum);
  FATALIF( fragNum < 0 || fragNum >= fragList[segNum].size(),
       "Tried to ack a fragment that doesn't exist in a segment. (%d, %d)",
       segNum, fragNum);
  FATALIF( ackedFragments[segNum][fragNum],
       "Tried to ack a fragment that has already been acked. (%d, %d)",
       segNum, fragNum);
  FATALIF( fragList[segNum][fragNum],
       "Tried to ack a fragment that has not yet been processed. (%d, %d)",
       segNum, fragNum);

  --toBeAcked;

  ackedFragments[segNum][fragNum] = true;
  acksRemaining[segNum] -= 1;
}

void GLALargeFragmentTracker :: fragDropped( const int segNum, const int fragNum ) {
  FATALIF( segNum < 0 || segNum >= fragList.size(),
       "Tried to drop a fragment for a segment that doesn't exist." );
  FATALIF( fragNum < 0 || fragNum >= fragList[segNum].size(),
       "Tried to drop a fragment that doesn't exist in a segment.");
  FATALIF( ackedFragments[segNum][fragNum],
       "Tried to drop a fragment that has already been acked." );
  FATALIF( fragList[segNum][fragNum],
       "Tried to drop a fragment that has not yet been processed." );

  fragList[segNum][fragNum] = true;
  ++numAvailable;
}

bool GLALargeFragmentTracker :: getDeallocatable( vector<int> & toDealloc ) {
  toDealloc.clear();

  for( int i = 0; i < acksRemaining.size(); ++i ) {
    if( acksRemaining[i] == 0 && !segsDeallocated[i] ) {
      toDealloc.push_back( i );
      segsDeallocated[i] = true;
    }
  }

  return toDealloc.size() > 0;
}

bool GLALargeFragmentTracker :: allDeallocated() {
  for( vector<bool>::iterator it = segsDeallocated.begin(); it != segsDeallocated.end(); ++it ) {
    if( *it == false )
      return false;
  }

  return true;
}

void GLALargeFragmentTracker :: swap( GLALargeFragmentTracker & fromMe ) {
  // Create temporary copies of my data members
  unsigned long _numAvailable = numAvailable;
  unsigned long _toBeAcked = toBeAcked;
  vector< vector<bool> > _fragList = fragList;
  segNum_t _segIter = segIter;
  fragNum_t _fragIter = fragIter;
  vector< vector<bool> > _ackedFragments = ackedFragments;
  vector< int > _acksRemaining = acksRemaining;
  vector< bool > _segsDeallocated = segsDeallocated;

  // Copy fromMe's data members into myself
  numAvailable = fromMe.numAvailable;
  toBeAcked = fromMe.toBeAcked;
  fragList = fromMe.fragList;
  segIter = fromMe.segIter;
  fragIter = fromMe.fragIter;
  ackedFragments = fromMe.ackedFragments;
  acksRemaining = fromMe.acksRemaining;
  segsDeallocated = fromMe.segsDeallocated;

  // Copy my data into fromMe
  fromMe.numAvailable = _numAvailable;
  fromMe.toBeAcked = _toBeAcked;
  fromMe.fragList = _fragList;
  fromMe.segIter = _segIter;
  fromMe.fragIter = _fragIter;
  fromMe.ackedFragments = _ackedFragments;
  fromMe.acksRemaining = _acksRemaining;
  fromMe.segsDeallocated = _segsDeallocated;
}
