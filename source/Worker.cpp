//
// Created by lire on 8/19/20.
//

#include "Worker.h"



using namespace boost::multiprecision;
using namespace std;

void Worker::initialize(int id, int nWorkers, sharedData *sd) {
    //this->id = id;
    this->g = sd->g;
    this->graphSize = g->nNodes;
    this->sd = sd;
}


void Worker::nodeSize() {

    int leaf;
    int preLeavesSize = g->preLeaves.size();
    Node *n;

    int nodeRead;
    int i = g->posIntoPreLeaves.fetch_add(1);

    while(i < preLeavesSize) {
        leaf = g->preLeaves.at(i);
        n = &g->nodes.at(leaf);
        for(int j=0; j<n->ancSize; j++) {
            int ancestor = n->ancestors->at(j);
            Node *father = &g->nodes.at(ancestor);
            father->bSem->wait();
            father->nodeWeight += n->nodeWeight;
            father->bSem->signal();
            int visited = sd->visitedNeighbours.at(ancestor).fetch_add(1);
            if (visited == father->adjSize-1) {
                sd->sq.push(ancestor);
                sd->nodeRead++;
            }
        }
        nodeRead = sd->nodeRead.fetch_add(1);
        i = g->posIntoPreLeaves.fetch_add(1);
    }

    while (nodeRead < graphSize) {
        n = &g->nodes.at(sd->sq.pop());
        for(int j=0; j<n->ancSize; j++) {
            int ancestor = n->ancestors->at(j);
            Node *father = &g->nodes.at(ancestor);
            father->bSem->wait();
            father->nodeWeight += n->nodeWeight;
            father->bSem->signal();
            int visited = sd->visitedNeighbours.at(ancestor).fetch_add(1);
            if (visited == father->adjSize-1) {
                sd->sq.push(ancestor);
                sd->nodeRead++;
            }
        }
        nodeRead = sd->nodeRead.load();
    }
    sd->sq.push(graphSize);

}

void Worker::nodeWeights(bool works_on_roots) {
    boost::multiprecision::cpp_int time;

    if(works_on_roots) {
        time = 0;
        for(int R : sd->g->roots) {
            Node *Rn = &sd->g->nodes.at(R);
            Rn->time = time;
            time += Rn->nodeWeight;
            Rn->nodeWeight = time;
            sd->sq.push(R);
            sd->nodeRead++;
        }
    }

    while(sd->nodeRead < graphSize) {
        Node *N = &sd->g->nodes.at(sd->sq.pop());
        time = N->time;
        for(int i=0; i<N->adjSize; i++) {
            int F = N->adj->at(i);
            Node *Fn = &sd->g->nodes.at(F);
            Fn->bSem->wait();
            if (time < Fn->time) {
                Fn->time = time;
                time += Fn->nodeWeight;
                //Fn.nodeWeight = time;
                Fn->bSem->signal();
            } else {
                Fn->bSem->signal();
                time += Fn->nodeWeight;
            }
            int visited = sd->visitedNeighbours.at(F).fetch_add(1, std::memory_order_relaxed);
            if (visited == Fn->ancSize-1) {
                Fn->nodeWeight += Fn->time;
                sd->sq.push(F);
                sd->nodeRead++;
            }
        }
    }
    sd->sq.push(graphSize);

}

void Worker::nodeTimes() {


    int leaf;
    int preLeavesSize = g->preLeaves.size();
    Node *n;

    int nodeRead;

    int i = g->posIntoPreLeaves.fetch_add(1);

    while(i < preLeavesSize) {
        leaf = g->preLeaves.at(i);
        n = &g->nodes.at(leaf);
        n->start = n->end;
        int start = n->start;
        for(int j=0; j<n->ancSize; j++) {
            int ancestor = n->ancestors->at(j);
            Node *father = &g->nodes.at(ancestor);
            father->bSem->wait();
            if (father->start > start) {
                father->start = start;
            }
            father->bSem->signal();
            int visited = sd->visitedNeighbours.at(ancestor).fetch_add(1);
            if (visited == father->adjSize-1) {
                sd->sq.push(ancestor);
                sd->nodeRead++;
            }
        }
        nodeRead = sd->nodeRead.fetch_add(1);
        i = g->posIntoPreLeaves.fetch_add(1);
    }

    while (nodeRead < graphSize) {
        n = &g->nodes.at(sd->sq.pop());
        int start = n->start;
        for(int j=0; j<n->ancSize; j++) {
            int ancestor = n->ancestors->at(j);
            Node *father = &g->nodes.at(ancestor);
            father->bSem->wait();
            if (father->start > start) {
                father->start = start;
            }
            father->bSem->signal();
            int visited = sd->visitedNeighbours.at(ancestor).fetch_add(1);
            if (visited == father->adjSize-1) {
                sd->sq.push(ancestor);
                sd->nodeRead++;
            }
        }
        nodeRead = sd->nodeRead.load();
    }
    sd->sq.push(graphSize);

}