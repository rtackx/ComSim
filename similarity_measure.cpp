#include "similarity_measure.h"

template <class T>
COMP<T>::COMP(string directory) : SIM()
{
	start_num_community = 0;
	//mod = new Modularity<T>(this);
}

template <class T>
COMP<T>::~COMP()
{
	//delete mod;
}

/*template <class T>
void COMP<T>::display_modularity(unordered_map<unsigned int, Node*>& list_node)
{
	printf("\tModularity of projection (weighted) : %f\n", mod->modularity_weighted(list_node));
	printf("\tModularity of projection (directed) : %f\n", mod->modularity_directed(list_node));
	printf("\tModularity of projection (directed and weighted) : %f\n", mod->modularity_directed_weighted(list_node));
}*/

template <class T>
void COMP<T>::clean()
{
	mapT.clear();
	map_com.clear();
	in_weight.clear();
	out_weight.clear();
	in_degree.clear();
	out_degree.clear();
}

template <class T>
void COMP<T>::save_projection(Graph* graph, string filename)
{
	ofstream f;
	//string path = save->get_path(filename);
	f.open(filename);

	for(auto& e : mapT)
	{
		for(auto& d : e.second)
		{
			f << graph->list_node[e.first]->id << " " << graph->list_node[d.first]->id << " " << d.second << endl;
		}
	}

	f.close();
}

/*template <class T>
void COMP<T>::merge_community(unordered_map<unsigned int, Node*>& list_node, float peripheric_threshold, bool top)
{
	unsigned int best_sim;
	bool improve;
	vector<unsigned int> list_element_rand;

	unordered_map<unsigned int, set<unsigned int>> community;
	for(auto& e : map_community_overlapping)
		community[e.first].insert(0);

	int nb_merge = 0;

	//save_state_partition(list_node, community, nb_merge, top);

	do
	{
		improve = false;
		//list_element_rand = random_map_element(list_best_similar);
		list_element_rand = sort_desc_map_element(list_best_similar);
    	for(int i=0; i<list_element_rand.size(); i++)
    	{
    		best_sim = list_best_similar[list_element_rand[i]];
    		if(map_community_overlapping[list_element_rand[i]][0] != map_community_overlapping[best_sim][0])
    		{
    			community[map_community_overlapping[best_sim][0]].insert(list_element_rand[i]);
				community[map_community_overlapping[list_element_rand[i]][0]].erase(list_element_rand[i]);
				if(community[map_community_overlapping[list_element_rand[i]][0]].size() == 0)
					community.erase(map_community_overlapping[list_element_rand[i]][0]);

    			map_community_overlapping[list_element_rand[i]][0] = map_community_overlapping[best_sim][0];
				improve = true;

				nb_merge++;
				//save_state_partition(list_node, community, nb_merge, top);
    		}
    	}
	} while(improve);

	// OVERLAPPING COMMUNITY //
	if(peripheric_threshold < 1.0)
	{
		float max_peripheric;

		for(auto& e : mapT)
		{
			for(auto& d : e.second)
			{
				max_peripheric = 1.0 * d.second / list_best_sim_value[e.first];
				if(d.first != list_best_similar[e.first] && max_peripheric >= peripheric_threshold)
				{
					map_community_overlapping[e.first].push_back(map_community_overlapping[d.first][0]);
				}
			}
		}
	}
}*/

/*template <class T>
void COMP<T>::save_state_partition(Graph* graph, unordered_map<unsigned int, unsigned int>* community, int state)
{
	ofstream file_evolution;
	string com_evolution;
	
	com_evolution += "evolution_";
	com_evolution += to_string(state);
	file_evolution.open(save->get_path(com_evolution), ofstream::out);

	unordered_map<unsigned int, set<unsigned int>> list_community;
	for(auto& e : *community)
		list_community[e.second].insert(e.first);

	for(auto& e : list_community)
	{
		file_evolution << e.first;
		for(auto& d : e.second)
			file_evolution << " " << graph->list_node[d]->id;
		file_evolution << endl;
	}

	file_evolution.close();
}*/

// template <class T>
// vector<unordered_map<unsigned int, unsigned int>*> COMP<T>::explore(Data_Explore& data_explore, int deep)
// {
// 	unsigned int id_node_similar;
// 	vector<unordered_map<unsigned int, unsigned int>*> list_map_community, tmp;
// 	vector<unsigned int> list_best_similar;	
// 	bool not_exist_map, not_exist_merging;
// 	T max;
// 	float max_peripheric;

