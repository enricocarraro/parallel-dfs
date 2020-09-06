#include "Graph.hpp"


using namespace std;

void Graph::init() {
    roots.reserve(nNodes);
    leafs.reserve(nNodes);
    nodes.resize(nNodes);
    if(roots.size() > 0)
        return;
    for (int i = 0; i < nNodes; i++) {
        nodes[i].id = i;
        roots.insert(i);
    }
}

// should only be used by build
void Graph::build_addEdges(unsigned u, vector<unsigned>& adj, unsigned adj_size)
{
    nodes[u].adj.resize(adj_size);
    for(int i = 0; i < adj_size; i++) {
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
    unsigned u, v;
    unsigned max_line_size = (log10(nNodes) + 2) * (nNodes + 1) + 3;
    char str[max_line_size];
    char dontcare[3];
    vector<unsigned> buf = vector<unsigned> (nNodes + 1);
    
    while(fscanf(fp, "%[^#]s", str) != EOF) {
        fscanf(fp, "%s", dontcare);
        char *token;
        unsigned i = 0;
        
        /* get the first token */
        token = strtok(str, " ");
#if GRAPH_DEBUG
        printf( " %s\n", token );
#endif
        sscanf(token, "%d", &u);
        token = strtok(NULL, " ");
        /* walk through other tokens */
        while(token != NULL){
#if GRAPH_DEBUG
            printf( " %s\n", token );
#endif
            sscanf(token, "%d", &v);
            
            roots.erase(v);
            
            buf.at(i++) = v;
            token = strtok(NULL, " ");
        }
        
        this->build_addEdges(u, buf, i);
        if(i == 0) leafs.insert(u);
    }
    
}



Graph::Graph(FILE * fp): nodes(), roots(), P()
{
    fscanf(fp, "%d", &nNodes);
    init();
    build(fp);
    
}

Graph::Graph(unsigned nNodes): nodes(), roots(), P()
{
    this->nNodes = nNodes;
    init();
}




void Graph::printGraph()
{
    
    for (int v = 0; v < nNodes; ++v)
    {
        cout << v << ": " << nodes[v].parent;
        /*
         for (auto x : nodes[v].adj)
         cout << x << " ";
         
         cout << "pre: " << nodes[v].pre << " post: " << nodes[v].post;
         */
        cout << endl;
        
    }
    
    cout << "Roots (" << roots.size() << "): " << endl;
    for ( const auto& x: roots )
        cout << x << " ";
    cout << endl;
    
    cout << "Leafs (" << leafs.size() << "): " << endl;
    for ( const auto& x: leafs )
        cout << x << " ";
    cout << endl;
    
}

void Graph::printNodesStatus()
{
    
    for (int v = 0; v < nNodes; ++v)
    {
        
        //cout << v << ": parent = " << nodes[v].parent << endl;
        cout << v << ": sub graph size = " << nodes[v].subgraph_size << endl;
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
void Graph::sortVectors()
{
    for (int v = 0; v < nNodes; ++v)
    {
        sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
    }
}

void Graph::addEdge(unsigned u, unsigned v)
{
    nodes[u].adj.push_back(v);
    nodes[v].inc_visited_count++;
}

void Graph::initThreadWorkers() {
    unsigned int hw_conc = std::thread::hardware_concurrency();
    if(!init_tw_done) {
        
        vector<ThreadWorker> to_swapp(hw_conc / 4 + 1);
        parent_workers.swap(to_swapp);
        
        vector<ThreadWorker> to_swapc(hw_conc * 3 / 4 + 1);
        child_workers.swap(to_swapc);
        
        vector<Semaphore> to_swap_sem(hw_conc);
        worker_semaphores.swap(to_swap_sem);
        init_tw_done = true;
    }
}
void Graph::computeSubGraphSize() {
    Semaphore sem;
    queue<unsigned> Q;
    initThreadWorkers();
    
    
    if(leafs.size() <= 0) {
        cout << "Error: no roots" << endl;
        return;
    }
    for (const auto& x: leafs) Q.push(x);
    
    while(Q.size() > 0) {
        
        size_t level_nodes = 0;
        while(Q.size() > 0) {
            unsigned node = Q.front();
            Q.pop();
            
            int parent = nodes[node].parent;
            if(parent >= 0) {
                level_nodes++;
                unsigned worker_id = hash(parent) %  parent_workers.size();
                parent_workers[worker_id].addTask([this, parent, node, &sem, worker_id] () -> void {
                    this->subGraphSize_processParent(parent, node);
                    sem.signal();
                });
            }
        }
        
        for(int i = 0; i < level_nodes; i++)
            sem.wait();
        
        level_nodes = C.size();
        while(C.size() > 0) {
            unsigned node = C.pop();
            Q.push(node);
            unsigned worker_id = hash(node) %  parent_workers.size();
            parent_workers[worker_id].addTask([this, node, &sem, worker_id] () -> void {
                this->subGraphSize_computePrefixSum(node);
                sem.signal();
            });
        }
        
        for(int i = 0; i < level_nodes; i++)
            sem.wait();
        
    }
}

//void Graph::subGraphSize_processChild(unsigned i, unsigned worker_id) {
//    if(nodes[i].parent >= 0) {
//        unsigned parent = nodes[i].parent;
//        child_workers[hash(parent) %  child_workers.size()].addTask([this, parent, i, worker_id] () -> void {
//            this->subGraphSize_processParent(parent, i);
//            this->worker_semaphores[worker_id].signal();
//        });
//
//        this->worker_semaphores[worker_id].wait();
//    }
//}

void Graph::subGraphSize_processParent(unsigned p, unsigned i) {
    if(!nodes[p].subgraph_size_parent_visited) {
        nodes[p].subgraph_size_parent_visited = true;
        sort(nodes[p].adj.begin(), nodes[p].adj.end(), std::less<int>());
    }
    
    if(nodes[p].adj.size() == ++nodes[p].adj_visited_count)
        C.push(p);
}

void Graph::subGraphSize_computePrefixSum(unsigned p) {
    for(int i = 0; i < nodes[p].adj.size(); i++)
        nodes[p].subgraph_size += nodes[nodes[p].adj[i]].subgraph_size;
}

void Graph::buildDT() {
    Semaphore sem;
    initThreadWorkers();
    queue<unsigned> Q;
    if(roots.size() <= 0) {
        cout << "Error: no roots" << endl;
        return;
    }
    
    for(auto& worker: parent_workers) {
        auto t = std::thread(&ThreadWorker::processTasks, &worker);
        t.detach();
    }
    for(auto& worker: child_workers) {
        auto t = std::thread(&ThreadWorker::processTasks, &worker);
        t.detach();
    }
    for (const auto& x: roots) Q.push(x);
    
    while(Q.size() > 0) {
        
        //      std::vector<std::promise<void>> p_list(Q.size());
        //        std::vector<std::future<void>> f_list;
        //        f_list.reserve(Q.size());
        
        size_t level_nodes = Q.size();
        //        unsigned index = 0;
        while(Q.size() > 0) {
            //            f_list.push_back(p_list[index++].get_future());
            unsigned node = Q.front();
            Q.pop();
            unsigned worker_id = hash(node) %  parent_workers.size();
            parent_workers[worker_id].addTask([this, node, &sem, worker_id] () -> void {
                this->buildDT_processParent(node, worker_id);
                sem.signal();
            });
        }
        
        for(int i = 0; i < level_nodes; i++)
            sem.wait();
        
        Q = P.move_underlying_queue();
        P = SafeQueue<unsigned>();
    }
    
    
    for(int i = 0; i < nodes.size(); i++) {
        unsigned worker_id = hash(i) %  parent_workers.size();
        parent_workers[worker_id].addTask([this, i, &sem, worker_id] () -> void {
            nodes[i].adj = move(nodes[i].dt_adj);
            sem.signal();
        });
    }
    
    
    for(int i = 0; i < nodes.size(); i++)
        sem.wait();
    
    for(unsigned i = 0; i < nodes.size(); i++)
        if(nodes[i].adj.size() == 0)
            leafs.insert(i);
    
    
}


void Graph::buildDT_processParent(const unsigned p, unsigned worker_id) {
    // This last part transforms the graph into a DT
    
    for(int i = 0; i < nodes[p].adj.size(); i++) {
        unsigned node = nodes[p].adj[i];
        child_workers[hash(node) %  child_workers.size()].addTask([this, node, p, worker_id] () -> void {
            this->buildDT_processChild(node, p);
            this->worker_semaphores[worker_id].signal();
        });
    }
    
    if(nodes[p].parent >= 0) {
        child_workers[hash(nodes[p].parent) %  child_workers.size()].addTask([this, p] () -> void {
            this->nodes[nodes[p].parent].dt_adj.push_back(p);
        });
    }
    
    for(int i = 0; i < nodes[p].adj.size(); i++)
        this->worker_semaphores[worker_id].wait();
}

void Graph::buildDT_processChild(unsigned child, unsigned p) {
    bool select_new_path = false;
#if GRAPH_DEBUG
    if(nodes[child].parent == p)
        throw "Error, a father should visit each child only once";
#endif
    unsigned long lenght = min(nodes[p].path.size() + 1, nodes[child].path.size());
    for(unsigned long i = 0; i < lenght; i++) {
        if(i < nodes[p].path.size()) {
            
            if(nodes[p].path[i] != nodes[child].path[i] ) {
                select_new_path = nodes[p].path[i] < nodes[child].path[i];
                
                break;
            }
        } else {
            
            select_new_path = p < nodes[child].path[i];
            
        }
    }
    
    if(select_new_path || nodes[child].no_path) {
        nodes[child].path = vector<unsigned>(nodes[p].path);
        nodes[child].path.push_back(p);
        nodes[child].no_path = false;
        nodes[child].parent = p;
    }
    
    
    
    nodes[child].inc_visited_count++;
    if(nodes[child].inc_visited_count == nodes[child].inc_count) {
        P.push(child);
    }
    
    
}

void Graph::sequentialDFS() {
    pre = post = 0;
    vector<unsigned int> roots_sorted(roots.begin(), roots.end());
    sort(roots_sorted.begin(), roots_sorted.end(), std::less<>());
    
    for(auto& root: roots_sorted)
        sequentialDFS_r(root);
    
    pre = post = 0;
}

void Graph::sequentialDFS_r(unsigned p) {
    nodes[p].visited = true; // node visited
    nodes[p].pre = pre++;
    for(auto& child: nodes[p].adj) {
        if(!nodes[child].visited) {
            nodes[child].parent = p;
            sequentialDFS_r(child);
        }
    }
    nodes[p].post = post++;
}



void Graph::computePrePostOrder(){}

unsigned int Graph::hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

