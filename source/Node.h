#ifndef SDP_PIPELINERESOLUTION_NODE_H
#define SDP_PIPELINERESOLUTION_NODE_H

struct Node {
    int id;
    int father = -1;
    int start = -1;
    int end = -1;
    int subTreeSize = -1;
    std::vector<int> adj;
    std::vector<int> trueAdj;
};

#endif //SDP_PIPELINERESOLUTION_NODE_H
