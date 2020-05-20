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

/*enum class Status: int8_t {
    Busy = 0,
    Free = 1,
    Temp = 2,
};*/

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
    //~BFSQueue();

    BFSNode data;
    BFSQueue *next = nullptr;
};

class Graph {
    public:
        Graph(int, int);
        ~Graph();
        //int distanceToNearestSupermarket(Coordinates&) const;
        //bool operator() (Coordinates&, Coordinates&) const;
        void addSupermarket(const int, const int);
        void addHome(const int, const int);
        //LinkedList* getMatrixPos(const Coordinates&) const;
        //void setMatrixPos(const Coordinates&, LinkedList* value);
        LinkedList* getSourceNode();
        int getSourceNodePos();
        LinkedList* getTargetNode();
        int getTargetNodePos();
        BFSNode* getFreeHome();
        //int toIndex(const Coordinates&) const;
        //Coordinates toPos(const int&) const;

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

// Coordinates

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
    //matrix = new Status[avenues * streets];
    // Initialize the matrix
    //fill(matrix, &(matrix[avenues * streets]), Status::Free);
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
    targets.insert(2*((a-1) * numStreets + (s - 1)));
}

void Graph::addHome(const int a, const int s) {
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
        /*unordered_set<int> homes = graph->homes;
        LinkedList* ll = graph->getSourceNode();
        while(ll != nullptr) {
            graph->homes.erase(ll->data);
            ll = ll->next;
        }*/

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
            // If child is connected to parent, it's a residual path; delete it
            if(graph->nodes[child] != nullptr && graph->nodes[child]->data == trace->vertex) {
                delete graph->nodes[child];
                graph->nodes[child] = nullptr;
            // Else we connect the child to the parent
            } else {
                LinkedList* ll = new LinkedList(child);
                if(trace->vertex == graph->getSourceNodePos())
                    ll->next = graph->nodes[trace->vertex];
                else
                    delete graph->nodes[trace->vertex];
                graph->nodes[trace->vertex] = ll;
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
        /*if(graph->nodes[position->vertex] != nullptr) {
            directions.remove(position->vertex);
        }*/
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
    bool hasPath = true;
    int interim = 0;

    while(hasPath || !homes.empty()) {
        hasPath = false;
        BFSQueue* queue = new BFSQueue();
        BFSQueue* head = queue;

        BFSNode sourceNode = {getSourceNodePos(), nullptr};
        queue->data = sourceNode;

        vector<bool> visited = vector<bool>(nodes.size());
        fill(visited.begin(), visited.end(), false);
        //cout << "BFS augmentation" << endl;

        while(queue != nullptr) {
            //printQueue(queue);
            if(visit(queue, this, visited)) {
                hasPath = true;
                ++interim;
                break;
            }
            // If, after a single BFS visit, it has no children, it means no augmenting paths exist
            /*if(head->next == nullptr) {
                done = true;
                break;
            }*/
            queue = queue->next;
        }

        while(head != nullptr) {
            BFSQueue* p = head;
            head = head->next;
            delete p;
        }

        //delete head;
    }

    int connections = 0;

    LinkedList* sourceLL = getSourceNode();
    while(sourceLL != nullptr) {
        ++connections;
        sourceLL = sourceLL->next;
    }

    cout << interim << endl;
    return connections;
}
