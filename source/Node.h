//
// Created by lire on 10/25/20.
//

#ifndef SDP_PARALLELSOLUTION_NODE_H
#define SDP_PARALLELSOLUTION_NODE_H

#include <boost/multiprecision/cpp_int.hpp>
#include "BusySemaphore.h"

struct Node {
    boost::multiprecision::uint1024_t nodeWeight = 1;
    boost::multiprecision::uint1024_t time = std::numeric_limits<boost::multiprecision::uint1024_t>::max();

    int start = INT32_MAX;
    int end = -1;


    std::vector<int> adj;
    int adjSize = 0;

    std::vector<int> ancestors;
    int ancSize = 0;

#if GRAPH_DOUBLE_READ | GRAPH_REREAD_GRAPH
    int ancNumber = 0;
#endif
#if !USE_BOOL
    //bool root = true;
#endif

    bool operator<(const Node &n) const {
        return (nodeWeight < n.nodeWeight);
    }

    BusySemaphore *bSem = new BusySemaphore;

    ~Node() { delete bSem; /*delete adj; delete ancestors;*/ };
};

#endif //SDP_PARALLELSOLUTION_NODE_H
