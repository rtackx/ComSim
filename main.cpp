#include "libs.h"
#include "graph.h"
#include "projection.h"
#include "community.h"

// void display_peakRSS()
// {
//     struct rusage r_usage;
//     getrusage(RUSAGE_SELF,&r_usage);
//     cerr <<  r_usage.ru_maxrss << endl;
// }

void display_ms(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2)
{
    auto duration = duration_cast<milliseconds>( t2 - t1 ).count();
    cerr << duration / 1000.0 << endl;
}

void usage()
{
	cerr << "Usage : comsim <link_list_file> <neighbor_distance> [similarity_index] [depth]" << endl << endl;

	cerr << "ComSim is a community detection algorithm." << endl;
	cerr << "A similarity index is used to measure similarities between nodes." << endl;
	cerr << "Basically the similarity is calculated between 2 nodes at a given distance." << endl;
	cerr << "This creates a new weighted graph on which is applied 2 steps :" << endl;
	cerr << "    1) Cycles are detected and considered as core communities. A cycle is a sequence of nodes which have the highest weights (similarities) all along its path." << endl;
	cerr << "    2) Based on communities previously detected, nodes that don't belong to any cycle are then placed to a neihbor community maximazing the sum of weights to that community." << endl;
	cerr << "After these 2 steps, if there are still remaining nodes and neighbor distance is greater than 0, then a new weighted graph is computed on the induced graph of the remaining nodes at a smaller distance." << endl << endl;

	cerr << "There is the list of available similarity indices (default is cn) :" << endl;
	cerr << "    cn : common neighbors" << endl;
	cerr << "    ji : jaccard index" << endl;
	cerr << "    aa : adamic-adar" << endl;
	cerr << "    ra : resource allocator" << endl;
	cerr << "    lhn : LHN1" << endl;
	cerr << "    pan : preferential attachment neighbor" << endl << endl;

	cerr << "A depth value can be chosen (default is 1). This value indicates how many cycles a node can be part of." << endl;
	cerr << "Choosing a depth value > 1 produces overlapping between communities." << endl << endl;

	cerr << "link_list_file : input network as a list of links" << endl; 
	cerr << "A link is : \nnode1 node2 [weight]" << endl << endl;

	cerr << "ComSim examples :" << endl;
	cerr << "\"./comsim uni_network_links 1 > detected_communities\" executes ComSim with cn similarity index between nodes at distance 1 on a unipartite network." << endl;
	cerr << "\"./comsim bip_links 2 aa > detected_communities\" executes ComSim with aa similarity index between nodes at distance 2 on a bipartite network. Since there is no common neighbor between 2 nodes at distance 1 in a bipartite network, it requires an initial distance > 1." << endl;
	cerr << "\"./comsim uni_network_links 1 cm 3 > detected_communities\" executes ComSim with cn similarity index between nodes at distance 1 on a unipartite network and it allows each node to be part of 3 cycles." << endl;
}

Projection* select_similarity(string similarity_index, Graph*& in_graph)
{
	if(similarity_index == "cn")
		return new CommonNeighbors(in_graph);
	else if(similarity_index == "ji")
		return new JaccardIndex(in_graph);
	else if(similarity_index == "aa")
		return new AdamicAdar(in_graph);
	else if(similarity_index == "ra")
		return new ResourceAllocator(in_graph);
	else if(similarity_index == "lhn")
		return new LHN1(in_graph);
	else if(similarity_index == "pan")
		return new PA_Neighbor(in_graph);
}

int main(int argc, const char* argv[])
{
	string path_dataset, similarity_index;
	int distance, depth_best;
	static set<string> list_valid_similarities = {"cn", "ji", "aa", "ra", "lhn", "pan"};

	if(argc < 3)
	{
		usage();
		return 0;
	}
	else
	{
		path_dataset = argv[1];
		distance = atoi(argv[2]);
		similarity_index = "cn";
		depth_best = 1;

		if(argc > 3)
		{
			similarity_index = argv[3];

			if(list_valid_similarities.find(similarity_index) == list_valid_similarities.end())
			{				
				usage();
				cerr << endl << "\"" << similarity_index << "\" is not a valid similarity index" << endl;
				return 0;
			}

			if(argc > 4)
			{
				if(argc == 5)
					depth_best = atoi(argv[4]);
				else
				{
					usage();
					return 0;
				}
			}			
		}
	}

	high_resolution_clock::time_point t1, t2;
	t1 = high_resolution_clock::now();
	
	Graph* main_graph = new Graph();
	cerr << "Loading graph..." << endl;
	main_graph->load_graph(path_dataset);
	cerr << "   n = " << main_graph->n << endl << "   m = " << main_graph->m/2 << endl;
	
	srand(unsigned(time(NULL)));

	Community* c = new Community(main_graph, depth_best);
	Projection* p;
	Graph* g;
	vector<unsigned int> list_ending_nodes;

	g = main_graph;
		
	p = select_similarity(similarity_index, g);
	cerr << endl << "Projecting graph (distance : " << distance << ") ..." << endl;
	p->project(distance);

	if(p->graph_projection->size_list_nodes > 0)
	{
		cerr << "Detecting communities..." << endl;
		c->detect(p->graph_projection, list_ending_nodes);

		//g = main_graph->get_subgraph(list_ending_nodes);
	}
	cerr << "Number of remaining nodes : " << list_ending_nodes.size() << endl;

	cerr << endl << "--------------" << endl;
	unsigned int i;
	for(auto& e : c->community)
	{
		for(i=0; i<e.second.size()-1; i++)
			cout << e.second[i]->id << " ";
		cout << e.second[e.second.size()-1]->id << endl;
	}
	cerr << "--------------" << endl;
	
	delete p;
	delete g;
	//delete main_graph;
	delete c;
	
	t2 = high_resolution_clock::now();
	cerr << "Time : ";
	display_ms(t1, t2);

	return 1;
}