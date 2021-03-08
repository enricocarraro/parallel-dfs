#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>

#include "OPTIONS.h"

#include "Graph.h"
#include "Worker.h"

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

void start(int nWorkers, Graph *g) {

    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).initialize(g);
    }
    vector<thread> t_workers(nWorkers-1);

    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i] = thread(newStart, &allWorkers.at(i), i);
    }

    int indice = allWorkers.at(nWorkers-1).start2(nWorkers-1);

    sort_start_indexes(g, indice);

    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i].join();
    }


}


int main(int argc, const char *argv[]) {

#if MULTIRUN
    for(int i=0; i<100000; i++) {
#endif
        FILE *fp;

        // 1 parameter of format .gra is required.
        if (argc != 3) {
            cout << "Usage: " << argv[0] << " input_file output_file" << endl;
            return -1;
        }
        string graname(argv[1]);
        string outname(argv[2]);


        if ((fp = fopen(graname.c_str(), "r")) == NULL) {
            cout << "Error: File doesn't exist." << endl;
            return -1;
        }

        auto timeStart = std::chrono::steady_clock::now();

        Graph g(fp);
        fclose(fp);

        g.sortVectors();

        auto timeEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = timeEnd - timeStart;
        std::cout << "Creazione grafo: " << elapsed_seconds.count() << "s\n";

        timeStart = std::chrono::steady_clock::now();
        start(N_THREADS, &g);
        timeEnd = std::chrono::steady_clock::now();
        elapsed_seconds = timeEnd - timeStart;
#if MULTIRUN
        std::cout << i << ":\t";
#endif
        std::cout << "Esecuzione: " << elapsed_seconds.count() << "s\n";


        if ((fp = fopen(outname.c_str(), "w")) == NULL) {
            cout << "Error: File doesn't exist." << endl;
            return -1;
        }

        for (int i = 0; i < g.nNodes; i++) {
            fprintf(fp, "%d %d %d\n", i, g.nodes.at(i).start, g.nodes.at(i).end);
        }

        fclose(fp);
#if MULTIRUN
    }
#endif
    return 0;

}