// 	if(find(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end(), data_explore.id_node) == data_explore.list_node_to_visit.end())
// 	{
// 		if(data_explore.map_community->find(data_explore.id_node) == data_explore.map_community->end())
// 		{
// 			for(auto& d : data_explore.merging)
// 				data_explore.map_community->insert(make_pair(d, data_explore.start_com));

// 			data_explore.start_com++;
// 		}
// 		else
// 		{
// 			for(auto& d : data_explore.merging)
// 				data_explore.map_community->insert(make_pair(d, data_explore.map_community->at(data_explore.id_node)));
// 		}

// 		data_explore.merging.clear();
// 		data_explore.id_node = get_last_element(data_explore.list_node_to_visit);
// 	}

// 	while(data_explore.id_node != -1)
// 	{
// 		data_explore.list_node_to_visit.erase(remove(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end(), data_explore.id_node), data_explore.list_node_to_visit.end());
// 		//swap(data_explore.id_node, data_explore.list_node_to_visit.back());
// 		//data_explore.list_node_to_visit.pop_back();
// 		data_explore.merging.push_back(data_explore.id_node);

// 		max = 0.0;
// 		if(mapT.find(data_explore.id_node) != mapT.end())
// 		{
// 			list_best_similar.clear();
// 			list_best_similar.push_back(data_explore.id_node);

// 			for(auto& d : mapT[data_explore.id_node])
// 			{
// 				if(d.second >= max)
// 				{
// 					if(d.second > max)
// 						list_best_similar.clear();

// 					max = d.second;
// 					list_best_similar.push_back(d.first);
// 				}
// 			}

// 			id_node_similar = get_random_element(list_best_similar);

// 			if(data_explore.id_node == id_node_similar)
// 			{
// 				data_explore.map_community->insert(make_pair(data_explore.id_node, data_explore.start_com));				
// 				data_explore.start_com++;

// 				data_explore.id_node = get_last_element(data_explore.list_node_to_visit);
// 				continue;
// 			}

// 			/*if(peripheric_threshold > 0.0 && deep < 1)
// 			{
// 				for(auto& d : mapT[data_explore.id_node])
// 				{
// 					if(id_node_similar == d.first)
// 						continue;

// 					max_peripheric = 1.0 * d.second / max;
// 					if(max_peripheric >= peripheric_threshold)
// 					{
// 						Data_Explore new_data_explore;
// 						new_data_explore.id_node = d.first;
// 						new_data_explore.list_node = data_explore.list_node;
// 						new_data_explore.list_node_to_visit = data_explore.list_node_to_visit;
// 						new_data_explore.merging = data_explore.merging;
// 						new_data_explore.start_com = data_explore.start_com;
// 						unordered_map<unsigned int, unsigned int>* new_map_community = new unordered_map<unsigned int, unsigned int>(data_explore.map_community->begin(), data_explore.map_community->end());
// 						new_data_explore.map_community = new_map_community;

// 						tmp = explore(new_data_explore, deep+1);
// 						for(auto& map : tmp)
// 							list_map_community.push_back(map);
// 					}
// 				}
// 			}*/
			
// 			not_exist_map = data_explore.map_community->find(id_node_similar) == data_explore.map_community->end();
// 			not_exist_merging = find(data_explore.merging.begin(), data_explore.merging.end(), id_node_similar) == data_explore.merging.end();
			
// 			if(not_exist_merging && not_exist_map)
// 				data_explore.id_node = id_node_similar;
// 			else
// 			{
// 				if(not_exist_map)
// 				{
// 					for(auto& d : data_explore.merging)
// 					{
// 						data_explore.map_community->insert(make_pair(d, data_explore.start_com));
// 					}
// 					data_explore.start_com++;
// 				}
// 				else
// 				{
// 					for(auto& d : data_explore.merging)
// 					{
// 						data_explore.map_community->insert(make_pair(d, data_explore.map_community->at(id_node_similar)));
// 					}
// 				}

// 				data_explore.merging.clear();
// 				data_explore.id_node = get_last_element(data_explore.list_node_to_visit);
// 			}
// 		}
// 		else
// 			data_explore.id_node = get_last_element(data_explore.list_node_to_visit);
// 	}

