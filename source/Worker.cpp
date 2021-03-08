//
// Created by lire on 8/19/20.
//

#include "Worker.h"


using namespace std;


void Worker::initialize(Graph *g) {
    this->g = g;
}

void insertGrigio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {

#if DEBUG
    if(toBeInserted->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "Grigio toBeInserted errore\t\t\t" << toBeInserted->id << " " <<  toBeInserted->countdown << endl;
    }
    if(prevNodeToBeVisited->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "Grigio prevNodeToBeVisited errore\t" << prevNodeToBeVisited->id << " " <<  prevNodeToBeVisited->countdown << endl;
    }
    if(prevNode->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "Grigio prevNode errore\t\t\t\t" << prevNode->id << " " <<  prevNode->countdown << endl;
    }
    if(nextNode->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "Grigio nextNode errore\t\t\t\t" << nextNode->id << " " <<  nextNode->countdown << endl;
    }
    if(nextNodeToBeInserted->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "Grigio nextNodeToBeInserted errore\t" << nextNodeToBeInserted->id << " " <<  nextNodeToBeInserted->countdown << endl;
    }
#endif

    prevNodeToBeVisited->nextNodeToVisit.at(doveInserire) = nextNodeToBeInserted->id;
    nextNodeToBeInserted->prevNodeToVisit.at(doveInserire) = prevNodeToBeVisited->id;

    toBeInserted->nextNode.at(doveInserire) = nextNode->id;
    nextNode->prevNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->prevNode.at(doveInserire) = prevNode->id;
    prevNode->nextNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->visitatoInQuestoLivello.at(questoLivello) = epoca;

}

void Worker::insertGiallo(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {

#if DEBUG
    if(toBeInserted->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "toBeInserted errore\t\t\t" << toBeInserted->id << " " <<  toBeInserted->countdown << endl;
    }
    if(prevNodeToBeVisited->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "prevNodeToBeVisited errore\t" << prevNodeToBeVisited->id << " " <<  prevNodeToBeVisited->countdown << endl;
    }
    if(prevNode->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "prevNode errore\t\t\t\t" << prevNode->id << " " <<  prevNode->countdown << endl;
    }
    if(nextNode->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "nextNode errore\t\t\t\t" << nextNode->id << " " <<  nextNode->countdown << endl;
    }
    if(nextNodeToBeInserted->pronto.at(doveInserire)) {
        printf("r ");
        std::cout << "nextNodeToBeInserted errore\t" << nextNodeToBeInserted->id << " " <<  nextNodeToBeInserted->countdown << endl;
    }
#endif

    toBeInserted->nextNode.at(doveInserire) = nextNode->id;
    nextNode->prevNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->nextNodeToVisit.at(doveInserire) = nextNodeToBeInserted->id;
    nextNodeToBeInserted->prevNodeToVisit.at(doveInserire) = toBeInserted->id;

    toBeInserted->prevNode.at(doveInserire) = prevNode->id;
    prevNode->nextNode.at(doveInserire) = toBeInserted->id;

    toBeInserted->prevNodeToVisit.at(doveInserire) = prevNodeToBeVisited->id;
    prevNodeToBeVisited->nextNodeToVisit.at(doveInserire) = toBeInserted->id;

    toBeInserted->visitatoInQuestoLivello.at(questoLivello) = epoca;

#if DEBUG
    if(prevNodeToBeVisited->pronto.at(doveInserire)) {
        std::cout <<"prevNodeToBeVisited già pronto: " << prevNodeToBeVisited->id << " " << (bool)prevNodeToBeVisited->pronto.at(doveInserire);
    }
#endif
    prevNodeToBeVisited->pronto.at(doveInserire) = true;
#if USE_BSEM
    prevNodeToBeVisited->bs.at(doveInserire)->signal();
#endif
}

bool Worker::insertArancio(int epoca, int questoLivello, int doveInserire, Node *prevNode, Node *prevNodeToBeVisited, Node *toBeInserted, Node *nextNode, Node *nextNodeToBeInserted) {

    toBeInserted->countdown--;
    toBeInserted->visitato.at(doveInserire) = true;
    if(toBeInserted->countdown > 0) {
        insertGiallo(epoca, questoLivello, doveInserire, prevNode, prevNodeToBeVisited, toBeInserted, nextNode, nextNodeToBeInserted);
        return true;
    }
    else if(toBeInserted->countdown == 0) {
        insertGrigio(epoca, questoLivello, doveInserire, prevNode, prevNodeToBeVisited, toBeInserted, nextNode, nextNodeToBeInserted);
    }
    return false;
}

bool Worker::controllaValidita (Node *n, int epoca, int epocaAttuale) {

#if USE_BSEM
    n->bs.at(epocaAttuale)->wait();
#endif
    while(!n->pronto.at(epocaAttuale)) {}
    n->pronto.at(epocaAttuale) = false;

    if(n->visitatoInQuestoLivello.at(epocaAttuale) == epoca) {

        Node *prevNode = &g->nodes.at(n->prevNode.at(epocaAttuale));
        Node *prevNodeToBeVisited = &g->nodes.at(n->prevNodeToVisit.at(epocaAttuale));
        Node *nextNode = &g->nodes.at(n->nextNode.at(epocaAttuale));
        Node *nextNodeToBeVisited = &g->nodes.at(n->nextNodeToVisit.at(epocaAttuale));

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
            prevNode->countdown += N_THREADS;

            prevNode->nextNode.at(epocaAttuale) = nextNode->id;
        }
#if DEBUG
        if(prevNodeToBeVisited -> pronto.at(epocaAttuale) == true) {
            std::cout << "errore non pronto: prevNode " << prevNodeToBeVisited->id << " epoca " << epoca << endl;
        }
#endif
        prevNodeToBeVisited->nextNodeToVisit.at(epocaAttuale) = nextNodeToBeVisited->id;

        if(nextNode != nextNodeToBeVisited) {
            for(int i=0; i<N_THREADS+1; i++) {
                nextNode->prevNode.at(i) = prevNode->id;
            }
        }
        else {
            nextNode->countdown += N_THREADS;

            nextNode->prevNode.at(epocaAttuale) = prevNode->id;
        }

#if DEBUG
        if(nextNodeToBeVisited -> pronto.at(epocaAttuale) == false) {
            std::cout << "errore non pronto: nextNode " << nextNodeToBeVisited->id << " epoca " << epoca << endl;
        }
#endif
        nextNodeToBeVisited->prevNodeToVisit.at(epocaAttuale) = prevNodeToBeVisited->id;


        return false;

    }
    return true;
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
        if(g->endNode->pronto.at(epocaSuccessiva)) {
            std::cout <<"prevNodeToBeVisited già pronto: " << g->endNode->id << " " << (bool)g->endNode->pronto.at(epocaSuccessiva);
        }
#endif
        g->endNode->pronto.at(epocaSuccessiva) = true;
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
        while(!nodoPrecedenteDaVisitare->pronto.at(epocaAttuale)) {}
        nodoPrecedenteDaVisitare->pronto.at(epocaAttuale) = false;

        while (nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale) != g->endingNode) {

            nodoAttuale = &g->nodes.at(nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale));

            while(!controllaValidita(nodoAttuale, epoca, epocaAttuale)) {
                nodoAttuale = &g->nodes.at(nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale));
            }

