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

#ifndef GLA_WAY_POINT_IMP
#define GLA_WAY_POINT_IMP

#include "ID.h"
#include "History.h"
#include "Tokens.h"
#include "WayPointImp.h"
#include "GLAData.h"
#include "GLAHelpers.h"
#include "Constants.h"

/** WARNING: The chunk processing function has to return 0 and the
        finalize function 3 otherwise acknowledgments are not sent
        properly in the system
*/
class GLAWayPointImp : public WayPointImp {
    // Constant states used by some Queries
    QueryToGLASContMap constStates;

    // A counter for each query representing how many state objects that GLA is
    // waiting on to begin processing.
    QueryIDToInt statesNeeded;

    // Used to keep track of which constant states go where in the constStates list.
    typedef map< WayPointID, int > ConstStateIndexMap;
    typedef map< QueryID, ConstStateIndexMap > QueryToConstStateIndexMap;
    QueryToConstStateIndexMap constStateIndex;

protected:

    /*************************************************************************/
    // The following methods are used by subclasses to access information about
    // constant states that are needed.
    /*************************************************************************/

    // Initializes information about constant states from configuration data.
    void InitConstStates( QueryToReqStates& reqStates );

    // Adds any generated states to the lists and adjusts indicies appropriately.
    void AddGeneratedStates( QueryToGLASContMap& genStates );

    // Returns the number of states needed for a particular query.
    int NumStatesNeeded( QueryID query );

    // Returns a copy of the constant states map.
    QueryToGLASContMap GetConstStates();

    // Enumeration for work function exit codes.
    enum ExitCode {
        PREPROCESSING   = -1,
        PROCESS_CHUNK   =  0,
        POST_PROCESSING =  1,
        PRE_FINALIZE    =  2,
        FINALIZE        =  3,
        POST_FINALIZE   =  4
    };

    // This method is called when the waypoint has received a data message
    // containing a chunk to be processed and a work token has been acquired.
    // This method should generate the necessary work description and other
    // necessary information to perform work on the chunk.
    virtual void GotChunkToProcess( CPUWorkToken& token, QueryExitContainer& whichOnes,
            ChunkContainer& chunk, HistoryList& lineage ) = 0;

    // Called when a wayoint has received a state from another waypoint.
    virtual void GotState( StateContainer& state );

    // Called when a waypoint has received all of the states it needs for a query.
    virtual void GotAllStates( QueryID query ) = 0;

    // Removes data pertaining to the specified queries from the waypoint.
    void RemoveQueryData( QueryIDSet queries );

    /*************************************************************************/
    // The following methods are to be defined by subclasses to determine the
    // behavior of the RequestGranted method. These will determine whether
    // or not a certain type of operation is possible and, if so, prepare
    // a work description to be sent for processing.
    // The default behavior for these functions is to return false.
    /*************************************************************************/

    // Pre-Processing occurs before any chunks have been processed by the GLA.
    virtual bool PreProcessingPossible( CPUWorkToken& token );

    // Post-Processing occurs after all chunks have been processed by the GLA
    // but before any finalization steps (e.g., merging states)
    virtual bool PostProcessingPossible( CPUWorkToken& token );

    // Pre-Finalize occurs after post-processing is complete, and it used to
    // perform work and collect data needed for the finalize stage.
    virtual bool PreFinalizePossible( CPUWorkToken& token );

    // Finalization occurs when the GLA is ready to produce data.
    virtual bool FinalizePossible( CPUWorkToken& token );

    // Post-Finalize occurs after some or all of the finalization is complete.
    // The purpose of Post-Finalize is to clean up data that is no longer
    // needed and deallocate states that are no longer used.
    virtual bool PostFinalizePossible( CPUWorkToken& token );

    /*************************************************************************/
    // The following methods are to be defined by subclasses to perform actions
    // necessary after a particular work function has completed its work.
    // The default behavior of these functions is to do nothing.
    /*************************************************************************/

    // Called when a Pre-Processing work function has finished.
    virtual bool PreProcessingComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data);

    // Called when a ProcessChunk work function has finished.
    // An Ack message will automatically be sent after this function is called.
    virtual bool ProcessChunkComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data);

    // Called when a Post-Processing function has finished.
    // Token Requests will automatically be generated after this function is
    // called.
    virtual bool PostProcessComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    // Called when a Pre-Finalize function has finished.
    // Token requests will automatically be generated after this function is
    // called.
    virtual bool PreFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    // Called when a Finalize function has finished.
    // Token Requests will automatically be generated after this function is
    // called.
    virtual bool FinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    // Called when a Post-Finalize function has finished.
    // Token requests will automatically be generated after this function is
    // called.
    virtual bool PostFinalizeComplete( QueryExitContainer& whichOnes, HistoryList& history, ExecEngineData& data );

    /*************************************************************************/
    // The following methods are for handling other types of message received
    // by the waypoint, such as Query Done and Start Producing messages.
    // These methods must be defined by the subtype.
    /*************************************************************************/

    // This method is called when a query done message is received from another
    // waypoint. whichOnes contains the query exits from the message.
    // This method should return true if additional cpu tokens should be
    // generated.
    virtual bool ReceivedQueryDoneMsg( QueryExitContainer& whichOnes ) = 0;

    // This method is called when a start producing message is received from
    // another waypoint downstream. whichOne is the query that is being
    // requested to start producing. This method should return true if
    // additional tokens should be generated.
    virtual bool ReceivedStartProducingMsg( HoppingUpstreamMsg& message, QueryExit& whichOne );

    /*************************************************************************/
    // The following methods are used for configuring the GLAWayPoint
    /*************************************************************************/

    // This method sets which exit code in DoneProducing to forward on the data received.
    // The data from any other return code is discarded.
    void SetResultExitCode( ExitCode exitCode );

private:

    ExitCode resultExitCode = FINALIZE;

public:

    // constructor and destructor
    GLAWayPointImp ();
    virtual ~GLAWayPointImp ();


    // these are just implementations of the standard WayPointImp functions

    /* Meaning of the return values:
     * -1 - Preprocessing
     *  0 - Chunk Processed
     *  1 - Post Processing
     *  2 - Pre-Finalize
     *  3 - Finalize
     *  4 - Post-Finalize
     */
    void DoneProducing (QueryExitContainer &whichOnes, HistoryList &history, int returnVal, ExecEngineData& data);
    void RequestGranted (GenericWorkToken &returnVal);
    void ProcessHoppingDataMsg (HoppingDataMsg &data);
    void ProcessHoppingDownstreamMsg (HoppingDownstreamMsg &message);
    void ProcessHoppingUpstreamMsg( HoppingUpstreamMsg& message);
};

#endif
