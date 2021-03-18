//
// Created by lire on 8/19/20.
//

#include "Worker.h"


using namespace std;


void Worker::initialize(Graph *g) {
    this->g = g;
}

void Worker::insertGrigio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {

    sem->wait();
#if DEBUG
    /*if(toBeInserted->pronto.at(doveInserire) == epoca-1) {
        printf("r ");
        std::cout << "Grigio toBeInserted errore\t\t\t" << toBeInserted->id << " " <<  toBeInserted->countdown << endl;
    }
    if(prevNodeToBeVisited->pronto.at(doveInserire) == epoca-1) {
        printf("r ");
        std::cout << "Grigio prevNodeToBeVisited errore\t" << prevNodeToBeVisited->id << " " <<  prevNodeToBeVisited->countdown << endl;
    }
    if(prevNode->pronto.at(doveInserire) == epoca-1) {
        printf("r ");
        std::cout << "Grigio prevNode errore\t\t\t\t" << prevNode->id << " " <<  prevNode->countdown << endl;
    }
    if(nextNode->pronto.at(doveInserire) == epoca-1) {
        printf("r ");
        std::cout << "Grigio nextNode errore\t\t\t\t" << nextNode->id << " " <<  nextNode->countdown << endl;
    }
    if(nextNodeToBeInserted->pronto.at(doveInserire) == epoca-1) {
        printf("r ");
        std::cout << "Grigio nextNodeToBeInserted errore\t" << nextNodeToBeInserted->id << " " <<  nextNodeToBeInserted->countdown << endl;
    }*/
#endif


    prevNodeToBeVisited->nextNodeToVisit.at(doveInserire) = nextNodeToBeInserted->id;
    nextNodeToBeInserted->prevNodeToVisit.at(doveInserire) = prevNodeToBeVisited->id;

    toBeInserted->nextNode.at(doveInserire) = nextNode->id;
    nextNode->prevNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->prevNode.at(doveInserire) = prevNode->id;
    prevNode->nextNode.at(doveInserire) = toBeInserted->id;

    int ultimaEpoca = toBeInserted->ultimaEpoca->load();
    while(!toBeInserted->ultimaEpoca->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));
    sem->signal();

}

void Worker::insertGiallo(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {

    sem->wait();

#if DEBUG
    /*if(toBeInserted->pronto.at(doveInserire) == epoca-1 && epoca!=0) {
        printf("r ");
        std::cout << "toBeInserted errore\t\t\t" << toBeInserted->id << " " <<  toBeInserted->countdown << endl;
    }
    if(prevNodeToBeVisited->pronto.at(doveInserire) == epoca-1 && epoca!=0) {
        printf("r ");
        std::cout << "prevNodeToBeVisited errore\t" << prevNodeToBeVisited->id << " " <<  prevNodeToBeVisited->countdown << endl;
    }
    if(prevNode->pronto.at(doveInserire) == epoca-1 && epoca!=0) {
        printf("r ");
        std::cout << "prevNode errore\t\t\t\t" << prevNode->id << " " <<  prevNode->countdown << endl;
    }
    if(nextNode->pronto.at(doveInserire) == epoca-1 && epoca!=0) {
        printf("r ");
        std::cout << "nextNode errore\t\t\t\t" << nextNode->id << " " <<  nextNode->countdown << endl;
    }
    if(nextNodeToBeInserted->pronto.at(doveInserire) == epoca-1 && epoca!=0) {
        printf("r ");
        std::cout << "nextNodeToBeInserted errore\t" << nextNodeToBeInserted->id << " " <<  nextNodeToBeInserted->countdown << endl;
    }*/
#endif


    /*
    if(toBeInserted->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }
    if(prevNodeToBeVisited->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }
    if(nextNodeToBeInserted->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }
    */

    if(nextNodeToBeInserted->id == -1 || nextNode->id == -1) {
        printf("no");
    }

    toBeInserted->prevNode.at(doveInserire) = prevNode->id;
    prevNode->nextNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->prevNodeToVisit.at(doveInserire) = prevNodeToBeVisited->id;
    prevNodeToBeVisited->nextNodeToVisit.at(doveInserire) = toBeInserted->id;

    toBeInserted->nextNode.at(doveInserire) = nextNode->id;
    nextNode->prevNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->nextNodeToVisit.at(doveInserire) = nextNodeToBeInserted->id;
    nextNodeToBeInserted->prevNodeToVisit.at(doveInserire) = toBeInserted->id;

    //toBeInserted->visitatoInQuestoLivello.at(questoLivello) = epoca;

    /*int ultimaEpoca = toBeInserted->ultimaEpoca->load();
    while(!toBeInserted->ultimaEpoca->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));*/

#if DEBUG
    /*
    if(prevNodeToBeVisited->pronto.at(doveInserire)==epoca-1 && epoca!=0) {
        //stampa epoca
        std::cout <<"prevNodeToBeVisited già pronto: " << prevNodeToBeVisited->id << " " << prevNodeToBeVisited->pronto.at(doveInserire) << endl;
    }
     */
#endif

    int ultimaEpoca = prevNodeToBeVisited->ultimaEpoca->load();
    while(!prevNodeToBeVisited->ultimaEpoca->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));
    //prevNodeToBeVisited->pronto.at(doveInserire) = epoca;

#if USE_BSEM
    prevNodeToBeVisited->bs.at(doveInserire)->signal();
#endif
    sem->signal();


    prevNodeToBeVisited->pronto.at(doveInserire) = epoca;
}

