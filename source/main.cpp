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
    
    
    
    
    
    
    Graph gp(fp);
    fclose(fp);
    
    gp.sortVectors();
    
    Timer tp;
    tp.start();
    gp.buildDT();
    tp.stop();
    gp.printNodesStatus();
    tp.printElapsed();
    
    
   
    
    if((fp = fopen(graname.c_str(), "r")) == NULL) {
        cout << "Error: File doesn't exist." << endl;
        return -1;
    }
    
    
    Graph gs(fp);
    fclose(fp);
    gs.sortVectors();
    
    Timer ts;
    ts.start();
    gs.sequentialDFS();
    ts.stop();
    gs.printNodesStatus();
    ts.printElapsed();
    
    
    
    return 0;
    
    
}
