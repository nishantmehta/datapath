m4_define(</SparsityMatrixGLA/>, </dnl
m4_divert_push(-1)
m4_redefine(</GLA_NAME/>, </$1/>)
m4_redefine(</NUM_ELEM/>, </$2/>)
m4_redefine(</ELEM_TYPE/>, VAR($3))
m4_redefine(</MAX_INDEX/>, m4_eval(NUM_ELEM - 1))

m4_if( m4_eval(NUM_ELEM < 1), 1, </m4_fatal(Cannot create vector with NUM_ELEM elements!)/>)

m4_redefine(</MY_INPUT/>, </(v, VECTOR)/>)

m4_ifdef_undef(</MY_OUTPUT/>)
m4_forloop(</_I_/>, 0, MAX_INDEX, </dnl
<//>m4_append(</MY_OUTPUT/>, </(x_/>_I_</, />ELEM_TYPE</)/>, </</, />/>)
/>)
m4_redefine(</MY_REZTYPE/>, </multi/>)
m4_divert_pop(-1)dnl
#include <iostream>
#include <armadillo>
#include <inttypes.h>

using namespace std;
using namespace arma;

/** Information for meta-GLAs
 *
 * m4_qdefine(</GLA_NAME</_INPUT/>/>, </MY_INPUT/>)
 * m4_qdefine(</GLA_NAME</_OUTPUT/>/>, </MY_OUTPUT/>)
 * m4_qdefine(</</GLA_REZTYPE_/>GLA_NAME/>, </MY_REZTYPE/>)
 * LIBS(armadillo)
 */

class GLA_NAME {

    uint64_t tupleCount;

    typedef Mat<ELEM_TYPE> MATRIX;
    MATRIX sparsityMatrix;

    uint64_t curRow;

    typedef Col<ELEM_TYPE> VECTOR;
public:

    GLA_NAME<//>() : tupleCount(0), sparsityMatrix(NUM_ELEM, NUM_ELEM) {
        sparsityMatrix.zeros();
    }

    void AddItem( const VECTOR & v ) {
        sparsityMatrix += v * trans(v);
        ++tupleCount;
    }

    void AddState( const GLA_NAME & other ) {
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

    bool GetNextResult(TYPED_REF_ARGS(MY_OUTPUT)) {
        if( curRow == NUM_ELEM )
            return false;

m4_forloop(</_I_/>, 0, MAX_INDEX, </dnl
        x_<//>_I_ = sparsityMatrix(curRow, _I_);
/>)dnl

        ++curRow;

        return true;
    }
};
/>)dnl
