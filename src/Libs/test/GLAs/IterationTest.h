/** Information for Meta-GLAs
 *
 *  GLA_DESC
 *      NAME(</IterationTest/>)
 *      INPUTS(</(x, DOUBLE)/>)
 *      OUTPUTS(</(iteration, BIGINT), (sum, DOUBLE)/>)
 *      RESULT_TYPE(</single/>)
 *
 *      OPT_ITERABLE
 *  END_DESC
 */

// declaration
class IterationTest;

class IterationTest_ConstState {
    uint64_t iteration;

    friend class IterationTest;

public:
    IterationTest_ConstState() : iteration(0) {}
};

class IterationTest {
    DOUBLE sum;

    const IterationTest_ConstState& state;

public:
    IterationTest( const IterationTest_ConstState& state ) :
        state(state), sum(0.0)
    { }

    void AddItem( const DOUBLE& x ) {
        sum += x;
    }

    void AddState( IterationTest& other ) {
        sum += other.sum;
    }

    bool ShouldIterate( IterationTest_ConstState& modibleState ) {
        modibleState.iteration += 1;

        if( modibleState.iteration < 5 )
            return true;
        else
            return false;
    }

    void GetResult( BIGINT& iteration, DOUBLE& sum ) {
        iteration = state.iteration;
        sum = this->sum;
    }
};
