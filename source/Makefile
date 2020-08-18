PROG = ../dfs
CC = g++
CPPFLAGS = -g -Wall -I./source 
OBJS = main.o Graph.o

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) $(OBJS) -lstdc++ -O3 	



main.o : Graph.o Semaphore.o Worker.o FeederManager.o EmptierManager.o
	$(CC) $(CPPFLAGS) -c main.cpp Timer.cpp Graph.o Semaphore.o Worker.o FeederManager.o EmptierManager.o -lstdc++

Worker.o : Worker.hpp Graph.o Semaphore.o
	$(CC) $(CPPFLAGS) -c Worker.cpp Graph.o Semaphore.o -lstdc++

FeederManager.o : FeederManager.hpp Graph.o Worker.o
	$(CC) $(CPPFLAGS) -c FeederManager.cpp Graph.o Worker.o -lstdc++

EmptierManager.o : EmptierManager.hpp FeederManager.o Worker.o
	$(CC) $(CPPFLAGS) -c EmptierManager.cpp FeederManager.o Worker.o -lstdc++

Graph.o : Graph.hpp
	$(CC) $(CPPFLAGS) -c Graph.cpp -lstdc++

Semaphore.o : Semaphore.hpp
	$(CC) $(CPPFLAGS) -c Semaphore.cpp -lstdc++



clean:
	rm -f $(PROG) $(OBJS)
