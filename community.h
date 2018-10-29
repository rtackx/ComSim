#ifndef COMMUNITY_H
#define COMMUNITY_H

#include "libs.h"
#include "graph.h"

class Community
{
private:
	Graph* main_graph;
	unsigned int index_community;
	unsigned int depth_best;
public:
	unordered_map<unsigned int, vector<unsigned int>> map_community;
	unordered_map<unsigned int, vector<Node*>> community;

	Community(Graph*&, unsigned int);
	~Community();

	unsigned int detect(Graph*&);
	void cycles(Graph*&, vector<unsigned int>&, unordered_set<unsigned int>&);
	void aggregate(Graph*&, unordered_set<unsigned int>&);
	void save_communities(Graph*&);
};

#endif