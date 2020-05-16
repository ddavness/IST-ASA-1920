/*
    ASA 19/20, 2nd project

    David Ferreira de Sousa Duque, nº 93698
    Ricardo Jorge Santos Subtil. nº 93752
*/

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <limits>
#include <unordered_set>
#include <functional>

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

/*class Coordinates {
    public:
        Coordinates(int, int);
        Coordinates operator+(const Coordinates&) const;
        Coordinates operator-() const;
        Coordinates operator() () const;
        bool operator<(Coordinates) const;
        bool operator==(Coordinates) const;
        int distance(const Coordinates&) const;
        int avenue;
        int street;
};

// Define hash for coordinates
namespace std {
    template<>
        struct hash<Coordinates> {
            size_t operator()(const Coordinates& obj) const {
                return hash<string>()(to_string(obj.avenue) + ',' + to_string(obj.street));
            }
        };
}*/

class Graph {
    public:
        Graph(int, int);
        //int distanceToNearestSupermarket(Coordinates&) const;
        //bool operator() (Coordinates&, Coordinates&) const;
        void addSupermarket(const int&, const int&);
        void addHome(const int&, const int&);
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

/*BFSQueue::~BFSQueue()
{
    if(next) delete next;
}*/

/*Coordinates::Coordinates(int a, int s): avenue(a), street(s) {}

Coordinates Coordinates::operator+(const Coordinates& other) const {
    return Coordinates(avenue + other.avenue, street + other.street);
}
Coordinates Coordinates::operator-() const {
    return Coordinates(-avenue, -street);
}
bool Coordinates::operator<(Coordinates other) const {
    return avenue < other.avenue || (avenue == other.avenue && street < other.street);
}
bool Coordinates::operator==(Coordinates other) const {
    return avenue == other.avenue && street == other.street;
}

int Coordinates::distance(const Coordinates& other) const {
    return abs(avenue - other.avenue) + abs(street - other.street);
}*/

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = unordered_set<int>();
    homes = unordered_set<int>();

    // Initialize all node edges to null
    nodes = vector<LinkedList*>(2 * (avenues * streets) + 2);
    fill(nodes.begin(), nodes.end(), nullptr);

    // Add source and target nodes
    nodes[(avenues*streets)] = nullptr;
    nodes[(avenues*streets)+1] = nullptr;
    //matrix = new Status[avenues * streets];
    // Initialize the matrix
    //fill(matrix, &(matrix[avenues * streets]), Status::Free);
}

/*int Graph::distanceToNearestSupermarket(Coordinates& start) const {
    double distance = numeric_limits<double>::infinity();

    for (unordered_set<Coordinates>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        int newDist = start.distance(*iter);
        distance = min(distance, static_cast<double>(newDist));
    }

    return static_cast<int>(distance);
}

bool Graph::operator() (Coordinates& alpha, Coordinates& beta) const {
    return distanceToNearestSupermarket(alpha) < distanceToNearestSupermarket(beta);
}*/

/*int Graph::toIndex(const Coordinates& pos) const {
    return (pos.avenue - 1) * numStreets + (pos.street - 1);
}

Coordinates Graph::toPos(const int& index) const {
    return Coordinates((index / numStreets) + 1, (index % numStreets) + 1);
}*/

void Graph::addSupermarket(const int& a, const int& s) {
    targets.insert(2*((a-1) * numStreets + (s - 1)));
}

void Graph::addHome(const int& a, const int& s) {
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

/*BFSNode* Graph::getFreeHome() {
    for (unordered_set<Coordinates>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        LinkedList* sourceLL = &source;
        Coordinates coord = (*iter);
        bool found = false;
        while(!found && sourceLL != nullptr) {
            int index = (coord.avenue - 1) * numAvenues + (coord.street - 1);
            if(sourceLL->data == index) {
                found = true;
                break;
            }
        }
        if(found) {
            continue;
        } else {
            return BFSNode(coord, nullptr, 0, 0, 0);
        }
    }
}*/

// getMatrixPos returns info of Vin
/*LinkedList* Graph::getMatrixPos(const Coordinates& coord) const {
    if (coord.avenue < 1 || coord.street < 1 || coord.avenue > numAvenues || coord.street > numStreets) {
        throw; // Nothing here, go back!
    }
    return nodes[2 * toIndex(coord)];
}

// setMatrixPos sets info for Vout
void Graph::setMatrixPos(const Coordinates& coord, LinkedList* edges) {
    if (coord.avenue < 1 || coord.street < 1 || coord.avenue > numAvenues || coord.street > numStreets) {
        throw; // Nothing here!
    }
    LinkedList* old = nodes[2*toIndex(coord) + 1];
    edges->next = old;
    nodes[2*toIndex(coord) + 1] = edges;
}*/

// ALGORITHM SPECIFICS
/*const Coordinates DIRECTIONS[4] = {
    Coordinates(1, 0),
    Coordinates(0, 1),
    Coordinates(-1, 0),
    Coordinates(0, -1)
};*/

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
    BFSNode* position = &queue->data;
    //cout << "Visiting " << position->vertex << "\n";

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
            graph->homes.erase(node);
            while(queue->next != nullptr) queue = queue->next;
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
    } else if (graph->targets.count(position->vertex-1)) {
        // We need to connect to super target
        int node = graph->getTargetNodePos();
        while(queue->next != nullptr) queue = queue->next;
        BFSQueue* queueNext = new BFSQueue(BFSNode{node, position});
        queue->next = queueNext;
        visited[position->vertex] = true;
        return false;
    }

    // There are no special cases, so visit all it's neighbors
    if(position->vertex % 2 == 0) {
        // Vertex is odd, therefore a Vin. Enter if it still hasn't a connection to Vout
        int node = position->vertex + 1;
        if(graph->nodes[position->vertex] == nullptr) {
            while(queue->next != nullptr) queue = queue->next;
            BFSQueue* queueNext = new BFSQueue(BFSNode{node, position});
            queue->next = queueNext;
        }
        // If this Vin has a connection, it means it can flow back.
        vector<int> neighbors = findNeighbors(position->vertex, graph);
        for(vector<int>::iterator iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
            if(graph->nodes[*iter] != nullptr && graph->nodes[*iter]->data == position->vertex) {
                while(queue->next != nullptr) queue = queue->next;
                BFSQueue* queueNext = new BFSQueue(BFSNode{*iter, position});
                queue->next = queueNext;
            }
        }
    } else {
        // Vertex is even, therefore a Vout
        vector<int> directions = findNeighbors(position->vertex, graph);

        // If Vout has flow, it means Vin is sending it
        if(graph->nodes[position->vertex-1] != nullptr)
            directions.push_back(position->vertex-1);

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
            if(visit(queue, this, visited)) {
                hasPath = true;
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

        //printQueue(head);
        //delete head;
    }

    int connections = 0;
    /*for (unordered_set<int>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        if(nodes[*iter] != nullptr) connections++;
    }*/
    LinkedList* sourceLL = getSourceNode();
    while(sourceLL != nullptr) {
        connections++;
        sourceLL = sourceLL->next;
    }

    return connections;
    //return match_count;

    /*vector<int> visitedVertices(numAvenues * numStreets);
    vector<BFSNode> heap = {};

    fill(visitedVertices.begin(), visitedVertices.end(), 0);

    while (!BFSQueue.empty()) {
        cout << "Queue Size: " << BFSQueue.size() << " -> ";
        printQueue(BFSQueue);
        bool match = visit(BFSQueue.front(), this, BFSQueue, visitedVertices, heap);
        if (match) {
            ++match_count;
        }
    }*/
}
