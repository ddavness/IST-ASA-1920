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
	cout << "LinkedList being deleted" << endl;
	delete next;
}

bool LinkedList::hasNext()
{
	return next != nullptr;
}

void LinkedList::printLinkedList(LinkedList* ll)
{
	if(ll == nullptr) {
		cout << endl;
		return;
	}

	while(ll->hasNext()) {
		cout << ll->data << " -> ";
		ll = ll->next;
	}

	cout << ll->data << endl;
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
	cout << "Graph being deleted" << endl;
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

void Graph::printGraph(Graph& g)
{
	for(int i = 0; i < g.numNodes; i++)
	{
		cout << "(" << g.getGrade(i) << ") " << i + 1 << ": ";
		LinkedList::printLinkedList(g.getConnections(i));
	}
}


/// Sanity tests
void linkedlist_sanity_test()
{
    cout << "[LinkedList] Sanity Test" << endl;
    LinkedList* ll = new LinkedList(1);
    ll->next = new LinkedList(2);
    ll->next->next = new LinkedList(3);

    LinkedList::printLinkedList(ll);

    delete ll;
}

void graph_sanity_test()
{
    cout << "[Graph] Sanity Test" << endl;
    /**
     * Testing the following graph
     *   0 -> 1
     *   |   /
     *   v  /
     *   2 <
     */
    Graph g(16, 32);

    // Creating grades
    g.setGrade(0, 10);
    g.setGrade(1, 14);
    g.setGrade(2, 16);

    // Creating connections
    g.addConnection(0, 1);
    g.addConnection(0, 2);
    g.addConnection(1, 2);

    // Printing graph
    Graph::printGraph(g);
}


// Main method
int main()
{
    linkedlist_sanity_test();
    graph_sanity_test();

    // Read contents from stdin
    // How many students? What kind of relationships?
    int numStudents, numChains;
    cin >> numStudents;
    cin.ignore(1); // Ignore the comma (,)
    cin >> numChains;

    int grade;
    int studentFrom, studentTo;

    Graph students(numStudents, numChains);

    for (int i = 0; i < numStudents; i++) {
        cin >> grade;
        students.setGrade(i, grade);
    }
    for (int i = 0; i < numChains; i++) {
        cin >> studentFrom;
        cin >> studentTo;
        students.addConnection(studentFrom, studentTo);
    }

    // TODO: Process graph

    // Dump graph contents here
    for (int i = 0; i < numStudents; i++) {
        cout << students.getGrade(i) << endl;
    }

    return 0;
}