bool Worker::insertArancio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {


    if(toBeInserted->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }
    if(prevNodeToBeVisited->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }
    if(nextNodeToBeInserted->ultimaEpoca->load() >= epoca) {
        printf("nono");
    }

    int countdown = toBeInserted->countdown->fetch_sub(1);
    toBeInserted->valoreCountdown.at(questoLivello) = countdown;
    /*if(toBeInserted->id == 7) {
        printf("ciao");
    }
    if(toBeInserted->valoreCountdown.at(questoLivello) != toBeInserted->valoreCountdown.at((questoLivello+N_THREADS)%(N_THREADS+1))-1 && toBeInserted->id == 7) {
        printf("ciao");
    }*/
    toBeInserted->visitato.at(doveInserire) = true;
    if(countdown > 0) {
        insertGiallo(epoca, questoLivello, doveInserire, prevNode, prevNodeToBeVisited, toBeInserted, nextNode, nextNodeToBeInserted);
        return true;
    }
    else if(countdown == 0) {
        insertGrigio(epoca, questoLivello, doveInserire, prevNode, prevNodeToBeVisited, toBeInserted, nextNode, nextNodeToBeInserted);
    }
    return false;
}

bool Worker::controllaValidita (Node *n, int epoca, int epocaAttuale) {

    if (n == g->endNode) {
        return true;
    }
    bool ans = true;
#if USE_BSEM
    n->bs.at(epocaAttuale)->wait();
#endif
    while(n->pronto.at(epocaAttuale) != epoca-1) {}
    while(n->ultimaEpoca->load() < epoca-1) {}
    //n->pronto.at(epocaAttuale) = false;

    if(n->ultimaEpoca->load() >= epoca || n->ultimaEpocaFantasma->load() >= epoca) {

        //while(g->nodes.at(n->nextNodeToVisit.at(epocaAttuale)).ultimaEpoca->load() < epoca-1) {}

        Node *nextNodeToBeVisited = &g->nodes.at(n->nextNodeToVisit.at(epocaAttuale));

        if (!controllaValidita(nextNodeToBeVisited, epoca, epocaAttuale)) {
            nextNodeToBeVisited = &g->nodes.at(n->nextNodeToVisit.at(epocaAttuale));
        }

        sem->wait();
        Node *prevNode = &g->nodes.at(n->prevNode.at(epocaAttuale));
        Node *prevNodeToBeVisited = &g->nodes.at(n->prevNodeToVisit.at(epocaAttuale));
        Node *nextNode = &g->nodes.at(n->nextNode.at(epocaAttuale));

        if(prevNode->id == 2 && nextNode->id == 7 && epoca > 11) {
            printf("voih");
        }

        /*if(prevNode->id == 7 && nextNode->id == 1) {
            printf("boh");
        }
        if(n->id == 8 && epoca == 2) {
            printf("NANI?!?");
        }*/

#if USE_BSEM
        //credo non serva, ma nel caso dobbiamo aggiungere while(!pronto)
        nextNodeToBeVisited->bs.at(epocaAttuale)->wait();
        nextNodeToBeVisited->bs.at(epocaAttuale)->signal();
#endif

        if(prevNode != prevNodeToBeVisited) {
#if DEBUG
            if(prevNode->countdown > 0){
                cout << "Grigio con contatore a " << prevNode->countdown << endl;
            }
#endif
            for(int i=0; i<N_THREADS+1; i++) {
                prevNode->nextNode.at(i) = nextNode->id;
            }
        }
        else {
            prevNode->countdown->store(N_THREADS);
            prevNode->valoreCountdown.at(epocaAttuale) = N_THREADS;
            /*if(prevNode->id == 7 && epoca>4) {
                printf("ciao");
            }*/

            prevNode->nextNode.at(epocaAttuale) = nextNode->id;
        }
#if DEBUG
        /*if(prevNodeToBeVisited -> pronto.at(epocaAttuale) == epoca-1) {
            std::cout << "errore non pronto: prevNode " << prevNodeToBeVisited->id << " epoca " << epoca << endl;
        }*/
#endif
        prevNodeToBeVisited->nextNodeToVisit.at(epocaAttuale) = nextNodeToBeVisited->id;

        if(nextNode != nextNodeToBeVisited) {
            for(int i=0; i<N_THREADS+1; i++) {
                nextNode->prevNode.at(i) = prevNode->id;
            }
        }
        else {
            nextNode->countdown->store(N_THREADS);
            nextNode->valoreCountdown.at(epocaAttuale) = N_THREADS;
            /*if(nextNode->id == 7 && epoca>4) {
                printf("ciao");
            }*/

            nextNode->prevNode.at(epocaAttuale) = prevNode->id;
        }

#if DEBUG
        /*if(nextNodeToBeVisited -> pronto.at(epocaAttuale) != epoca-1) {
            std::cout << "errore non pronto: nextNode " << nextNodeToBeVisited->id << " epoca " << epoca << endl;
        }*/
#endif
        nextNodeToBeVisited->prevNodeToVisit.at(epocaAttuale) = prevNodeToBeVisited->id;


        ans = false;
        sem->signal();

    }
/*
    if (&g->nodes.at(n->prevNodeToVisit.at(epocaAttuale)) == &g->nodes.at(n->nextNodeToVisit.at(epocaAttuale)) && ans) {
        printf("...");
    }
    if (&g->nodes.at(n->prevNodeToVisit.at(epocaAttuale)) == n && ans) {
        printf(".. ma");
    }
    if (&g->nodes.at(n->nextNodeToVisit.at(epocaAttuale)) == n && ans) {
        printf(".. ma");
    }*/

    int ultimaEpoca = n->ultimaEpocaFantasma->load();
    while(!n->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));

    return ans;
}

