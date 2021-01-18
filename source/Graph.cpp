#include "Graph.hpp"

using namespace std;

void Graph::init()
{
	roots.reserve(nNodes);
	leafs.reserve(nNodes);
	nodes.resize(nNodes + 1);
	if (roots.size() > 0)
		return;
	for (unsigned int i = 0; i < nNodes; i++)
	{
		nodes[i].id = i;
		roots.insert(i);
	}
}

Graph::~Graph()
{
	if (init_tw_done)
	{
		for (auto &cw : child_workers)
			cw.addTask([]() -> bool {
				return true;
			});
		for (auto &pw : parent_workers)
			pw.addTask([]() -> bool {
				return true;
			});
	}
}

// should only be used by build
void Graph::build_addEdges(unsigned int u, vector<unsigned int> &adj, unsigned int adj_size)
{
	nodes[u].adj.resize(adj_size);
	for (unsigned int i = 0; i < adj_size; i++)
	{
		nodes[u].adj[i] = adj[i];
		nodes[adj[i]].inc_count++;

		nodes[adj[i]].inc.push_back(u);
	}
}
Graph::Graph(FILE *fp) : nodes(), roots(), P() {
    unsigned int u;
    char str;
    vector<unsigned> buf;
    unsigned val;
    
    fscanf(fp, "%d\n", &nNodes);
    init();
	buf = vector<unsigned int>(nNodes + 1);
    while (fscanf(fp, "%d: ", &u) != EOF) {
        while(fscanf(fp, "%d ", &val)) {
            buf.push_back(val);
            roots.erase(val);
        }
        build_addEdges(u, buf, buf.size());
        if(buf.size() == 0) {
			leafs.insert(u);
        }
        buf.clear();
        fscanf(fp, "#\n");
    }
    o_leafs = vector<unsigned int>(leafs.begin(), leafs.end());

	// hidden parent of all roots
	nodes[nNodes].adj = vector<unsigned int>(roots.begin(), roots.end());
	sort(nodes[nNodes].adj.begin(), nodes[nNodes].adj.end(), std::less<unsigned int>());
}

Graph::Graph(ifstream &input_stream) : nodes(), roots(), P()
{
    
	unsigned int u, v;
	string node_adj;
	vector<unsigned int> buf;
	if (getline(input_stream, node_adj))
	{
		istringstream iss(node_adj);
		iss >> nNodes;
		init();
		buf = vector<unsigned int>(nNodes + 1);
	}
	while (getline(input_stream, node_adj))
	{

		unsigned int i = 0;
		string dontcare;
		node_adj.pop_back();
		istringstream iss(node_adj);
		iss >> u;
		iss >> dontcare;
		// get the first token
		// walk through other tokens
		while (iss >> v)
		{
			roots.erase(v);
			buf.at(i++) = v;
		}
		this->build_addEdges(u, buf, i);
		if (i == 0)
			leafs.insert(u);
	}

	o_leafs = vector<unsigned int>(leafs.begin(), leafs.end());

	// hidden parent of all roots
	nodes[nNodes].adj = vector<unsigned int>(roots.begin(), roots.end());
	sort(nodes[nNodes].adj.begin(), nodes[nNodes].adj.end(), std::less<unsigned int>());
}

Graph::Graph(unsigned int nNodes) : nodes(), roots(), P()
{
	this->nNodes = nNodes;
	init();
}

void Graph::printGraph()
{

	for (int v = 0; v < nNodes; ++v)
	{
		cout << v << ": " << (nodes[v].parent == nNodes ? -1 : nodes[v].parent);
		cout << endl;
	}

	cout << "Roots (" << roots.size() << "): " << endl;
	for (const auto &x : roots)
		cout << x << " ";
	cout << endl;

	cout << "Leafs (" << leafs.size() << "): " << endl;
	for (const auto &x : leafs)
		cout << x << " ";
	cout << endl;
}

void Graph::printNodesStatus()
{
	for (int v = 0; v < nNodes; ++v)
		cout << v << " " << nodes[v].s << " " << nodes[v].e << endl;
}

