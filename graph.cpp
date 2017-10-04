#include "graph.h"

Graph::Graph()
{
	m = 0;
}

Graph::~Graph()
{
	for(auto& e : list_node)
		delete e.second;
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
	unsigned int id = 0;

	char* w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
	string word;
	char c;
	float weight;
	int i, size_w, count;
	bool new_w, new_line;
	new_line = new_w = false;

	unordered_map<string, unsigned int> map_node;
	size_w = 0;
	count = 0;
	weight = 1.0;
	node_tmp = NULL;
	unsigned k = 0;
	do
	{
		i = read(fd, &c, 1);

		if(size_w == SIZE_BUFFER)
			w = (char*) realloc(w, sizeof(char) * SIZE_BUFFER + size_w);

		if(c == delim || c == '\n')
		{
			if(c == '\n')
			{
				new_line = true;
			}

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
				if(map_node.find(word) == map_node.end())
				{
					node = new Node(id, word);
					list_node[id] = node;
					map_node[word] = id;
					id++;
				}
				else
					node = list_node[map_node[word]];
			}

			if(new_line)
			{
				node->add_neighbor(node_tmp, weight);
				node_tmp->add_neighbor(node, weight);

				new_line = false;
				count = 0;
				weight = 1.0;
				node_tmp = NULL;
			}
			else
			{
				if(node_tmp == NULL)
					node_tmp = node;
			}
			
			free(w);
			w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
			size_w = 0;
		}

	} while(i != 0);
	free(w);

	n = list_node.size();
	m = 0;
	for(auto& e : list_node)
		m += e.second->list_neighbor.size();
	//m /= 2;
}

Graph* Graph::get_subgraph(set<unsigned int> set_node_remaining)
{
	Graph* g = new Graph();

	for(auto& e : set_node_remaining)
		g->list_node[e] = new Node(e, list_node[e]->id);

	for(auto& e : set_node_remaining)
	{
		for(auto neighbor : list_node[e]->list_neighbor)
		{
			if(set_node_remaining.find(neighbor->index)	!= set_node_remaining.end())
				g->list_node[e]->add_neighbor(g->list_node[neighbor->index], list_node[e]->list_weights[neighbor->index]);
		}
	}

	return g;
}