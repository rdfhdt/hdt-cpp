/*
 * ClusterAlgorithm1.h
 *
 *  Created on: 18/02/2011
 *      Author: mck
 */

#ifndef CLUSTERALGORITHM1_H_
#define CLUSTERALGORITHM1_H_

#include "Triples.h"
#include "ClusterAlgorithm.h"

class ClusterAlgorithm1: public ClusterAlgorithm {

private:
	Triples *triples;
	Dictionary *dictionary;

	unsigned int new_shared, new_subject, new_object;
	bool repeated;
	unsigned int x, y, z;
	TripleID previous;
	unsigned int currentIteration;

public:
	ClusterAlgorithm1();
	void startClustering(Triples *triples);
	bool doClusteringIteration(unsigned int numiter);
	void endClustering();
	bool isActive();
};

#endif /* CLUSTERALGORITHM1_H_ */
