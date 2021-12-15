#include "projection.h"

Projection::Projection(Graph*& in_graph)
{
	graph_projection = new Graph();
	this->in_graph = in_graph;
	nb_threads = 5;
}

Projection::~Projection()
{
	if(graph_projection)
		delete graph_projection;
}

void* projection_main_thread(void* main_struct_args)
{
	projection_thread_args* main_args = static_cast<projection_thread_args*>(main_struct_args);

	pthread_t thread;
	unsigned int index;

	projection_thread_args args;
	args.p = main_args->p;
	args.map_node = main_args->map_node;
	args.map_neighbor = main_args->map_neighbor;
	args.distance = main_args->distance;

	pthread_mutex_lock(&projection_main_mutex);
	if(main_args->index == main_args->p->in_graph->n)
	{
		pthread_mutex_unlock(&projection_main_mutex);
		return NULL;
	}
	pthread_mutex_unlock(&projection_main_mutex);

	do
	{
		pthread_mutex_lock(&projection_main_mutex);
		args.index = main_args->index;
		main_args->index++;
		pthread_mutex_unlock(&projection_main_mutex);

		pthread_create(&thread, NULL, &compute_projection_thread, &args);
		pthread_join(thread, NULL);

		pthread_mutex_lock(&projection_main_mutex);
		index = main_args->index;
		pthread_mutex_unlock(&projection_main_mutex);

	} while(index < main_args->p->in_graph->n);

	pthread_exit(NULL);
}

void* compute_projection_thread(void* struct_args)
{
	projection_thread_args* args = static_cast<projection_thread_args*>(struct_args);

	pair<unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>::iterator, bool> it_map;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;

	vector<Node*> list_distance_neighbors = args->p->get_neighbors(args->index, args->distance);
	float projection_weight;

	Node* node = new Node();
	node->id = args->p->in_graph->list_nodes[args->index]->id;
	node->main_index = args->p->in_graph->list_nodes[args->index]->main_index;

	it_map = map_neighbor.emplace(args->index, set<pair<unsigned int, float>, comp_pair>());

	for(auto& node_neighbor : list_distance_neighbors)
	{
		projection_weight = args->p->similarity_projection(args->p->in_graph->list_nodes[args->index], node_neighbor);

		if(projection_weight > 0.0)
			it_map.first->second.insert(make_pair(node_neighbor->index, projection_weight));
	}

	pthread_mutex_lock(&compute_projection_mutex);
	args->map_node->emplace(make_pair(args->index, node));
	args->map_neighbor->insert(map_neighbor.begin(), map_neighbor.end());
	pthread_mutex_unlock(&compute_projection_mutex);

	pthread_exit(NULL);
}

void Projection::project(unsigned int distance)
{
	unordered_map<unsigned int, Node*> map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;

	pthread_t threads[nb_threads];
	projection_thread_args main_args;
	main_args.index = 0;
	main_args.p = this;
	main_args.map_node = &map_node;
	main_args.map_neighbor = &map_neighbor;
	main_args.distance = distance;

	unsigned int i;
	for(i=0; i<nb_threads; i++)
		pthread_create(&threads[i], NULL, &projection_main_thread, &main_args);

	for(i=0; i<nb_threads; i++)
		pthread_join(threads[i], NULL);

	graph_projection->create_graph(map_node, map_neighbor);
}

