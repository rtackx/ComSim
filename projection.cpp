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
	vector<Node*> list_distance_neighbors;
	float projection_weight;
	Node* node;
	
	unordered_map<unsigned int, Node*> map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;
	pair<unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>::iterator, bool> it_map;
	vector<unsigned int> shuffle;
	
	index = 0;
	while(index < in_graph->size_list_nodes)
	{
		list_distance_neighbors = get_neighbors(in_graph, index, distance);

		node = new Node();
		node->id = in_graph->list_nodes[index]->id;
		node->main_index = in_graph->list_nodes[index]->main_index;
		map_node[index] = node;

		for(auto& n : list_distance_neighbors)
			shuffle.push_back(n->index);
		random_shuffle(shuffle.begin(), shuffle.end());

		it_map = map_neighbor.emplace(index, set<pair<unsigned int, float>, comp_pair>());

		for(auto& index_neighbor : shuffle)
		{
			projection_weight = similarity_projection(in_graph, index, index_neighbor);

			if(projection_weight > 0.0)
				it_map.first->second.insert(make_pair(index_neighbor, projection_weight));
		}
		
		index += in_graph->list_nodes[index]->nb_neighbors + 1;
		shuffle.clear();
	}

	graph_projection->create_graph(map_node, map_neighbor);
}

vector<Node*> Projection::get_neighbors(Graph*& in_graph, unsigned int index, unsigned int distance) const
{
	vector<Node*> list_neighbors, hist_list_neighbors, list_distance_neighbors;
	unsigned int i, d = 0;
	Node* node = in_graph->list_nodes[index];
	list_neighbors.push_back(node);

	while(d < distance && list_neighbors.size() > 0)
	{
		hist_list_neighbors.insert(hist_list_neighbors.end(), list_neighbors.begin(), list_neighbors.end());
		list_distance_neighbors.clear();

		for(auto& n : list_neighbors)
			list_distance_neighbors.insert(list_distance_neighbors.end(), in_graph->list_nodes+(n->index+1), in_graph->list_nodes+(n->index+n->nb_neighbors+1));

		sort(list_distance_neighbors.begin(), list_distance_neighbors.end());
		sort(hist_list_neighbors.begin(), hist_list_neighbors.end());
		list_distance_neighbors.erase(unique(list_distance_neighbors.begin(), list_distance_neighbors.end()), list_distance_neighbors.end());
		list_neighbors.clear();
		
		set_difference(list_distance_neighbors.begin(), list_distance_neighbors.end(),
					   hist_list_neighbors.begin(), hist_list_neighbors.end(),
					   back_inserter(list_neighbors));

		d++;
	}

	return list_neighbors;
}

set<Node*> Projection::get_intersection(Graph*& in_graph, Node* node1, Node* node2) const
{
	vector<Node*> set_node1, set_node2;
	set<Node*> intersection;

	set_node1.insert(set_node1.end(), in_graph->list_nodes+(node1->index+1), in_graph->list_nodes+(node1->index+node1->nb_neighbors+1));
	set_node2.insert(set_node2.end(), in_graph->list_nodes+(node2->index+1), in_graph->list_nodes+(node2->index+node2->nb_neighbors+1));
	
	sort(set_node1.begin(), set_node1.end());
	sort(set_node2.begin(), set_node2.end());

	set_intersection(set_node1.begin(), set_node1.end(),
					 set_node2.begin(), set_node2.end(),
					 inserter(intersection, intersection.begin()));

	return intersection;
}

/*********************** SIMILARITY MEASURES ***********************/

CommonNeighbors::CommonNeighbors() : Projection()
{}

float CommonNeighbors::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
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

JaccardIndex::JaccardIndex() : Projection()
{}

float JaccardIndex::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
	float projection_weight, sum_weight;
	projection_weight = sum_weight = 0.0;
	unsigned int index;
	
	index = 0;
	while(index < node1->nb_neighbors)
	{
		if(intersection.find(in_graph->list_nodes[index1+index+1]) != intersection.end())
			projection_weight += node1->neighbor_weights[index];

		sum_weight += node1->neighbor_weights[index];

		index++;
	}

	index = 0;
	while(index < node2->nb_neighbors)
	{
		if(intersection.find(in_graph->list_nodes[index2+index+1]) != intersection.end())
			projection_weight += node2->neighbor_weights[index];

		sum_weight += node2->neighbor_weights[index];

		index++;
	}

	return 1.0 * projection_weight / sum_weight;
}

AdamicAdar::AdamicAdar() : Projection()
{}

float AdamicAdar::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
	float projection_weight, sum_weight;
	projection_weight = 0.0;

	for(auto& n : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : n->neighbor_weights)
			sum_weight += w;
		projection_weight += 1.0 / log2(sum_weight);
	}

	return projection_weight;
}

ResourceAllocator::ResourceAllocator() : Projection()
{}

float ResourceAllocator::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
	float projection_weight, sum_weight;
	projection_weight = 0.0;

	for(auto& n : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : n->neighbor_weights)
			sum_weight += w;
		projection_weight += 1.0 / sum_weight;
	}

	return projection_weight;
}

LHN1::LHN1() : Projection()
{}

float LHN1::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
	float projection_weight, sum_weight1, sum_weight2;
	projection_weight = sum_weight1 = sum_weight2 = 0.0;
	unsigned int index;
	
	index = 0;
	while(index < node1->nb_neighbors)
	{
		if(intersection.find(in_graph->list_nodes[index1+index+1]) != intersection.end())
			projection_weight += node1->neighbor_weights[index];

		sum_weight1 += node1->neighbor_weights[index];

		index++;
	}

	index = 0;
	while(index < node2->nb_neighbors)
	{
		if(intersection.find(in_graph->list_nodes[index2+index+1]) != intersection.end())
			projection_weight += node2->neighbor_weights[index];

		sum_weight2 += node2->neighbor_weights[index];

		index++;
	}

	return 1.0 * projection_weight / (sum_weight1 * sum_weight2);
}

PA_Neighbor::PA_Neighbor() : Projection()
{}

float PA_Neighbor::similarity_projection(Graph*& in_graph, unsigned int index1, unsigned int index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(in_graph, node1, node2);
	
	float projection_weight, sum_weight;
	projection_weight = 1.0;

	for(auto& n : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : n->neighbor_weights)
			sum_weight += w;
		projection_weight *= sum_weight;
	}

	return projection_weight;
}
