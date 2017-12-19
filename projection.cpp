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
	unsigned int index, size_list_nodes;
	size_list_nodes = main_args->p->in_graph->size_list_nodes;

	projection_thread_args args;
	args.p = main_args->p;
	args.map_node = main_args->map_node;
	args.map_neighbor = main_args->map_neighbor;
	args.distance = main_args->distance;

	pthread_mutex_lock(&projection_main_mutex);
	if(main_args->index == size_list_nodes)
	{
		pthread_mutex_unlock(&projection_main_mutex);
		return NULL;
	}
	pthread_mutex_unlock(&projection_main_mutex);
	
	do
	{
		pthread_mutex_lock(&projection_main_mutex);
		args.index = main_args->index;
		main_args->index += main_args->p->in_graph->list_nodes[main_args->index]->nb_neighbors + 1;
		pthread_mutex_unlock(&projection_main_mutex);

		pthread_create(&thread, NULL, &compute_projection_thread, &args);
		pthread_join(thread, NULL);

		pthread_mutex_lock(&projection_main_mutex);
		index = main_args->index;
		pthread_mutex_unlock(&projection_main_mutex);


	} while(index < size_list_nodes);
}

void* compute_projection_thread(void* struct_args)
{
	projection_thread_args* args = static_cast<projection_thread_args*>(struct_args);

	pair<unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>::iterator, bool> it_map;
	vector<unsigned int> shuffle;
	unordered_map<unsigned int, Node*> map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;

	vector<Node*> list_distance_neighbors = args->p->get_neighbors(args->index, args->distance);
	float projection_weight;

	Node* node = new Node();
	node->id = args->p->in_graph->list_nodes[args->index]->id;
	node->main_index = args->p->in_graph->list_nodes[args->index]->main_index;

	for(auto& n : list_distance_neighbors)
		shuffle.push_back(n->index);
	random_shuffle(shuffle.begin(), shuffle.end());

	it_map = map_neighbor.emplace(args->index, set<pair<unsigned int, float>, comp_pair>());
	
	for(auto& index_neighbor : shuffle)
	{
		projection_weight = args->p->similarity_projection(args->index, index_neighbor);

		if(projection_weight > 0.0)
			it_map.first->second.insert(make_pair(index_neighbor, projection_weight));
	}

	pthread_mutex_lock(&compute_projection_mutex);
	args->map_node->emplace(make_pair(args->index, node));
	args->map_neighbor->insert(map_neighbor.begin(), map_neighbor.end());
	pthread_mutex_unlock(&compute_projection_mutex);
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

set<Node*> Projection::get_intersection(Node*& node1, Node*& node2) const
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

CommonNeighbors::CommonNeighbors(Graph*& in_graph) : Projection(in_graph)
{}

float CommonNeighbors::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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

JaccardIndex::JaccardIndex(Graph*& in_graph) : Projection(in_graph)
{}

float JaccardIndex::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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

AdamicAdar::AdamicAdar(Graph*& in_graph) : Projection(in_graph)
{}

float AdamicAdar::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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

ResourceAllocator::ResourceAllocator(Graph*& in_graph) : Projection(in_graph)
{}

float ResourceAllocator::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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

LHN1::LHN1(Graph*& in_graph) : Projection(in_graph)
{}

float LHN1::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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

PA_Neighbor::PA_Neighbor(Graph*& in_graph) : Projection(in_graph)
{}

float PA_Neighbor::similarity_projection(unsigned int& index1, unsigned int& index2)
{
	Node* node1 = in_graph->list_nodes[index1];
	Node* node2 = in_graph->list_nodes[index2];

	set<Node*> intersection = get_intersection(node1, node2);
	
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
