//
// Created by lire on 8/19/20.
//

#include "Worker.h"


using namespace std;


void Worker::initialize(Graph *g) {
    this->g = g;
}

void insert(int epoca, int questoLivello, int doveInserire, Node *N, int nextNode, Node *visitedNode) {
    N->nextNode.at(doveInserire) = nextNode;
    visitedNode->visitatoInQuestoLivello.at(questoLivello) = epoca;
    N->pronto.at(doveInserire) = true;
}

#if PRINT_MAT
void popMat(Graph *g, int epoca, int questoLivello) {
    Node *N = &g->nodes.at(g->nNodes);
    int i=0, val;
    while(N->nextNode.at(questoLivello) != -2) {
        val = N->nextNode.at(questoLivello);
        N = &g->nodes.at(val);
        g->mat.at(epoca).at(i++) = val;
    }
}
#endif

int Worker::start(int position) {
    int epocaAttuale = 0;
    int epocaSuccessiva = 1;
    char compiuteModifiche = true;
    int prevInt;
    Node *prevNode;
    Node *Rn, *C1, *C2;
    int N1, N2;
    int epoca = position;

    if (epoca == 0) {
        prevInt = g->startingNode;
        prevNode = &g->nodes.at(prevInt);
        prevNode->visitatoInQuestoLivello.at(epocaAttuale) = true;
        for (int R : g->roots) {
            Rn = &g->nodes.at(R);
            for (int i = 0; i < Rn->adjSize; i++) {
                N1 = Rn->adj.at(i);
                C1 = &g->nodes.at(N1);
                if(C1->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {
                    insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, N1, C1);

                    C1->visitato.at(epocaSuccessiva) = false;
                    //prevInt = N1;   //never used
                    prevNode = C1;
                }
            }
            insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, R, Rn);

            Rn->visitato.at(epocaSuccessiva) = true;
            //prevInt = R;    //never used
            prevNode = Rn;
        }
        for(int i=0; i<N_THREADS+1; i++) {
            prevNode->visitato.at(i) = true;
            prevNode->nextNode.at(i) = -2;
            prevNode->pronto.at(i) = true;
        }
        epoca += N_THREADS;
    }


    while(compiuteModifiche == true) {
        prevInt = g->startingNode;
        prevNode = &g->nodes.at(prevInt);
        epocaAttuale = epoca%(N_THREADS+1);
        epocaSuccessiva = (epoca+1)%(N_THREADS+1);

        compiuteModifiche = false;

        while(prevNode->nextNode.at(epocaAttuale) != -2) {
            while(!prevNode->pronto.at(epocaAttuale)) {}
            prevNode->pronto.at(epocaAttuale) = false;

            N1 = prevNode->nextNode.at(epocaAttuale);
            C1 = &g->nodes.at(N1);
            while (!C1->pronto.at(epocaAttuale)) {}
            if(C1->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

                if (!C1->visitato.at(epocaAttuale)) {
                    for (int i = 0; i < C1->adjSize; i++) {
                        N2 = C1->adj.at(i);
                        C2 = &g->nodes.at(N2);
                        if(C2->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

                            insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, N2, C2);

                            C2->visitato.at(epocaSuccessiva) = false;
                            //prevInt = N2;   //never used
                            prevNode = C2;

                            compiuteModifiche = true;
#if PRINT_MAT
                            printf("%d: %d -> %d, %d\n", epoca, N1, N2, C1->visitato.at(epocaAttuale));
                            if(C1->visitato.at(epocaAttuale)) {
                                printf("Errore");
                            }
#endif
                        }
                    }
                }
                insert(epoca, epocaAttuale, epocaSuccessiva, prevNode, N1, C1);

                C1->visitato.at(epocaSuccessiva) = true;
                //prevInt = N1;   //never used
                prevNode = C1;
            }
            else {
                C1->pronto.at(epocaAttuale) = false;

                N2 = C1->nextNode.at(epocaAttuale);
                //C2 = &g->nodes.at(N2);  //never used
                prevNode->nextNode.at(epocaAttuale) = N2;
                prevNode->pronto.at(epocaAttuale) = true;
            }
        }
        //prevNode->epoca.at(epocaSuccessiva) = epoca;
#if PRINT_MAT
        popMat(g, epoca, epocaAttuale);
#endif
        epoca = (epoca+N_THREADS);
    }
    return epocaAttuale;
}