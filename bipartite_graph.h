#ifndef BIPGRAPH_H
#define BIPGRAPH_H

#include "libs.h"
#include "node.h"
#include "graph.h"

class Bipgraph : public Graph
{
public:
	unsigned int n_top;
	unsigned int n_bot;

	unordered_map<unsigned int, Node*> list_top_node;
	unordered_map<unsigned int, Node*> list_bot_node;

	Bipgraph();
	~Bipgraph();
	void load_bip(string);
	void load_bip_adj(string);
	//Graph* convert_community_to_graph() const;
	//void remove_duplicate_community();
	//void save_community_reverse(const char*);
	//void save_community_graph(Graph*, string) const;
	Bipgraph* get_subgraph(set<unsigned int>, bool);
};

#endif