#if DEBUG
            if(nodoAttuale->id != nodoPrecedenteDaVisitare->nextNodeToVisit.at(epocaAttuale)) {
                cout << "Nodo attuale non corretto\n";
            }
#endif

            if (nodoAttuale->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

                if (!nodoAttuale->visitato.at(epocaAttuale)) {

                    if (nodoAttuale->nFathers == nodoAttuale->nVisitedFathers->load()) {

                        Node *nodoFiglio = nullptr;

                        Node *nodoFiglioSuccessivo;

                        nodoPrecedenteScritto = &g->nodes.at(nodoAttuale->prevNode.at(epocaAttuale));

                        for(int i = 0; i < nodoAttuale->adjSize; i++) {

                            nodoFiglioSuccessivo = &g->nodes.at(nodoAttuale->adj.at(i));

                            nodoFiglioSuccessivo->nVisitedFathers->fetch_add(1);

                            if(nodoFiglioSuccessivo->visitatoInQuestoLivello.at(epocaAttuale) != epoca) {

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

                        while (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
                            nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                            nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));
                        }

                        //nodoAttuale->visitato.at(epocaAttuale) = true;

                        if(insertArancio(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoAttuale, nodoSuccessivo, nodoSuccessivoDaVisitare)) {
                            nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                        }
                        else {
                            nodoPrecedenteScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;
                        }

#if DEBUG
                        if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva)) {
                            std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << (bool)nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                        }
#endif
                        nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = true;
#if USE_BSEM
                        nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif

                        modificato = true;

                    }
                    else {

                        nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                        nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));

                        while (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
                            nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                            nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));
                        }

                        nodoPrecedenteScritto = &g->nodes.at(nodoAttuale->prevNode.at(epocaAttuale));
                        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, nodoAttuale, nodoSuccessivo, nodoSuccessivoDaVisitare);
                        nodoPrecedenteScritto = nodoPrecedenteDaVisitareScritto = nodoPrecedente = nodoPrecedenteDaVisitare = nodoAttuale;

#if DEBUG
                        if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva)) {
                            std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << (bool)nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                        }
#endif
                        nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = true;
#if USE_BSEM
                        nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif

                    }

                }
                else {

                    nodoSuccessivo = &g->nodes.at(nodoAttuale->nextNode.at(epocaAttuale));
                    nodoSuccessivoDaVisitare = &g->nodes.at(nodoAttuale->nextNodeToVisit.at(epocaAttuale));

                    while (!controllaValidita(nodoSuccessivoDaVisitare, epoca, epocaAttuale)) {
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
                    if(nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva)) {
                        std::cout <<"prevNodeToBeVisited già pronto: " << nodoSuccessivoDaVisitare->id << " " << (bool)nodoSuccessivoDaVisitare->pronto.at(epocaSuccessiva);
                    }
#endif
                    nodoSuccessivoDaVisitare->pronto.at(epocaAttuale) = true;
#if USE_BSEM
                    nodoSuccessivoDaVisitare->bs.at(epocaAttuale)->signal();
#endif


                }

            }

        }

        nodoPrecedenteScritto = &g->nodes.at(g->endNode->prevNode.at(epocaAttuale));
        insertGiallo(epoca, epocaAttuale, epocaSuccessiva, nodoPrecedenteScritto, nodoPrecedenteDaVisitareScritto, g->endNode, g->endNode, g->endNode);

        g->endNode->pronto.at(epocaAttuale) = false;
#if USE_BSEM
        g->endNode->bs.at(epocaAttuale)->designal();
#endif

#if DEBUG
        if(g->endNode->pronto.at(epocaSuccessiva)) {
            std::cout <<"prevNodeToBeVisited già pronto: " << g->endNode->id << " " << (bool)g->endNode->pronto.at(epocaSuccessiva);
        }
#endif

        g->endNode->pronto.at(epocaSuccessiva) = true;

#if USE_BSEM
        g->endNode->bs.at(epocaSuccessiva)->signal();
#endif

        epoca += N_THREADS;

    }
    return epocaAttuale;
}