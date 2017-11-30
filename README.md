# ComSim

ComSim is a community detection algorithm.
Given a input network a partition of nodes into communities is identified.
Several types of network (unipartite, bipartite, multipartite, multilayer) can be used.
If network has a bipartite structure, <neighbor_distance> should be set to 2 (or any power of 2). 

ComSim can also produce an output set of overlapping communities (see option [depth]).


Usage : comsim <link_list_file> <neighbor_distance> [similarity_index] [depth]

##Information :

A similarity index is used to measure similarities between nodes.
Basically the similarity is calculated between 2 nodes at a given distance.
This creates a new weighted graph on which is applied 2 steps :

    1) Cycles are detected and considered as core communities. In this context, a cycle is defined as a closed chain of nodes that has the highest weights (similarities) all along its path.
    2) Based on communities previously detected, nodes that don't belong to any cycle are then placed into a neighbor community maximazing the sum of link weights towards that community.
	
	After these 2 steps, if there are still remaining nodes and neighbor distance is greater than 0, then a new weighted graph is computed on the induced graph of the remaining nodes at a smaller distance.

##Parameters :

*link_list_file : input network as a list of links.
A link is : "node1 node2 [weight]"

*neighbor_distance : similarity projection between nodes at distance <neighbor_distance> (integer).

*similarity_index : a measurement of similarity between two nodes.

There is the list of available similarity indices (default is cn) :

	    cn : common neighbors
	    aa : adamic-adar
	    ji : jaccard index
	    ra : ressource allocator

*depth : a depth value can be chosen (default is 1). This value indicates how many cycles a node can be part of.
Choosing a depth value > 1 produces overlapping between communities.


##ComSim examples :

	"./comsim uni_network_links 1 > detected_communities" executes ComSim with cn similarity index between nodes at distance 1 on a unipartite network.
	"./comsim bip_links 2 aa > detected_communities" executes ComSim with aa similarity index between nodes at distance 2 on a bipartite network. Since there is no common neighbor between 2 nodes at distance 1 in a bipartite network, it requires an initial distance > 1.
	"./comsim uni_network_links 1 cn 3 > detected_communities" executes ComSim with cn similarity index between nodes at distance 1 on a unipartite network and it allows each node to be part of 3 cycles.
