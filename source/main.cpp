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
    
    {
        auto istart = std::chrono::steady_clock::now();
        Graph gp(fp);
        fclose(fp);
        gp.sortVectors();
        auto iend = std::chrono::steady_clock::now();
        //gp.printGraph();
        std::chrono::duration<double> ielapsed_seconds = iend - istart;
        std::cout << "Init elapsed time: " << ielapsed_seconds.count() << "s\n";
        auto start = std::chrono::steady_clock::now();
        gp.buildDT();
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "DT elapsed time: " << elapsed_seconds.count() << "s\n";
        //gp.printNodesStatus();
        auto sstart = std::chrono::steady_clock::now();
        gp.computeSubGraphSize();
        auto send = std::chrono::steady_clock::now();
        std::chrono::duration<double> selapsed_seconds = send-sstart;
        std::cout << "subgraph size elapsed time: " << selapsed_seconds.count() << "s\n";
        gp.printNodesStatus();
        //gp.printGraph();
    }
    
    if((fp = fopen(graname.c_str(), "r")) == NULL) {
        cout << "Error: File doesn't exist." << endl;
        return -1;
    }
    
    {
        auto istart = std::chrono::steady_clock::now();
        Graph gs(fp);
        fclose(fp);
        gs.sortVectors();
        auto iend = std::chrono::steady_clock::now();
        std::chrono::duration<double> ielapsed_seconds = iend - istart;
        std::cout << "Init elapsed time: " << ielapsed_seconds.count() << "s\n";
        auto start = std::chrono::steady_clock::now();
        gs.sequentialDFS();
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::cout << "Sequential elapsed time: " << elapsed_seconds.count() << "s\n";
       // gs.printNodesStatus();
    }
    return 0;
    
    
}
