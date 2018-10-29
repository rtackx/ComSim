#include "community.h"

Community::Community(Graph*& main_graph, unsigned int depth_best) : main_graph(main_graph), depth_best(depth_best)
{
	index_community = 0;
}

Community::~Community()
{}

unsigned int Community::detect(Graph*& graph_projection)
{	
	vector<unsigned int> list_index_nodes, list_ending_nodes;
	unordered_set<unsigned int> list_remaining_nodes;
	unsigned int index = 0;
	
	while(index < graph_projection->n)
	{
		if(graph_projection->list_nodes[index]->nb_neighbors == 0)
			list_ending_nodes.push_back(graph_projection->list_nodes[index]->main_index);
		else
			list_index_nodes.push_back(index);

		index++;
	}
	
	if(list_index_nodes.size() > 0)
	{
 		cycles(graph_projection, list_index_nodes, list_remaining_nodes);
		aggregate(graph_projection, list_remaining_nodes);
	}
	
	save_communities(graph_projection);

	return list_remaining_nodes.size() + list_ending_nodes.size();
}

void Community::cycles(Graph*& graph_projection, vector<unsigned int>& list_index_nodes, unordered_set<unsigned int>& list_remaining_nodes)
{
	vector<unsigned int> list_visited_nodes;
	unsigned int current, neighbor, most_similar, max_depth;
	vector<unsigned int>::iterator it_list_visited, it_list_index, it_current;
	vector<unsigned int>::reverse_iterator rit_current;
	float max_similarity;
	vector<unsigned int> list_similarity;
	
	unordered_map<unsigned int, pair<unsigned int, unsigned int>> map_cycle_nodes;
	for(auto& index : list_index_nodes)
	{
		max_depth = depth_best;
		if(depth_best > graph_projection->list_nodes[index]->nb_neighbors)
			max_depth = graph_projection->list_nodes[index]->nb_neighbors;

		map_cycle_nodes[index] = make_pair(0, max_depth);
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

		max_similarity = 0.0;
		for(auto& neighbor : graph_projection->list_nodes[current]->neighbor_weights)
		{
			if(neighbor.second >= max_similarity)
			{
				if(neighbor.second > max_similarity)
					list_similarity.clear();

				max_similarity = neighbor.second;
				list_similarity.push_back(neighbor.first->index);
			}
		}
		if(list_visited_nodes.size() > 1)
		{
			rit_current = list_visited_nodes.rbegin();
			while(rit_current != list_visited_nodes.rend())
			{
				if(list_similarity.size() > 1 && find(list_similarity.begin(), list_similarity.end(), *rit_current) != list_similarity.end())
					list_similarity.erase(remove(list_similarity.begin(), list_similarity.end(), *rit_current));

				++rit_current;
			}
		}

		if(list_similarity.size() > 0)
		{
			most_similar = list_similarity[rand() % list_similarity.size()];

			it_list_visited = find(list_visited_nodes.begin(), list_visited_nodes.end(), most_similar);
			it_list_index = find(list_index_nodes.begin(), list_index_nodes.end(), most_similar);		
		}
		else
		{
			it_list_visited = list_visited_nodes.end();
			it_list_index = list_index_nodes.end();
		}

		
		if(it_list_visited == list_visited_nodes.end() && it_list_index != list_index_nodes.end())
			current = most_similar;
		else
		{
			// Already visited, cycle detected 
			if(it_list_visited != list_visited_nodes.end())
			{
				it_current = it_list_visited;
				while(it_current != list_visited_nodes.end())
				{
					current = *it_current;
					map_community[current].push_back(index_community);
										
					advance(it_current, 1);
					if(it_current != list_visited_nodes.end())
						neighbor = *it_current;
					else
						neighbor = *it_list_visited;

					graph_projection->list_nodes[current]->neighbor_weights.erase(graph_projection->list_nodes[neighbor]);
					graph_projection->list_nodes[neighbor]->neighbor_weights.erase(graph_projection->list_nodes[current]);
				}

				index_community++;

				it_current = it_list_visited;
				while(it_current != list_visited_nodes.end())
				{
					current = *it_current;
					map_cycle_nodes[current].first++;
					if(map_cycle_nodes[current].first == map_cycle_nodes[current].second)
							list_index_nodes.erase(remove(list_index_nodes.begin(), list_index_nodes.end(), current));
					
					++it_current;
				}
			}
			else
			{
				for(auto& current : list_visited_nodes)
				{					
					map_cycle_nodes[current].first++;
					if(map_cycle_nodes[current].first == map_cycle_nodes[current].second)
					{
						if(map_community.find(current) == map_community.end())
							list_remaining_nodes.insert(current);

						list_index_nodes.erase(remove(list_index_nodes.begin(), list_index_nodes.end(), current));
					}
				}
			}
			
			new_node = true;
		}

	} while(list_index_nodes.size() > 0);
}

void Community::aggregate(Graph*& graph_projection, unordered_set<unsigned int>& list_remaining_nodes)
{
	Node* node;
	unsigned int index_com_similar;
	float max_similarity;
	unordered_map<unsigned int, float> map_similar_community;
	unordered_map<unsigned int, vector<unsigned int>> map_community_tmp;
	unordered_map<unsigned int, vector<unsigned int>> map_best_similarity;
	vector<unsigned int> list_similarity;
	
	for(auto& index : list_remaining_nodes)
	{
		node = graph_projection->list_nodes[index];

		max_similarity = 0.0;
		for(auto& neighbor : node->neighbor_weights)
		{
			if(map_community.find(neighbor.first->index) != map_community.end())
			{	
				for(auto& index_com : map_community[neighbor.first->index])
				{
					map_similar_community.emplace(make_pair(index_com, 0.0));
					map_similar_community[index_com] += neighbor.second;
				}
			}
			else
			{
				if(neighbor.second >= max_similarity)
				{
					if(neighbor.second > max_similarity)
						list_similarity.clear();

					max_similarity = neighbor.second;
					list_similarity.push_back(neighbor.first->index);
				}
			}
		}

		max_similarity = 0.0;
		index_com_similar = -1;

		for(auto& e : map_similar_community)
		{
			if(e.second > max_similarity)
			{
				max_similarity = e.second;
				index_com_similar = e.first;
			}
		}
		map_similar_community.clear();
		
		if(index_com_similar == -1)
			map_best_similarity[list_similarity[rand() % list_similarity.size()]].push_back(index);
		else
		{
			map_community_tmp[index].push_back(index_com_similar);
			
			// Add neighors of "index" into community
			if(map_best_similarity.find(index) != map_best_similarity.end())
			{
				for(auto& index_neighbor : map_best_similarity[index])
					map_community_tmp[index_neighbor].push_back(index_com_similar);
			}
		}
	}
	
	// Saving modification
	for(auto& e : map_community_tmp)
	{
		for(auto& index_com : e.second)
			map_community[e.first].push_back(index_com);

		list_remaining_nodes.erase(e.first);
	}
}

void Community::save_communities(Graph*& graph_projection)
{
	for(auto& e : map_community)
	{
		for(auto& index_com : e.second)
			community[index_com].push_back(main_graph->list_nodes[graph_projection->list_nodes[e.first]->main_index]);
	}
}