int Worker::start2(int position) {
    int epoca = position, epocaAttuale = position, epocaSuccessiva = position+1;
    Node *nodoPrecedente;
    Node *nodoPrecedenteDaVisitare;
    Node *nodoPrecedenteScritto;
    Node *nodoPrecedenteDaVisitareScritto;
    Node *nodoAttuale;
    Node *nodoSuccessivo;
    Node *nodoSuccessivoDaVisitare;
    bool modificato = true;
    int ultimaEpoca;

    if (epoca == 0) {
        nodoPrecedente = g->startNode;
        nodoAttuale = &g->nodes.at(g->roots.at(0));

        for (int i = 1; i < g->rootsSize; i++) {
            int R = g->roots.at(i);
            nodoSuccessivo = &g->nodes.at(R);

            insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedente, nodoPrecedente, nodoAttuale, nodoSuccessivo, nodoSuccessivo);

            nodoPrecedente = nodoAttuale;
            nodoAttuale = nodoSuccessivo;
        }
        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedente, nodoPrecedente, nodoAttuale, g->endNode, g->endNode);

        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoAttuale, nodoAttuale, g->endNode, g->endNode, g->endNode);
#if DEBUG
        /*if(g->endNode->pronto.at(epocaSuccessiva) != -1) {
            //stampa epoca
            std::cout <<"prevNodeToBeVisited già pronto: " << g->endNode->id << " " << g->endNode->pronto.at(epocaSuccessiva);
        }*/