// meant to be used after building the graph.
void Graph::sortVectors()
{

	FastSemaphore sem;
	initThreadWorkers();

	for (int v = 0; v < nNodes; v++)
	{
		parent_workers[hash(v) % parent_workers.size()].addTask([this, v, &sem]() -> bool {
			sort(nodes[v].adj.begin(), nodes[v].adj.end(), std::less<int>());
			sem.signal();
			return false;
		});
	}

	for (unsigned int i = 0; i < nNodes; i++)
		sem.wait();
}

void Graph::addEdge(unsigned int u, unsigned int v)
{
	nodes[u].adj.push_back(v);
	nodes[v].inc_count++;
	nodes[v].inc.push_back(u);
}

void Graph::initThreadWorkers()
{
	unsigned int hw_conc = std::thread::hardware_concurrency() * 2;
	if (!init_tw_done)
	{

		vector<ThreadWorker> to_swapp(hw_conc);
		parent_workers.swap(to_swapp);

		vector<ThreadWorker> to_swapc(hw_conc);
		child_workers.swap(to_swapc);

		vector<FastSemaphore> to_swap_sem(hw_conc);
		worker_semaphores.swap(to_swap_sem);

		threads.reserve(parent_workers.size() + child_workers.size());

		for (auto &worker : parent_workers)
		{
			threads.emplace_back(&ThreadWorker::processTasks, &worker);
			threads.back().detach();
		}
		for (auto &worker : child_workers)
		{
			threads.emplace_back(&ThreadWorker::processTasks, &worker);
			threads.back().detach();
		}

		init_tw_done = true;
	}
}
void Graph::computeSubDTSize()
{
	FastSemaphore sem;
	initThreadWorkers();

	if (leafs.size() <= 0)
	{
		cout << "Error: no leafs" << endl;
		return;
	}

	vector<unsigned int> Q(leafs.begin(), leafs.end());

	while (Q.size() > 0)
	{

		for (const auto &node : Q)
		{

			int parent = nodes[node].parent;
			if (parent >= 0)
			{
				C.push(parent);
			}
		}

		vector<unsigned int> tmp = C.move_underlying_queue();
		C = SafeQueue<unsigned int>();
		Q = vector<unsigned int>();
		Q.reserve(tmp.size());
		unsigned int lnodes = 0;
		for (const auto &node : tmp)
		{
			if (nodes[node].adj.size() == ++nodes[node].adj_visited_count)
			{
				lnodes++;

				unsigned int worker_id = hash(node) % parent_workers.size();
				parent_workers[worker_id].addTask([this, node, &sem, worker_id]() -> bool {
					this->subDTSize_computePrefixSum(node);
					sem.signal();
					return false;
				});
				Q.push_back(node);
			}
		}

		for (unsigned int i = 0; i < lnodes; i++)
			sem.wait();
	}
}

void Graph::subDTSize_computePrefixSum(unsigned int p)
{
	for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
	{
		nodes[p].subgraph_size += nodes[nodes[p].adj[i]].subgraph_size;
	}
}

