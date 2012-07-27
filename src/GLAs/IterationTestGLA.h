/** Information for Meta-GLAs
 *
 *  NAME(</IterationTestGLA/>)
 *  INPUTS(</(x, DOUBLE)/>)
 *  OUTPUTS(</(iteration, BIGINT), (sum, DOUBLE)/>)
 *  RESULT_TYPE(</single/>)
 *
 *  OPT_ITERABLE
 */

// declaration
class IterationTestGLA;

class IterationTestGLA_ConstState {
    uint64_t iteration;

    friend class IterationTestGLA;

public:
    IterationTestGLA_ConstState() : iteration(0) {}
};

class IterationTestGLA {
    DOUBLE sum;

    const IterationTestGLA_ConstState& state;

public:
    IterationTestGLA( const IterationTestGLA_ConstState& state ) :
        state(state), sum(0.0)
    { }

    void AddItem( const DOUBLE& x ) {
        sum += x;
    }

    void AddState( IterationTestGLA& other ) {
        sum += other.sum;
    }

    bool ShouldIterate( IterationTestGLA_ConstState& modibleState ) {
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
