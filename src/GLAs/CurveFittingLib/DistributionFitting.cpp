#include "DistributionFitting.h"
#include "MLE.h"


int FitDistribution(vector<double>& distrib, double& param1, double& param2, double& param3){
	double bestC=0.0; // 1.0e+100;
	int best = 0;
	double fit; // current fit
	double p1, p2, p3; // param vars
	double bp1, bp2, bp3; // best params so far
	
	fit = - mle_Poisson_val(distrib, &p1);
	if (fit > bestC){ best = 1; bestC = fit; bp1=p1; bp2=0.0; bp3=0.0; }

	fit = - mle_exponential_val(distrib, &p1);
	if (fit > bestC){ best = 2; bestC = fit; bp1=p1; bp2=0.0; bp3=0.0; }

	fit = - mle_gamma_val(distrib, &p1, &p2);
	if (fit > bestC){ best = 3; bestC = fit; bp1=p1; bp2=p2; bp3=0.0; }

	fit = - mle_power_law_val(distrib, 1, &p1);
	if (fit > bestC){ best = 4; bestC = fit; bp1=p1; bp2=0.0; bp3=0.0; }

	fit = - mle_lognormal_val(distrib, &p1, &p2);
	if (fit > bestC){ best = 5; bestC = fit; bp1=p1; bp2=p2; bp3=0.0; }

	fit = - mle_stretched_exponential_val(distrib, &p1, &p2);
	if (fit > bestC){ best = 6; bestC = fit; bp1=p1; bp2=p2; bp3=0.0; }

	param1 = bp1; param2 = bp2; param3 = fit/*bp3*/;
	return best;
}
