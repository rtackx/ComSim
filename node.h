#ifndef NODE_H
#define NODE_H

#include "libs.h"

class Node
{
public:
	unsigned int index;
	int main_index;
	string id;

	unsigned int nb_neighbors;
	unordered_map<Node*, float> neighbor_weights;
	
	Node();
	~Node();
};

struct comp_node
{
	inline bool operator()(Node* const& n1, Node* const& n2) const
	{
		return n1->index > n2->index;
	}
};

#endif