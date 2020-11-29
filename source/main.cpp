#include <iostream>
#include <stdio.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <numeric>

#include "OPTIONS.h"

#include "Graph.h"
#include "Worker.h"
#if !USE_QUICK_SEM
#include "Semaphore.h"
#else
#include "FastSemaphore.h"
#endif

using namespace std;

void nodeSize(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->nodeSize();
}

void nodeWeights(Worker *worker, bool works_on_roots) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->nodeWeights(works_on_roots);
}

void nodeTimes(Worker *worker) {
    //cout << "Starting worker " << worker->getId() << "\n";
    worker->nodeTimes();
}


template<typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

    // initialize original index locations
    vector<size_t> idx(v.size()-1);
    iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    // using std::stable_sort instead of std::sort
    // to avoid unnecessary index re-orderings
    // when v contains elements of equal values
    stable_sort(idx.begin(), idx.end(),
                [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });

    return idx;
}


void reset(sharedData *sd) {
    sd->sq.reset();
    sd->nodeRead = 0;
    fill(sd->visitedNeighbours.begin(), sd->visitedNeighbours.end(), 0);
    sd->g->posIntoPreLeaves = 0;
//    sd->g->posIntoRoots = 0;
}

void start(int nWorkers, Graph *g) {

    sharedData sd(g, nWorkers);

    vector<Worker> allWorkers(nWorkers);
    for (int i = 0; i < nWorkers; i++) {
        allWorkers.at(i).initialize(&sd);
    }



    //pre phase
#if VERBOSE_TIMERS
    auto istart = std::chrono::steady_clock::now();
#endif

    vector<thread> t_workers(nWorkers);

    for (int i = 0; i < nWorkers; i++) {
        t_workers[i] = thread(nodeSize, &allWorkers.at(i));
    }

    for (int i = 0; i < nWorkers; i++) {
        t_workers[i].join();
    }

    reset(&sd);

#if VERBOSE_TIMERS
    auto iend = std::chrono::steady_clock::now();
    std::chrono::duration<double> ielapsed_seconds = iend - istart;
    std::cout << "Pre-subsize elapsed time: " << ielapsed_seconds.count() << "s\n";
#endif



    //first phase
#if VERBOSE_TIMERS
    auto start = std::chrono::steady_clock::now();
#endif


    t_workers[0] = thread(nodeWeights, &allWorkers.at(0), true);
    for (int i = 1; i < nWorkers; i++) {
        t_workers[i] = thread(nodeWeights, &allWorkers.at(i), false);
    }

    for (int i = 0; i < nWorkers; i++) {
        t_workers[i].join();
    }

    reset(&sd);


#if VERBOSE_TIMERS
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Weights elapsed time: " << elapsed_seconds.count() << "s\n";
#endif



    //second phase
#if VERBOSE_TIMERS
    start = std::chrono::steady_clock::now();
#endif

    vector<size_t> tmp = sort_indexes(g->nodes);
    for (int i=0; i<tmp.size(); i++) {
        g->nodes.at(static_cast<int> (tmp.at(i))).end = i+1;
    }

#if VERBOSE_TIMERS
    end = std::chrono::steady_clock::now();
    elapsed_seconds = end - start;
    std::cout << "End elapsed time: " << elapsed_seconds.count() << "s\n";
#endif

#if CATCH_BIGGEST_INT
    int indice = static_cast<int>(tmp.at(tmp.size()-1));
    boost::multiprecision::uint1024_t check = g->nodes.at(indice).nodeWeight;
#endif



    //third phase
#if VERBOSE_TIMERS
    start = std::chrono::steady_clock::now();
#endif

    for (int i = 0; i < nWorkers; i++) {
        t_workers[i] = thread(nodeTimes, &allWorkers.at(i));
    }

    for (int i = 0; i < nWorkers; i++) {
        t_workers[i].join();
    }

#if VERBOSE_TIMERS
    end = std::chrono::steady_clock::now();
    elapsed_seconds = end - start;
    std::cout << "Labels elapsed time: " << elapsed_seconds.count() << "s\n";
#endif



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
    for(int i=0; i<100; i++) {
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
        g.printTrueLabelsPreWeights();      //prints everything
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
