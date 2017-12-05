#ifndef PROJ_H
#define PROJ_H

#include "libs.h"
#include "graph.h"

class Projection
{
protected:
	vector<Node*> get_neighbors(Graph*&, unsigned int, unsigned int) const;
	set<Node*> get_intersection(Graph*&, Node*, Node*) const;
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

class JaccardIndex : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	JaccardIndex();
};

class AdamicAdar : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	AdamicAdar();
};

class ResourceAllocator : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	ResourceAllocator();
};

class LHN1 : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	LHN1();
};

class PA_Neighbor : public Projection
{
protected:
	float similarity_projection(Graph*&, unsigned int, unsigned int);
public:
	PA_Neighbor();
};

#endif