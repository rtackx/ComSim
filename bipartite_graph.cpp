#include "bipartite_graph.h"

Bipgraph::Bipgraph()
{}

Bipgraph::~Bipgraph()
{}

void Bipgraph::load_bip_adj(string bip_filename)
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

					if(count == 1)
						list_top_node[id] = node;
					else
						list_bot_node[id] = node;

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

	n_top = list_top_node.size();
	n_bot = list_bot_node.size();
	n = list_node.size();

	m = 0;
	for(auto& e : list_node)
		m += e.second->list_neighbor.size();
	m /= 2;
}

void Bipgraph::load_bip(string bip_filename)
{
	int fd = open(bip_filename.c_str(), O_RDONLY);
	if(fd == -1)
	{
		perror("Can't open file.");
		exit(EXIT_FAILURE);
	}

	Node* node, *node2;
	unsigned int id = 0;

	char* w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
	string word;
	char c;
	int i, size_w;
	bool is_top, new_w, new_line;
	is_top = true;
	new_line = new_w = false;

	unordered_map<string, unsigned int> map_bot_node;
	size_w = 0;
	do
	{
		i = read(fd, &c, 1);

		if(size_w == SIZE_BUFFER)
			w = (char*) realloc(w, sizeof(char) * SIZE_BUFFER + size_w);

		if(c == ' ' || c == '\n')
		{
			if(c == '\n')
			{
				new_line = true;
			}

			w[size_w] = '\0';
			new_w = true;
			word = w;
		}
		else
		{
			w[size_w] = c;
			size_w++;
		}

		if(new_w && size_w > 0)
		{
			new_w = false;

			if(is_top)
			{
				is_top = false;
				node = new Node(id, word);
				list_top_node[id] = node;
				id++;
			}
			else
			{
				if(map_bot_node.find(word) == map_bot_node.end())
				{
					map_bot_node[word] = id;
					list_bot_node[id] = new Node(id, word);
					id++;
				}

				node->add_neighbor(list_bot_node[map_bot_node[word]], 1.0);
				list_bot_node[map_bot_node[word]]->add_neighbor(node, 1.0);

				if(new_line)
				{
					is_top = true;
					new_line = false;
				}
			}
			
			free(w);
			w = (char*) malloc(sizeof(char) * SIZE_BUFFER);
			size_w = 0;
		}

	} while(i != 0);
	free(w);


	n_top = list_top_node.size();
	n_bot = list_bot_node.size();
	n = id;
	list_node.insert(list_top_node.begin(), list_top_node.end());
	list_node.insert(list_bot_node.begin(), list_bot_node.end());

	m = 0;
	for(auto& e : list_node)
		m += e.second->list_neighbor.size();
	m /= 2;
}

/*void Bipgraph::remove_duplicate_community()
{
	vector<Node*> intersection;
	vector<unsigned int> remove_community;

	for(auto& c : list_community_top)
	{
		for(auto& e : list_community_top)
		{
			if(c < e || e.first == c.first)
				continue;
			
			intersection.clear();	
			set_intersection(c.second.begin(), c.second.end(),
				e.second.begin(), e.second.end(),
				back_inserter(intersection));

			if(intersection.size() == c.second.size())
			{
				remove_community.push_back(c.first);
				break;
			}
		}
	}

	for(auto& e : remove_community)
	{
		list_community_top.erase(e);
	}

	remove_community.clear();

	for(auto& c : list_community_bot)
	{
		for(auto& e : list_community_bot)
		{
			if(c < e || e.first == c.first)
				continue;
			
			intersection.clear();	
			set_intersection(c.second.begin(), c.second.end(),
				e.second.begin(), e.second.end(),
				back_inserter(intersection));

			if(intersection.size() == c.second.size())
			{
				remove_community.push_back(c.first);
				break;
			}
		}
	}

	for(auto& e : remove_community)
	{
		list_community_bot.erase(e);
	}
}

void Bipgraph::(save_community_reverse)(const char* filename)
{
	FILE* f1, *f2;

	string filename_top = filename;
	filename_top += ".top_reverse";
	string filename_bot = filename;
	filename_bot += ".bot_reverse";

	string path1 = save->get_path(filename_top);
	string path2 = save->get_path(filename_bot);

	set<Node*, comp_node> list_n;

	f1 = fopen(path1.c_str(), "w");
	for(auto& e : list_community_top)
	{
		fprintf(f1, "%i", e.first);
		
		list_n.clear();
		for(auto& e2 : e.second)
			list_n.insert(e2->list_neighbor.begin(), e2->list_neighbor.end());

		for(auto& d : list_n)
			fprintf(f1, " %s", d->id.c_str());
		fprintf(f1, "\n");
	}
	fclose(f1);

	f2 = fopen(path2.c_str(), "w");
	for(auto& e : list_community_bot)
	{
		fprintf(f2, "%i", e.first);
		
		list_n.clear();
		for(auto& e2 : e.second)
			list_n.insert(e2->list_neighbor.begin(), e2->list_neighbor.end());

		for(auto& d : list_n)
			fprintf(f2, " %s", d->id.c_str());
		fprintf(f2, "\n");
	}
	fclose(f2);
}*/

