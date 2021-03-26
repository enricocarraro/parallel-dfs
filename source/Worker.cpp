//
// Created by lire on 8/19/20.
//

#include "Worker.h"


using namespace std;


void Worker::initialize(Graph *g) {
    this->g = g;
}

void insert(int epoca, int questoLivello, int doveInserire, Node *N, Node *visitedNode) {
    N->nextNode.at(doveInserire) = visitedNode;
    visitedNode->visitatoInQuestoLivello.at(questoLivello) = epoca;
    N->pronto.at(doveInserire) = true;
}

int Worker::start(int position) {
    int epocaAttuale = 0;
    int epocaSuccessiva = 1;
    char compiuteModifiche = true;
    //int prevInt;
    Node *prevNode;
    Node *N;
    int epoca = position;

    if (epoca == 0) {
        //prevInt = g->startingNode;
        //prevNode = &g->nodes.at(prevInt);
        prevNode = g->startNode;
        prevNode->visitatoInQuestoLivello.at(epocaAttuale) = 0;
        for (int R : g->roots) {
            N = &g->nodes.at(R);

            for (Node* C1 : N->adjN) {
                if(C1->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {
                    insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, C1);

                    C1->visitato.at(epocaSuccessiva) = false;
                    prevNode = C1;
                }
            }

            insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, N);

            N->visitato.at(epocaSuccessiva) = true;
            prevNode = N;
        }
        for(int i=0; i<N_THREADS+1; i++) {
            prevNode->visitato.at(i) = true;
            prevNode->nextNode.at(i) = &g->terminator;
            prevNode->pronto.at(i) = true;
        }
        epoca += N_THREADS;
    }


    while(compiuteModifiche == true) {
        //prevInt = g->startingNode;
        //prevNode = &g->nodes.at(prevInt);
        prevNode = g->startNode;
        epocaAttuale = epoca%(N_THREADS+1);
        epocaSuccessiva = (epoca+1)%(N_THREADS+1);

        compiuteModifiche = false;

        while(!prevNode->pronto.at(epocaAttuale)) {}
        while(prevNode->nextNode.at(epocaAttuale) != &g->terminator) {
            prevNode->pronto.at(epocaAttuale) = false;

            N = prevNode->nextNode.at(epocaAttuale);
            while (!N->pronto.at(epocaAttuale)) {}
            if(N->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {
                if(!N->visitato.at(epocaAttuale)) {
                    for(Node* C2 : N->adjN) {
                        if(C2->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

                            insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, C2);

                            C2->visitato.at(epocaSuccessiva) = false;
                            prevNode = C2;

                            compiuteModifiche = true;
                        }
                    }
                }

                insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, N);

                N->visitato.at(epocaSuccessiva) = true;
                prevNode = N;
            }
            else {
                N->pronto.at(epocaAttuale) = false;

                prevNode->nextNode.at(epocaAttuale) = N->nextNode.at(epocaAttuale);
                //prevNode->pronto.at(epocaAttuale) = true;
            }
        }
        epoca = (epoca+N_THREADS);
    }
    return epocaAttuale;
}