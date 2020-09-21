//
// Created by lire on 8/25/20.
//

#include "recalST.h"

using namespace std;

void recalST::init(Graph *g) {
    this->g = g;
}

void recalST::work() {
    //semaforo
    /*
    vector<intint> *ce = g->cancelledEdges;
    vector<intVet> *st = g->st_father;
    vector<bool> *modified = g->modified;



    for(int i=ce->size()-1; i>=0; i--) {
        int father = ce->at(i).father, child = ce->at(i).child;

        iterFather(father, child);
    }
    for(int i=0; i<g->nNodes; i++) {
        if(modified->at(i) == true) {
            g->nodes.at(i).start = st->at(i).start;
        }
    }
     */
}

void recalST::iterFather (int father, int child) {
    /*
    vector<intVet> *st = g->st_father;
    vector<bool> *modified = g->modified;

    if(st->at(father).start > st->at(child).start) {
        st->at(father).start = st->at(child).start;
        modified->at(father) = true;
        st->at(child).fathers->push_back(father);
        for(int i=0; i<st->at(father).fathers->size(); i++) {
            if(st->at(father).fathers->at(i) != -1) {
                iterFather(st->at(father).fathers->at(i), father);
            }
        }
    }
     */
}