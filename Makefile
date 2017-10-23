STD = -std=c++11
NAME = comsim

all: $(NAME)

$(NAME): similarity_measure.o bipartite_graph.o node.o libs.o main.o graph.o partition.o
	g++ similarity_measure.o bipartite_graph.o node.o libs.o main.o graph.o partition.o -o $(NAME) $(STD)
#$(NAME): modularity.o similarity_measure.o bipartite_graph.o node.o libs.o main.o graph.o partition.o
#	g++ modularity.o similarity_measure.o bipartite_graph.o node.o libs.o main.o graph.o partition.o -o $(NAME) $(STD)

bipartite_graph.o : bipartite_graph.cpp
	g++ -c bipartite_graph.cpp $(STD) -O5

main.o : main.cpp
	g++ -c main.cpp $(STD) -O5

libs.o : libs.cpp
	g++ -c libs.cpp $(STD) -O5

modularity.o : modularity.cpp
	g++ -c modularity.cpp $(STD) -O5

similarity_measure.o : similarity_measure.cpp
	g++ -c similarity_measure.cpp $(STD) -O5

node.o : node.cpp
	g++ -c node.cpp $(STD) -O5

graph.o : graph.cpp
	g++ -c graph.cpp $(STD) -O5

partition.o : partition.cpp
	g++ -c partition.cpp $(STD) -O5	

clean:
	rm $(NAME) *.o