void Graph::buildDT()
{
	FastSemaphore sem;
	initThreadWorkers();

	if (nodes[nNodes].adj.size() <= 0 || nNodes <= 0)
	{
		throw "Error: no roots";
	}

	nodes[nNodes].path = {nNodes};
	nodes[nNodes].no_path = false;
	for (const auto &child : nodes[nNodes].adj)
	{
		unsigned int worker_id = hash(child) % parent_workers.size();
		parent_workers[worker_id].addTask([this, child, &sem, worker_id]() -> bool {
			nodes[child].inc_count = 1;
			sem.signal();
			return false;
		});
	}

	for (unsigned int i = 0; i < nodes[nNodes].adj.size(); i++)
		sem.wait();

	vector<unsigned int> Q = {nNodes};

	while (Q.size() > 0)
	{
		for (const auto &node : Q)
		{
			unsigned int worker_id = hash(node) % parent_workers.size();
			parent_workers[worker_id].addTask([this, node, &sem, worker_id]() -> bool {
				this->buildDT_processParent(node, worker_id);
				sem.signal();
				return false;
			});
		}

		for (unsigned int i = 0; i < Q.size(); i++)
			sem.wait();

		Q = P.move_underlying_queue();
		P = SafeQueue<unsigned int>();
	}

	for (unsigned int i = 0; i < nodes.size() - 1; i++)
	{
		unsigned int worker_id = hash(i) % parent_workers.size();
		parent_workers[worker_id].addTask([this, i, &sem, worker_id]() -> bool {
			swap(nodes[i].adj, nodes[i].dt_adj);
			sort(nodes[i].adj.begin(), nodes[i].adj.end(), std::less<unsigned int>());
			sem.signal();
			return false;
		});
	}

	for (unsigned int i = 0; i < nodes.size() - 1; i++)
		sem.wait();

	for (unsigned int i = 0; i < nodes.size() - 1; i++)
		if (nodes[i].adj.size() == 0)
			leafs.insert(i);
}

void Graph::buildDT_processParent(const unsigned int p, unsigned int worker_id)
{
	// This last part transforms the graph into a DT

	for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
	{
		unsigned node = nodes[p].adj[i];
		child_workers[hash(node) % child_workers.size()].addTask([this, node, p, worker_id]() -> bool {
			this->buildDT_processChild(node, p);
			this->worker_semaphores[worker_id].signal();
			return false;
		});
	}

	if (nodes[p].parent >= 0 && nodes[p].parent < nNodes)
	{
		child_workers[hash(nodes[p].parent) % child_workers.size()].addTask([this, p, worker_id]() -> bool {
			this->nodes[nodes[p].parent].dt_adj.push_back(p);
			this->worker_semaphores[worker_id].signal();
			return false;
		});

		this->worker_semaphores[worker_id].wait();
	}

	for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
		this->worker_semaphores[worker_id].wait();
}

void Graph::buildDT_processChild(unsigned int child, unsigned int p)
{
	bool select_new_path = false;
#if GRAPH_DEBUG
	if (nodes[child].parent == p)
		throw "Error, a father should visit each child only once";
#endif
	unsigned long lenght = min(nodes[p].path.size() + 1, nodes[child].path.size());
	for (unsigned long i = 0; i < lenght; i++)
	{
		if (i < nodes[p].path.size())
		{
			if (nodes[p].path[i] != nodes[child].path[i])
			{
				select_new_path = nodes[p].path[i] < nodes[child].path[i];
				break;
			}
		}
		else
		{
			// path pruning
			if (nodes[p].adj.size() > 1)
				select_new_path = child < nodes[child].path[i];
		}
	}

	if (select_new_path || nodes[child].no_path)
	{
		nodes[child].path = vector<unsigned int>(nodes[p].path);
		// path pruning
		if (nodes[p].adj.size() > 1)
			nodes[child].path.push_back(child);
		nodes[child].no_path = false;
		nodes[child].parent = p;
	}

	nodes[child].inc_visited_count++;
	if (nodes[child].inc_visited_count == nodes[child].inc_count)
	{
		nodes[child].inc_visited_count = 0;
		P.push(child);
	}
}

void Graph::sequentialDFS()
{
	pre = post = 0;

	for (auto &root : nodes[nNodes].adj)
		sequentialDFS_r(root);

	pre = post = 0;
}

void Graph::sequentialDFS_r(unsigned int p)
{
	nodes[p].visited = true; // node visited
	nodes[p].pre = pre++;
	for (auto &child : nodes[p].adj)
	{
		if (!nodes[child].visited)
		{
			nodes[child].parent = p;
			sequentialDFS_r(child);
		}
		if (nodes[p].s > nodes[child].s)
			nodes[p].s = nodes[child].s;
	}
	nodes[p].post = post++;
	nodes[p].e = nodes[p].post + 1;
	if (nodes[p].s > nodes[p].e)
		nodes[p].s = nodes[p].e;
}

