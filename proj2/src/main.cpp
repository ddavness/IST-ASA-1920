/*
    ASA 19/20, 2nd project

    David Ferreira de Sousa Duque, nº 93698
    Ricardo Jorge Santos Subtil. nº 93752
*/

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <limits>

using namespace std;

enum class Status: int8_t {
    Busy = 0,
    Free = 1,
    Temp = 2,
};

class Coordinates {
    public:
        Coordinates(int, int);
        Coordinates operator+(const Coordinates&) const;
        Coordinates operator-() const;
        bool operator<(Coordinates) const;
        bool operator==(Coordinates) const;
        int distance(const Coordinates&) const;
        int avenue;
        int street;
};

class Graph {
    public:
        Graph(int, int);
        int distanceToNearestSupermarket(Coordinates&) const;
        bool operator() (Coordinates&, Coordinates&) const;
        bool canReachSupermarket(const Coordinates&) const;
        void addSupermarket(const Coordinates&);
        void addHome(const Coordinates&);
        Status getMatrixPos(const Coordinates&) const;
        void setMatrixPos(const Coordinates&, Status value);
        set<Coordinates> targets;
        set<Coordinates> homes;

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
    //vector<Coordinates> homes;

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

    int reachable = 0;
    //sort(city.homes.begin(), city.homes.end(), city);
    for (set<Coordinates>::iterator iter = city.homes.begin(); iter != city.homes.end(); ++iter) {
        reachable += city.canReachSupermarket(*iter) ? 1 : 0;
    }

    cout << reachable << endl;

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

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = set<Coordinates>();
    homes = set<Coordinates>();
    matrix = new Status[avenues * streets];
    // Initialize the matrix
    fill(matrix, &(matrix[avenues * streets]), Status::Free);
}

int Graph::distanceToNearestSupermarket(Coordinates& start) const {
    double distance = numeric_limits<double>::infinity();

    for (set<Coordinates>::iterator iter = targets.begin(); iter != targets.end(); ++iter) {
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

typedef struct bfs BFSNode;

struct bfs {
    Coordinates me;
    BFSNode* parent;
};

const Coordinates DIRECTIONS[4] = {
    Coordinates(1, 0),
    Coordinates(0, 1),
    Coordinates(-1, 0),
    Coordinates(0, -1)
};

bool visit(BFSNode& position, const Graph* graph, queue<BFSNode>& queue, vector<int>& visitedVertices) {
    cout << "Visiting " << position.me.avenue << ',' << position.me.street << "\n";
    Graph city = *graph;
    queue.pop(); // Remove this element
    visitedVertices[position.me.avenue * graph->numAvenues + position.me.street] = 1; // Mark vertex as visited
    if (city.getMatrixPos(position.me) != Status::Free) {
        return false;
    } else if (city.homes.count(position.me) && position.parent != nullptr) {
        return false;
    } else if (city.targets.count(position.me)) {
        // We found it! Lock the paths!
        BFSNode* trace = &position;
        while (trace) {
            city.setMatrixPos(trace -> me, Status::Busy);
            trace = trace -> parent;
        }
        return true;
    }

    //city.setMatrixPos(position.me, Status::Temp);
    for (int i = 0; i < 4; ++i) {
        Coordinates neighbor = position.me + DIRECTIONS[i];
        if(neighbor.avenue != 0 && neighbor.street != 0
        && neighbor.avenue <= graph->numAvenues && neighbor.street <= graph->numStreets     // Check if coordinates are in-bounds
        && visitedVertices[neighbor.avenue * graph->numAvenues + neighbor.street] == 0) { // Check if vertex is unvisited
            //cout << "Neighbor is (" << neighbor.avenue << ", " << neighbor.street << "). Visited is " << visitedVertices[neighbor.avenue * graph->numAvenues + neighbor.street] << endl;
            queue.push({ neighbor, &position });
        }
    }
    return false;
}

void printQueue(queue<BFSNode> q) {
    while(!q.empty()) {
        cout << "(" << q.front().me.avenue << ", " << q.front().me.street << ") ";
        q.pop();
    }
    cout << endl;
}

bool Graph::canReachSupermarket(const Coordinates& start) const {
    queue<BFSNode> BFSQueue;
    BFSNode initial = { move(start), nullptr };
    BFSQueue.push(initial);

    vector<int> visitedVertices(numAvenues * numStreets);
    fill(visitedVertices.begin(), visitedVertices.end(), 0);

    cout << "Performing BFS for home (" << start.avenue << ", " << start.street << ")" << endl;
    while (!BFSQueue.empty()) {
        cout << "Queue Size: " << BFSQueue.size() << " -> ";
        printQueue(BFSQueue);
        bool match = visit(BFSQueue.front(), this, BFSQueue, visitedVertices);
        if (match) {
            cout << "Home (" << start.avenue << "," << start.street << ") got matched with a supermarket!" << endl;
            return true;
        }
    }

    cout << "Home (" << start.avenue << "," << start.street << ") didn't get matched!" << endl;
    return false;
}