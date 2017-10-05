#include "bipartite_graph.h"
#include "graph.h"
#include "node.h"
#include "similarity_measure.h"
#include "libs.h"

bool verbose = false;

void usage()
{
	printf("./bipsim -b/-ba/-g <dataset> <out_directory> -m <similarity_measure> [-p <peripheric_value>] [-d <distance_value>]\n");
	printf("-b : bipartite graph\n-ba : bipartite (adjacency)\n-g : unipartite (adjacency)\n");
	printf("-d : depth value\n");
	//printf("-p : peripheric value of research of neighbor (default : 1 for unipartite, 2 for bipartite\n");
	printf("There is a list of available methods for similarity measure -m :\n");
	printf("cn : common neighbors\nji : jaccard index\naa : adamic-adar\nid : independence\nde : dependence\nra : ressource allocator\nlhn : LHN1 (Leicht-Holme-Newman Index)\n");
	exit(EXIT_FAILURE);
}

void graph_run(SIM* sim, string path_dataset, string method, float peripheric_threshold, string out_directory, int distance)
{
	Graph* graph = new Graph();
	graph->load_graph(path_dataset);
	
	sim->m = graph->m;
	sim->peripheric_threshold = peripheric_threshold;

	if(verbose)
	{
		printf("  Graph features\n");
		printf("n : %d\nlinks : %d\ndensity : %f\n", graph->n, graph->m, 1.0 * graph->m / (graph->n * (graph->n-1)));
	}

	int num_partition = 0;
	set<unsigned int> set_node_remaining, set_node_remaining_final;
	vector<Graph*> list_graph;
	list_graph.push_back(graph);

	Store_Partition* store_partition = new Store_Partition();
	string name_dataset = path_dataset.substr(path_dataset.find_last_of('/')+1, path_dataset.size());
	string filename_projection = name_dataset + "_" + method + ".projection";
	Save save(out_directory, "classic");
	int nb_community_found = 0;

	do
	{
		if(verbose)
			printf("\tComputing similarities | distance = %i\n", distance);	
		sim->similarity_projection(graph->list_node, distance);
		
		if(verbose)
			printf("\tDetecting communities\n");
		
		set_node_remaining = sim->find_community(graph->list_node, store_partition);
		
		if(verbose)
		{
			nb_community_found = sim->start_num_community - nb_community_found;
			printf("- Found %i communities (%i node(s) remaining)\n", nb_community_found, set_node_remaining.size());
		}

		distance--;
		graph = graph->get_subgraph(set_node_remaining);
		list_graph.push_back(graph);

		//sim->clean();

	} while(set_node_remaining.size() != 0 && distance >= 0);

	sim->aggregate(set_node_remaining, set_node_remaining_final);

	Partition* partition = new Partition();
	for(auto& e : sim->map_com)
	{
		partition->update(e.first, e.second);
	}
	store_partition->insert(partition);

	sim->community_isolted_nodes(set_node_remaining_final, store_partition);

	if(verbose)
		printf("\tSaving partition\n");
	store_partition->save_partitions(&save, list_graph[0]->list_node);

	for(auto g : list_graph)
		delete g;
}

void bipartite_run(SIM* sim, string path_dataset, string method, float peripheric_threshold, string out_directory, int distance, int bipartite)
{
	Bipgraph* bip = new Bipgraph();
	if(bipartite == 1)
		bip->load_bip(path_dataset);
	else
		bip->load_bip_adj(path_dataset);

	sim->m = bip->m;
	sim->peripheric_threshold = peripheric_threshold;

	if(verbose)
	{
		printf("  Bipartite graph features\n");
		printf("n : %d (n_top : %d | n_bot : %d)\nlinks : %d\ndensity : %f\n", bip->n, bip->n_top, bip->n_bot, bip->m, 1.0 * bip->m / (bip->n_top * bip->n_bot));
	}

	string name_dataset = path_dataset.substr(path_dataset.find_last_of('/')+1, path_dataset.size());

	/************* BIPARTITE computation *************/
	set<unsigned int> set_node_remaining, set_node_remaining_final;
	vector<Bipgraph*> list_bigraph;
	int num_partition, distance_tmp;

	/******** TOP part ********/
	list_bigraph.push_back(bip);
	distance_tmp = distance;
	num_partition = 0;

	Store_Partition* store_partition_top = new Store_Partition();
	string filename_projection_top = name_dataset + "_" + method + ".top_projection";
	Save save_top(out_directory, "top");
	int nb_community_found = 0;
	
	do
	{
		if(verbose)
			printf("\t[TOP] Computing similarities | distance = %i\n", distance_tmp);
		sim->similarity_projection(bip->list_top_node, distance_tmp);
		if(verbose)
			printf("\t[TOP] Detecting communities\n");
		set_node_remaining = sim->find_community(bip->list_top_node, store_partition_top);

		if(verbose)
		{
			nb_community_found = sim->start_num_community - nb_community_found;
			printf("- Found %i communities (%i node(s) remaining)\n", nb_community_found, set_node_remaining.size());
		}
		
		distance_tmp -= 2;
		bip = bip->get_subgraph(set_node_remaining, true);
		list_bigraph.push_back(bip);
		//sim->clean();
		
	} while(set_node_remaining.size() != 0 && distance_tmp > 0);

	sim->aggregate(set_node_remaining, set_node_remaining_final);

	Partition* partition = new Partition();
	for(auto& e : sim->map_com)
	{
		partition->update(e.first, e.second);
	}
	store_partition_top->insert(partition);

	sim->community_isolted_nodes(set_node_remaining_final, store_partition_top);

	if(verbose)
		printf("\t[TOP] Saving partition\n");
	store_partition_top->save_partitions(&save_top, list_bigraph[0]->list_node);
	
	for(auto b : list_bigraph)
		delete b;
	delete store_partition_top;

	/******** BOT part ********/
	/*bip_tmp = bip;
	distance_tmp = distance;

	Store_Partition* store_partition_bot = new Store_Partition();
	string filename_projection_bot = name_dataset + "_" + method + ".bot_projection";
	Save save_bot(out_directory, "bot");

	do
	{
		if(verbose)
			printf("\t[BOT] Computing similarities | distance = %i\n", distance_tmp);

		sim->similarity_projection(bip_tmp->list_bot_node, distance_tmp);
		set_node_remaining = sim->find_community(bip_tmp->list_bot_node, bip_tmp->store_partition_bot, bip_tmp);

		if(verbose)
		{
			if(bip_tmp->store_partition_bot->list_partition.size() > 0)
				printf("- Found %i communities (%i node(s) remaining)\n", bip_tmp->store_partition_bot->list_partition[0]->list_community.size(), set_node_remaining.size());
			else
				printf("- Found 0 community (%i node(s) remaining)\n", set_node_remaining.size());	
		}

		distance_tmp -= 2;
		bip_tmp = bip_tmp->get_subgraph(set_node_remaining, false);
		sim->clean();

		sim->save_projection(bip_tmp, "test");

	} while(set_node_remaining.size() != 0 && distance_tmp > 0);

	sim->community_isolted_nodes(bip_tmp->list_bot_node, set_node_remaining, bip_tmp->store_partition_bot);	

	if(verbose)
		printf("\t[BOT] Saving partition\n");
	bip->save_partition(&save_bot, bip->store_partition_bot, bip->overlapping_partition_bot);*/
}

