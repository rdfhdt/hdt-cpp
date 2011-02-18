/*
 * ClusterAlgorithm1.cpp
 *
 *  Created on: 18/02/2011
 *      Author: mck
 */

#include "Triples.h"

#include "ClusterAlgorithm1.h"


ClusterAlgorithm1::ClusterAlgorithm1() {
	triples = NULL;
	dictionary = NULL;
}

void ClusterAlgorithm1::startClustering(Triples *triples) {
	this->triples = triples;
	this->dictionary = triples->getDictionary();

	dictionary->beginClustering();

	new_shared = 1;
	new_subject = dictionary->getSsubobj() + 1;

	if (dictionary->getMapping() == MAPPING1)
		new_object = dictionary->getNsubjects() + 1;
	else
		new_object = dictionary->getSsubobj() + 1;

	repeated = false;
	x = 0, y = 0, z = 0;
	currentIteration=0;
}

bool ClusterAlgorithm1::doClusteringIteration(unsigned int numiter) {

	if(!triples) {
		cout << "Inactive" << endl;
		return false;
	}

	vector < TripleID > &graph = triples->getGraph();

	unsigned int first = currentIteration;
	unsigned int last = currentIteration+numiter;

	if(numiter==0 || currentIteration>=graph.size()) {
		last = graph.size()-1;
	}

	for (unsigned int i = first; i<last && i<graph.size(); i++, currentIteration++) {
		//cout << "Process: " << i << "/"<<graph.size() << "   " << graph[i].x << ", " << graph[i].y << ", "<< graph[i].z << ", "<< endl; cout.flush();

		if (x == graph[i].x) {
			if (y == graph[i].y) {
				if (z == graph[i].z) {
					repeated = true;

					graph[i].x = previous.x;
					graph[i].y = previous.y;
					graph[i].z = previous.z;
				} else {
					z = graph[i].z;
				}

				graph[i].y = previous.y;
			} else {
				y = graph[i].y;
				z = graph[i].z;

				// Subject change: ckeck if it clusterized
				unsigned int new_id = dictionary->isClusterized(graph[i].y,
						VOC_SUBJECT);

				if (new_id == 0) {
					// It is not previously clusterized
					if (graph[i].y <= dictionary->getSsubobj()) {
						dictionary->updateID(graph[i].y, new_shared,
								VOC_SUBJECT);
						graph[i].y = new_shared;
						new_shared++;
					} else {
						dictionary->updateID(graph[i].y, new_subject,
								VOC_SUBJECT);
						graph[i].y = new_subject;
						new_subject++;
					}
				} else {
					graph[i].y = new_id;
				}
			}
		} else {
			x = graph[i].x;
			y = graph[i].y;
			z = graph[i].z;

			// Subject change: ckeck if it clusterized
			unsigned int new_id = dictionary->isClusterized(graph[i].y,
					VOC_SUBJECT);

			if (new_id == 0) {
				if (graph[i].y <= dictionary->getSsubobj()) {
					dictionary->updateID(graph[i].y, new_shared, VOC_SUBJECT);
					graph[i].y = new_shared;
					new_shared++;
				} else {
					dictionary->updateID(graph[i].y, new_subject, VOC_SUBJECT);
					graph[i].y = new_subject;
					new_subject++;
				}
			} else {
				graph[i].y = new_id;
			}
		}

		if (repeated == false) {
			// Object change: check if it clusterized
			unsigned int new_id = dictionary->isClusterized(graph[i].z,
					VOC_OBJECT);

			if (new_id == 0) {
				if (graph[i].z <= dictionary->getSsubobj()) {
					dictionary->updateID(graph[i].z, new_shared, VOC_OBJECT);
					graph[i].z = new_shared;
					new_shared++;
				} else {
					dictionary->updateID(graph[i].z, new_object, VOC_OBJECT);
					graph[i].z = new_object;
					new_object++;
				}
			} else {
				graph[i].z = new_id;
			}
		} else {
			repeated = false;
		}

		previous = graph[i];
	}

	if(currentIteration>=graph.size()-1) {
		endClustering();
		return false;
	}

	return true;
}

void ClusterAlgorithm1::endClustering() {
	if(isActive()) {

		if(dictionary!=NULL) {
			dictionary->endClustering();
		}
		dictionary = NULL;
		triples = NULL;
	}
}

bool ClusterAlgorithm1::isActive() {
	return (triples!=NULL);
}
