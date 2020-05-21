/*
    ASA 19/20, 2nd project

    David Ferreira de Sousa Duque, nº 93698
    Ricardo Jorge Santos Subtil. nº 93752
*/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

class LinkedList
{
public:
    LinkedList();
    LinkedList(int);
    ~LinkedList();

    int data;
    LinkedList *next = nullptr;
};

struct BFSNode {
    int vertex;
    BFSNode* parent;
};

class BFSQueue
{
public:
    BFSQueue();
    BFSQueue(BFSNode);

    BFSNode data;
    BFSQueue *next = nullptr;
};

class Graph {
    public:
        Graph(int, int);
        ~Graph();
        void addSupermarket(const int, const int);
        void addHome(const int, const int);
        LinkedList* getSourceNode();
        int getSourceNodePos();
        LinkedList* getTargetNode();
        int getTargetNodePos();

        int getMaxSafeFlow();

        unordered_set<int> targets;
        unordered_set<int> homes;

        int numAvenues;
        int numStreets;

    //private:
        vector<LinkedList*> nodes;
};

int main() {
    int avenues;
    int streets;
    int supermarkets;
    int citizens;
    cin >> avenues;
    cin >> streets;
    cin >> supermarkets;
    cin >> citizens;

    Graph city(avenues, streets);

    int a, s;
    for (int i = 0; i < supermarkets; ++i) {
        cin >> a;
        cin >> s;
        city.addSupermarket(a, s);
    }

    for (int i = 0; i < citizens; ++i) {
        cin >> a;
        cin >> s;
        city.addHome(a, s);
    }

    cout << city.getMaxSafeFlow() << endl;

    return EXIT_SUCCESS;
}

LinkedList::LinkedList() {}

LinkedList::LinkedList(int data)
    : data(data) {}

LinkedList::~LinkedList()
{
    delete next;
}

BFSQueue::BFSQueue()
    : data({0, nullptr}) {}

BFSQueue::BFSQueue(BFSNode data)
    : data(data) {}

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = unordered_set<int>();
    homes = unordered_set<int>();

    // Initialize all node edges to null
    nodes = vector<LinkedList*>(2 * (avenues * streets) + 2);
    fill(nodes.begin(), nodes.end(), nullptr);

    // Add source and target nodes
    nodes[2 * (avenues*streets)] = nullptr;
    nodes[2 * (avenues*streets) + 1] = nullptr;
}

Graph::~Graph() {
    for (vector<LinkedList*>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
        if (*i) {
            delete *i;
        }
    }

    nodes.clear();
}

void Graph::addSupermarket(const int a, const int s) {
    if(targets.count(2*((a-1) * numStreets + (s - 1))) == 0)
        targets.insert(2*((a-1) * numStreets + (s - 1)));
}

void Graph::addHome(const int a, const int s) {
    if(homes.count(2*((a-1) * numStreets + (s - 1))) == 0)
        homes.insert(2*((a-1) * numStreets + (s - 1)));
}

LinkedList* Graph::getSourceNode() {
    return nodes[getSourceNodePos()];
}

int Graph::getSourceNodePos() {
    return 2 * (numAvenues * numStreets);
}

LinkedList* Graph::getTargetNode() {
    return nodes[getTargetNodePos()];
}

int Graph::getTargetNodePos() {
    return 2 * (numAvenues * numStreets) + 1;
}

vector<int> findNeighbors(int vertex, Graph* graph) {
    vector<int> directions;
    if(vertex % 2 == 0) {
        // vertex is Vin; neighbors are Vout
        int realVertexPos = (vertex) / 2;
        // Calculate the directions
        // Direction up
        if(realVertexPos / graph->numStreets > 0) {
            directions.push_back((realVertexPos - graph->numStreets) * 2 + 1);
        }
        // Direction down
        if(realVertexPos / graph->numStreets < graph->numAvenues - 1) {
            directions.push_back((realVertexPos + graph->numStreets) * 2 + 1);
        }
        // Direction left
        if(realVertexPos % graph->numStreets > 0) {
            directions.push_back((realVertexPos - 1) * 2 + 1);
        }
        // Direction right
        if(realVertexPos % graph->numStreets < graph->numStreets - 1) {
            directions.push_back((realVertexPos + 1) * 2 + 1);
        }
    } else {
        // vertex is Vout; neighbors are Vin
        int realVertexPos = (vertex - 1) / 2;
        // Calculate the directions
        // Direction up
        if(realVertexPos / graph->numStreets > 0) {
            directions.push_back((realVertexPos - graph->numStreets) * 2);
        }
        // Direction down
        if(realVertexPos / graph->numStreets < graph->numAvenues - 1) {
            directions.push_back((realVertexPos + graph->numStreets) * 2);
        }
        // Direction left
        if(realVertexPos % graph->numStreets > 0) {
            directions.push_back((realVertexPos - 1) * 2);
        }
        // Direction right
        if(realVertexPos % graph->numStreets < graph->numStreets - 1) {
            directions.push_back((realVertexPos + 1) * 2);
        }
    }

    return directions;
}

