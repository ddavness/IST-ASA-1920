/*
    ASA 19/20, 2nd project

    David Ferreira de Sousa Duque, nº 93698
    Ricardo Jorge Santos Subtil. nº 93752
*/

#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

class Coordinates {
    public:
        Coordinates(int, int);
        Coordinates operator+(const Coordinates&);
        Coordinates operator-();
        int avenue;
        int street;
};

class Graph {
    public:
        Graph(int, int);
        int distanceToNearestSupermarket(const Coordinates&);
        bool canReachSupermarket(const Coordinates&);
        void addSupermarket(const Coordinates&);

    private:
        int numAvenues;
        int numStreets;

        bool getMatrixPos(const Coordinates&);
        bool* matrix;
        vector<Coordinates&> targets;
};

bool compareDistances(const Coordinates&, const Coordinates&);

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
    vector<const Coordinates&> homes;

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

    sort(homes.begin(), homes.end(), compareDistances);

    exit(EXIT_SUCCESS);
}

// Coordinates

Coordinates::Coordinates(int a, int s): avenue(a), street(s) {}

Coordinates Coordinates::operator+(const Coordinates& other) {
    return Coordinates(avenue + other.avenue, street + other.street);
}
Coordinates Coordinates::operator-() {
    return Coordinates(-avenue, -street);
}

// City graph

Graph::Graph(int avenues, int streets): numAvenues(avenues), numStreets(streets) {
    targets = vector<Coordinates&>();
    matrix = new bool[avenues * streets];
}

bool Graph::getMatrixPos(const Coordinates& coord) {
    if (coord.avenue > numAvenues || coord.street > numStreets) {
        throw "Invalid position!";
    }
    return matrix[(coord.avenue - 1) * numStreets + (coord.street - 1)];
}
