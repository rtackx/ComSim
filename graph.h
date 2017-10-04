#ifndef GRAPH_H
#define GRAPH_H

#include "libs.h"
#include "node.h"
#include "partition.h"

class Graph
{
public:
	unsigned int n;
	unsigned int m;

	unordered_map<unsigned int, Node*> list_node;
	
	Graph();
	~Graph();
	void load_graph(string);	
	Graph* get_subgraph(set<unsigned int>);
};

#endif