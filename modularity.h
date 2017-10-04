#ifndef MOD_H
#define MOD_H

#include "libs.h"
#include "bipartite_graph.h"

template <typename T> class COMP;

template <typename T>
class Modularity
{
public:
	COMP<T>* sim;

	Modularity(COMP<T>*);
	float bimodularity();
	float modularity_weighted(unordered_map<unsigned int, Node*>&);
	float modularity_directed(unordered_map<unsigned int, Node*>&);
	float modularity_directed_weighted(unordered_map<unsigned int, Node*>&);
};

#include "similarity_measure.h"

#endif