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
#include <limits>

using namespace std;

class Coordinates {
    public:
        Coordinates(int, int);
        Coordinates operator+(const Coordinates&);
        Coordinates operator-() const;
        bool operator<(Coordinates) const;
        int distance(const Coordinates&) const;
        int avenue;
        int street;
};

class Graph {
    public:
        Graph(int, int);
        int distanceToNearestSupermarket(Coordinates&);
        bool operator() (Coordinates&, Coordinates&);
        bool canReachSupermarket(const Coordinates&);
        void addSupermarket(const Coordinates&);

    private:
        int numAvenues;
        int numStreets;

        bool getMatrixPos(const Coordinates&);
        bool* matrix;
        set<Coordinates> targets;
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
    vector<Coordinates> homes;

    int a, s;
    for (int i = 0; i < supermarkets; ++i) {
        cin >> a;
        cin >> s;
        city.addSupermarket(Coordinates(a, s));
    }

    for (int i = 0; i < citizens; ++i) {
        cin >> a;
        cin >> s;
        homes.push_back(Coordinates(a, s));
    }

    int reachable = 0;
    sort(homes.begin(), homes.end(), city);
    for (vector<Coordinates>::iterator iter = homes.begin(); iter != homes.end(); ++iter) {
        reachable += city.canReachSupermarket(*iter) ? 1 : 0;
    }

    cout << reachable << endl;

    exit(EXIT_SUCCESS);
}

// Coordinates

Coordinates::Coordinates(int a, int s): avenue(a), street(s) {}

Coordinates Coordinates::operator+(const Coordinates& other) {
    return Coordinates(avenue + other.avenue, street + other.street);
}
Coordinates Coordinates::operator-() const {
    return Coordinates(-avenue, -street);
}
bool Coordinates::operator<(Coordinates other) const {
    return avenue < other.avenue || (avenue == other.avenue && street < other.street);
}
int Coordinates::distance(const Coordinates& other) const {
    return abs(avenue - other.avenue) + abs(street - other.street);
}

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = set<Coordinates>();
    matrix = new bool[avenues * streets];
}

int Graph::distanceToNearestSupermarket(Coordinates& start) {
    double distance = numeric_limits<double>::infinity();

    for (set<Coordinates>::iterator iter = targets.begin(); iter != targets.end(); ++iter) {
        int newDist = start.distance(*iter);
        distance = min(distance, static_cast<double>(newDist));
    }

    return static_cast<int>(distance);
}

bool Graph::operator() (Coordinates& alpha, Coordinates& beta) {
    return distanceToNearestSupermarket(alpha) < distanceToNearestSupermarket(beta);
}

bool Graph::canReachSupermarket(const Coordinates& start) {
    return true;
}

void Graph::addSupermarket(const Coordinates& supermarket) {
    targets.insert(supermarket);
}

// Private methods

bool Graph::getMatrixPos(const Coordinates& coord) {
    if (coord.avenue > numAvenues || coord.street > numStreets) {
        throw "Invalid position!";
    }
    return matrix[(coord.avenue - 1) * numStreets + (coord.street - 1)];
}
