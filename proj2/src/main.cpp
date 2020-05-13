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

enum class Status: int8_t {
    Busy = 0,
    Free = 1,
    Temp = 2,
};

class Coordinates {
    public:
        Coordinates() = default;
        Coordinates(int, int);
        Coordinates operator+(const Coordinates&) const;
        Coordinates operator-() const;
        Coordinates operator() () const;
        Coordinates toCanonicalForm() const;
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
}

class Graph {
    public:
        Graph(int, int);
        int distanceToNearestSupermarket(Coordinates&) const;
        bool operator() (Coordinates&, Coordinates&) const;
        void addSupermarket(const Coordinates&);
        void addHome(const Coordinates&);
        Status getMatrixPos(const Coordinates&) const;
        void setMatrixPos(const Coordinates&, Status value);

        int getMaxSafeFlow();

        unordered_set<Coordinates> targets;
        unordered_set<Coordinates> homes;

        int numAvenues;
        int numStreets;

    private:
        Status* matrix;
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
        city.addSupermarket(Coordinates(a, s));
    }

    for (int i = 0; i < citizens; ++i) {
        cin >> a;
        cin >> s;
        city.addHome(Coordinates(a, s));
    }

    cout << city.getMaxSafeFlow() << endl;

    exit(EXIT_SUCCESS);
}

// Coordinates

Coordinates::Coordinates(int a, int s): avenue(a), street(s) {}

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
    return avenue == other.avenue && street < other.street;
}

int Coordinates::distance(const Coordinates& other) const {
    return abs(avenue - other.avenue) + abs(street - other.street);
}

Coordinates Coordinates::toCanonicalForm() const {
    return Coordinates(avenue - 1, street - 1);
}

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = unordered_set<Coordinates>();
    homes = unordered_set<Coordinates>();
    matrix = new Status[avenues * streets];
    // Initialize the matrix
    fill(matrix, &(matrix[avenues * streets]), Status::Free);
}

int Graph::distanceToNearestSupermarket(Coordinates& start) const {
    double distance = numeric_limits<double>::infinity();

    for (unordered_set<Coordinates>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        int newDist = start.distance(*iter);
        distance = min(distance, static_cast<double>(newDist));
    }

    return static_cast<int>(distance);
}

bool Graph::operator() (Coordinates& alpha, Coordinates& beta) const {
    return distanceToNearestSupermarket(alpha) < distanceToNearestSupermarket(beta);
}

void Graph::addSupermarket(const Coordinates& supermarket) {
    targets.insert(supermarket);
}

void Graph::addHome(const Coordinates& home) {
    homes.insert(home);
}

Status Graph::getMatrixPos(const Coordinates& coord) const {
    if (coord.avenue < 1 || coord.street < 1 || coord.avenue > numAvenues || coord.street > numStreets) {
        return Status::Temp; // Nothing here, go back!
    }
    return matrix[(coord.avenue - 1) * numStreets + (coord.street - 1)];
}

void Graph::setMatrixPos(const Coordinates& coord, Status val) {
    if (coord.avenue < 1 || coord.street < 1 || coord.avenue > numAvenues || coord.street > numStreets) {
        throw; // Nothing here!
    }
    matrix[(coord.avenue - 1) * numStreets + (coord.street - 1)] = val;
}

// ALGORITHM SPECIFICS

class BFSNode {
    public:
        BFSNode() = delete;
        BFSNode(Coordinates coords, int node_house, BFSNode* parent_node);
        Coordinates position;
        BFSNode* parent;
        int house;
        int16_t distance;
        int8_t children;
};

BFSNode::BFSNode(Coordinates coords, int node_house, BFSNode* parent_node) {
    position = coords;
    parent = parent_node;
    house = node_house;
    children = 0;
    if (parent_node) {
        distance = parent_node -> distance + 1;
    } else {
        distance = 0;
    }
}

const Coordinates DIRECTIONS[4] = {
    Coordinates(1, 0),
    Coordinates(0, 1),
    Coordinates(-1, 0),
    Coordinates(0, -1)
};

bool visit(BFSNode* node, const Graph* graph, queue<BFSNode*>& queue, vector<int>& visitedVertices, vector<BFSNode*>& heap) {
    cout << "Visiting " << node -> position.avenue << ',' << node -> position.street << "\n";
    Graph city = *graph;

    // Remove from the queue, but keep in the heap so that we can clean them in the end
    heap.push_back(node);
    queue.pop();

    Coordinates c = node -> position.toCanonicalForm();
    visitedVertices[c.avenue * graph->numAvenues + c.street] = node -> house; // Mark vertex as visited

    if (city.getMatrixPos(node -> position) != Status::Free) {
        return false;
    } else if (city.homes.count(node -> position) && node -> parent == nullptr) {
        return false;
    } else if (city.targets.count(node -> position)) {
        // We found it! Lock the paths!
        BFSNode* trace = node;
        while (trace) {
            city.setMatrixPos(trace -> position, Status::Busy);
            trace = trace -> parent;
        }
        return true;
    }

    int8_t children = 0;

    for (int i = 0; i < 4; ++i) {
        Coordinates neighbor = node -> position + DIRECTIONS[i];
        if(neighbor.avenue != 0 && neighbor.street != 0
        && neighbor.avenue <= graph->numAvenues && neighbor.street <= graph->numStreets     // Check if coordinates are in-bounds
        && visitedVertices[neighbor.avenue * graph->numAvenues + neighbor.street] == 0) {   // Check if vertex is unvisited
            ++children;
            queue.push(new BFSNode(neighbor, node -> house, node));
        }
    }

    node -> children = children;
    return false;
}

void printQueue(queue<BFSNode*> q) {
    while(!q.empty()) {
        cout << "(" << q.front() -> position.avenue << ", " << q.front() -> position.street << ") ";
        q.pop();
    }
    cout << endl;
}

int Graph::getMaxSafeFlow() {
    queue<BFSNode*> BFSQueue;

    // Iterate through all homes (more or less arbitrarily)
    int c = 1;
    for (unordered_set<Coordinates>::const_iterator it = homes.begin(); it != homes.end(); ++it) {
        BFSNode* initial = new BFSNode(*it, c, nullptr);
        BFSQueue.push(initial);
        ++c;
    }

    vector<int> visitedVertices(numAvenues * numStreets);
    vector<BFSNode*> heap = {};

    fill(visitedVertices.begin(), visitedVertices.end(), 0);

    int match_count = 0;

    while (!BFSQueue.empty()) {
        cout << "Queue Size: " << BFSQueue.size() << " -> ";
        printQueue(BFSQueue);
        bool match = visit(BFSQueue.front(), this, BFSQueue, visitedVertices, heap);
        if (match) {
            ++match_count;
        }
    }

    return match_count;
}
