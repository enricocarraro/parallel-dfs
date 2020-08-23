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



int main(int argc, const char * argv[]) {
    FILE * fp;
    
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
    
    auto istart = std::chrono::steady_clock::now();
    Graph g(fp);
    fclose(fp);
   
    auto iend = std::chrono::steady_clock::now();
    std::chrono::duration<double> ielapsed_seconds = iend - istart;
    std::cout << "Init elapsed time: " << ielapsed_seconds.count() << "s\n";
    g.sortVectors();
    g.printGraph();
    return 0;
}
