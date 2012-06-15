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
#ifndef _GLALARGE_FRAGMENT_TRACKER_H_
#define _GLALARGE_FRAGMENT_TRACKER_H_

#include <vector>

/**
 * \class GLALargeFragmentTracker
 *
 * This class is designed to keep track of the fragments for each segment of a
 * large GLA that still need to be finalized and output. It defines an
 * iterator-style interface to allow others to query if there are still
 * fragments available to be processed, and to get the segment and fragment
 * number of the next fragment to be processed.
 *
 * The tracker must be told if each fragment has been processed successfully or
 * unsucessfully in order to ensure that all fragments are processed exactly
 * once.
 *
 * Note: This class is NOT THREADSAFE.
 */

class GLALargeFragmentTracker {

private:

  /**
   * The number of fragments that are still available to be processed.
   */
  unsigned long numAvailable;

  /**
   * The number of fragments that have yet to be acknowledged
   */
  unsigned long toBeAcked;

  /**
   * For each segment, remembers whether a particular fragment still needs to
   * be processed.
   */
  std::vector<std::vector< bool > > fragList;

  /**
   * The index for the current place in the list of segments.
   */
  typedef std::vector<std::vector< bool > >::size_type segNum_t;
  segNum_t segIter;

  /**
   * The index for the current place in the list of fragments in the current
   * segment.
   */
  typedef std::vector<bool>::size_type fragNum_t;
  fragNum_t fragIter;

  /**
   * For each segment, keeps track of which fragments have been acked.
   */
  std::vector< std::vector< bool > > ackedFragments;

  /**
   * Keeps track of the number of fragments remaining to be acknowledged.
   *
   * This is simply for speed, so that you don't need to check through an entire
   * vector in the ackedFragments list to determine if all of them have been acked.
   */
  std::vector< int > acksRemaining;

  /**
   * Keeps track of which segments have been marked as deallocated.
   */
  std::vector< bool > segsDeallocated;

public:

  /**
   * Default constructor, creates a tracker with no segments or fragments, and
   * is in a state where processing has been completed.
   */
  GLALargeFragmentTracker() : numAvailable(0), toBeAcked(0)
  {  }

  /**
   * Creates a new fragment tracker with a given number of fragments for each
   * segment as given by the vector of integers.
   *
   * \param[in] fragPerSeg a vector containing the number of fragments to be
   *                       produced by each segment.
   */
  GLALargeFragmentTracker(const std::vector<int> & fragPerSeg);

  /**
   * Default destructor.
   */
  virtual ~GLALargeFragmentTracker( );

  /**
   * Gets the next fragment to be processed.
   *
   * \param[out] segNum  the number of the segment to which the fragment belongs.
   * \param[out] fragNum the number of the fragment to be processed.
   *
   * \return true if segNum and fragNum are valid, false if there was nothrow out_of_range("");
   *         available fragment to be processed.
   */
  bool getNext( int & segNum, int & fragNum );

  /**
   * Tells the caller whether or not all fragments have been successfully
   * processed.
   *
   * \return true if all fragments have been successfully processed and acked,
   *         false otherwise.
   */
  bool isFinished();

  /**
   * Tells the tracker that a particular fragment has been acknowledged, meaning
   * that processing was successful on that fragment.
   *
   * \param[in] segNum  the segment to which the fragment belongs.
   * \param[in] fragNum the number of the fragment to be acknowledged.
   *
   * \exception runtime_error thrown if debugging is enabled and the acked
   *   fragment is marked as available.
   * \exception out_of_range thrown if debugging is enabled and the segment
   *   number or fragment number are invalid.
   */
  void fragAcked( const int segNum, const int fragNum );

  /**
   * Tells the tracker that a particular fragment has been dropped and will need
   * to be processed again in the future.
   *
   * \param[in] segNum  the segment to which the fragment belongs.
   * \param[in] fragNum the number of the fragment that was dropped.
   *
   * \exception runtime_error thrown if debugging is enabled and the dropped
   *   fragment was marked as available.
   * \exception out_of_range thrown if debugging is enabled and the segment
   *   number or fragment number are invalid.
   */
  void fragDropped( const int segNum, const int fragNum );

  /**
   * Populates the vector toDealloc with the IDs of segments that are
   * deallocatable (i.e., all of that segment's fragments have been acked and
   * have not yet been deallocated) and marks those segments as having been
   * deallocated.
   *
   * \param[out] toDealloc the segments that are safe to deallocate.
   *
   * \return true if there are segments to deallocate, false if not.
   */
  bool getDeallocatable( std::vector<int> & toDealloc );

  /**
   * Returns whether or not all of the segments have been deallocated.
   *
   * \return true if all segments deallocated, false otherwise.
   */
  bool allDeallocated();

  /**
   * Swaps this fragment tracker with the given fragment tracker.
   *
   * \param[in|out] fromMe the fragment tracker to swap with.
   */
  void swap( GLALargeFragmentTracker & fromMe );

};

#endif // _GLALARGE_FRAGMENT_TRACKER_H_
