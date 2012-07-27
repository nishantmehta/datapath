#include <armadillo>

/** Information for Meta-GLAs
 *
 * NAME(</SparsityMatrixGLA/>)
 * INPUTS(</(v, VECTOR)/>)
 * OUTPUTS(</(count, DOUBLE)/>)
 * RESULT_TYPE(</single/>)
 *
 * LIBS(armadillo)
 */

class SparsityMatrixGLA {

    uint64_t tupleCount;

    typedef Mat<DOUBLE> MATRIX;
    MATRIX sparsityMatrix;

    uint64_t curRow;

    typedef Col<DOUBLE> VECTOR;
public:

    SparsityMatrixGLA<//>() : tupleCount(0), sparsityMatrix(NUM_ELEM, NUM_ELEM) {
        sparsityMatrix.zeros();
    }

    void AddItem( const VECTOR & v ) {
        sparsityMatrix += v * trans(v);
        ++tupleCount;
    }

    void AddState( const SparsityMatrixGLA & other ) {
        tupleCount += other.tupleCount;
        sparsityMatrix += other.sparsityMatrix;
    }

    void Finalize() {
        curRow = 0;

        cout << endl;
        cout << "</SparsityMatrixGLA/>: tuples processed = " << tupleCount << endl;
        cout << "Sparsity Matrix:" << endl;
        cout << sparsityMatrix << endl;
    }

    void GetResult( DOUBLE& count ) {
        count = tupleCount;
    }
};
