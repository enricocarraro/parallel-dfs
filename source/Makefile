PROG = ../dfs
CC = g++
CPPFLAGS = -g -Wall -I./source
OBJS = main.o Graph.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) -lstdc++



main.o : Graph.o
	$(CC) $(CPPFLAGS) -c main.cpp Graph.o -lstdc++

Graph.o : Graph.hpp
	$(CC) $(CPPFLAGS) -c Graph.cpp -lstdc++


clean:
	rm -f $(PROG) $(OBJS)
