PROG = ../dfs
CC = g++
CPPFLAGS = -g -std=c++17 -Wall -I./source 
OBJS = main.o Graph.o SafeQueue.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) -lstdc++ -O3 	



main.o : Graph.o
	$(CC) $(CPPFLAGS) -c main.cpp Graph.o -lstdc++

Graph.o : Graph.hpp SafeQueue.o
	$(CC) $(CPPFLAGS) -c Graph.cpp SafeQueue.o  -lstdc++

SafeQueue.o : SafeQueue.hpp
	$(CC) $(CPPFLAGS) -c SafeQueue.cpp -lstdc++

clean:
	rm -f $(PROG) $(OBJS)
