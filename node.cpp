#include "node.h"

Node::Node(unsigned int index, string id)
{
	this->index = index;
	this->id = id;
	sum_weights = 0.0;
}

Node::~Node()
{}

void Node::add_neighbor(Node* node_neighbor, float weight)
{
	//if(list_neighbor.find(node_neighbor) != list_neighbor.end())
	//	return;

	list_neighbor.insert(node_neighbor);
	list_weights[node_neighbor->index] = weight;
	sum_weights += weight;
}

/*bool Node::same_community(Node* node) const
{
	for(auto& e : node->list_community)
	{
		if(list_community.find(e) != list_community.end())
			return true;
	}

	return false;
}*/