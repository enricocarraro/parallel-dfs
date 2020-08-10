//
//  main.cpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "Graph.hpp"
#include "Timer.cpp"

using namespace std;


#define E 50000
#define V 10000000

int main(int argc, const char * argv[]) {
    /*FILE * fp;
    
    // 1 parameter of format .gra is required.
    if(argc != 2) {
        cout << "Error: Missing parameter" << endl;
        return -1;
    }
    string graname(argv[1]);
    
    if((fp = fopen(graname.c_str(), "r")) == NULL) {
        cout << "Error: File doesn't exist." << endl;
        return -1;
    }
    
    Graph g(fp);
    fclose(fp);
    
    g.sortVectors();
    g.printGraph();*/
	
	Graph grafo(E);
	vector<int> adj[E];
	int ran, val1, val2;
	srand(time(nullptr));
	ran = rand();
	
	Timer t;
	t.start();
	
	for (int i = 0; i < V; i++)
	{
		srand(ran);
		ran = rand();
		val1 = ran % E;
		srand(ran);
		ran = rand();
		val2 = ran % E;
		grafo.addEdge(val1, val2);
	}
	
	t.stop();
	t.printElapsed();
    return 0;


}