/*Graph* Bipgraph::convert_community_to_graph() const
{
	//if(list_community_top.size() == 0 || list_community_bot.size() == 0)
	//	return NULL;

	Graph* graph = new Graph();
	unordered_map<unsigned int, float> list_neighbor_community;
	set<unsigned int> list_node_neighbor;
	vector<Node*> intersection;

	graph->n = store_partition_top->list_partition[0]->list_community.size() + store_partition_bot->list_partition[0]->list_community.size();

	for(auto& e : store_partition_top->list_partition[0]->list_community)
	{
		graph->list_node[e.first] = new Node(e.first, to_string(e.first));

		list_neighbor_community.clear();
		for(auto& node_top : e.second)
		{
			for(auto& node_bot : node_top->list_neighbor)
			{
				for(auto& id_com : node_bot->list_community)
				{
					if(list_neighbor_community.find(id_com) == list_neighbor_community.end())
						list_neighbor_community[id_com] = 0.0;

					list_neighbor_community[id_com] += node_top->list_weights[node_bot->index];
				}
			}
		}

		for(auto& d : list_neighbor_community)
		{
			if(graph->list_node.find(d.first) == graph->list_node.end())
				graph->list_node[d.first] = new Node(d.first, to_string(d.first));

			graph->list_node[e.first]->add_neighbor(graph->list_node[d.first], d.second);
			graph->list_node[d.first]->add_neighbor(graph->list_node[e.first], d.second);
			
			graph->m++;
		}
	}

	return graph;
}*/

/*void Bipgraph::save_community_graph(Graph* graph, string path) const
{
	FILE* f1;
	string filename = path;
	filename += "/community_graph_unfold";

	f1 = fopen(filename.c_str(), "w");
	for(auto& e : graph->store_partition->list_partition[0]->list_community)
	{
		fprintf(f1, "%i", e.first);

		for(auto& v : e.second)
		{
			if(store_partition_top->list_partition[0]->list_community.find(v->index) != store_partition_top->list_partition[0]->list_community.end())
			{
				for(auto& e2 : store_partition_top->list_partition[0]->list_community[v->index])
				{
					fprintf(f1, " %s", e2->id.c_str());
				}
			}
			else
			{
				for(auto& e2 : store_partition_bot->list_partition[0]->list_community[v->index])
				{
					fprintf(f1, " %s", e2->id.c_str());
				}
			}
		}
		fprintf(f1, "\n");
	}		
	fclose(f1);
}*/


Bipgraph* Bipgraph::get_subgraph(set<unsigned int> set_node_remaining, bool top)
{
	Bipgraph* bip = new Bipgraph();
	
	if(top)
	{
		for(auto& e : set_node_remaining)
			bip->list_top_node[e] = new Node(e, list_top_node[e]->id);

		for(auto& e : set_node_remaining)
		{
			for(auto neighbor_bot : list_top_node[e]->list_neighbor)
			{
				if(bip->list_bot_node.find(neighbor_bot->index) == bip->list_bot_node.end())
					bip->list_bot_node[neighbor_bot->index] = new Node(neighbor_bot->index, list_bot_node[neighbor_bot->index]->id);

				bip->list_top_node[e]->add_neighbor(bip->list_bot_node[neighbor_bot->index], list_top_node[e]->list_weights[neighbor_bot->index]);

				for(auto neighbor_top : list_bot_node[neighbor_bot->index]->list_neighbor)
				{
					if(set_node_remaining.find(neighbor_top->index)	!= set_node_remaining.end())
						bip->list_bot_node[neighbor_bot->index]->add_neighbor(bip->list_top_node[neighbor_top->index], list_bot_node[neighbor_bot->index]->list_weights[neighbor_top->index]);
				}
			}
		}
	}
	else
	{
		for(auto& e : set_node_remaining)
			bip->list_bot_node[e] = new Node(e, list_bot_node[e]->id);
		
		for(auto& e : set_node_remaining)
		{
			for(auto neighbor_top : list_bot_node[e]->list_neighbor)
			{
				if(bip->list_top_node.find(neighbor_top->index) == bip->list_top_node.end())
					bip->list_top_node[neighbor_top->index] = new Node(neighbor_top->index, list_top_node[neighbor_top->index]->id);

				bip->list_bot_node[e]->add_neighbor(bip->list_top_node[neighbor_top->index], list_bot_node[e]->list_weights[neighbor_top->index]);

				for(auto neighbor_bot : list_top_node[neighbor_top->index]->list_neighbor)
				{
					if(set_node_remaining.find(neighbor_bot->index)	!= set_node_remaining.end())
						bip->list_top_node[neighbor_top->index]->add_neighbor(bip->list_bot_node[neighbor_bot->index], list_top_node[neighbor_top->index]->list_weights[neighbor_bot->index]);
				}
			}
		}
	}
	
	return bip;
}