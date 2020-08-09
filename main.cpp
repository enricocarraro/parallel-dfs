//
//  main.cpp
//  ParallelDFS
//
//  Created by Enrico on 09/08/2020.
//  Copyright © 2020 Enrico. All rights reserved.
//

#include <iostream>
#include "Graph.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
 /*
    Graph grafo(5);
    grafo.addEdge(0, 4);
    grafo.addEdge(0, 1);
    grafo.addEdge(1, 2);
    grafo.addEdge(1, 4);
    grafo.addEdge(1, 3);
    grafo.addEdge(2, 3);
    grafo.addEdge(3, 4);
    grafo.sortVectors();
    grafo.printGraph();
    char c;
    cin >> c;
  */
    ifstream file;
    file.open("v10e3.que");
    Graph g(10);
    g.build(move(file));
    
    g.printGraph();
    return 0;


}
