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
#ifndef _KMEANS_GLA_H_
#define _KMEANS_GLA_H_

#include <vector>
#include <math.h>

#include "GLA.h"
#include "Archive.h"
#include "DataTypes.h"

using namespace std;


#define pow2(x) ((x)*(x))
#define DOUBLE_ARRAY(_name, _size) DOUBLE _name[_size]

// function to compute the squared distance between two tuples
inline double Distance(DOUBLE& t1, DOUBLE& t2) {
	double theDistance = pow2((double)t1 - (double)t2);
	theDistance = sqrt(theDistance);

	return theDistance;
}

// InTuple: vector of Type (float, double, etc)
// OutTuple: each cluster is an output tuple (vector of Type)

/** Class to implement k-means clustering. The existing clusters are
	*	accessible through a static member (initialized in a static variable).

	*	The state consists in enough information to allow the computation of
	*	future clusters.
**/

template<
	int K // number of clusters
>
class GLA_KMeans : public GLA {
private:
	/** Auxiliary types to implement kmeans */

	// the statistics we maintain
	struct ClusterStatistics {
		int count; // number of points in this cluster
		double Sum; // the sum on each coordinate of this cluster
		double Means;

		// initialize the cluster
		ClusterStatistics() {
			count = 0;
			Sum = 0;
			Means = 0;
		}

		// add a new tuple
		void AddItem(DOUBLE& t) {
			count++;
			Sum += t;
		}

		// add state from another cluster
		void AddState(ClusterStatistics& other) {
			count += other.count;
			Sum += other.Sum;
		}

		// finalize the state
		void Finalize() {
			if (count > 0) Means = Sum / count;
		}

		// extract the cluster center
		void GetClusterCenter(DOUBLE& where) {
			where = Means;
		}
	};

	// the clusters and their corresponding statistics
	DOUBLE clusters[K];
	ClusterStatistics clusterStats[K];

	// the total cost of the clustering using these centers
	DOUBLE totalScore;

	// indicator for extracting the results
	bool done;

public:
	// constructor & destructor
	GLA_KMeans() : done(false) {
		for (int k = 0; k < K; k++) {
			clusters[k] = 0.0;
		}

		totalScore = 0.0;
	}

	GLA_KMeans(DOUBLE initialCenters[K]) : done(false) {
		for (int k = 0; k < K; k++) {
			clusters[k] = initialCenters[k];
		}

		totalScore = 0.0;
	}

	~GLA_KMeans() {}

	// GLA interface
	void AddItem(FLOAT& t) {
		DOUBLE tt = t;

		// find the closest cluster
		int minC = 0; // which cluster, first for now
		double minDist = Distance(clusters[0], tt);

		for (int i = 1; i < K; i++) {
			double candDist = Distance(clusters[i], tt);
			if (candDist < minDist) {
				minC = i;
				minDist = candDist;
			}
		}

		// update the cluster that gets the point
		clusterStats[minC].AddItem(tt);
		totalScore += minDist;
	}

	void AddState(GLA_KMeans& other) {
		for (int i = 0; i < K; i++) {
			clusterStats[i].AddState(other.clusterStats[i]);
		}

		totalScore += other.totalScore;
	}

	void Finalize() {
		done = false;

		for (int i = 0; i < K; i++) clusterStats[i].Finalize();
	}

	bool GetNext(DOUBLE newCenters[K], DOUBLE& score) {
		if (done == false) {
			for (int i = 0; i < K; i++) clusterStats[i].GetClusterCenter(newCenters[i]);
			score = totalScore;

			done = true;
			return true;
		}
		else {
			return false;
		}
	}

	ARCHIVER_SIMPLE_DEFINITION()
};

#endif // _KMEANS_GLA_H_