#endif
        //g->endNode->pronto.at(epocaSuccessiva) = epoca;
        g->endNode->ultimaEpoca->store(0);
        g->endNode->pronto.at(epocaSuccessiva) = epoca;
#if USE_BSEM
        g->endNode->bs.at(epocaSuccessiva)->signal();
#endif

        epoca += N_THREADS;
    }


    while(modificato == true) {
        epocaAttuale = epoca%(N_THREADS+1);
        epocaSuccessiva = (epoca+1)%(N_THREADS+1);
        modificato = false;

        nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = g->startNode;

#if USE_BSEM
        nodoPrecedenteDaVisitare->bs.at(epocaAttuale)->wait();
#endif
        while(nodoPrecedenteDaVisitare->pronto.at(epocaAttuale) != epoca-1) {}

        while(nodoPrecedenteDaVisitare->ultimaEpoca->load() < epoca-1) {}
        //nodoPrecedenteDaVisitare->pronto.at(epocaAttuale) = false;

        while (nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale) != g->endingNode) {

            nodoAttuale = &g->nodes.at(nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale));

            if(!controllaValidita(nodoAttuale, epoca, epocaAttuale)) {
                continue;
            }

#if DEBUG
            if(nodoPrecedenteDaVisitare == g->startNode) {
                if(g->startNode->nextNodeToVisit.at(epocaAttuale) != nodoAttuale->id) {
                    printf("Errore");
                }
            }

            if(nodoAttuale->id != nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale)) {
                cout << "Nodo attuale non corretto\n";
            }
#endif

            //if (nodoAttuale->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

                if (!nodoAttuale->visitato.at(epocaAttuale)) {

                    if (nodoAttuale->nFathers == nodoAttuale->nVisitedFathers->load()) {

                        Node *nodoFiglio = nullptr;

                        Node *nodoFiglioSuccessivo;

                        nodoPrecedenteScritto = &g->nodes.at(nodoAttuale->prevNode.at(epocaAttuale));

                        for(int i = 0; i < nodoAttuale->adjSize; i++) {

                            nodoFiglioSuccessivo = &g->nodes.at(nodoAttuale->adj.at(i));

                            //if(nodoFiglioSuccessivo->ultimaEpoca->load() < epoca) {
                                ultimaEpoca = nodoFiglioSuccessivo->ultimaEpocaFantasma->load();
                                while(!nodoFiglioSuccessivo->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca-1 : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));
                                //nodoFiglioSuccessivo->ultimaEpoca->store(epoca-1);
                            //}

                            nodoFiglioSuccessivo->nVisitedFathers->fetch_add(1);

                            if(/*nodoFiglioSuccessivo->ultimaEpoca->load() < epoca &&*/ nodoFiglioSuccessivo->ultimaEpocaFantasma->load() < epoca) {

                                ultimaEpoca = nodoFiglioSuccessivo->ultimaEpocaFantasma->load();
                                while(!nodoFiglioSuccessivo->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));

                                if(nodoFiglio == nullptr) {

                                    nodoFiglio = nodoFiglioSuccessivo;

                                }
                                else {

                                    insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoFiglio, nodoFiglioSuccessivo, nodoFiglioSuccessivo);

                                    nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoFiglio;
                                    nodoFiglio = nodoFiglioSuccessivo;

                                }

                            }


                        }

                        if (nodoFiglio != nullptr) {

                            insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoFiglio, nodoAttuale, nodoAttuale);
                            nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoFiglio;

                        }

                        nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                        nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));

                        if (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
                            nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                            nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));
                        }

                        //nodoAttuale->visitato.at(epocaAttuale) = true;

#if DEBUG
                        if (nodoPrecedenteDaVisitareScritto == nodoSuccessivoDaVisitare) {
                            printf("...");
                        }
                        if (nodoPrecedenteDaVisitareScritto == nodoAttuale) {
                            printf("ma");
                        }
                        if (nodoSuccessivoDaVisitare == nodoAttuale) {
                            printf("che");
                        }
