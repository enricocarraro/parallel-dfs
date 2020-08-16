#ifndef Graph_hpp
#define Graph_hpp

#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

#define GRAPH_DEBUG 0

using namespace std;

struct Node {
	int id;
	int father = -1;
	int start = -1;
	int end = -1;
	int subTreeSize = -1;
	vector<int> adj;
	vector<int> trueAdj;
};

class Graph {
	int nNodes;
	std::vector<bool> bits;
public:
	vector<Node> nodes;
	Graph(FILE *fp);
	Graph(int nodes);
	void addEdge(int u, int v);
	void addEdges(int u, const int adj[], int adj_size);
	void printGraph();
	void printTrueGraph();
	void sortVectors();
	void build(FILE *fp);
	std::vector<bool> returnRoots();
	int size() {
		return nNodes;
	}
};

#endif /* Graph_hpp */