vector<Node*> Projection::get_neighbors(unsigned int& index, unsigned int& distance) const
{
	vector<Node*> list_neighbors, hist_list_neighbors, list_distance_neighbors;
	vector<Node*> neighbors_node;
	unsigned int i, d = 0;
	Node* node = in_graph->list_nodes[index];
	list_neighbors.push_back(node);

	while(d < distance && list_neighbors.size() > 0)
	{
		hist_list_neighbors.insert(hist_list_neighbors.end(), list_neighbors.begin(), list_neighbors.end());
		list_distance_neighbors.clear();

		for(auto& n : list_neighbors)
		{
			neighbors_node.clear();
			for(auto& neighbor : n->neighbor_weights)
				neighbors_node.push_back(neighbor.first);

			list_distance_neighbors.insert(list_distance_neighbors.end(), neighbors_node.begin(), neighbors_node.end());
		}

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

set<Node*> Projection::get_intersection(Node*& node1, Node*& node2) const
{
	vector<Node*> set_node1, set_node2;
	vector<Node*> neighbors_node1, neighbors_node2;

	neighbors_node1.reserve(node1->nb_neighbors);
	neighbors_node2.reserve(node2->nb_neighbors);
	for(auto k : node1->neighbor_weights)
		neighbors_node1.push_back(k.first);
	for(auto k : node2->neighbor_weights)
		neighbors_node2.push_back(k.first);

	set<Node*> intersection;

	sort(neighbors_node1.begin(), neighbors_node1.end());
	sort(neighbors_node2.begin(), neighbors_node2.end());

	set_intersection(neighbors_node1.begin(), neighbors_node1.end(),
					 neighbors_node2.begin(), neighbors_node2.end(),
					 inserter(intersection, intersection.begin()));

	return intersection;
}

/*********************** SIMILARITY MEASURES ***********************/

CommonNeighbors::CommonNeighbors(Graph*& in_graph) : Projection(in_graph)
{}

float CommonNeighbors::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight = 0.0;

	for(auto& neighbor : node1->neighbor_weights)
	{
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += node1->neighbor_weights[neighbor.first];
		}
	}
	for(auto& neighbor : node2->neighbor_weights)
	{
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += node2->neighbor_weights[neighbor.first];
		}
	}

	return projection_weight;
}

JaccardIndex::JaccardIndex(Graph*& in_graph) : Projection(in_graph)
{}

float JaccardIndex::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight, sum_weight, wtemp;
	projection_weight = sum_weight = 0.0;

	for(auto& neighbor : node1->neighbor_weights)
	{
		wtemp = node1->neighbor_weights[neighbor.first];
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += wtemp;
		}

		sum_weight += wtemp;
	}
	for(auto& neighbor : node2->neighbor_weights)
	{
		wtemp = node2->neighbor_weights[neighbor.first];
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += wtemp;
		}

		sum_weight += wtemp;
	}

	return 1.0 * projection_weight / sum_weight;
}

AdamicAdar::AdamicAdar(Graph*& in_graph) : Projection(in_graph)
{}

float AdamicAdar::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight, sum_weight;
	projection_weight = 0.0;

	for(auto& neighbor : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : neighbor->neighbor_weights)
			sum_weight += w.second;
		projection_weight += 1.0 / log2(sum_weight);
	}

	return projection_weight;
}

ResourceAllocator::ResourceAllocator(Graph*& in_graph) : Projection(in_graph)
{}

float ResourceAllocator::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight, sum_weight;
	projection_weight = 0.0;

	for(auto& neighbor : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : neighbor->neighbor_weights)
			sum_weight += w.second;
		projection_weight += 1.0 / sum_weight;
	}

	return projection_weight;
}

LHN1::LHN1(Graph*& in_graph) : Projection(in_graph)
{}

float LHN1::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight, sum_weight1, sum_weight2, wtemp;
	projection_weight = sum_weight1 = sum_weight2 = 0.0;

	for(auto& neighbor : node1->neighbor_weights)
	{
		wtemp = node1->neighbor_weights[neighbor.first];
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += wtemp;
		}

		sum_weight1 += wtemp;
	}
	for(auto& neighbor : node2->neighbor_weights)
	{
		wtemp = node2->neighbor_weights[neighbor.first];
		if(intersection.find(neighbor.first) != intersection.end())
		{
			projection_weight += wtemp;
		}

		sum_weight2 += wtemp;
	}

	return 1.0 * projection_weight / (sum_weight1 * sum_weight2);
}

PA_Neighbor::PA_Neighbor(Graph*& in_graph) : Projection(in_graph)
{}

float PA_Neighbor::similarity_projection(Node*& node1, Node*& node2)
{
	set<Node*> intersection = get_intersection(node1, node2);

	float projection_weight, sum_weight;
	projection_weight = 1.0;

	for(auto& neighbor : intersection)
	{
		sum_weight = 0.0;
		for(auto& w : neighbor->neighbor_weights)
			sum_weight += w.second;
		projection_weight *= sum_weight;
	}

	return projection_weight;
}
