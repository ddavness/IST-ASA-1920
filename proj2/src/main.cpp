/*
    ASA 19/20, 2nd project

    David Ferreira de Sousa Duque, nº 93698
    Ricardo Jorge Santos Subtil. nº 93752
*/

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <set>

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
        void shuffleHomes();

        int getMaxSafeFlow();

        unordered_set<int> targets;
        unordered_set<int> homes;
        vector<int> shuffledHomes;

        // Temp Debug
        map<int, int> connections;

        int numAvenues;
        int numStreets;

    //private:
        vector<LinkedList*> nodes;
};

void dumpGraph(Graph* g, unordered_set<int> h);

int main() {
    int avenues;
    int streets;
    int supermarkets;
    int citizens;
    cin >> avenues;
    cin >> streets;
    cin >> supermarkets;
    cin >> citizens;

    srand(time(NULL));

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

    Graph copy = city;

    city.shuffleHomes();
    int firstMaxFlow = city.getMaxSafeFlow();

    cout << "First flow was " << firstMaxFlow << ", now running until flow is different." << endl;

    dumpGraph(&city, city.homes);

    int newFlow = firstMaxFlow;
    int iterations = 0;
    int div = iterations;
    vector<int> order;
    while(firstMaxFlow == newFlow && iterations < 100) {
        city = copy;
        city.shuffleHomes();
        order = city.shuffledHomes;
        newFlow = city.getMaxSafeFlow();
        iterations++;
        if (iterations / 1 > div) {
            div = iterations / 1;
            cout << "[" << div << "%]" << endl;
        }
    }

    if(iterations == 100) {
        cout << "Ran 100 iterations, flow stayed the same.";
        return 0;
    }

    cout << "New flow is different: First flow = " << firstMaxFlow << ", End flow = " << newFlow << endl;

    cout << "The order the algorithm ran was: ";
    for(vector<int>::iterator it = order.begin(); it != order.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    dumpGraph(&city, city.homes);

    return -1;
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
    connections = map<int, int>();
    
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

void Graph::shuffleHomes() {
    shuffledHomes = vector<int>();
    vector<int> homesCopy = vector<int>();
    for(unordered_set<int>::iterator it = homes.begin(); it != homes.end(); ++it) {
        homesCopy.push_back(*it);
    }
    while(!homesCopy.empty()) {
        int i = rand() % homesCopy.size();
        int j = 0;
        for(vector<int>::iterator it = homesCopy.begin(); it != homesCopy.end(); ++it) {
            if (i == j) {
                shuffledHomes.push_back(*it);
                homesCopy.erase(it);
                break;
            } else {
                j++;
            }
        }
    }
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

        if(graph->shuffledHomes.empty()) {
            // There are no more augmenting paths, algorithm is over!
            return false;
        } else {
            // Augmenting path; Add it
            int node = (*(graph->shuffledHomes.begin()));
            graph->shuffledHomes.erase(graph->shuffledHomes.begin());
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
        int targetV = trace->parent->vertex;
        //cout << "Trace: ";
        do {
            int child = trace->vertex;
            //cout << child << " ";
            trace = trace->parent;
            // If child is connected to parent, it's a residual path; it was
            // overwritten in the last loop, so now we need to skip it
            //if(graph->nodes[child] != nullptr && graph->nodes[child]->data == trace->vertex) {
            // Else we connect the child to the parent
            LinkedList* ll = new LinkedList(child);
            if(trace->vertex == graph->getSourceNodePos())
                ll->next = graph->nodes[trace->vertex];
            else if(graph->nodes[trace->vertex] == nullptr)
                graph->nodes[trace->vertex] = ll;
            else {
                // Parent has a connection; it's augmenting path
                graph->nodes[trace->vertex] = ll;
                trace = trace->parent;
            }
            if(trace->parent != nullptr && trace->parent->parent == nullptr)
                graph->connections[trace->vertex] = targetV - 1;
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

string indToCoord(int v, Graph* g) {
    string ret = "(";
    int a = (v / 2) / g->numStreets + 1;
    int s = (v / 2) % g->numStreets + 1;
    return ret + to_string(a) + ", " + to_string(s) + ")";
}

void dumpGraph(Graph* g, unordered_set<int> origHomes) {
    cout << "Dumping completed graph..." << endl << endl;
    cout << "C: " << origHomes.size() << "\t\tS: " << g->targets.size() << endl;
    cout << "-------------------------------" << endl;
    cout << "|    Vertex    | Connected to |" << endl;
    cout << "-------------------------------" << endl;
    for(unordered_set<int>::iterator it = origHomes.begin(); it != origHomes.end(); ++it) {
        if(g->nodes[*it] == nullptr) {
            cout << "| " << *it << " | " << "None" << endl;
            //cout << "| " << indToCoord(*it, g) << " | " << "None" << endl;
        } else {
            int to = g->connections[*it];
            //cout << "| " << indToCoord(*it, g) << " | "  << indToCoord(to, g) << endl;
            cout << "| " << *it << " | "  << to << endl;
        }
    }
    cout << "-------------------------------" << endl << endl;
    cout << "Dumping traces..." << endl << endl;
    for(unordered_set<int>::iterator it = origHomes.begin(); it != origHomes.end(); ++it) {
        if(g->nodes[*it] != nullptr) {
            cout << *it << " -> " << *it << " ";
            //cout << indToCoord(*it, g) << " -> " << indToCoord(*it, g) << " ";
            int i = *it;
            while(i != g->getTargetNodePos()) {
                i = g->nodes[i]->data;
                //if(i % 2 == 0)
                //    cout << indToCoord(i, g) << " ";
                cout << i << " ";
            }
        }
        cout << endl;
    }

    cout << "Dumping done" << endl;
}

int Graph::getMaxSafeFlow() {
    //bool hasPath = true;
    int interim = 0;

    while(/*hasPath ||*/ !shuffledHomes.empty()) {
        //hasPath = false;
        BFSQueue* queue = new BFSQueue();
        BFSQueue* head = queue;

        BFSNode sourceNode = {getSourceNodePos(), nullptr};
        queue->data = sourceNode;

        vector<bool> visited = vector<bool>(nodes.size());
        fill(visited.begin(), visited.end(), false);
        //cout << "BFS augmentation" << endl;

        //bool once = true;
        while(queue != nullptr) {
            //printQueue(queue);
            if(visit(queue, this, visited)) {
                //hasPath = true;
                ++interim;
                break;
            }
            /*if(once) {
                once = false;
                if(queue->next)
                cout << "Visiting vertex " << queue->next->data.vertex << "..." << endl;
            }*/
            // If, after a single BFS visit, it has no children, it means no augmenting paths exist
            /*if(head->next == nullptr) {
                done = true;
                break;
            }*/
            //printQueue(queue);
            queue = queue->next;
        }

        while(head != nullptr) {
            BFSQueue* p = head;
            head = head->next;
            delete p;
        }

        //dumpGraph(this, homes);

        //printQueue(head);
        //delete head;
    }

    int connections = 0;
    for (unordered_set<int>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        if(nodes[*iter] != nullptr) connections++;
    }
    /*LinkedList* sourceLL = getSourceNode();
    while(sourceLL != nullptr) {
        ++connections;
        sourceLL = sourceLL->next;
    }*/

    //dumpGraph(this, origHomes);

    //cout << interim << endl;
    return connections;
}
