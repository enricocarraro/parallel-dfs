#include "GraphBuilder.h"

using namespace std;

void GraphBuilder::initialize(Graph &g, int size) {
    this->g = &g;
    this->size = size;
}

void GraphBuilder::buildLoop() {
    //char *token;
    int i = 0;
    int u, v;
    vector<int> buf(size);

    while(true) {
        working.wait();
        if(!str.compare("end"))
        {
            break;
        }
        std::istringstream iss(str);
        std::string token;
        /* get the first token */
        std::getline(iss, token, ' ');
        if(str.compare("\n")) {
            u = stoi(token);
            i=0;
            /* walk through other tokens */
            while (std::getline(iss, token, ' '))
            {
                v = stoi(token);
                //std::cout << token << std::endl;
                buf.at(i) = v;
                i++;
            }
            g->addEdges(u, buf, i);

        }
        free.signal();
    }
    //cout << "fine ";
}

