#include "modularity.h"

template <class T>
Modularity<T>::Modularity(COMP<T>* sim)
{
	this->sim = sim;
}

// Bimodularity of bipartite community
template <class T>
float Modularity<T>::bimodularity()
{
}

template <class T>
float Modularity<T>::modularity_weighted(unordered_map<unsigned int, Node*>& list_node)
{
	float m_projection = 0.0;

	for(auto& e : sim->out_weight)
	{
		m_projection += e.second;
	}
	for(auto& e : sim->in_weight)
	{
		m_projection += e.second;
	}
	
	m_projection = 1.0 / m_projection;

	unsigned int w1, w2;
	float aij;
	float modularity = 0.0;

	for(auto& e1 : list_node)
	{
		for(auto& e2 : list_node)
		{
			if(!e1.second->same_community(e2.second))
				continue;

			w1 = sim->out_weight[e1.first] + sim->in_weight[e2.first];
			w2 = sim->out_weight[e1.first] + sim->in_weight[e2.first];

			if(sim->mapT[e1.first].find(e2.first) != sim->mapT[e1.first].end())
				aij = sim->mapT[e1.first][e2.first] + sim->mapT[e2.first][e1.first];
			else
				aij = 0.0;

			modularity += aij - w1 * w2 * m_projection;
		}
	}

	return 1.0 * modularity * m_projection;
}

template <class T>
float Modularity<T>::modularity_directed(unordered_map<unsigned int, Node*>& list_node)
{
	// m_proection = 2*m
	float m_projection = 0.0;

	for(auto& e : sim->out_degree)
	{
		m_projection += e.second;
	}
	m_projection = 1.0 / m_projection;

	unsigned int d1, d2;
	float aij;
	float modularity = 0.0;

	for(auto& e1 : list_node)
	{
		for(auto& e2 : list_node)
		{
			if(!e1.second->same_community(e2.second))
				continue;

			d1 = sim->out_degree[e1.first];
			d2 = sim->in_degree[e2.first];

			if(sim->mapT[e2.first].find(e1.first) != sim->mapT[e2.first].end())
				aij = 1.0;
			else
				aij = 0.0;

			modularity += aij - d1 * d2 * m_projection;
		}
	}

	return 1.0 * modularity * m_projection;
}


// Modularity of top projection
template <class T>
float Modularity<T>::modularity_directed_weighted(unordered_map<unsigned int, Node*>& list_node)
{
	float m_projection = 0.0;
	
	for(auto& e : sim->out_weight)
	{
		m_projection += e.second;
	}
	for(auto& e : sim->in_weight)
	{
		m_projection += e.second;
	}
	m_projection = 1.0 / m_projection;

	unsigned int w1_out, w2_in;
	float aij;
	float modularity = 0.0;

	for(auto& e1 : list_node)
	{
		for(auto& e2 : list_node)
		{
			if(!e1.second->same_community(e2.second))
				continue;
			
			w1_out = sim->out_weight[e1.first] + sim->in_weight[e1.first];
			w2_in = sim->out_weight[e2.first] + sim->in_weight[e2.first];

			if(sim->mapT[e2.first].find(e1.first) != sim->mapT[e2.first].end())
				aij = sim->mapT[e1.first][e2.first] + sim->mapT[e2.first][e1.first];
			else
				aij = 0.0;

			modularity += aij - w1_out * w2_in * m_projection;
		}
	}

	return 1.0 * modularity * m_projection;
}

template class Modularity<int>;
template class Modularity<float>;
template class Modularity<double>;