int main(int argc, const char* argv[])
{
	if(argc < 5)
		usage();

	string method, path_dataset, out_directory;
	float peripheric_threshold = -1.0;
	int distance;
	int bipartite = -1;

	for(int i=1; i<argc; i++)
	{
		if(strcmp("-h", argv[i]) == 0)
		{
			usage();
			exit(EXIT_FAILURE);
		}
		if(strcmp("-v", argv[i]) == 0)
		{
			verbose = true;
		}
		else if(strcmp("-b", argv[i]) == 0)
		{
			bipartite = 1;
			distance = 2;
		}
		else if(strcmp("-ba", argv[i]) == 0)
		{
			bipartite = 2;
			distance = 2;
		}
		else if(strcmp("-g", argv[i]) == 0)
		{
			bipartite = 0;
			distance = 1;
		}
		else if(strcmp("-m", argv[i]) == 0)
		{
			method = argv[i+1];
			i++;
		}
		else if(strcmp("-p", argv[i]) == 0)
		{
			peripheric_threshold = atof(argv[i+1]);
			i++;
		}
		else if(strcmp("-d", argv[i]) == 0)
		{
			distance = atoi(argv[i+1]);
			i++;
		}
		else if(i == 2)
		{
			path_dataset = argv[i];
		}
		else if(i == 3)
		{
			out_directory = argv[i];
		}
	}

	if(bipartite == -1)
	{
		usage();
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));

	high_resolution_clock::time_point t1, t2;
	t1 = high_resolution_clock::now();

	if(method == "cn")
	{
		if(verbose)
			printf("\t[Similarity measure  : common neighbors]\n");
		COMP_CommonNeighbors cn(out_directory);
		if(bipartite == 0)
			graph_run(&cn, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&cn, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);
	}
	else if(method == "ji")
	{
		if(verbose)
			printf("\t[Similarity measure : jaccard index]\n");
		COMP_JaccardIndex ji(out_directory);
		if(bipartite == 0)
			graph_run(&ji, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&ji, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);			
	}
	else if(method == "aa")
	{
		if(verbose)
			printf("\t[Similarity measure : adamic-adar]\n");
		COMP_AdamicAdar aa(out_directory);
		if(bipartite == 0)
			graph_run(&aa, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&aa, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);			
	}
	else if(method == "id")
	{
		if(verbose)
			printf("\t[Similarity measure : independence]\n");
		COMP_Independence id(out_directory);
		if(bipartite == 0)
			graph_run(&id, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&id, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);
	}
	else if(method == "de")
	{
		if(verbose)
			printf("\t[Similarity measure : dependence]\n");
		COMP_Dependence de(out_directory);
		if(bipartite == 0)
			graph_run(&de, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&de, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);			
	}
	else if(method == "ra")
	{
		if(verbose)
			printf("\t[Similarity measure : ressource allocator]\n");
		COMP_RessourceAllocator ra(out_directory);
		if(bipartite == 0)
			graph_run(&ra, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&ra, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);
			
	}
	else if(method == "lhn")
	{
		if(verbose)
			printf("\t[Similarity measure : LHN1]\n");
		COMP_LHN1 lhn(out_directory);
		if(bipartite == 0)
			graph_run(&lhn, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&lhn, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);
			
	}
	else if(method == "mod")
	{
		if(verbose)
			printf("\t[Similarity measure : modularity]\n");
		COMP_MOD mod(out_directory);
		if(bipartite == 0)
			graph_run(&mod, path_dataset, method, peripheric_threshold, out_directory, distance);
		else
			bipartite_run(&mod, path_dataset, method, peripheric_threshold, out_directory, distance, bipartite);
	}
	else
	{
		printf("Bad similarity measure...\n");
		usage();
	}

	t2 = high_resolution_clock::now();
	display_ms(t1, t2);
	display_peakRSS();

	return 0;
}
