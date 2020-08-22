#include "Graph.hpp"


using namespace std;

void Graph::init() {
    roots.reserve(nNodes);
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
        nodes[adj[i]].inc.emplace(u, false);
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
        
        cout << "\n Adjacency list of vertex " << v << "\n head ";
        
        for (auto x : nodes[v].adj)
            cout << "-> " << x;
        cout << endl;
        
        
    }
    
    cout << "Roots (" << roots.size() << "): " << endl;
    for ( const auto& x: roots )
        cout << x << " ";
    cout << endl;
}

void Graph::printNodesStatus()
{
    
    for (int v = 0; v < nNodes; ++v)
    {
        bool is_root = roots.find(v) != roots.end();
        string is_root_str = is_root ? " (root)" : "";
        
        cout << v << is_root_str << ": parent = " << nodes[v].parent << endl;
        
        if(!is_root) {
            cout << "Path: ";
            for ( const auto& x: nodes[v].path )
                cout << x << " ";
            cout << endl;
            
            
            cout << "Incoming edges status: visited=" << nodes[v].inc_visited_count << " total=" << nodes[v].inc.size() << endl;
            /*    for (auto& x : nodes[v].inc)
             cout << x.first << ": " << x.second << endl;
             */
            
        }
        cout << endl;
        
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
    nodes[v].inc.emplace(u, false);
    
}

void Graph::buildDT() {
    Semaphore sem;
    vector<ThreadWorker> to_swapp(std::thread::hardware_concurrency());
    parent_workers.swap(to_swapp);
    vector<ThreadWorker> to_swapc(std::thread::hardware_concurrency());
    child_workers.swap(to_swapc);
    vector<Semaphore> to_swap_sem(std::thread::hardware_concurrency());
    worker_semaphores.swap(to_swap_sem);
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
    
    
}


void Graph::buildDT_processParent(const unsigned p, unsigned worker_id) {
//  std::vector<std::promise<void>> p_list(nodes[p].adj.size());
//    std::vector<std::future<void>> f_list;
//    f_list.reserve(nodes[p].adj.size());
    for(int i = 0; i < nodes[p].adj.size(); i++) {
//        f_list.push_back(p_list[i].get_future());
        unsigned node = nodes[p].adj[i];
        child_workers[hash(node) %  child_workers.size()].addTask([this, node, p, worker_id] () -> void {
            this->buildDT_processChild(node, p);
            this->worker_semaphores[worker_id].signal();
        });
    }
    
    for(int i = 0; i < nodes[p].adj.size(); i++)
        this->worker_semaphores[worker_id].wait();
}

void Graph::buildDT_processChild(unsigned child, unsigned p) {
    bool select_new_path = false;
    vector<unsigned> new_path(nodes[p].path);
    new_path.push_back(p);
    
    
    for(int i = 0; i < min(new_path.size(), nodes[child].path.size()); i++) {
        if(new_path[i] != nodes[child].path[i] ) {
            select_new_path = new_path[i] < nodes[child].path[i];
            break;
        }
    }
    
    if(select_new_path || nodes[child].no_path) {
        nodes[child].path = std::move(new_path);
        nodes[child].parent = p;
    }
    
    if(nodes[child].inc.find(p) == nodes[child].inc.end() || nodes[child].inc[p]) {
        throw string("Error, impossible, father must be in inc list and must visit child only once.");
    }
    
    if(!nodes[child].inc[p]) {
        nodes[child].inc[p] = true;
        nodes[child].inc_visited_count++;
    }
    
    if(nodes[child].inc_visited_count == nodes[child].inc.size())
        P.push(child);
}

void Graph::sequentialDFS() {
    pre = post = 0;
    
    for(auto& root: roots)
        sequentialDFS_r(root);
    
    pre = post = 0;
}

void Graph::sequentialDFS_r(unsigned p) {
    nodes[p].visited = true; // node visited
    nodes[p].start = pre++;
    for(auto& child: nodes[p].adj) {
        if(!nodes[child].visited) {
            nodes[child].parent = p;
            sequentialDFS_r(child);
        }
    }
    nodes[p].end = post++;
}

void Graph::computeSubGraphSize(){}
void Graph::computePrePostOrder(){}

unsigned int Graph::hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