#endif

                        if(insertArancio(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoAttuale, nodoSuccessivo, nodoSuccessivoDaVisitare)) {
                            nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                        }
                        else {
                            nodoPrecedenteScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                        }

#if DEBUG
                        /*if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva) == epoca-1) {
                            //stampa epoca
                            std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                        }*/
#endif
                        //nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = epoca;
#if USE_BSEM
                        nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif

                        modificato = true;

                        ultimaEpoca = nodoSuccessivoDaVisitare->ultimaEpocaFantasma->load();
                        nodoSuccessivoDaVisitare->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, ultimaEpoca == epoca ? epoca-1 : ultimaEpoca);

                    }
                    else if (nodoAttuale->nVisitedFathers->load() > nodoAttuale->nFathers) {
                        printf("Eh, no!");
                    }
                    else {

                        modificato = true;

                        nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                        nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));

                        if (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
                            nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                            nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));
                        }

                        nodoPrecedenteScritto = &g->nodes.at(nodoAttuale->prevNode.at(epocaAttuale));
                        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoAttuale, nodoSuccessivo, nodoSuccessivoDaVisitare);
                        nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;

                        //nodoSuccessivoDaVisitare->ultimaEpocaFantasma

                        ultimaEpoca = nodoSuccessivoDaVisitare->ultimaEpocaFantasma->load();
                        nodoSuccessivoDaVisitare->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, ultimaEpoca == epoca ? epoca-1 : ultimaEpoca);

#if DEBUG
                        /*if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva) == epoca-1) {
                            //stampa epoca
                            std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                        }*/
#endif
                        //nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = epoca;
#if USE_BSEM
                        nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif

                    }

                }
                else {

                    nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                    nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));

                    if (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
                        nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                        nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));
                    }

                    nodoPrecedenteScritto = &g->nodes.at(nodoAttuale->prevNode.at(epocaAttuale));
                    if(insertArancio(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoAttuale, nodoSuccessivo, nodoSuccessivoDaVisitare)) {
                        nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                    }
                    else {
                        nodoPrecedenteScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                    }

#if DEBUG
                    /*if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva) == epoca-1) {
                        //stampa epoca
                        std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                    }*/
#endif

                    ultimaEpoca = nodoSuccessivoDaVisitare->ultimaEpoca->load();
                    while(!nodoSuccessivoDaVisitare->ultimaEpoca->compare_exchange_weak(ultimaEpoca, epoca > ultimaEpoca ? epoca-1 : ultimaEpoca, std::memory_order_release, std::memory_order_relaxed));
                    //nodoSuccessivoDaVisitare->ultimaEpoca->store(epoca-1);
                    nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = epoca-1;
#if USE_BSEM
                    nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif

                    //nodoSuccessivoDaVisitare->ultimaEpocaFantasma

                    ultimaEpoca = nodoSuccessivoDaVisitare->ultimaEpocaFantasma->load();
                    nodoSuccessivoDaVisitare->ultimaEpocaFantasma->compare_exchange_weak(ultimaEpoca, ultimaEpoca == epoca ? epoca-1 : ultimaEpoca);

                }

            }

        //}

        nodoPrecedenteScritto = &g->nodes.at(g->endNode->prevNode.at(epocaAttuale));
        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, g->endNode, g->endNode, g->endNode);

        //g->endNode->pronto.at(epocaAttuale) = false;
#if USE_BSEM
        g->endNode->bs.at(epocaAttuale)->designal();
#endif

#if DEBUG
        /*if(g->endNode->pronto.at(epocaSuccessiva) == epoca-1) {
            //stampa epoca
            std::cout <<"prevNodeToBeVisited già pronto: " << g->endNode->id << " " << g->endNode->pronto.at(epocaSuccessiva);
        }*/
#endif

        g->endNode->ultimaEpoca->store(epoca);
        //g->endNode->pronto.at(epocaSuccessiva) = epoca;
        g->endNode->pronto.at(epocaSuccessiva) = epoca;

#if USE_BSEM
        g->endNode->bs.at(epocaSuccessiva)->signal();
#endif

        epoca += N_THREADS;

    }
    return epoca-N_THREADS;
}