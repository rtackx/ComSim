# ComSim

ComSim is a community detection algorithm for :
- unipartite networks
- bipartite networks
- any network depicting a bipartite structure

ComSim { COMMUNITY SIMILARITY }. uses a similarity measure to calculate similiarity between nodes (see option -m).

Usage :  ./bipsim  -b/-ba/-g <dataset> <out_directory> -m <similarity_measure>

----=== Data format ===----
# unipartite (-g) => list of edges : 
v1 v2 [weight]\n
...
# bipartite (-b) => (v1,v2,..,vN) top nodes - (w1,w2,...,wN) bot nodes :
v1 w1 w2 w3\n
v2 w2 w3 w4\n
v3 w1 w4\n
...
# bipartite with weights (-ba) => (v1,v2,..,vN) top nodes - (w1,w2,...,wN) bot nodes :
v1 w1 weight\n
v1 w2 weight\n
v1 w3 weight\n
v2 w2 weight\n
...
