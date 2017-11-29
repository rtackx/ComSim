#ifndef PROJ_H
#define PROJ_H

#include "libs.h"
#include "graph.h"

class Projection
{
protected:
	set<Node*, comp_node> get_neighbors(Graph*&, unsigned int, unsigned int) const;
	virtual float similarity_projection(Graph*&, unsigned int, unsigned int)=0;
public:
	Graph* graph_projection;
	
	Projection();
	~Projection();

	void project(Graph*&, unsigned int);
};

class CommonNeighbors : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	CommonNeighbors();
	
};

#endif