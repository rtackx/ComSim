#ifndef GRAPH_H
#define GRAPH_H

#include "libs.h"
#include "node.h"

class Graph
{
public:
	unsigned int n;
	unsigned int m;

	Node** list_nodes;

	Graph();
	~Graph();
	void load_graph(string);
	void create_graph(unordered_map<unsigned int, Node*>&, unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>&);
	Graph* get_subgraph(vector<unsigned int>&) const;
};

#endif