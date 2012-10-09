#ifndef COREFERENCE_H_
#define COREFERENCE_H_

/* Meta-information
 *
 *  GIST_DESC
 *      NAME(CoreferenceState)
 *      OUTPUTS()   // Fill this in!
 *      RESULT_TYPE() // Fill this in!
 *
 *      TASK_TYPE(Task)
 *      LOCAL_SCHEDULER_TYPE(LocalScheduler)
 *      GLA_TYPE(CGLA)
 *  END_DESC
 */

#include <vector>

class GlobalScheduler {
	int numThreads;
	int numMentions;
public:
	GlobalScheduler(int _numThreads, int _numMentions):
		numThreads(_numThreads), numMentions(_numMentions){}

	template <typename LSched>
	void GetLocalSchedulers(vector<LSched>& lSched){
		for(int i=0; i<numThreads; i++){
			LSched l(numMentions, numThreads);
			lSched.push_back(l);
		}
	}

};


/** Task, in this case a pair of mentions or entities that should be merged

    NOTE: Since we have as many entities as we have mentions, the
    numers can be interpreted in any way the state wants.
 */
class Task{
	int item1;
	int item2;

public:
	// make up an invalid task
	Task():_item1(-1), item2(-1){}
	Task(int _item1, int _item2):item1(_item1), item2(_item2){}
	int GetItem1(void){ return item1; }
	int GetItem2(void){ return item2; }

	bool IsValid(){ return (item1!=-1 && item2!=-1); }
};

class LocalScheduler{
	int numMentions;
	int numToGenerate; // how many tasks we still need to generate?
public:
	LocalScheduler(int _numMentions, int numThreads):
		numMentions(_nunMentions), numToGenerate(_numMentions/numThreads){}

	Task GetNextTask(void){
		if (numToGenerate<=0)
			return Task();
		else {
			numToGenerate--;
			return Task(rand()%numMentions, rand()%numMentions);
		}
	}
};

/* convergence GLA */
class CGLA{
	int numIter;
public:
	CGLA(int _numIter):numIter(_numIter){}

	// not based on data
	// not needed void AddItem(int){}
	void AddState(CGLA& other){}

	bool ShouldIterate(void){
		return ((numIter--)>=0);
	}
};

/* State object for coreference */
class CoreferenceState{

public:

	// Mentions datastructure

	class Mention{
		int feature; // mock feature
		int entity; // the id of the entity this belongs to
	public:
		Mention(int val):feature(val){}

		int GetEntity(void){ return entity; }
	};

	class Entity{
		std::atomic<vector<int>*> mentions;
	public:
		// constructor from a single mention
		Entity(int _mention){
			mentions.push_back(_mention);
		}

		AddMention(int _mention){
			// create another vector with the new mention added
			vector<int>* _mentions = mentions.load();


			// swap the two vectors


		}
	};

	typedef vector<Mention> Mentions;
	typedef vector<Entity> Entities;

    // Work unit typedef, used for preparing new rounds
    typedef std::pair<LocalScheduler*, CGLA*> WorkUnit;
    typedef std::vector<WorkUnit> WUVector;

private:
	Mentions mentions;
	Entities entities;

public:

    // Constructor
    CoreferenceState() { }
    // Destructor
    ~CoreferenceState() {}

    void PrepareRound( WUVector& workUnits, size_t paraHint ) {
        // Generate pairs of local schedulers and GLAs,
        // using paraHint as a hint about the level of parallelization desired
        // by the system.
    }

	void DoStep(Task& task, CGLA& cGla){

		int mention = task.GetItem1();
		int entity = mentions[task.GetItem2()].GetEntity();

		// flip a coin and move mention into entity if heads

	}


    // Use one of the output methods below:

#ifdef COREF_USE_SINGLE
    ///// single /////
    void GetResult( /* outputs go here */ ) {
        // Set the outputs here.
    }
#endif

#ifdef COREF_USE_MULTI
    ///// multi /////
private:
    // used for internal iteration
    // this is just an example, you can have any kind of state needed here to
    // determine what to produce next.
    int curResult;

public:
    void Finalize() {
        // Do any necessary finalization here.
        // This will likely set the internal iterator (in this case curResult)
    }

    bool GetNextResult( /* outputs go here */ ) {
        // Return false is there are no more results to be produced.
        // Otherwise, set the outputs to the next values and return true.
    }
#endif

#ifdef COREF_USE_FRAGMENT
    ///// fragment /////

    // First we need an iterator type
    struct Iterator {
        // Any state needed to determine iteration goes here.
    }

    int GetNumFragments() {
       // Do anything here you need to figure out how many fragments you will produce.
       // 0 is a valid answer.
    }

    Iterator* Finalize( int fragment ) {
        // Create the iterator for this fragment and return a pointer to it.
    }

    bool GetNextResult( Iterator*, /* outputs go here */ ) {
        // use the iterator to determine the next result to produce.
        // if there is no result to be produced, return false,
        // otherwise return true;
    }
#endif
}

#ifdef COREF_USE_FRAGMENT
// typedef for fragment interface
typedef CoreferenceState::Iterator Coreference_Iterator;
#endif


#endif //  COREFERENCE_H_
