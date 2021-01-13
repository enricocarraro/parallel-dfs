#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>

#include "OPTIONS.h"

#include "Graph.h"
#include "Worker.h"
/*
#if !USE_QUICK_SEM
#include "Semaphore.h"
#else
#include "FastSemaphore.h"
#endif
*/

using namespace std;

void newStart(Worker *worker, int pos) {
    worker->start(pos);
}


void sort_start_indexes(Graph *g, int indice) {
    int i=1;
    Node *prevNode = &g->nodes.at(g->nNodes);
    Node *currNode;
    while(prevNode->nextNode.at(indice) != -2) {
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

#if PRINT_MAT
void printMat(Graph *g) {
    for(int i=0; i<g->mat.size(); i++) {
        for(int j=0; j<g->nNodes; j++) {
            printf("%d ", g->mat.at(i).at(j));
        }
        printf("\n");
    }
}
#endif

void start(int nWorkers, Graph *g) {

    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).initialize(g);
    }
    vector<thread> t_workers(nWorkers-1);

    auto timeStart = std::chrono::steady_clock::now();

    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i] = thread(newStart, &allWorkers.at(i), i);
    }


    int indice = allWorkers.at(nWorkers-1).start(nWorkers-1);

    auto timeEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = timeEnd - timeStart;
    std::cout << "Interesting elapsed time: " << elapsed_seconds.count() << "s\n";

    sort_start_indexes(g, indice);

#if PRINT_MAT
    printMat(g);
#endif


    for (int i = 0; i < nWorkers-1; i++) {
        t_workers[i].join();
    }


}


int main(int argc, const char *argv[]) {
    FILE *fp;

#if QUICK_RUN
    string graname;
    switch (FILE_N) {
        case 0:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v4e2.gra");
            break;
        case 1:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v10e3.gra");
            break;
        case 2:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v10e3v2.gra");
            break;
        case 3:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v10e3v3.gra");
            break;
        case 4:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v5000e50.gra");
            break;
        case 5:
            graname = string("/home/lire/CLionProjects/sdp_pipelineReSolution/v100000e100.gra");
            break;
    }
#if PRINT_RESULT
    string outname;
    outname = string("/home/lire/CLionProjects/Results/out.txt");
#endif
#else
#if !PRINT_RESULT
    // 1 parameter of format .gra is required.
    if (argc != 2) {
        cout << "Error: Missing parameter" << endl;
        return -1;
    }
    string graname(argv[1]);
#else
    // 1 parameter of format .gra is required.
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " input_file output_file" << endl;
        return -1;
    }
    string graname(argv[1]);
    string outname(argv[2]);
#endif
#endif

#if REPEAT_TEST
    for(int i=0; i<10000; i++) {
        std::cout << i << ": " << std::endl;
#endif

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
        std::cout << "Graph creation elapsed time: " << elapsed_seconds.count() << "s\n";

        timeStart = std::chrono::steady_clock::now();

        start(N_THREADS, &g);

        timeEnd = std::chrono::steady_clock::now();
        elapsed_seconds = timeEnd - timeStart;
        std::cout << "All calculations elapsed time: " << elapsed_seconds.count() << "s\n\n";

#if FILE_N <= 3
        //g.printTrueLabelsPreWeights();      //prints everything
#endif

#if PRINT_RESULT

        if ((fp = fopen(outname.c_str(), "w")) == NULL) {
            cout << "Error: File doesn't exist." << endl;
            return -1;
        }

        for (int i = 0; i < g.nNodes; i++) {
            fprintf(fp, "%d %d %d\n", i, g.nodes.at(i).start, g.nodes.at(i).end);
        }

        fclose(fp);

#endif

        //std::cout << g.var << std::endl;
#if REPEAT_TEST
    }
#endif
    return 0;

}
