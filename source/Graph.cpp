#include "Graph.hpp"


using namespace std;

bool compareNodeCost(const Node& lhs, const Node& rhs) {
    return lhs.cost < rhs.cost;
}

bool compareNodeId(const Node& lhs, const Node& rhs) {
    return lhs.id < rhs.id;
}

void Graph::init() {
    roots.reserve(nNodes);
    leafs.reserve(nNodes);
    nodes.resize(nNodes + 1);
    for (unsigned int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        roots.insert(i);
    }
    nodes[nNodes].id = nNodes;
}

// should only be used by build
void Graph::build_addEdges(unsigned int u, vector < unsigned int > & adj, unsigned int adj_size) {
    nodes[u].adj.resize(adj_size);
    for (unsigned int i = 0; i < adj_size; i++) {
        nodes[u].adj[i] = adj[i];
        nodes[adj[i]].inc.push_back(u);
    }
}

/*
 Presuming the graph input file is well formed:
 max_size = (log10(nNodes) + 2) * (nNodes + 1) + 3
 log10(nNodes) = max characters to represent nodeId
 + 1 for the intermitting space between edges
 + 1 for slack
 * nNodes = maxmimumg amount of nodes in adj list
 + includes the node to which the list refers to
 + 3 slack
 */

void Graph::build(FILE * fp) {
    unsigned int u, v;
    unsigned int max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector < unsigned int > buf = vector < unsigned int > (nNodes + 1);
    
    while (fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", dontcare);
        char * token;
        unsigned int i = 0;
        
        /* get the first token */
        token = strtok(str, " ");
        sscanf(token, "%d", & u);
        token = strtok(NULL, " ");
        /* walk through other tokens */
        while (token != NULL) {
            sscanf(token, "%d", & v);
            
            roots.erase(v);
            
            buf.at(i++) = v;
            token = strtok(NULL, " ");
        }
        
        this -> build_addEdges(u, buf, i);
        if (i == 0) leafs.insert(u);
    }
    
    // hidden parent of all roots
    nodes[nNodes].adj = vector<unsigned int>(roots.begin(), roots.end());
    sort(nodes[nNodes].adj.begin(), nodes[nNodes].adj.end(), std::less < unsigned int > ());
    
}

Graph::Graph(FILE * fp): nodes(), roots(), P() {
    fscanf(fp, "%d", & nNodes);
    init();
    build(fp);
}

Graph::Graph(unsigned int nNodes): nodes(), roots(), P() {
    this -> nNodes = nNodes;
    init();
}

void Graph::printGraph() {
    
    for (int v = 0; v < nodes.size(); ++v) {
        cout << nodes[v].id << ": " << (nodes[v].parent == nNodes ? -1 : nodes[v].parent);
        /*
         for (auto x : nodes[v].adj)
         cout << x << " ";
         
         cout << "pre: " << nodes[v].pre << " post: " << nodes[v].post;
         */
        cout << endl;
        
    }
    
    cout << "Roots (" << roots.size() << "): " << endl;
    for (const auto & x: roots)
        cout << x << " ";
    cout << endl;
    
    cout << "Leafs (" << leafs.size() << "): " << endl;
    for (const auto & x: leafs)
        cout << x << " ";
    cout << endl;
    
}

void Graph::printNodesStatus() {
    
    for (int v = 0; v < nodes.size(); ++v) {
        
        //cout << v << ": parent = " << nodes[v].parent << endl;
        //cout << nodes[v].id << ": sub graph size = " << nodes[v].subgraph_size << " depth: " << nodes[v].depth << " cost: " << nodes[v].cost << " pre: " << nodes[v].pre << " post: " << nodes[v].post << endl;
        cout << nodes[v].id << ":\tparent=" << (nodes[v].parent == nNodes ? -1 : nodes[v].parent) << "\tpre=" << nodes[v].pre << "\tpost=" << nodes[v].post << endl;
        /*cout << "\n Adjacency list of vertex " << v << endl;
         
         for (auto x : nodes[v].adj)
         cout << " " << x;
         cout << endl;
         
         if(!is_root) {
         cout << "Path: ";
         for ( const auto& x: nodes[v].path )
         cout << x << " ";
         cout << endl;
         */
        
        //        cout << "Incoming edges status: visited=" << nodes[v].inc_visited_count << " total=" << nodes[v].inc_count << endl;
        /*    for (auto& x : nodes[v].inc)
         cout << x.first << ": " << x.second << endl;
         
         
         }
         */
        //cout << endl;
        
    }
}

// meant to be used after building the graph.
void Graph::sortVectors() {
    FastSemaphore sem;
    initThreadWorkers();
    
    for (int v = 0; v < nNodes; ++v) {
        unsigned int worker_id = hash(v) % parent_workers.size();
        parent_workers[worker_id].addTask([this, v, &sem]() -> void {
            sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less < int > ());
            sem.signal();
        });
        
    }
    
    for(unsigned int i = 0; i < nNodes; i++)
        sem.wait();
}