bool visit(BFSQueue* queue, Graph* graph, vector<bool>& visited) {
    BFSNode* position = &(queue->data);
    // cout << "Visiting " << position->vertex << "\n";

    // Check if it's either the super source or super target
    if(position->vertex == graph->getSourceNodePos()) {
        // Super source - BFS is beginning
        if(graph->homes.empty()) {
            // There are no more augmenting paths, algorithm is over!
            return false;
        } else {
            // Augmenting path; Add it
            int node = (*(graph->homes.begin()));
            graph->homes.erase(graph->homes.begin());
            while(queue->next != nullptr) {
                queue = queue->next;
            }
            BFSQueue* queueNext = new BFSQueue(BFSNode{node, position});
            queue->next = queueNext;
            visited[position->vertex] = true;
            return false;
        }
    } else if(position->vertex == graph->getTargetNodePos()) {
        // Super target - BFS is over
        //cout << "Super target" << endl;
        BFSNode* trace = position;
        //cout << "Trace: ";
        do {
            int child = trace->vertex;
            //cout << child << " ";
            trace = trace->parent;
            LinkedList* ll = new LinkedList(child);

            // If we are the super-source, we add a connection instead of replacing
            if(trace->vertex == graph->getSourceNodePos())
                ll->next = graph->nodes[trace->vertex];
            // Parent has no connection, so it's a new path
            else if(graph->nodes[trace->vertex] == nullptr)
                graph->nodes[trace->vertex] = ll;
            else {
                // Parent has a connection; this is an augmenting path; replace connection and skip this one
                graph->nodes[trace->vertex] = ll;
                trace = trace->parent;
            }
        } while(trace->parent != nullptr);
        //cout << trace->vertex << endl;

        visited[position->vertex] = true;
        return true;
    }

    // Is this vertex already visited?
    if (visited[position->vertex]) {
        return false;
    // Is this vertex a supermarket and a vout?
    } else if (graph->targets.find(position->vertex - 1) != graph->targets.end()) {
        // We need to connect to super target
        int node = graph->getTargetNodePos();
        while(queue->next != nullptr) {
            queue = queue->next;
        }
        BFSQueue* queueNext = new BFSQueue(BFSNode{node, position});
        queue->next = queueNext;
        visited[position->vertex] = true;
        return false;
    }

    // There are no special cases, so visit all it's neighbors
    if(position->vertex % 2 == 0) {
        // Vertex is even, therefore a Vin. Enter if it still hasn't a connection to Vout
        int node = position->vertex + 1;
        if(graph->nodes[position->vertex] == nullptr) {
            while(queue->next != nullptr) {
                queue = queue->next;
            }
            BFSQueue* queueNext = new BFSQueue(BFSNode{node, position});
            queue->next = queueNext;
        }
        // If this Vin has a connection, it means it can flow back.
        vector<int> neighbors = findNeighbors(position->vertex, graph);
        for(vector<int>::iterator iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
            if(graph->nodes[*iter] != nullptr && graph->nodes[*iter]->data == position->vertex) {
                while(queue->next != nullptr) {
                    queue = queue->next;
                }
                BFSQueue* queueNext = new BFSQueue(BFSNode{*iter, position});
                queue->next = queueNext;
            }
        }
    } else {
        // Vertex is odd, therefore a Vout
        vector<int> directions = findNeighbors(position->vertex, graph);

        // If Vout has flow, it means Vin is sending it
        if(graph->nodes[position->vertex-1] != nullptr) {
            directions.push_back(position->vertex-1);
        }

        // Remove from the options the current connection, if there's any
        for (vector<int>::iterator iter = directions.begin(); iter != directions.end(); ++iter) {
            //cout << "Neighbor is (" << neighbor.avenue << ", " << neighbor.street << "). Visited is " << visitedVertices[neighbor.avenue * graph->numAvenues + neighbor.street] << endl;
            if(graph->nodes[position->vertex] == nullptr || graph->nodes[position->vertex]->data != *iter) {
                while(queue->next != nullptr) queue = queue->next;
                BFSQueue* queueNext = new BFSQueue(BFSNode{*iter, position});
                queue->next = queueNext;
            }
        }
    }

    // Mark node as visited
    visited[position->vertex] = true;

    return false;
}

void printQueue(BFSQueue* q) {
    cout << "Queue: ";
    while(q != nullptr) {
        cout << q->data.vertex << " ";
        q = q->next;
    }
    cout << endl;
}

int Graph::getMaxSafeFlow() {
    while(!homes.empty()) {
        BFSQueue* queue = new BFSQueue();
        BFSQueue* head = queue;

        // Initiate a BFSQueue at the super-source
        BFSNode sourceNode = {getSourceNodePos(), nullptr};
        queue->data = sourceNode;

        vector<bool> visited = vector<bool>(nodes.size());
        fill(visited.begin(), visited.end(), false);

        while(queue != nullptr) {
            if(visit(queue, this, visited)) {
                break;
            }
            queue = queue->next;
        }

        // Queue is empty, so the algorithm is over. Clean up the queue
        while(head != nullptr) {
            BFSQueue* p = head;
            head = head->next;
            delete p;
        }

        //printQueue(head);
    }

    // Query the number of connections the super-source has
    int connections = 0;
    for (unordered_set<int>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        if(nodes[*iter] != nullptr) connections++;
    }

    return connections;
}
