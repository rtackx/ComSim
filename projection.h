#ifndef PROJ_H
#define PROJ_H

#include "libs.h"
#include "graph.h"

class Projection
{
private:
	unsigned int nb_threads;
public:
	Graph* in_graph;
	vector<Node*> get_neighbors(unsigned int&, unsigned int&) const;
	set<Node*> get_intersection(Node*&, Node*&) const;
	virtual float similarity_projection(unsigned int&, unsigned int&)=0;

	Graph* graph_projection;
	
	Projection(Graph*&);
	~Projection();

	void project(unsigned int);
};

static pthread_mutex_t projection_main_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t compute_projection_mutex = PTHREAD_MUTEX_INITIALIZER;
void* projection_main_thread(void*);
void* compute_projection_thread(void*);

typedef struct
{
	Projection* p;
	unsigned int index;
	unsigned int distance;
	unordered_map<unsigned int, Node*>* map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>* map_neighbor;
} projection_thread_args;

class CommonNeighbors : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	CommonNeighbors(Graph*&);
};

class JaccardIndex : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	JaccardIndex(Graph*&);
};

class AdamicAdar : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	AdamicAdar(Graph*&);
};

class ResourceAllocator : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	ResourceAllocator(Graph*&);
};

class LHN1 : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	LHN1(Graph*&);
};

class PA_Neighbor : public Projection
{
protected:
	float similarity_projection(unsigned int&, unsigned int&);
public:
	PA_Neighbor(Graph*&);
};

#endif