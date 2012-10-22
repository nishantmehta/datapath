#include <armadillo>
#include "arma/Types/VECTOR.h"
#include "arma/Types/MATRIX.h"

/** Information for Meta-GLAs
 *
 * GLA_DESC
 *  NAME(</SparsityMatrix/>)
 *  INPUTS(</(v, VECTOR)/>)
 *  OUTPUTS(</(count, DOUBLE)/>)
 *  RESULT_TYPE(</single/>)
 *  CONSTRUCTOR(</(numElem, INT)/>)
 * END_DESC
 *
 * LIBS(armadillo)
 */

class SparsityMatrix {

    uint64_t tupleCount;

    MATRIX sparsityMatrix;

    uint64_t curRow;

public:

    SparsityMatrix ( const INT numElem ) : tupleCount(0), sparsityMatrix(numElem, numElem) {
        sparsityMatrix.zeros();
    }

    void AddItem( const VECTOR & v ) {
        sparsityMatrix += v * trans(v);
        ++tupleCount;
    }

    void AddState( const SparsityMatrix & other ) {
        tupleCount += other.tupleCount;
        sparsityMatrix += other.sparsityMatrix;
    }

    void Finalize() {
        curRow = 0;

        cout << endl;
        cout << "SparsityMatrix: tuples processed = " << tupleCount << endl;
        cout << "Sparsity Matrix:" << endl;
        cout << sparsityMatrix << endl;
    }

    void GetResult( DOUBLE& count ) {
        count = tupleCount;
    }
};