// 	list_map_community.push_back(data_explore.map_community);
// 	return list_map_community;
// }

/*template <class T>
void COMP<T>::explore_comsim(Data_Explore& data_explore)
{
	unsigned int best_similar_community, id_com, id_com_neighbor;
	map<unsigned int, T> map_neighbor_com;
	T max;

	while(data_explore.id_node != -1)
	{
		data_explore.list_node_to_visit.erase(remove(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end(), data_explore.id_node), data_explore.list_node_to_visit.end());
		//data_explore.merging.push_back(data_explore.id_node);
		map_neighbor_com.clear();

		id_com = node_com[data_explore.id_node];

		if(mapT.find(data_explore.id_node) != mapT.end())
		{
			for(auto& d : mapT[data_explore.id_node])
			{
				id_com_neighbor = node_com[d.first];
				if(map_neighbor_com.find(id_com_neighbor) == map_neighbor_com.end())
				{
					map_neighbor_com[id_com_neighbor] = 0.0;
				}

				map_neighbor_com[id_com_neighbor] += d.second;
			}
		}

		best_similar_community = id_com;
		max = 0.0;
		for(auto& d : map_neighbor_com)
		{
			if(d.second > max)
			{
				max = d.second;
				best_similar_community = d.first;
			}
		}

		node_com[data_explore.id_node] = best_similar_community;

		data_explore.map_community->insert(make_pair(data_explore.id_node, best_similar_community));
		data_explore.id_node = get_random_element(data_explore.list_node_to_visit);
	}
}*/

template <class T>
 unsigned int COMP<T>::get_most_similar(unsigned int id_node)
{
	T max = 0;
	vector<unsigned int> list_best_similar;

	for(auto& d : mapT[id_node])
	{
		if(d.second >= max && d.second > 0.0)
		{
			if(d.second > max)
				list_best_similar.clear();

			max = d.second;
			list_best_similar.push_back(d.first);
		}
	}

	return get_random_element(list_best_similar);
}

template <class T>
void COMP<T>::explore_simclosure(Data_Explore<T>& data_explore, set<unsigned int>& set_node_non_simclosure)
{
	int id_node_similar;
	unsigned int nb_node_visit = data_explore.list_node_to_visit.size();
	vector<unsigned int>::iterator it, it_list_visit, it_current;
	bool not_exist_merging, new_com;
	T cumul = 0;

	unordered_map<unsigned int, unsigned int> best;

	while(data_explore.list_node_to_visit.size() > 0)
	{
		it_current = find(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end(), data_explore.id_node);
		if(it_current != data_explore.list_node_to_visit.end())
			data_explore.list_node_to_visit.erase(it_current);

		id_node_similar = get_most_similar(data_explore.id_node);

		// int p = -1;
		// if(data_explore.merging.size() > 0)
		// 	p = data_explore.merging.back();

		// T max = 0;
		// vector<unsigned int> list_best_similar;

		// for(auto& d : mapT[data_explore.id_node])
		// {
		// 	if(d.first != p && d.second >= max && d.second > 0.0)
		// 	{
		// 		if(d.second > max)
		// 			list_best_similar.clear();

		// 		max = d.second;
		// 		list_best_similar.push_back(d.first);
		// 	}
		// }

		// id_node_similar = get_random_element(list_best_similar);		

		if(id_node_similar == -1)
		{
			set_node_non_simclosure.insert(data_explore.id_node);
			data_explore.id_node = data_explore.list_node_to_visit.back();
		}
		else
		{
			data_explore.merging.push_back(data_explore.id_node);
			it = find(data_explore.merging.begin(), data_explore.merging.end(), id_node_similar);
			it_list_visit = find(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end(), id_node_similar);
			best[data_explore.id_node] = id_node_similar;
			
			if(it == data_explore.merging.end() && it_list_visit != data_explore.list_node_to_visit.end())
				data_explore.id_node = id_node_similar;
			else
			{
				set_node_non_simclosure.insert(data_explore.merging.begin(), it);

				new_com = false;
				while(it != data_explore.merging.end())
				{
					new_com = true;
					map_com[*it] = data_explore.start_com;

					++it;
				}

				if(new_com)
					data_explore.start_com++;

				data_explore.merging.clear();
				data_explore.id_node = data_explore.list_node_to_visit.back();
			}
		}		
	}
}

