#ifndef SIM_H
#define SIM_H

#include "libs.h"
#include "bipartite_graph.h"

//template <class T> class Modularity;

template <class T>
struct Data_Explore
{
	unordered_map<unsigned int, Node*> list_node;
	int id_node;
	vector<unsigned int> list_node_to_visit;
	vector<unsigned int> merging;
	unordered_map<unsigned int, vector<unsigned int>> list_community;
	unsigned int start_com;
};

class SIM
{
public:
	float peripheric_threshold;
	unsigned int depth_threshold;
	unsigned int m;

	//virtual void display_modularity(unordered_map<unsigned int, Node*>&)=0;
	virtual void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int)=0;
	virtual set<unsigned int> find_community(unordered_map<unsigned int, Node*>&, Store_Partition*&)=0;
	virtual void save_projection(Graph*, string)=0;
	virtual void community_isolted_nodes(set<unsigned int>, Store_Partition*&)=0;
	virtual void clean()=0;
};

template <class T>
class COMP : public SIM
{
private:
	unsigned int get_most_similar(unsigned int);
protected:
	void no_distance_map(unordered_map<unsigned int, Node*>&);
	void get_distance_neighbor(Node*&, unsigned int, set<Node*, comp_node>&);
	void explore_simclosure(Data_Explore<T>&, set<unsigned int>&);
	void aggregate(Data_Explore<T>&, set<unsigned int>&, set<unsigned int>&);

	//vector<unordered_map<unsigned int, unsigned int>*> explore(Data_Explore<T>&, int);
	//void explore_comsim(Data_Explore&);
	//void infer_overlapping_partition(Store_Partition&) const;
	//void save_state_partition(Graph*, unordered_map<unsigned int, unsigned int>*, int);
	void calculate_in_out_links();
public:	
	//Modularity<T>* mod;
	unsigned int start_num_community;
	// Measure map
	//map<unsigned int, map<unsigned int, T>> mapT;
	unordered_map<unsigned int, unordered_map<unsigned int, T>> mapT;
	unordered_map<unsigned int, unsigned int> map_com;
	//unordered_map<unsigned int, vector<pair<unsigned int, T>>> mapT;

	unordered_map<unsigned int, T> in_weight;
	unordered_map<unsigned int, T> out_weight;
	unordered_map<unsigned int, int> in_degree;
	unordered_map<unsigned int, int> out_degree;

	COMP(string);
	~COMP();
	virtual void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int)=0;
	//void display_modularity(unordered_map<unsigned int, Node*>&);
	set<unsigned int> find_community(unordered_map<unsigned int, Node*>&, Store_Partition*&);
	void save_projection(Graph*, string);
	void community_isolted_nodes(set<unsigned int>, Store_Partition*&);
	void clean();
};

class COMP_Independence : public COMP<float>
{
public:
	COMP_Independence(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_Dependence : public COMP<float>
{
public:
	COMP_Dependence(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_CommonNeighbors : public COMP<float>
{
public:
	COMP_CommonNeighbors(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_JaccardIndex : public COMP<float>
{
public:
	COMP_JaccardIndex(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_AdamicAdar : public COMP<float>	
{
public:	
	COMP_AdamicAdar(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_RessourceAllocator : public COMP<float>
{
public:
	COMP_RessourceAllocator(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_LHN1 : public COMP<float>
{
public:
	COMP_LHN1(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};
class COMP_MOD : public COMP<float>
{
public:
	COMP_MOD(string);
	void similarity_projection(unordered_map<unsigned int, Node*>&, unsigned int);
};


//#include "modularity.h"

#endif