void Graph::computePrePostOrder()
{
	FastSemaphore sem;
	initThreadWorkers();
	if (nodes[nNodes].adj.size() <= 0 || nNodes <= 0)
	{
		throw "Error: no roots";
	}

	vector<unsigned int> Q = {nNodes};

	int depth = 0;

	while (Q.size() > 0)
	{
		for (const auto &node : Q)
		{
			unsigned int worker_id = hash(node) % parent_workers.size();
			parent_workers[worker_id].addTask([this, node, depth, &sem, worker_id]() -> bool {
				this->computePrePost_processParent(node, depth, worker_id);
				sem.signal();
				return false;
			});
		}

		for (unsigned int i = 0; i < Q.size(); i++)
			sem.wait();

		Q = P.move_underlying_queue();
		P = SafeQueue<unsigned int>();

		depth++;
	}
}

void Graph::computePrePost_processParent(const unsigned int p, unsigned int depth, unsigned int worker_id)
{
	unsigned int pre = nodes[p].pre;
	unsigned int post = nodes[p].post;

	for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
	{
		unsigned int node = nodes[p].adj[i];
		child_workers[hash(node) % child_workers.size()].addTask([this, node, pre, post, worker_id]() -> bool {
			this->computePrePost_processChild(node, pre, post);
			this->worker_semaphores[worker_id].signal();
			return false;
		});
	}

	if (p == nNodes)
	{
		nodes[p].pre = pre + depth;
		nodes[p].post = post + nodes[p].subgraph_size;
	}
	else
	{
		nodes[p].pre = pre + depth - 1;
		nodes[p].post = post + nodes[p].subgraph_size - 1;
	}

	for (unsigned int i = 0; i < nodes[p].adj.size(); i++)
		this->worker_semaphores[worker_id].wait();
}

void Graph::computePrePost_processChild(unsigned int child, unsigned int pre, unsigned int post)
{
	if (nodes[child].parent >= 0)
	{
		unsigned int parent = nodes[child].parent;
		for (unsigned int i = 0; i < nodes[parent].adj.size() && nodes[parent].adj[i] < child; i++)
		{
			unsigned int brother = nodes[parent].adj[i];
			nodes[child].prefix_subgraph_size += nodes[brother].subgraph_size;
		}
	}

	nodes[child].pre = pre + nodes[child].prefix_subgraph_size;
	nodes[child].post = post + nodes[child].prefix_subgraph_size;

	if (++nodes[child].inc_visited_count == 1)
	{
		P.push(child);
	}
#if GRAPH_DEBUG
	else
	{
		throw "Nodes should only be visited by one parent, graph is not a DT";
	}
#endif
}

void Graph::computeLabels()
{
	FastSemaphore sem;
	initThreadWorkers();

	if (leafs.size() <= 0)
	{
		cout << "Error: no leafs" << endl;
		return;
	}

	vector<unsigned int> Q(o_leafs.begin(), o_leafs.end());
	while (Q.size() > 0)
	{
		for (const auto node : Q)
		{
			unsigned int worker_id = hash(node) % child_workers.size();
			child_workers[worker_id].addTask([this, node, &sem]() -> bool {
				nodes[node].s = nodes[node].e = nodes[node].post + 1;
				for (const auto child : nodes[node].dt_adj)
				{
					if (nodes[node].s > nodes[child].s)
						nodes[node].s = nodes[child].s;
				}
				for (const auto parent : nodes[node].inc)
					C.push(parent);
				sem.signal();
				return false;
			});
		}

		for (unsigned int i = 0; i < Q.size(); i++)
			sem.wait();

		vector<unsigned int> tmp = C.move_underlying_queue();
		C = SafeQueue<unsigned int>();
		Q = vector<unsigned int>();
		Q.reserve(C.size());
		for (const auto n : tmp)
		{
			if (nodes[n].dt_adj.size() == ++nodes[n].lab_visited_count)
				Q.push_back(n);
		}
	}
}

unsigned int Graph::hash(unsigned int x)
{
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return x;
}