template <class T>
void COMP<T>::aggregate(set<unsigned int>& set_node_non_simclosure, set<unsigned int>& set_node_remaining)
{
	int best_similar_community;
	unsigned int id_node, nb_neighbor;
	unordered_map<unsigned int, T> map_neighbor_com;
	unordered_map<unsigned int, unsigned int> node_com_affectation;
	T max;
	unsigned int id_com;
	set<unsigned int>::const_iterator it;

	for(auto& id_node : set_node_non_simclosure)
	{
		map_neighbor_com.clear();

		for(auto& d : mapT[id_node])
		{
			if(map_com.find(d.first) == map_com.end())
				continue;
			
			id_com = map_com[d.first];
			
			if(map_neighbor_com.find(id_com) == map_neighbor_com.end())
			{
				map_neighbor_com[id_com] = 0.0;
			}
			map_neighbor_com[id_com] += d.second;
		}

		max = 0.0;
		best_similar_community = -1;

		for(auto& d : map_neighbor_com)
		{
			if(d.second > max)
			{
				max = d.second;
				best_similar_community = d.first;
			}
		}

		if(best_similar_community == -1)
			set_node_remaining.insert(id_node);
		else
			map_com[id_node] = best_similar_community;
	}	
	
	set_node_non_simclosure.clear();
}

template <class T>
set<unsigned int> COMP<T>::find_community(unordered_map<unsigned int, Node*>& list_node, Store_Partition*& store_partition)
{
	Data_Explore<T> data_explore;
	data_explore.list_node = list_node;
	data_explore.start_com = start_num_community;
	
	for(auto e : mapT)
	{
		if(list_node.find(e.first) != list_node.end())
			data_explore.list_node_to_visit.push_back(e.first);
	}
	random_shuffle(data_explore.list_node_to_visit.begin(), data_explore.list_node_to_visit.end());
	
	data_explore.id_node = data_explore.list_node_to_visit.back();

	set<unsigned int> set_node_non_simclosure, set_node_remaining;
	
	explore_simclosure(data_explore, set_node_non_simclosure);	
	aggregate(set_node_non_simclosure, set_node_remaining);

	start_num_community += data_explore.start_com;

	return set_node_remaining;
}

template <class T>
void COMP<T>::community_isolted_nodes(set<unsigned int> set_node_remaining, Store_Partition*& store_partition)
{
	if(set_node_remaining.size() > 0)
	{
		Partition* partition = new Partition();

		for(auto& e : set_node_remaining)
		{
			partition->update(e, start_num_community);
			start_num_community += 1;
		}

		store_partition->insert(partition);
	}
}

/*template <class T>
void COMP<T>::infer_overlapping_partition(Store_Partition& store_partition, Partition*& overlapping_partition) const
{
	overlapping_partition = new Partition();
	vector<Node*> intersection;
	unsigned int id_com = 0;
	bool find;

	for(auto& partition : store_partition.list_partition)
	{
		for(auto& community : partition->list_community)
		{
			find = false;

			for(auto& community_overlapping : overlapping_partition->list_community)
			{
				intersection.clear();
				set_intersection(community.second.begin(), community.second.end(),
					community_overlapping.second.begin(), community_overlapping.second.end(),
					back_inserter(intersection));

				if(intersection.size() == community_overlapping.second.size())
				{
					find = true;
					break;					
				}

			}

			if(!find)
			{
				overlapping_partition->list_community.insert(make_pair(id_com, community.second));
				id_com++;
			}
		}
	}
}*/

template <class T>
void COMP<T>::no_distance_map(unordered_map<unsigned int, Node*>& list_node)
{
	for(auto& e : list_node)
	{
		for(auto& neighbor : e.second->list_neighbor)
			mapT[e.first][neighbor->index] = e.second->list_weights[neighbor->index];
		//mapT[e.first].push_back(make_pair(neighbor->index, e.second->list_weights[neighbor->index]));
	}
}

