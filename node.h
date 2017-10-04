#ifndef NODE_H
#define NODE_H

#include "libs.h"

class Node
{
public:
	unsigned int index;
	string id;
	set<Node*> list_neighbor;
	vector<unsigned int> list_community;
	unordered_map<unsigned int, float> list_weights;
	float sum_weights;
	
	Node(unsigned int, string);
	~Node();
	void add_neighbor(Node*, float);
	//bool same_community(Node* node) const;
};

struct comp_node
{
	inline bool operator()(Node* const& n1, Node* const& n2) const
	{
		return n1->index < n2->index;
	}
};

#endif