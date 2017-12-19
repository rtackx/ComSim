STD = -std=c++11
NAME = comsim

all: $(NAME)

$(NAME): node.o main.o graph.o projection.o community.o
	g++ node.o main.o graph.o projection.o community.o -o $(NAME) $(STD) -lpthread

main.o : main.cpp
	g++ -c main.cpp $(STD) -O5

node.o : node.cpp
	g++ -c node.cpp $(STD) -O5

graph.o : graph.cpp
	g++ -c graph.cpp $(STD) -O5

projection.o : projection.cpp
	g++ -c projection.cpp $(STD) -O5

community.o : community.cpp
	g++ -c community.cpp $(STD) -O5

clean:
	rm $(NAME) *.o
