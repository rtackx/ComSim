#include "projection.h"

Projection::Projection()
{
	graph_projection = new Graph();
}

Projection::~Projection()
{
	if(graph_projection)
		delete graph_projection;
}

void Projection::project(Graph*& in_graph, unsigned int distance)
{
	unsigned int d, index;
	set<Node*, comp_node> list_distance_neighbors;
	float projection_weight;
	
	unordered_map<unsigned int, Node*> map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;
	vector<unsigned int> shuffle;

	index = 0;
	while(index < in_graph->size_list_nodes)
	{
		list_distance_neighbors = get_neighbors(in_graph, index, distance);

		map_node[index] = new Node();
		map_node[index]->id = in_graph->list_nodes[index]->id;
		map_node[index]->main_index = in_graph->list_nodes[index]->main_index;
		
		for(auto& n : list_distance_neighbors)
			shuffle.push_back(n->index);
		random_shuffle(shuffle.begin(), shuffle.end());

		for(auto& index_neighbor : shuffle)
		{
			projection_weight = similarity_projection(in_graph, index, index_neighbor);

			if(projection_weight > 0.0)
				map_neighbor[index].insert(make_pair(index_neighbor, projection_weight));
		}
		
		index += in_graph->list_nodes[index]->nb_neighbors + 1;
		shuffle.clear();
	}

	graph_projection->create_graph(map_node, map_neighbor);
}

set<Node*, comp_node> Projection::get_neighbors(Graph*& in_graph, unsigned int index, unsigned int distance) const
{
	set<Node*, comp_node> list_neighbors, hist_list_neighbors, list_distance_neighbors;
	unsigned int i, d = 0;	
	Node* node = in_graph->list_nodes[index];
	list_neighbors.insert(node);

	while(d < distance && list_neighbors.size() > 0)
	{
		hist_list_neighbors.insert(list_neighbors.begin(), list_neighbors.end());
		list_distance_neighbors.clear();

		for(auto& n : list_neighbors)
		{
			for(i=0; i<n->nb_neighbors; i++)
				list_distance_neighbors.insert(in_graph->list_nodes[n->index+i+1]);
		}
		
		for(auto& n : hist_list_neighbors)
			list_distance_neighbors.erase(n);
		list_distance_neighbors.erase(node);

		list_neighbors = list_distance_neighbors;

		d++;
	}

	return list_distance_neighbors;
}

/*********************** SIMILARITY MEASURES ***********************/

CommonNeighbors::CommonNeighbors() : Projection()
{}

float CommonNeighbors::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*, comp_node> intersection;
	set<Node*> set_node1, set_node2;
	set_node1.insert(in_graph->list_nodes+(node1->index+1), in_graph->list_nodes+(node1->index+node1->nb_neighbors+1));
	set_node2.insert(in_graph->list_nodes+(node2->index+1), in_graph->list_nodes+(node2->index+node2->nb_neighbors+1));

	set_intersection(set_node1.begin(), set_node1.end(),
					 set_node2.begin(), set_node2.end(),
					 inserter(intersection, intersection.begin()));

	float projection_weight = 0.0;
	unsigned int cpt, index;

	cpt = index = 0;
	while(index < node1->nb_neighbors && cpt < intersection.size())
	{
		if(intersection.find(in_graph->list_nodes[index1+index+1]) != intersection.end())
		{
			projection_weight += node1->neighbor_weights[index];
			cpt++;
		}
		index++;
	}

	cpt = index = 0;
	while(index < node2->nb_neighbors && cpt < intersection.size())
	{
		if(intersection.find(in_graph->list_nodes[index2+index+1]) != intersection.end())
		{
			projection_weight += node2->neighbor_weights[index];
			cpt++;
		}
		index++;
	}

	return projection_weight;
}