template <class T>
void COMP<T>::get_distance_neighbor(Node*& node, unsigned int distance, set<Node*, comp_node>& list_distance_node)
{
	set<Node*, comp_node> list_node;
	set<Node*, comp_node> hist_list_node;
	unsigned int d = 0;

	if(distance == 2)
	{
		for(auto& neighbor : node->list_neighbor)
			list_distance_node.insert(neighbor->list_neighbor.begin(), neighbor->list_neighbor.end());
		list_distance_node.erase(node);
	}
	else
	{
		list_node.insert(node);

		while(d < distance && list_node.size() > 0)
		{
			hist_list_node.insert(list_node.begin(), list_node.end());

			list_distance_node.clear();
			for(auto& n : list_node)
				list_distance_node.insert(n->list_neighbor.begin(), n->list_neighbor.end());
			
			for(auto& n : hist_list_node)
				list_distance_node.erase(n);
			list_distance_node.erase(node);

			list_node = list_distance_node;

			d++;
		}
	}
}

template <class T>
void COMP<T>::calculate_in_out_links()
{
	// Initialisation
	for(auto& e : mapT)
	{
		out_weight[e.first] = 0.0;
		in_weight[e.first] = 0.0;
		out_degree[e.first] = 0.0;
		in_degree[e.first] = 0.0;

		for(auto& d : mapT[e.first])
		{
			out_weight[e.first] += d.second;
			in_weight[e.first] += d.second;
			out_degree[e.first] += 1;
			in_degree[e.first] += 1;
		}
	}
}

COMP_CommonNeighbors::COMP_CommonNeighbors(string directory) : COMP<float>(directory)
{}

void COMP_CommonNeighbors::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	
	vector<Node*> intersection;
	set<Node*, comp_node> list_direct_node;
	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		list_direct_node.clear();

		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));
				
				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += 1.0 * node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index];

				mapT[e.first][neighbor->index] = weight;
				//mapT[e.first].push_back(make_pair(neighbor->index, weight));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();	
}

COMP_AdamicAdar::COMP_AdamicAdar(string directory) : COMP<float>(directory)
{}

void COMP_AdamicAdar::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	float sum_log;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				sum_log = 0.0;
				for(auto& direct_neighbor : intersection)
					sum_log += (node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index]) / (1.0 + log2(direct_neighbor->sum_weights));

				mapT[e.first][neighbor->index] = sum_log;
				//mapT[e.first].push_back(make_pair(neighbor->index, sum_log));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();
}

COMP_JaccardIndex::COMP_JaccardIndex(string directory) : COMP<float>(directory)
{}

void COMP_JaccardIndex::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection, union_set;
	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				//union_set.clear();

				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index];
				
				/*set_union(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(union_set));*/

				mapT[e.first][neighbor->index] = 1.0 * weight / (node->sum_weights + neighbor->sum_weights);
				//mapT[e.first].push_back(make_pair(neighbor->index, 1.0 * weight / (node->sum_weights + neighbor->sum_weights)));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();
}

COMP_Dependence::COMP_Dependence(string directory) : COMP<float>(directory)
{}

void COMP_Dependence::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	unordered_map<unsigned int, unordered_map<unsigned int, float>> list_weights;

	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);
		
		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			list_direct_node.insert(node);

			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += (1.0 * node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index]) / direct_neighbor->sum_weights;

				list_weights[e.first][neighbor->index] = 1.0 * weight / node->sum_weights;
			}
		}

		list_direct_node.clear();
	}

	for(auto& e : list_weights)
	{
		for(auto& d : e.second)
		{
			if(d.first == e.first)
				continue;

			mapT[e.first][d.first] = 1.0 * list_weights[e.first][d.first] / list_weights[e.first][e.first];
			//mapT[e.first].push_back(make_pair(d.first, 1.0 * list_weights[e.first][d.first] / list_weights[e.first][e.first]));
		}
	}

	calculate_in_out_links();
}

COMP_Independence::COMP_Independence(string directory) : COMP<float>(directory)
{}

void COMP_Independence::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	unordered_map<unsigned int, unordered_map<unsigned int, float>> list_weights;
	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			list_direct_node.insert(node);

			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += (1.0 * node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index]) / direct_neighbor->sum_weights;

				list_weights[e.first][neighbor->index] = 1.0 * weight / node->sum_weights;
			}
		}

		list_direct_node.clear();
	}

	for(auto& e : list_weights)
	{
		for(auto& d : e.second)
		{
			if(d.first == e.first)
				continue;

			mapT[e.first][d.first] = 1.0 * list_weights[d.first][e.first] / list_weights[e.first][e.first];
			//mapT[e.first].push_back(make_pair(d.first, 1.0 * list_weights[d.first][e.first] / list_weights[e.first][e.first]));
		}
	}

	calculate_in_out_links();
}

