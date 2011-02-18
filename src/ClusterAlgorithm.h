/*
 * ClusterAlgorithm.h
 *
 *  Created on: 18/02/2011
 *      Author: mck
 */

#ifndef CLUSTERALGORITHM_H_
#define CLUSTERALGORITHM_H_

#include "Triples.h"

class ClusterAlgorithm {
public:
	virtual void startClustering(Triples *triples) =0;
	virtual bool doClusteringIteration(unsigned int numiter) =0;
	virtual void endClustering() = 0;
	virtual bool isActive() = 0;
};

#endif /* CLUSTERALGORITHM_H_ */
