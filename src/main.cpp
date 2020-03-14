#include <cmath>
#include <iostream>

using namespace std;

/// Class Definitions
class LinkedList
{
public:
    LinkedList();
    LinkedList(int );
    ~LinkedList();

    bool hasNext();

    static void printLinkedList(LinkedList* );

    int data;
    LinkedList* next = nullptr;
};

class Graph
{
public:
    Graph(int , int );
    ~Graph();

    void setGrade(int , int );
    void addConnection(int , int );

    int getGrade(int );
    LinkedList* getConnections(int );

    int getNumNodes();

    static void printGraph(Graph& );

private:
    int numNodes, numConnections;
    int* nodes;
    LinkedList** connections;
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
    cout << data <<endl;
    return next != nullptr;
}

Graph::Graph(int nodes, int connections)
    : numNodes(nodes), numConnections(connections)
{
    this->nodes = new int[nodes];
    this->connections = new LinkedList*[nodes];

    for(int i = 0; i < nodes; i++)
    {
        this->nodes[i] = 0;
        this->connections[i] = nullptr;
    }
}

Graph::~Graph()
{
    delete[] nodes;

    // We need to delete each LinkedList*
    for(int i = 0; i < numNodes; i++)
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
    LinkedList* ll = new LinkedList(to);
    ll->next = connections[node];

    // Add it as root
    connections[node] = ll;
}

int Graph::getGrade(int node)
{
    return nodes[node];
}

LinkedList* Graph::getConnections(int node)
{
    return connections[node];
}

int Graph::getNumNodes()
{
    return numNodes;
}

void performSearchOver(Graph&, void (Graph&, int, int));

void maxGrade(Graph&, int, int);

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
        cin >> studentFrom;
        cin >> studentTo;
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

void visit(Graph& g, int node, bool* explored, void (*visitor)(Graph&, int, int))
{
    LinkedList* connection = g.getConnections(node);
    while (connection)
    {
        int child = connection->data;
        visitor(g, node, child);
        visit(g, child, explored, visitor);
        connection = connection->next;
    }

    explored[node] = true;
}

void performSearchOver(Graph& g, void (*visitor)(Graph&, int, int))
{
    int nodes = g.getNumNodes();

    // Initialize tracking array
    bool* explored = new bool[nodes];
    for (int i = 0; i < nodes; i++)
    {
        explored[i] = false;
    }

    for (int i = 0; i < nodes; i++)
    {
        if (explored[i])
        {
            continue;
        }

        visit(g, i, explored, visitor);
    }

    delete[] explored;
}

void maxGrade(Graph& g, int studentFrom, int studentTo)
{
    int grade = max(g.getGrade(studentFrom), g.getGrade(studentTo));
    g.setGrade(studentTo, grade);
}
