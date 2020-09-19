#include "Graph.hpp"


using namespace std;

void Graph::init() {
        roots.reserve(nNodes);
        leafs.reserve(nNodes);
        nodes.resize(nNodes + 1);
        if (roots.size() > 0)
                return;
        for (unsigned int i = 0; i < nNodes; i++) {
                nodes[i].id = i;
                roots.insert(i);
        }
}

// should only be used by build
void Graph::build_addEdges(unsigned int u, vector < unsigned int > & adj, unsigned int adj_size) {
        nodes[u].adj.resize(adj_size);
        for (unsigned int i = 0; i < adj_size; i++) {
                nodes[u].adj[i] = adj[i];
                nodes[adj[i]].inc_count++;
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
    
        for (int v = 0; v < nNodes; ++v) {
            cout << v << ": " << (nodes[v].parent == nNodes ? -1 : nodes[v].parent);
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
    
        for (int v = 0; v < nNodes; ++v) {
        
                //cout << v << ": parent = " << nodes[v].parent << endl;
//                cout << v << ": sub graph size = " << nodes[v].subgraph_size << " prefix sub graph size: " << nodes[v].prefix_subgraph_size << " pre: " << nodes[v].pre << " post: " << nodes[v].post << endl;
            cout << v << ":\tparent=" << (nodes[v].parent == nNodes ? -1 : nodes[v].parent) << "\tpre=" << nodes[v].pre << "\tpost=" << nodes[v].post << endl;
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
        for (int v = 0; v < nNodes; ++v) {
                sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less < int > ());
        }
}

void Graph::addEdge(unsigned int u, unsigned int v) {
        nodes[u].adj.push_back(v);
        nodes[v].inc_count++;
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
                                parent_workers[worker_id].addTask([this, parent, node, & sem, worker_id]() -> void {
                                                                          this -> subDTSize_processParent(parent, node);
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
                                                                  this -> subDTSize_computePrefixSum(node);
                                                                  sem.signal();
                                                          });
                }
        
                for (unsigned int i = 0; i < Q.size(); i++)
                        sem.wait();
        
        }
}

void Graph::subDTSize_processParent(unsigned int p, unsigned int i) {
        if (nodes[p].adj.size() == ++nodes[p].adj_visited_count)
                C.push(p);
}

void Graph::subDTSize_computePrefixSum(unsigned int p) {
        for (unsigned int i = 0; i < nodes[p].adj.size(); i++) {
                nodes[p].subgraph_size += nodes[nodes[p].adj[i]].subgraph_size;
        }
}

void Graph::buildDT() {
        FastSemaphore sem;
        initThreadWorkers();
    
        if (nodes[nNodes].adj.size() <= 0 || nNodes <= 0 ) {
                throw "Error: no roots";
        }
    
        nodes[nNodes].path = {nNodes};
        nodes[nNodes].no_path = false;
        for(const auto& child: nodes[nNodes].adj) {
                unsigned int worker_id = hash(child) % parent_workers.size();
                parent_workers[worker_id].addTask([this, child, &sem, worker_id]() -> void {
                                                          nodes[child].inc_count = 1;
                                                          sem.signal();
                                                  });
        }
    
        for(unsigned int i = 0; i < nodes[nNodes].adj.size(); i++)
                sem.wait();
    
        vector < unsigned int > Q = {nNodes};
        
            while (Q.size() > 0) {
            for(const auto& node: Q) {
                        unsigned int worker_id = hash(node) % parent_workers.size();
                        parent_workers[worker_id].addTask([this, node, & sem, worker_id]() -> void {
                                                                  this -> buildDT_processParent(node, worker_id);
                                                                  sem.signal();
                                                          });
                }
        
                for (unsigned int i = 0; i < Q.size(); i++)
                        sem.wait();
        
                Q = P.move_underlying_queue();
                P = SafeQueue < unsigned int > ();
        }
    
        for (unsigned int i = 0; i < nodes.size() - 1; i++) {
                unsigned int worker_id = hash(i) % parent_workers.size();
                parent_workers[worker_id].addTask([this, i, & sem, worker_id]() -> void {
                                                          nodes[i].adj = move(nodes[i].dt_adj);
                                                          sort(nodes[i].adj.begin(), nodes[i].adj.end(), std::less < unsigned int > ());
                                                          sem.signal();
                                                  });
        }
    
        for (unsigned int i = 0; i < nodes.size() - 1; i++)
                sem.wait();
    
        for (unsigned int i = 0; i < nodes.size() - 1; i++)
                if (nodes[i].adj.size() == 0)   leafs.insert(i);
    
}

