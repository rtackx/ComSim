#include "community.h"

Community::Community(Graph*& main_graph, unsigned int depth_best) : main_graph(main_graph), depth_best(depth_best)
{
	index_community = 0;
}

Community::~Community()
{
	for(auto& e : map_similar_community)
		delete e.second;
}

void Community::detect(Graph*& graph_projection, vector<unsigned int>& list_ending_nodes)
{	
	vector<unsigned int> list_index_nodes;
	unordered_set<unsigned int> list_remaining_nodes;
	unsigned int index = 0;
	
	while(index < graph_projection->size_list_nodes)
	{
		if(graph_projection->list_nodes[index]->nb_neighbors == 0)
			list_ending_nodes.push_back(graph_projection->list_nodes[index]->main_index);
		else
			list_index_nodes.push_back(index);

		index += graph_projection->list_nodes[index]->nb_neighbors + 1;
	}
	
	if(list_index_nodes.size() > 0)
	{
 		cycles(graph_projection, list_index_nodes, list_remaining_nodes);
		aggregate(graph_projection, list_remaining_nodes, list_ending_nodes);
	}

	save_communities(graph_projection);
	map_community.clear();
}

void Community::cycles(Graph*& graph_projection, vector<unsigned int>& list_index_nodes, unordered_set<unsigned int>& list_remaining_nodes)
{
	vector<unsigned int> list_visited_nodes;	

	unsigned int current, current_tmp, most_similar;
	vector<unsigned int>::iterator it_current, it_list_visited, it_list_index;
	vector<unsigned int>* vec_depth;

	unordered_map<unsigned int, vector<unsigned int>*> map_cycle_nodes;
	for(auto& index : list_index_nodes)
	{
		map_cycle_nodes[index] = new vector<unsigned int>;
		vec_depth = map_cycle_nodes[index];

		if(depth_best > graph_projection->list_nodes[index]->nb_neighbors)
			vec_depth->push_back(graph_projection->list_nodes[index]->nb_neighbors);
		else
			vec_depth->push_back(depth_best);

		vec_depth->push_back(0);
	}

	int i = rand() % list_index_nodes.size();
	bool new_node = true;

	do 
	{
		if(new_node)
		{
			list_visited_nodes.clear();
			i = rand() % list_index_nodes.size();
			current = list_index_nodes[i];
			new_node = false;
		}
		
		list_visited_nodes.push_back(current);

		most_similar = graph_projection->list_nodes[current+map_cycle_nodes[current]->at(1)+1]->index;

		it_list_visited = find(list_visited_nodes.begin(), list_visited_nodes.end(), most_similar);
		it_list_index = find(list_index_nodes.begin(), list_index_nodes.end(), most_similar);

		if(it_list_visited == list_visited_nodes.end() && it_list_index != list_index_nodes.end())
			current = most_similar;
		else
		{
			if(it_list_visited != list_visited_nodes.end())
			{
				while(it_list_visited != list_visited_nodes.end())
				{
					map_community[*it_list_visited].push_back(index_community);
					++it_list_visited;
				}

				index_community++;
			}

			for(auto& index : list_visited_nodes)
			{
				vec_depth = map_cycle_nodes[index];
				vec_depth->at(1)++;

				if(vec_depth->at(1) == vec_depth->at(0))
				{
					if(map_community.find(index) == map_community.end())
					 	list_remaining_nodes.insert(index);

					list_index_nodes.erase(remove(list_index_nodes.begin(), list_index_nodes.end(), index));
				}
			}

			new_node = true;
		}

	} while(list_index_nodes.size() > 0);

	for(auto& index : list_index_nodes)
		delete map_cycle_nodes[index];
}

void Community::aggregate(Graph*& graph_projection, unordered_set<unsigned int>& list_remaining_nodes, vector<unsigned int>& list_ending_nodes)
{
	unordered_map<unsigned int, float>* map_similar_community_tmp;
	vector<pair<unsigned int, unsigned int>> map_community_tmp;
	Node* node;
	unsigned int index_neighbor, i;
	float max;
	int most_similar_com;

	for(auto& index : list_remaining_nodes)
	{
		node = graph_projection->list_nodes[index];

		if(map_similar_community.find(node->main_index) == map_similar_community.end())
			map_similar_community[node->main_index] = new unordered_map<unsigned int, float>();
		map_similar_community_tmp = map_similar_community[node->main_index];

		for(i=0; i<node->nb_neighbors; i++)
		{
			if(map_community.find(graph_projection->list_nodes[index+i+1]->index) == map_community.end())
				continue;

			for(auto& index_com : map_community[graph_projection->list_nodes[index+i+1]->index])
			{
				map_similar_community_tmp->emplace(make_pair(index_com, 0.0));
				/*if(map_similar_community_tmp->find(index_com) == map_similar_community_tmp->end())
				 	map_similar_community_tmp->insert(make_pair(index_com, 0.0));*/
				map_similar_community_tmp->at(index_com) += node->neighbor_weights[i];
			}
		}

		max = 0.0;
		most_similar_com = -1;

		for(auto& e : *map_similar_community_tmp)
		{
			if(e.second > max)
			{
				max = e.second;
				most_similar_com = e.first;
			}
		}
		
		if(most_similar_com == -1)
			list_ending_nodes.push_back(graph_projection->list_nodes[index]->main_index);
		else
			map_community_tmp.push_back(make_pair(index, most_similar_com));
	}
	
	for(auto& e : map_community_tmp)
		map_community[e.first].push_back(e.second);
}

void Community::save_communities(Graph*& graph_projection)
{
	for(auto& e : map_community)
	{
		for(auto& index_com : e.second)
			community[index_com].push_back(main_graph->list_nodes[graph_projection->list_nodes[e.first]->main_index]);
	}
}