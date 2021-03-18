#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>

#include "OPTIONS.h"

#include "Graph.h"
#include "Worker.h"
#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>

bool compareFiles(const std::string& p1, const std::string& p2) {
    std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
        return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
}

using namespace std;

void newStart(Worker *worker, int pos) {
    worker->start2(pos);
}


void sort_start_indexes(Graph *g, int indice) {
    int i=1;
    Node *prevNode = &g->nodes.at(g->nNodes);
    Node *currNode;
    while(prevNode->nextNode.at(indice) != g->endingNode) {
        currNode = &g->nodes.at(prevNode->nextNode.at(indice));
        int start = i++;
        currNode->end = start;
        for(int j=0; j<currNode->adjSize; j++){
            Node *tmp = &g->nodes.at(currNode->adj.at(j));
            if(start > tmp->start) {
                start = tmp->start;
            }
        }
        currNode->start = start;
        prevNode = currNode;
    }
}

int start(int nWorkers, Graph *g) {

    BusySemaphore *sem = new BusySemaphore(false);
    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).initialize(g);
        allWorkers.at(i).sem = sem;
    }
    vector<thread> t_workers(nWorkers-1);

    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i] = thread(newStart, &allWorkers.at(i), i);
    }

    int indice = allWorkers.at(nWorkers-1).start2(nWorkers-1);

    sort_start_indexes(g, indice%(N_THREADS+1));

    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i].join();
    }

    return indice;

}


int main(int argc, const char *argv[]) {

#if DEBUG
    int indicePrecedente;
#endif
    /*
    // 1 parameter of format .gra is required.
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " input_file output_file" << endl;
        return -1;
    }
    string graname(argv[1]);
    string outname(argv[2]);
    */


    if (argc != 2) {
        cout << "Usage: " << argv[0] << " file_name" << endl;
        return -1;
    }
    string pathSourceName = "/home/lire/CLionProjects/sdp_pipelineReSolution/";
    string pathDstName = "/home/lire/CLionProjects/Results/out";
    string graname(argv[1]);
    string srcName = pathSourceName + graname + ".gra";
    string dstName = pathDstName + ".txt";
    string checkName = pathDstName + "_" + graname + ".txt";
    //string outname(argv[2]);
    //bool termina = false;

    for(int i=0; i<MULTIRUN; i++) {

        FILE *fp;


        if ((fp = fopen(srcName.c_str(), "r")) == NULL) {
            cout << "Error: File doesn't exist." << endl;
            return -1;
        }

        auto timeStart = std::chrono::steady_clock::now();

        Graph *g = new Graph(fp);
        fclose(fp);

        g->sortVectors();

        auto timeEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = timeEnd - timeStart;
        std::cout << "Creazione grafo: " << elapsed_seconds.count() << "s\n";

        timeStart = std::chrono::steady_clock::now();
        int check = start(N_THREADS, g);
        timeEnd = std::chrono::steady_clock::now();
        elapsed_seconds = timeEnd - timeStart;

        std::cout << i << ":\t";
        std::cout << "Esecuzione: " << elapsed_seconds.count() << "s\n";


        if ((fp = fopen(dstName.c_str(), "w")) == NULL) {
            cout << "Error: File doesn't exist." << endl;
            return -1;
        }

        for (int i = 0; i < g->nNodes; i++) {
            fprintf(fp, "%d %d %d\n", i, g->nodes.at(i).start, g->nodes.at(i).end);
        }

        fclose(fp);

        //if(!compareFiles(outname, "/home/lire/CLionProjects/Results/out_v5000e50.txt")) {
        if(!compareFiles(dstName, checkName)) {
            printf("Errore\n");
            //termina = true;
        }
        /*else {
            int indicePrecedente = check;
        }*/

        delete g;
    }
    return 0;

}
