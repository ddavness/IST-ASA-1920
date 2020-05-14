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
    Invalid = -1
};

class Coordinates {
    public:
        static Coordinates delta(int, int);
        Coordinates() = default;
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
                return hash<string>()(to_string(obj.avenue) + "," + to_string(obj.street));
            }
        };
}

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

        int numAvenues;
        int numStreets;

    private:
        bool visit(BFSNode*, queue<BFSNode*>&, vector<BFSNode*>&);
        vector<Status> matrix;

        unordered_set<Coordinates> targets;
        unordered_set<Coordinates> homes;
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
        Coordinates c(a, s);
        cout << c.avenue << ',' << c.street << '\n';
        city.addSupermarket(Coordinates(a, s));
    }
    cout << endl;

    for (int i = 0; i < citizens; ++i) {
        cin >> a;
        cin >> s;
        city.addHome(Coordinates(a, s));
    }

    cout << city.getMaxSafeFlow() << endl;

    return EXIT_SUCCESS;
}

// Coordinates

Coordinates Coordinates::delta(int a, int s) {
    return Coordinates(a + 1, s + 1);
}

Coordinates::Coordinates(int a, int s): avenue(a - 1), street(s - 1) {}

Coordinates Coordinates::operator+(const Coordinates& other) const {
    return Coordinates(avenue + other.avenue + 1, street + other.street + 1);
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

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = unordered_set<Coordinates>();
    homes = unordered_set<Coordinates>();
    matrix = vector<Status>(numAvenues * numStreets);
    // Initialize the matrix
    fill(matrix.begin(), matrix.end(), Status::Free);
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
    if (coord.avenue < 0 || coord.street < 0 || coord.avenue >= numAvenues || coord.street >= numStreets) {
        return Status::Invalid; // Nothing here, go back!
    }
    return matrix[coord.avenue * numStreets + coord.street];
}

void Graph::setMatrixPos(const Coordinates& coord, Status val) {
    if (coord.avenue < 0 || coord.street < 0 || coord.avenue >= numAvenues || coord.street >= numStreets) {
        throw; // Nothing here!
    }
    matrix[coord.avenue * numStreets + coord.street] = val;
}

// ALGORITHM SPECIFICS

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
    Coordinates::delta(1, 0),
    Coordinates::delta(0, 1),
    Coordinates::delta(-1, 0),
    Coordinates::delta(0, -1)
};

bool Graph::visit(BFSNode* node, queue<BFSNode*>& queue, vector<BFSNode*>& heap) {
    cout << "Visiting " << node -> position.avenue << ',' << node -> position.street << ',' << node -> house << ':' << ' ';

    // Remove from the queue, but keep in the heap so that we can clean them in the end
    heap.push_back(node);
    queue.pop();

    Coordinates coords = node -> position;

    if (getMatrixPos(coords) != Status::Free /*|| (city.homes.count(coords) && node -> parent != nullptr)*/) {
        cout << " Already in use!" << endl;
        // Traceback
        BFSNode* trace = node -> parent;
        while (trace) {
            --trace -> children;
            if (trace -> children == 0) {
                setMatrixPos(trace -> position, Status::Free);
                trace = trace -> parent;
            } else {
                break;
            }
        }
        return false;
    } else if (targets.find(node -> position) != targets.end()) {
        cout << " Found!" << endl;
        // We found it! Lock the paths!
        BFSNode* trace = node;
        while (trace) {
            setMatrixPos(trace -> position, Status::Busy);
            trace = trace -> parent;
        }
        return true;
    }

    cout << " Not there yet..." << endl;
    setMatrixPos(node -> position, Status::Temp);
    int8_t children = 0;

    for (int i = 0; i < 4; ++i) {
        Coordinates neighbor = node -> position + DIRECTIONS[i];
        if (getMatrixPos(neighbor) == Status::Free) {   // Check if vertex is unvisited
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

    vector<BFSNode*> heap = {};
    int match_count = 0;

    while (!BFSQueue.empty()) {
        cout << "Queue Size: " << BFSQueue.size() << " -> ";
        printQueue(BFSQueue);
        bool match = visit(BFSQueue.front(), BFSQueue, heap);
        if (match) {
            ++match_count;
        }
    }

    for (vector<BFSNode*>::iterator it = heap.begin(); it != heap.end(); ++it) {
        delete(*it);
    }

    heap.clear();

    return match_count;
}
