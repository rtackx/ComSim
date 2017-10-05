#ifndef COM_H
#define COM_H

#include "libs.h"
#include "node.h"

class Graph;

class Partition
{
public:
	unordered_map<unsigned int, vector<unsigned int>> list_community;
	//unordered_map<unsigned int, unsigned int> id_community;

	Partition();
	~Partition();

	void save(string, unordered_map<unsigned int, Node*>&);
	//Graph* to_graph() const;
	void update(unsigned int, unsigned int);
	void clean();
	//void save_state_partition(unordered_map<unsigned int, Node*>&, unordered_map<unsigned int, set<unsigned int>>&, int, bool);
};

class Store_Partition
{
	public:
		Store_Partition();
		~Store_Partition();

		vector<Partition*> list_partition;
		int size() const;
		bool insert(Partition*);
		void save_partitions(Save*, unordered_map<unsigned int, Node*>&) const;
		unordered_map<unsigned int, vector<unsigned int>> get_list_community_all_partition() const;
};

#include "graph.h"

#endif