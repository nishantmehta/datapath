#ifndef COREFERENCE_H_
#define COREFERENCE_H_

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

private:
	Mentions mentions;
	Entities entities;

public:
	void Step(Task& task, CGLA& cGla){

		int mention = task.GetItem1();
		int entity = mentions[task.GetItem2()].GetEntity();

		// flip a coin and move mention into entity if heads

	}

}



#endif //  COREFERENCE_H_
