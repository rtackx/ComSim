#include "partition.h"

Store_Partition::Store_Partition()
{}

Store_Partition::~Store_Partition()
{
	for(auto& p : list_partition)
		delete p;
	list_partition.clear();
}

int Store_Partition::size() const
{
	return list_partition.size();
}

void Store_Partition::save_partitions(Save* save, unordered_map<unsigned int, Node*>& list_node) const
{
	string path;
	unsigned int i = 0;

	for(auto& partition : list_partition)
	{
		path = save->get_path("partition_" + to_string(i));
		partition->save(path, list_node);
		i++;
	}
}

bool Store_Partition::insert(Partition* new_part)
{
	unsigned int nb_identical;
	bool find;

	if(new_part->list_community.size() == 0)
		return false;

	/*for(auto& part : list_partition)
	{
		find = true;
		nb_identical = 0;
		for(auto& e1 : part->id_community)
		{
			for(auto& e2 : new_part->id_community)
			{
				if(e1.second == e2.second)
				{
					nb_identical++;
					break;
				}
			}
		}

		if(nb_identical == part->list_community.size())
			return false;
	}*/

	list_partition.push_back(new_part);
	return true;
}

Partition::Partition()
{}

Partition::~Partition()
{}

void Partition::update(unsigned int id_node, unsigned int id_com)
{
	list_community[id_com].push_back(id_node);
	//list_node->at(id_node)->list_community.push_back(id_com);

	/*if(id_community.find(id_com) == id_community.end())
		id_community[id_com] = 0;
	id_community[id_com] += (node->index+1);*/
}

void Partition::clean()
{
	/*set<unsigned int> list_delete;

	for(auto& e1 : list_community)
	{
		for(auto& e2 : list_community)
		{
			if(e1 == e2 || list_delete.find(e2.first) != list_delete.end())
				continue;

			if(id_community[e1.first] == id_community[e2.first])
			{
				list_delete.insert(e1.first);
				break;
			}
		}
	}

	for(auto& d : list_delete)
	{
		list_community.erase(d);
		id_community.erase(d);
	}*/
}

void Partition::save(string filename_path, unordered_map<unsigned int, Node*>& list_node)
{
	FILE* f1;

	f1 = fopen(filename_path.c_str(), "w");
	for(auto& e : list_community)
	{
		//fprintf(f1, "%i ", e.first);
		for(auto& e2 : e.second)
		{
			fprintf(f1, "%s ", list_node[e2]->id.c_str());
		}
		fprintf(f1, "\n");
	}		
	fclose(f1);
}

/*Graph* Partition::to_graph() const
{
	if(list_community.size() == 0)
		return NULL;

	Graph* graph = new Graph();
	unordered_map<unsigned int, float> list_neighbor_community;
	set<unsigned int> list_node_neighbor;
	vector<Node*> intersection;

	for(auto& e : list_community)
	{
		graph->list_node[e.first] = new Node(e.first, to_string(e.first));

		list_neighbor_community.clear();
		for(auto& node1 : e.second)
		{
			for(auto& node2 : node1->list_neighbor)
			{
				for(auto& id_com : node2->list_community)
				{
					if(list_neighbor_community.find(id_com) == list_neighbor_community.end())
					{
						list_neighbor_community[id_com] = 0.0;
					}

					list_neighbor_community[id_com] += node1->list_weights[node2->index];
				}
			}
		}

		for(auto& d : list_neighbor_community)
		{
			if(graph->list_node.find(d.first) == graph->list_node.end())
				graph->list_node[d.first] = new Node(d.first, to_string(d.first));

			graph->list_node[e.first]->add_neighbor(graph->list_node[d.first], d.second);
			graph->list_node[d.first]->add_neighbor(graph->list_node[e.first], d.second);
		}
	}

	return graph;
}*/