void Graph::addEdge(unsigned int u, unsigned int v) {
    nodes[u].adj.push_back(v);
    nodes[v].inc.push_back(u);
}

void Graph::initThreadWorkers() {
    unsigned int hw_conc = std::thread::hardware_concurrency() * 2;
    if (!init_tw_done) {
        
        vector < ThreadWorker > to_swapp(hw_conc);
        parent_workers.swap(to_swapp);
        
        vector < ThreadWorker > to_swapc(hw_conc);
        child_workers.swap(to_swapc);
        
        vector < FastSemaphore > to_swap_sem(hw_conc);
        worker_semaphores.swap(to_swap_sem);
        
        for (auto & worker: parent_workers) {
            auto t = std::thread( & ThreadWorker::processTasks, & worker);
            t.detach();
        }
        for (auto & worker: child_workers) {
            auto t = std::thread( & ThreadWorker::processTasks, & worker);
            t.detach();
        }
        
        init_tw_done = true;
    }
}
void Graph::computeSubGraphSize() {
    FastSemaphore sem;
    
    initThreadWorkers();
    
    if (nodes[nNodes].adj.size() <= 0 || nNodes <= 0 ) {
        throw "Error: no roots";
    }
    
    // set hidden node as only incoming node of roots
    for(const auto& child: nodes[nNodes].adj) {
        unsigned int worker_id = hash(child) % parent_workers.size();
        parent_workers[worker_id].addTask([this, child, &sem, worker_id]() -> void {
            nodes[child].inc = { nNodes };
            sem.signal();
        });
    }
    
    for(unsigned int i = 0; i < nodes[nNodes].adj.size(); i++)
        sem.wait();
    
    if (leafs.size() <= 0) {
        throw "Error: no leafs";
    }
    
    vector < unsigned int > Q (leafs.begin(), leafs.end());
    
    while (Q.size() > 0) {
        
        size_t level_nodes = Q.size();
        for(const auto& node: Q) {
            
            unsigned int worker_id = hash(node) % child_workers.size();
            child_workers[worker_id].addTask([this, node, &sem, worker_id]() -> void {
                this -> subGraphSize_processChild(node, worker_id);
                sem.signal();
            });
            
        }
        
        for (unsigned int i = 0; i < level_nodes; i++)
            sem.wait();
        
        Q = C.move_underlying_queue();
        C = SafeQueue < unsigned int > ();
        for(const auto& node: Q) {
            unsigned int worker_id = hash(node) % parent_workers.size();
            parent_workers[worker_id].addTask([this, node, & sem, worker_id]() -> void {
                this -> subGraphSize_computePrefixSum(node);
                sem.signal();
            });
        }
        
        for (unsigned int i = 0; i < Q.size(); i++)
            sem.wait();
        
    }
}


void Graph::subGraphSize_processChild(unsigned i, unsigned worker_id) {
    for(const auto& parent: nodes[i].inc) {
        parent_workers[hash(parent) % parent_workers.size()].addTask([this, parent, worker_id] () -> void {
            if (nodes[parent].adj.size() == ++nodes[parent].adj_visited_count) C.push(parent);
            this->worker_semaphores[worker_id].signal();
        });
        
        this->worker_semaphores[worker_id].wait();
    }
}

void Graph::subGraphSize_computePrefixSum(unsigned int p) {
    for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
        nodes[p].subgraph_size += nodes[nodes[p].adj[i]].subgraph_size;
}

void Graph::computeSubDTSize() {
    FastSemaphore sem;
    
    initThreadWorkers();
    
    if (leafs.size() <= 0) {
        cout << "Error: no leafs" << endl;
        return;
    }
    
    vector < unsigned int > Q (leafs.begin(), leafs.end());
    
    while (Q.size() > 0) {
        
        size_t level_nodes = 0;
        for(const auto& node: Q) {
            int parent = nodes[node].parent;
            if (parent >= 0) {
                level_nodes++;
                unsigned int worker_id = hash(parent) % parent_workers.size();
                parent_workers[worker_id].addTask([this, parent, & sem, worker_id]() -> void {
                    if (nodes[parent].adj.size() == ++nodes[parent].adj_visited_count) C.push(parent);
                    sem.signal();
                });
            }
        }
        
        for (unsigned int i = 0; i < level_nodes; i++)
            sem.wait();
        
        Q = C.move_underlying_queue();
        C = SafeQueue < unsigned int > ();
        for(const auto& node: Q) {
            unsigned int worker_id = hash(node) % parent_workers.size();
            parent_workers[worker_id].addTask([this, node, & sem, worker_id]() -> void {
                this -> subGraphSize_computePrefixSum(node);
                sem.signal();
            });
        }
        
        for (unsigned int i = 0; i < Q.size(); i++)
            sem.wait();
        
    }
}

