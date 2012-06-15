//
//  Copyright 2012 Alin Dobra and Christopher Jermaine
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#ifndef _T_HISTOGRAM_GLA_H
#define _T_HISTOGRAM_GLA_H

/* Info for meta-GLAs
   m4_define(</THistogramGLA_INPUT/>, </(v, INT)/>)dnl
   m4_define(</THistogramGLA_OUTPUT/>, </(fit, INT), (param1, DOUBLE), (param2, DOUBLE), (param3, DOUBLE)/>)dnl
 */

#include <vector>
#include "DistributionFitting.h"
#include "stdlib.h"

using namespace std;

// function to compute the second from the packed time (mmddhhmmss) 
inline int ExtractSecond(int x){
	int sec = x-(x/100)*100;
	x /=100;
	int min= x-(x/100)*100;
	x /=100;
	int hour = x-(x/100)*100;
	return sec+60*min+3600*hour;
}

inline int ExtractMinute(int x){
	x /=100;
	int min= x-(x/100)*100;
	x /=100;
	int hour = x-(x/100)*100;
	return min+60*hour;
}


/** This class implements a specialized time histogram.
		
		The input is a point in time between 0 and M-1. The number of
		events in each time slot is counted. At the end of the process,
		the counts in each point are placed in a histogram, that is
		further fitted with parametric distributions.

*/

class THistogramGLA {
	int N; // size of initial counters
	int* counters; // the counters maintained
	int M; // size of histogram computed
	const char* file; // file where to place the histogram
	// if null, no output

	// compute the histogram, space is allocated, should be dealocated by callee
	bool ComputeHistogram(vector<double>& distrib){
		int total = 0;

		// find max counter
		int max=0;
		for (int i=0; i<N; i++){
			if (counters[i]>max)
				max = counters[i];
		}
		
		if (max==0)
			return false; // everything is 0

		// max should fall in last bucket
		float bkSize = 1.0*max/M;
		if (bkSize<1.0)
			bkSize = 1.0;

		for (int i=0; i<N; i++){
			int bk = counters[i]/bkSize;
			distrib[ bk ]+=1.0;
			total+=1.0;
		}
		if (file!=NULL){
			FILE* fd = fopen(file, "w");
			for (int i=0; i<M; i++){
				fprintf(fd, "%d\t%e\n", i, distrib[i]);
			}
			fclose(fd);
		}
		if (total == 0) 
			return false;
		for (int i=0; i<M; i++){
			distrib[i]/=total;
		}
		return true;
	}

public:
	/** Constructor
			
			N: size of the maintained counters. Only values 0-(N-1) are counted
			M: number of buckets for the histogram. if counter[i]=x, element x is incremented
			   in the histogram. Counts over are discarded
	*/
 THistogramGLA():N(1440), M(300), file(NULL), counters(NULL){}

	/* constructor that is used when data needs to be printed 
	   in a file */
 THistogramGLA(const char* _file, int _N, int _M)
	 :file(_file), N(_N), M(_M), counters(NULL){}

	void AddItem(int v){ 
		if (counters == NULL){// delayed initialization so copy constructor copies NULL
			counters = (int*) malloc(sizeof(int)*N);
			for (int i=0; i<N; i++) 
				counters[i]=0;
		}
		if (v>0 && v<N) counters[v]++; 
	}
	void AddState(THistogramGLA& o){
		for (int i=0; i<N; i++)
			counters[i]+=o.counters[i];
	}

	/* curve fitting interface. 
		 fit parameter=0 if bad fit, 1,2,3 for other fits. see DistributionFitting
		 
		 param1, param2, param3 are the parameters (up to three)
	 */
	void GetResult(int& fit, double& param1, double& param2, double& param3){
		vector<double> distrib(M,0.0);
		if (!ComputeHistogram(distrib)){
			fit = 0;
			param1=param2=param3=0.0;
			return; // no fit possible
		}

		fit = FitDistribution(distrib, param1, param2, param3);
	}

	~THistogramGLA(void){ free(counters); counters=NULL; }
};


#endif // _T_HISTOGRAM_GLA_H
