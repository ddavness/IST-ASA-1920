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

enum class Status: int {
    Locked = -1,
    Free = 0
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

class Supermarket {
    public:
        Coordinates pos;
        int num;

        bool operator<(Coordinates) const;
        bool operator<(Supermarket) const;
        bool operator==(Supermarket) const;
};

class Home {
    public:
        Coordinates pos;
        int num;
        Supermarket* supermarket;

        bool operator<(Coordinates) const;
        bool operator<(Home) const;
        bool operator==(Home) const;
};

class Graph {
    public:
        Graph(int, int);
        int distanceToNearestSupermarket(Coordinates&) const;
        bool operator() (Coordinates&, Coordinates&) const;
        bool canReachSupermarket(const Home&) const;
        void addSupermarket(const Supermarket&);
        void addHome(const Home&);
        Status getMatrixPos(const Coordinates&) const;
        void setMatrixPos(const Coordinates&, Status value);
        set<Supermarket> supermarkets;
        set<Home> homes;

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
        Supermarket sm = {Coordinates(a, s), i + 1};
        city.addSupermarket(sm);
    }

    for (int i = 0; i < citizens; ++i) {
        cin >> a;
        cin >> s;
        Home hm = {Coordinates(a, s), i + 1, nullptr};
        city.addHome(hm);
    }

    int reachable = 0;
    //sort(city.homes.begin(), city.homes.end(), city);
    // Compute quick solution
    for (set<Home>::iterator iter = city.homes.begin(); iter != city.homes.end(); ++iter) {
        reachable += city.canReachSupermarket(*iter) ? 1 : 0;
    }

    // For each unconnected home, try maximizing the solution
    queue<Home> homesDisconnected;
    for(set<Home>::iterator iter = city.homes.begin(); iter != city.homes.end(); ++iter) {
        if((*iter).supermarket == nullptr) {
            homesDisconnected.push(*iter);
        }
    }

    cout << "Size of queue of disconnected houses: " << homesDisconnected.size() << endl;

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

// Homes and Supermarkets
bool Home::operator==(Home other) const {
    return pos == other.pos;
}

bool Supermarket::operator==(Supermarket other) const {
    return pos == other.pos;
}

bool Home::operator<(Coordinates other) const {
    return pos < other;
}

bool Supermarket::operator<(Coordinates other) const {
    return pos < other;
}

bool Home::operator<(Home other) const {
    return pos < other.pos;
}

bool Supermarket::operator<(Supermarket other) const {
    return pos < other.pos;
}

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    supermarkets = set<Supermarket>();
    homes = set<Home>();
    matrix = new Status[avenues * streets];
    // Initialize the matrix
    fill(matrix, &(matrix[avenues * streets]), Status::Free);
}

int Graph::distanceToNearestSupermarket(Coordinates& start) const {
    double distance = numeric_limits<double>::infinity();

    for (set<Supermarket>::iterator iter = supermarkets.begin(); iter != supermarkets.end(); ++iter) {
        int newDist = start.distance((*iter).pos);
        distance = min(distance, static_cast<double>(newDist));
    }

    return static_cast<int>(distance);
}

bool Graph::operator() (Coordinates& alpha, Coordinates& beta) const {
    return distanceToNearestSupermarket(alpha) < distanceToNearestSupermarket(beta);
}

void Graph::addSupermarket(const Supermarket& supermarket) {
    supermarkets.insert(supermarket);
}

void Graph::addHome(const Home& home) {
    homes.insert(home);
}

Status Graph::getMatrixPos(const Coordinates& coord) const {
    /*if (coord.avenue < 1 || coord.street < 1 || coord.avenue > numAvenues || coord.street > numStreets) {
        return Status::Temp; // Nothing here, go back!
    }*/
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

bool visit(BFSNode& position, const int num, Graph* graph, queue<BFSNode>& queue, vector<int>& visitedVertices) {
    cout << "Visiting " << position.me.avenue << ',' << position.me.street << "\n";
    Graph city = *graph;
    queue.pop(); // Remove this element
    visitedVertices[position.me.avenue * graph->numAvenues + position.me.street] = 1; // Mark vertex as visited
    if (city.getMatrixPos(position.me) != Status::Free) {
        return false;
    } else if (city.homes.count({position.me, -1, nullptr}) && position.parent != nullptr) {
        return false;
    } else if (city.supermarkets.count({position.me, -1})) {
        // We found it! Lock the paths!
        BFSNode* trace = &position;
        Supermarket s = *(city.supermarkets.find({trace->me,-1}));
        while (trace) {
            city.setMatrixPos(trace -> me, (Status)(num));
            if(trace->parent == nullptr) {
                // Trace is the root, aka the original home
                Home h = *((*graph).homes.find({trace->me, num, nullptr}));
                h.supermarket = &s;
                (*graph).homes.erase((*graph).homes.find({trace->me, num, nullptr}));
                (*graph).homes.insert(h);
            }
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

bool Graph::canReachSupermarket(const Home& start) const {
    queue<BFSNode> BFSQueue;
    BFSNode initial = { move(start.pos), nullptr };
    BFSQueue.push(initial);

    vector<int> visitedVertices(numAvenues * numStreets);
    fill(visitedVertices.begin(), visitedVertices.end(), 0);

    cout << "Performing BFS for home (" << start.pos.avenue << ", " << start.pos.street << ")" << endl;
    while (!BFSQueue.empty()) {
        cout << "Queue Size: " << BFSQueue.size() << " -> ";
        printQueue(BFSQueue);
        bool match = visit(BFSQueue.front(), start.num, const_cast<Graph*>(this), BFSQueue, visitedVertices);
        if (match) {
            cout << "Home (" << start.pos.avenue << "," << start.pos.street << ") got matched with a supermarket!" << endl;
            return true;
        }
    }

    cout << "Home (" << start.pos.avenue << "," << start.pos.street << ") didn't get matched!" << endl;
    return false;
}