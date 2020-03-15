#include <cstring>
#include <iostream>

using namespace std;

/// Class Definitions
class LinkedList
{
public:
    LinkedList();
    LinkedList(int);
    ~LinkedList();

    bool hasNext();

    static void printLinkedList(LinkedList *);

    int data;
    LinkedList *next = nullptr;
};

class Graph
{
public:
    Graph(int, int);
    ~Graph();

    void setGrade(int, int);
    void addConnection(int, int);

    int getGrade(int);
    LinkedList *getConnections(int);

    int getNumNodes();

    static void printGraph(Graph &);

private:
    int numNodes, numConnections;
    int *nodes;
    LinkedList **connections;
};

/// Class Implementations
LinkedList::LinkedList() {}

LinkedList::LinkedList(int data)
    : data(data) {}

LinkedList::~LinkedList()
{
    delete next;
}

bool LinkedList::hasNext()
{
    cout << data << endl;
    return next != nullptr;
}

Graph::Graph(int nodes, int connections)
    : numNodes(nodes), numConnections(connections)
{
    this->nodes = new int[nodes];
    this->connections = new LinkedList *[nodes];

    memset(this->connections, 0, sizeof(LinkedList *) * numNodes);
    memset(this->nodes, 0, sizeof(int) * numNodes);
}

Graph::~Graph()
{
    delete[] nodes;

    // We need to delete each LinkedList*
    for (int i = 0; i < numNodes; i++)
    {
        delete connections[i];
    }
    delete[] connections;
}

void Graph::setGrade(int node, int grade)
{
    nodes[node] = grade;
}

void Graph::addConnection(int node, int to)
{
    // Make the new LL at the start
    LinkedList *ll = new LinkedList(to);
    ll->next = connections[node];

    // Add it as root
    connections[node] = ll;
}

int Graph::getGrade(int node)
{
    return nodes[node];
}

LinkedList *Graph::getConnections(int node)
{
    return connections[node];
}

int Graph::getNumNodes()
{
    return numNodes;
}

void performSearchOver(Graph &, bool(Graph &, int, int));

bool maxGrade(Graph &, int, int);

// Main method
int main()
{
    // Read contents from stdin
    // How many students? What kind of relationships?
    int numStudents, numChains;
    cin >> numStudents;
    cin.ignore(1); // Ignore the comma (,)
    cin >> numChains;

    int grade;
    int studentFrom, studentTo;

    Graph students(numStudents, numChains);

    for (int i = 0; i < numStudents; i++)
    {
        cin >> grade;
        students.setGrade(i, grade);
    }
    for (int i = 0; i < numChains; i++)
    {
        cin >> studentTo;
        cin >> studentFrom;
        students.addConnection(studentFrom - 1, studentTo - 1);
    }

    performSearchOver(students, maxGrade);

    // Dump graph contents here
    for (int i = 0; i < numStudents; i++)
    {
        cout << students.getGrade(i) << endl;
    }

    return 0;
}

void visit(Graph &g, int node, bool *explored, bool (*visitor)(Graph &, int, int))
{
    if (explored[node])
    {
        return;
    }
    explored[node] = true;

    LinkedList *connection = g.getConnections(node);
    while (connection)
    {
        int child = connection->data;
        if (visitor(g, node, child))
        {
            // We need to take a second look here
            explored[child] = false;
        }
        visit(g, child, explored, visitor);
        connection = connection->next;
    }
}

void performSearchOver(Graph &g, bool (*visitor)(Graph &, int, int))
{
    int nodes = g.getNumNodes();

    // Initialize tracking array
    bool *explored = new bool[nodes];
    memset(explored, false, sizeof(bool) * nodes);

    for (int i = 0; i < nodes; i++)
    {
        if (explored[i])
        {
            continue;
        }

        visit(g, i, explored, visitor);
    }

    memset(explored, false, sizeof(bool) * nodes);

    for (int i = nodes - 1; i >= 0; i--)
    {
        if (explored[i])
        {
            continue;
        }

        visit(g, i, explored, visitor);
    }

    delete[] explored;
}

bool maxGrade(Graph &g, int studentFrom, int studentTo)
{
    int newGrade = g.getGrade(studentFrom);
    if (newGrade > g.getGrade(studentTo))
    {
        g.setGrade(studentTo, newGrade);
        return true;
    }

    return false;
}
