#include <armadillo>
#include <math.h>

#define ADD(x) x += other.x;

/*
 *  This is an implementation of Logistic Regression based heavily on the
 *  MADlib implementation.
 */

/** Information for Meta-GLAs
 *
 *  NAME(</LogisticRegressionGLA/>)
 *  INPUTS(</(x, VECTOR), (y, DOUBLE)/>)
 *  OUTPUTS(</(count, DOUBLE)/>)
 *  CONSTRUCTOR(</(width, BIGINT)/>)
 *  RESULT_TYPE(</multi/>)
 *
 *  OPT_ITERABLE
 *
 *  LIBS(armadillo)
 */

using namespace arma;

typedef Col<DOUBLE> VECTOR;
typedef Mat<DOUBLE> MATRIX;

class LogisticRegressionGLA {

    // Inter-iteration components
    uint64_t iteration;         // current iteration
    VECTOR coef;                // vector of coefficients
    VECTOR dir;                 // direction
    VECTOR grad;                // gradient
    DOUBLE beta;                // scale factor

    // Intra-iteration components
    uint64_t numRows;           // number of rows processed this iteration
    VECTOR gradNew;             // intermediate value for gradient
    MATRIX sparsity;            // X^T A X
    DOUBLE loglikelihood;       // ln(l(c))

    // Keep track of internal iteration for retrieving results
    uint64_t tuplesProduced;

    // Helper function
    double sigma( double x ) {
        return 1.0 / (1.0 + std::exp(-x));
    }

public:

    // Initial constructor, no previous state.
    LogisticRegressionGLA( const BIGINT & width ) :
        // Inter-iteration components
        iteration       (0),
        coef            (width),
        dir             (width),
        grad            (width),
        beta            (0.0),
        // Intra-iteration components
        numRows         (0),
        gradNew         (width),
        sparsity        (width, width),
        loglikelihood   (0.0)
    {
        gradNew.zeroes();
        sparsity.zeroes();
    }

    // Constructor for later iterations, using constant state
    LogisticRegressionGLA( const BIGINT & width, const LogisticRegressionGLA & state ) :
        // Inter-iteration components
        iteration       (state.iteration + 1),
        coef            (state.coef),
        dir             (state.dir),
        grad            (state.grad),
        beta            (state.beta),
        // Intra-iteration components
        numRows         (0),
        gradNew         (width),
        sparsity        (width, width),
        loglikelihood   (0.0)
    {
        gradNew.zeroes();
        sparsity.zeroes();
    }

    void AddItem( const VECTOR & x, const DOUBLE & y ) {
        ++numRows;

        DOUBLE xc = dot(x, coef);
        gradNew += sigma(-y * xc) * y * trans(x);

        double a = sigma(xc) * sigma(-xc);
        sparsity += x * trans(x) * a;

        loglikelihood -= std::log( 1.0 + std::exp(-y * xc) );
    }

    void AddState( const LogisticRegressionGLA & other ) {
        ADD(sparsity);
        ADD(gradNew);
        ADD(numRows);
        ADD(loglikelihood);
    }

    void Finalize() {
        if( iteration != 0 ) {
            // We use the Hestenes-Stiefel update formula:
            //
            //          g_k^T (g_k - g_{k-1})
            // beta_k = -------------------------
            //          d_{k-1}^T (g_k - g_{k-1})
            VECTOR gradNewMinusGrad = gradNew - grad;
            beta = dot(gradNew, gradNewMinusGrad) /
                dot(dir, gradNewMinusGrad);

            // Alternatively, we could use Polak-Ribière
            // state.beta
            //     = dot(state.gradNew, gradNewMinusGrad)
            //     / dot(state.grad, state.grad);

            // Or Fletcher–Reeves
            // state.beta
            //     = dot(state.gradNew, state.gradNew)
            //     / dot(state.grad, state.grad);

            // Do a direction restart (Powell restart)
            // Note: This is testing whether state.beta < 0 if state.beta were
            // assigned according to Polak-Ribière
            if( dot(gradNew, gradNewMinusGrad) /
                    dot(grad, grad) < 0 )
                beta = 0;

            // d_k = g_k - beta_k * d_{k-1}
            dir = gradNew - beta * dir;
            grad = gradNew;
        }
        else {
            dir = gradNew;
            grad = gradNew;
        }

        // H_k = - X^T A_k X
        // where A_k = diag(a_1, ..., a_n) and a_i = sigma(x_i c_{k-1}) sigma(-x_i c_{k-1})
        //
        //             g_k^T d_k
        // alpha_k = -------------
        //           d_k^T H_k d_k
        //
        // c_k = c_{k-1} - alpha_k * d_k
        coef += dot(grad, dir) / as_scalar(trans(dir) * sparsity * dir) * dir;

        if( !coef.is_finite() ) {
            cerr << "Over- or underflow in "
                "conjugate-gradient step, while updating coefficients. Input data "
                "is likely of poor numerical condition" << endl;

            // Should probably replace this with an actual exception.
            throw 1;
        }

        // Set internal iterator
        tuplesProduced = 0;
    }

    // FIXME: Add in iteration condition.

    // FIXME: The output should technically be a variety of things, including
    // vectors and other complex types.
    // May need to just produce itself as a state.
    bool GetNextResult( DOUBLE& count ) {
        if( tuplesProduced < 1 ) { // fast track
            ++tuplesProduced;
            count = numRows;

            // Produce the diagnostic statistics of the state.
            VECTOR eigenValues(coef.n_cols);
            MATRIX eigenVectors(sparsity.n_rows, sparsity.n_cols);

            eig_sym(eigenValues, eigenVectors, sparsity);

            VECTOR diagInvSparse = diagvec(inv(eigenVectors));

            // FIXME: How do you calculate the condition number?

            // Set up vectors to hold diagnostics
            VECTOR stdErr(coef.n_cols);
            VECTOR waldZStats(coef.n_cols);
            VECTOR waldPValues(coef.n_cols);
            VECTOR oddsRatios(coef.n_cols);

            for( size_t i = 0; i < coef.n_cols; ++i ) {
                stdErr(i) = std::sqrt(diagInvSparse(i));
                waldZStats(i) = coef(i) / stdErr(i);
                // FIXME: Need to figure out what library is needed for the
                // probability functions.
                waldPValues(i) = 2.0 * prob::cdf( prob::normal(),
                        - std::abs(waldZStats(i)));
                oddsRatios(i) = std::exp( coef(i) );
            }

            cout << endl;
            cout << "[LogisticRegressionGLA] Output for iteration " << iteration << ":" << endl;
            cout << "Coefficients:" << endl;
            cout << coef << endl;
            cout << "Standard Error:" << endl;
            cout << stdErr << endl;
            cout << "Z Statistics:" << endl;
            cout << waldZStats << endl;
            cout << "P Values:" << endl;
            cout << waldPValues << endl;

            return true;
        }

        return false;
    }
};