COMP_RessourceAllocator::COMP_RessourceAllocator(string directory) : COMP<float>(directory)
{}

void COMP_RessourceAllocator::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	float sum;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				sum = 0.0;
				for(auto& direct_neighbor : intersection)
					sum += (1.0 * node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index]) / direct_neighbor->sum_weights;

				mapT[e.first][neighbor->index] = sum;
				//mapT[e.first].push_back(make_pair(neighbor->index, sum));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();
}

COMP_LHN1::COMP_LHN1(string directory) : COMP<float>(directory)
{}

void COMP_LHN1::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += 1.0 * node->list_weights[direct_neighbor->index] + neighbor->list_weights[direct_neighbor->index];

				//mapT[e.first][neighbor->index] = intersection.size() / (node->list_neighbor.size() * neighbor->list_neighbor.size());
				mapT[e.first][neighbor->index] = weight / (1.0 * node->sum_weights * neighbor->sum_weights);
				//mapT[e.first].push_back(make_pair(neighbor->index, weight / (1.0 * node->sum_weights * neighbor->sum_weights)));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();
}

COMP_MOD::COMP_MOD(string directory) : COMP<float>(directory)
{}

void COMP_MOD::similarity_projection(unordered_map<unsigned int, Node*>& list_node, unsigned int distance)
{
	if(distance == 0)
	{
		no_distance_map(list_node);
		return;
	}

	Node* node;
	set<Node*, comp_node> list_direct_node;
	vector<Node*> intersection;
	float weight;

	for(auto& e : list_node)
	{
		node = e.second;
		get_distance_neighbor(node, distance, list_direct_node);

		if(list_direct_node.size() == 0)
			mapT[e.first];
		else
		{
			for(auto& neighbor : list_direct_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					neighbor->list_neighbor.begin(), neighbor->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
					weight += 1.0 * (node->list_weights[direct_neighbor->index] * neighbor->list_weights[direct_neighbor->index]) / (2.0*m);

				//mapT[e.first][neighbor->index] = intersection.size() / (node->list_neighbor.size() * neighbor->list_neighbor.size());
				mapT[e.first][neighbor->index] = weight / (1.0 * node->sum_weights * neighbor->sum_weights);
				//mapT[e.first].push_back(make_pair(neighbor->index,  weight / (1.0 * node->sum_weights * neighbor->sum_weights)));
			}
		}

		list_direct_node.clear();
	}

	calculate_in_out_links();
}

/*class COMP_LocalRank : public COMP<float>
{
public:
	COMP_LocalRank(Bipgraph* bip) : COMP(bip)
	{}

	void similarity_projection()
	{
		Node* node;
		set<Node*, comp_node> list_projection_node;
		vector<Node*> intersection;
		unordered_map<unsigned int, unordered_map<unsigned int, float>> list_weights;
		float weight;

		for(auto& e : list_node)
		{
			node = e.second;
			get_distance_neighbor(node, distance, list_direct_node);

			for(auto& neighbor : list_projection_node)
			{
				intersection.clear();
				set_intersection(node->list_neighbor.begin(), node->list_neighbor.end(),
					list_node[neighbor]->list_neighbor.begin(), list_node[neighbor]->list_neighbor.end(),
					back_inserter(intersection));

				weight = 0.0;
				for(auto& direct_neighbor : intersection)
				{
					weight += 1.0 / bip->list_bot_node[direct_neighbor]->list_neighbor.size();
				}

				list_weights[e.first][neighbor] = 1.0 * weight / node->list_neighbor.size();
			}

			list_projection_node.clear();
		}

		for(auto& e : list_weights)
		{
			for(auto& d : e.second)
			{
				if(d.first == e.first)
					continue;

				mapT[e.first][d.first] = 1.0 * list_weights[e.first][d.first] / list_weights[e.first][e.first];
			}
		}
	}

	calculate_in_out_links();
};*/

template class COMP<int>;
template class COMP<float>;
template class COMP<double>;
