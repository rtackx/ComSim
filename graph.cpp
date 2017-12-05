#include "graph.h"

Graph::Graph()
{
	n = 0;
	m = 0;
	size_list_nodes = 0;
	list_nodes = NULL;
}

Graph::~Graph()
{
	unsigned int index, nb;

	index = 0;
	while(index < size_list_nodes)
	{
		nb = list_nodes[index]->nb_neighbors;
		delete list_nodes[index];
		index += nb + 1;		
	}

	if(list_nodes)
		delete[] list_nodes;
}

void Graph::load_graph(string bip_filename)
{
	char delim = ' ';

	int fd = open(bip_filename.c_str(), O_RDONLY);
	if(fd == -1)
	{
		perror("Can't open file.");
		exit(EXIT_FAILURE);
	}

	Node* node, *node_tmp;

	char* w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
	string word;
	char c;
	float weight;
	int i, size_w, count;
	bool new_w, new_line;
	new_line = new_w;

	unsigned int index = 0;
	int index1, index2;
	unordered_map<string, unsigned int> map_word;
	unordered_map<unsigned int, Node*> map_node;
	unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;

	index2 = -1;
	size_w = 0;
	count = 0;
	weight = 1.0;
	node_tmp = NULL;

	do
	{
		i = read(fd, &c, 1);

		if(size_w == SIZE_BUFFER)
			w = (char*) realloc(w, sizeof(char) * SIZE_BUFFER + size_w);

		if(c == '\r')
			continue;

		if(c == delim || c == '\n')
		{
			if(c == '\n')
				new_line = true;

			w[size_w] = '\0';
			new_w = true;
			word = w;
			count++;
		}
		else
		{
			w[size_w] = c;
			size_w++;
		}

		if(new_w && size_w > 0)
		{
			new_w = false;
			if(count == 3)
			{
				weight = stof(word);
			}
			else
			{
				if(map_word.find(word) == map_word.end())
				{
					node = new Node();
					node->id = word;
					map_node[index] = node;
					map_word[word] = index;

					index1 = index;
					index++;
				}
				else
					index1 = map_word[word];
			}

			if(new_line)
			{
				map_neighbor[index1].insert(make_pair(index2, weight));
				map_neighbor[index2].insert(make_pair(index1, weight));

				new_line = false;
				count = 0;
				weight = 1.0;
				index2 = -1;
			}
			else
			{
				if(index2 == -1)
					index2 = index1;
			}
			
			free(w);
			w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
			size_w = 0;
		}

	} while(i != 0);
	free(w);

	create_graph(map_node, map_neighbor);
}

void Graph::create_graph(unordered_map<unsigned int, Node*>& map_node, unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>& map_neighbor)
{
	Node* node;
	unsigned int index;
	float weight;

	n = map_node.size();
	for(auto& e : map_neighbor)
		m += e.second.size();

	size_list_nodes = n + (m);
	list_nodes = new Node*[size_list_nodes];
	
	index = 0;
	for(auto& e : map_node)
	{
		node = e.second;
		node->nb_neighbors = map_neighbor[e.first].size();
		
		node->index = index;
		if(node->main_index == -1)
			node->main_index = index;
		
		list_nodes[index] = node;
		index++;

		weight = 0.0;
		for(auto& d : map_neighbor[e.first])
		{
			node->neighbor_weights.push_back(d.second);
			list_nodes[index] = map_node[d.first];
			index++;
		}
	}
}

Graph* Graph::get_subgraph(vector<unsigned int>& list_ending_nodes) const
{
	Graph* new_graph = NULL;

	if(list_ending_nodes.size() > 0)
	{
		new_graph = new Graph();

		unordered_map<unsigned int, Node*> map_node;
		unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>> map_neighbor;
		pair<unordered_map<unsigned int, set<pair<unsigned int, float>, comp_pair>>::iterator, bool> it_map;
		unsigned int index;
		Node* node;
		Node* new_node;

		set<unsigned int> list_accepted_index;
		for(auto& index : list_ending_nodes)
			list_accepted_index.insert(index);

		index = 0;
		while(index < size_list_nodes)
		{
			if(list_accepted_index.find(index) != list_accepted_index.end())
			{
				node = list_nodes[index];
				new_node = new Node();				
				new_node->id = node->id;
				new_node->main_index = node->main_index;
				map_node[index] = new_node;

				it_map = map_neighbor.emplace(index, set<pair<unsigned int, float>, comp_pair>());

				for(unsigned int i=0; i<node->nb_neighbors; i++)
				{
					if(list_accepted_index.find(list_nodes[index+i+1]->index) != list_accepted_index.end())
						it_map.first->second.insert(make_pair(list_nodes[index+i+1]->index, node->neighbor_weights[i]));
					//map_neighbor[index].insert(make_pair(list_nodes[index+i+1]->index, node->neighbor_weights[i]));
				}
			}

			index += list_nodes[index]->nb_neighbors + 1;
		}

		new_graph->create_graph(map_node, map_neighbor);
	}

	return new_graph;
}