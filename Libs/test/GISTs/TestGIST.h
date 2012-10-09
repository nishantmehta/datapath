#ifndef _TEST_GIST_H_
#define _TEST_GIST_H_

#include "base/Types/INT.h"
#include <vector>
#include <iostream>

/*  Meta-information
 *
 *  GIST_DESC
 *      NAME(</TestGIST/>)
 *      CONSTRUCTOR(</(numSteps, INT)/>)
 *      OUTPUTS(</(x, INT)/>)
 *      RESULT_TYPE(</single/>)
 *
 *      TASK_TYPE(</INT/>)
 *      LOCAL_SCHEDULER_TYPE(</TestLS/>)
 *      GLA_TYPE(</TestGLA/>)
 *  END_DESC
 */

class TestLS {

    int ID;
    int numIter;
    int curIter;

public:

    TestLS(int _ID, int _numIter) : ID(_ID), numIter(_numIter), curIter(0) {
    }

    bool GetNextTask( int& task ) {
        if( curIter == numIter )
            return false;

        ++curIter;
        task = ID;

        return true;
    }

};

class TestGLA {

    int roundNo;

public:

    TestGLA(int _roundNo) : roundNo(_roundNo) {
    }

    void AddState( TestGLA& other ) {
    }

    bool ShouldIterate() {
        return roundNo < 5;
    }
};

class TestGIST {

    typedef std::vector<INT> MyVector;
    MyVector myVec;

    typedef std::pair<TestLS*, TestGLA*> WorkUnit;
    typedef std::vector<WorkUnit> WUVector;

    INT numSteps;

    int roundNo;

public:

    TestGIST( INT _numSteps ) : numSteps(_numSteps), roundNo(0) {
    }

    ~TestGIST() {
    }

    void PrepareRound( WUVector& workUnits, int parHint ) {
        ++roundNo;
        myVec = MyVector(parHint, 0);

        for( int i = 0; i < parHint; ++i ) {
            TestLS* localScheduler = new TestLS(i, numSteps);
            TestGLA* gla = new TestGLA(roundNo);

            WorkUnit wu(localScheduler, gla);
            workUnits.push_back(wu);
        }
    }

    void DoStep( INT& task, TestGLA& gla ) {
        myVec[task] += 1;
    }

    void GetResult( INT& x ) {
        x = 0;
        for( int i = 0; i < myVec.size(); ++i ) {
            x += myVec[i];
        }
    }
};

#endif // _TEST_GIST_H_