void Graph::computeParentSSSP() {
    FastSemaphore sem;
    initThreadWorkers();
    vector < unsigned int > Q = {nNodes};
    
    nodes[nNodes].cost = 0;
    
    int level = 0;
    
    while (Q.size() > 0) {
        for(const auto& node: Q) {
            
            unsigned int worker_id = hash(node) % parent_workers.size();
            parent_workers[worker_id].addTask([this, node, &sem, level, worker_id]() -> void {
                nodes[node].depth = level;
                this -> computeParentSSSP_processParent(node, worker_id);
                sem.signal();
            });
        }
        
        for (unsigned int i = 0; i < Q.size(); i++)
            sem.wait();
        
        level++;
        
        Q = P.move_underlying_queue();
        P = SafeQueue < unsigned int > ();
    }
    
    
    for (unsigned int i = 0; i < nodes.size() - 1; i++)
        if (nodes[i].adj.size() == 0)   leafs.insert(i);
    
}

void Graph::computeParentSSSP_processParent(const unsigned int p, unsigned int worker_id) {
    
    for(unsigned int i = 0; i < nodes[p].adj.size(); i++) {
        unsigned int node = nodes[p].adj[i];
        child_workers[hash(node) %  child_workers.size()].addTask([this, node, p, worker_id] () -> void {
            this->computeParentSSSP_processChild(node, p);
            this->worker_semaphores[worker_id].signal();
        });
    }
    
    for(unsigned int i = 0; i < nodes[p].adj.size(); i++)
        this->worker_semaphores[worker_id].wait();
}

void Graph::computeParentSSSP_processChild(unsigned int child, unsigned int p) {
    // this for loop could be optimized to break if partial alpha is larger than current cost
    uint1024_t prefix = 1;
    /*if(child == 2)
     cout << "here";
     if(child == 4)
     cout << "here";
     */
    if(p >= 0) {
        for(unsigned int i = 0; i < nodes[p].adj.size() && nodes[p].adj[i] < child; i++) {
            unsigned int brother = nodes[p].adj[i];
            prefix += 1 + nodes[brother].subgraph_size;
        }
    }
#if GRAPH_DEBUG
    else {
        throw "Impossible, only child with parents can reach this function";
    }
#endif
    uint1024_t alpha = nodes[p].cost + prefix;
    if(nodes[child].inc.size() == 1 || alpha < nodes[child].cost) {
        nodes[child].cost = alpha;
        nodes[child].parent = p;
    }
    
    if (++nodes[child].inc_visited_count == nodes[child].inc.size())
        P.push(child);
#if GRAPH_DEBUG
    else if(nodes[child].inc_visited_count >= nodes[child].inc.size()) {
        throw "Nodes should only be visited as many times as the number of parents";
    }
#endif
}

void Graph::sequentialDFS() {
    pre = post = 0;
    
    for (auto & root: nodes[nNodes].adj)
        sequentialDFS_r(root);
    
    pre = post = 0;
}

void Graph::sequentialDFS_r(unsigned int p) {
    nodes[p].visited = true; // node visited
    nodes[p].pre = pre++;
    for (auto & child: nodes[p].adj) {
        if (!nodes[child].visited) {
            nodes[child].parent = p;
            sequentialDFS_r(child);
        }
    }
    nodes[p].post = post++;
}

void Graph::computePrePostOrder() {
    FastSemaphore sem;
    initThreadWorkers();
    
    
    for(unsigned int i = 0; i < nodes.size(); i++) {
        unsigned int worker_id = hash(i) % child_workers.size();
        child_workers[worker_id].addTask([this, i, &sem]() -> void {
            nodes[i].subgraph_size = 1;
            nodes[i].adj_visited_count = 0;
            nodes[i].adj = vector<unsigned int> ();
            sem.signal();
        });
    }
    
    for(unsigned int i = 0; i < nodes.size(); i++)
        sem.wait();
    
    
    
    for(unsigned int i = 0; i < nodes.size(); i++) {
        if(nodes[i].parent >= 0 && nodes[i].parent < nNodes) {
            parent_workers[hash(nodes[i].parent) %  parent_workers.size()].addTask([this, i, &sem] () -> void {
                this->nodes[nodes[i].parent].adj.push_back(i);
                sem.signal();
            });
            
        }
    }
    
    for(unsigned int i = 0; i < nodes.size() - roots.size() - 1; i++)
        sem.wait();
    
    computeSubDTSize();
    
    sort(nodes.begin(), nodes.end(), compareNodeCost);
    
    
    for(unsigned int i = 0; i < nodes.size(); i++) {
        unsigned int worker_id = hash(i) % parent_workers.size();
        parent_workers[worker_id].addTask([this, i, &sem]() -> void {
            nodes[i].pre = i - 1;
            nodes[i].post = (uint1024_t (i - nodes[i].depth - 1)) + nodes[i].subgraph_size;
            sem.signal();
        });
    }
    
    for(unsigned int i = 0; i < nodes.size(); i++)
        sem.wait();
    
    sort(nodes.begin(), nodes.end(), compareNodeId);
}

unsigned int Graph::hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