void Graph::buildDT_processParent(const unsigned int p, unsigned int worker_id) {
        // This last part transforms the graph into a DT
    
        for(unsigned int i = 0; i < nodes[p].adj.size(); i++) {
                unsigned node = nodes[p].adj[i];
                child_workers[hash(node) %  child_workers.size()].addTask([this, node, p, worker_id] () -> void {
                                                                                  this->buildDT_processChild(node, p);
                                                                                  this->worker_semaphores[worker_id].signal();
                                                                          });
        }
    
        if(nodes[p].parent >= 0 && nodes[p].parent < nNodes) {
                child_workers[hash(nodes[p].parent) %  child_workers.size()].addTask([this, p, worker_id] () -> void {
                                                                                             this->nodes[nodes[p].parent].dt_adj.push_back(p);
                                                                                             this->worker_semaphores[worker_id].signal();
                                                                                     });
        
                this->worker_semaphores[worker_id].wait();
        }
    
        for(unsigned int i = 0; i < nodes[p].adj.size(); i++)
                this->worker_semaphores[worker_id].wait();
}

void Graph::buildDT_processChild(unsigned int child, unsigned int p) {
        bool select_new_path = false;
#if GRAPH_DEBUG
        if (nodes[child].parent == p)
                throw "Error, a father should visit each child only once";
#endif
        unsigned long lenght = min(nodes[p].path.size() + 1, nodes[child].path.size());
        for (unsigned long i = 0; i < lenght; i++) {
                if (i < nodes[p].path.size()) {
                        if (nodes[p].path[i] != nodes[child].path[i]) {
                                select_new_path = nodes[p].path[i] < nodes[child].path[i];
                                break;
                        }
                } else {
                        // path pruning
                        if(nodes[p].adj.size() > 1)
                                select_new_path = child < nodes[child].path[i];
                }
        }
    
        if (select_new_path || nodes[child].no_path) {
                nodes[child].path = vector < unsigned int > (nodes[p].path);
                // path pruning
                if(nodes[p].adj.size() > 1)
                        nodes[child].path.push_back(child);
                nodes[child].no_path = false;
                nodes[child].parent = p;
        }
    
        nodes[child].inc_visited_count++;
        if (nodes[child].inc_visited_count == nodes[child].inc_count) {
                nodes[child].inc_visited_count = 0;
                P.push(child);
        }
    
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
        if (nodes[nNodes].adj.size() <= 0 || nNodes <= 0) {
                throw "Error: no roots";
        }
    
        vector < unsigned int > Q = {nNodes};
        
        int depth = 0;
    
        while (Q.size() > 0) {
            for(const auto& node: Q) {
                        unsigned int worker_id = hash(node) % parent_workers.size();
                        parent_workers[worker_id].addTask([this, node, depth, & sem, worker_id]() -> void {
                                                                  this -> computePrePost_processParent(node, depth, worker_id);
                                                                  sem.signal();
                                                          });
            
                }
        
            for (unsigned int i = 0; i < Q.size(); i++)
                        sem.wait();
        
                Q = P.move_underlying_queue();
                P = SafeQueue < unsigned int > ();
        
                depth++;
        }
}

void Graph::computePrePost_processParent(const unsigned int p, unsigned int depth, unsigned int worker_id) {
        unsigned int pre = nodes[p].pre;
        unsigned int post = nodes[p].post;
    
        for (unsigned int i = 0; i < nodes[p].adj.size(); i++) {
                unsigned int node = nodes[p].adj[i];
                child_workers[hash(node) % child_workers.size()].addTask([this, node, pre, post, worker_id]() -> void {
                                                                                 this -> computePrePost_processChild(node, pre, post);
                                                                                 this -> worker_semaphores[worker_id].signal();
                                                                         });
        }
    
        if(p == nNodes) {
                nodes[p].pre = pre + depth;
                nodes[p].post = post + nodes[p].subgraph_size;
        } else {
                nodes[p].pre = pre + depth - 1;
                nodes[p].post = post + nodes[p].subgraph_size - 1;
        }

    
        for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
                this -> worker_semaphores[worker_id].wait();
}

void Graph::computePrePost_processChild(unsigned int child, unsigned int pre, unsigned int post) {
        if(nodes[child].parent >= 0) {
                unsigned int parent = nodes[child].parent;
                for(unsigned int i = 0; i < nodes[parent].adj.size() && nodes[parent].adj[i] < child; i++) {
                        unsigned int brother = nodes[parent].adj[i];
                        nodes[child].prefix_subgraph_size += nodes[brother].subgraph_size;
                }
        }
    
        nodes[child].pre = pre + nodes[child].prefix_subgraph_size;
        nodes[child].post = post + nodes[child].prefix_subgraph_size;
    
        if (++nodes[child].inc_visited_count == 1) {
                P.push(child);
        }
#if GRAPH_DEBUG
        else {throw "Nodes should only be visited by one parent, graph is not a DT";}
#endif
}

unsigned int Graph::hash(unsigned int x) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
}
