#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <unordered_set>
#include <utility>

using namespace std;

#define TRY_FACTOR 10

typedef struct {
  int u;
  int v;
} Edge;

int _V;
int _E = 0;
int _maxE;
int _nNetworks;
int _minV = 1;
int _maxV = 10;

list<int>* _g;
unordered_set<int>* _edges;

int* _netVert; // #V in each Network
int* _initNetID; // #V in each Network
int* _mapVertNet; // Map each vertex into SCC ID
int* _vertexMap;  // Map vertex into new ID


int randomValue(int max) {
  // Returns a random value between 0 and max-1
  return rand() % max;
}

void addEdge(int u, int v) {
  _g[u].push_back(v);
  _edges[u].insert(v);
  _g[v].push_back(u);
  _edges[v].insert(u);
  _E++;
}


bool existsEdge(int u, int v) {
  return (_edges[u].find(v) != _edges[u].end());
}


void printUsage() {
  printf("\nUtilização: gerador #P #R #SubR #m #M seed\n\n");
  printf("\t#P: número de alunos na rede social\n");
  printf("\t#R: número de ligações entre alunos na rede\n");
  printf("\t#SubR: número de sub-redes\n");
  printf("\t#m: número mínimo de alunos em cada sub-rede (opcional, default: 1)\n");
  printf("\t#M: número máximo de alunos em cada sub-rede (opcional, default: 10)\n");
  printf("\tseed: seed para o gerador aleatório (opcional, default: 0)\n");
  printf("\n\n");
}

void parseCommandLine(int argc, char **argv) {
  int seed = 0;
  
  if (argc < 4) {
    printUsage();
    exit(0);
  }
  sscanf(argv[1], "%d", &_V);
  sscanf(argv[2], "%d", &_maxE);
  sscanf(argv[3], "%d", &_nNetworks);
  if (argc >= 5) sscanf(argv[4], "%d", &_minV);
  if (argc >= 6) sscanf(argv[5], "%d", &_maxV);
  if (argc >= 7) sscanf(argv[6], "%d", &seed);
  
  //Initialize rand seed
  srand(seed);
  
  if (_nNetworks > _V) {
    printf("\nERRO: #SubR não pode ser maior que #P\n\n");
    printUsage();
    exit(0);
  }
  
  if (_maxV < _minV) {
    printf("\nERRO: #m não pode ser maior que #M\n\n");
    exit(0);    
  }
  
  if (_nNetworks*_minV > _V) {
    printf("\nERRO: #SubR * #m não pode ser maior do que #P\n");
    printUsage();
    exit(0);
  }

  if (_nNetworks*_maxV < _V) {
    printf("\nERRO: #SubR * #M não pode ser menor do que #P\n");
    printUsage();
    exit(0);
  }
}


void initDataStructures() {
  _g = new list<int>[_V];
  _edges = new unordered_set<int>[_V];
  _netVert = new int[_nNetworks];
  _initNetID = new int[_nNetworks];
  _mapVertNet = new int[_V];
  _vertexMap = new int[_V];
  
  for (int i = 0; i < _V; i++)
    _vertexMap[i] = i;
}



void vertexDistribution() {
  int n = 0;
  for (int i = 0; i < _nNetworks; i++) {
    _netVert[i] = _minV;
    n += _minV;
  }
  
  while (n < _V) {
    int scc = randomValue(_nNetworks);
    while (_netVert[scc] >= _maxV)
      scc = (scc + 1) % _nNetworks;
    _netVert[scc]++;
    n++;
  }

  n = 0;
  for (int i = 0; i < _nNetworks; i++) {
    _initNetID[i] = n;
    //printf("%d: %d,%d \n", i, _netVert[i], _initNetID[i]);
    for (int j = 0; j < _netVert[i]; j++) {
      _mapVertNet[n] = i;
      n++;
    }
  }
  
  //for (int i = 0; i < _V; i++)
  //  printf("%d:%d ", i, _mapVertNet[i]);
  //printf("\n");
  
}

void addInternalSCCEdges() {
  int m = 0, M = 0;
  for (int i = 0; i < _nNetworks; i++) {
    m = M;
    M += _netVert[i];
    
    // One vertex in Network...
    if (_netVert[i] == 1) continue;
    
    int r = randomValue(3);
    if (r == 0) {
      // Add cycle
      //printf("r:0\n");
      for (int j = m; j < M-1; j++)
	addEdge(j, j+1);
      if (_netVert[i] > 2) addEdge(M-1, m);
    }
    else if (r == 1) {
      //printf("r:1\n");
      // Add line
      for (int j = m; j < M-1; j++) {
	addEdge(j, j+1);
	//addEdge(j+1, j);
      }
    }
    else if (r == 2) {
      // printf("r:2 i:%d\n", i);
      // Add tree
      int children = randomValue(3)+1;
      int parent = m;
      for (int j = m+1; j < M; j++) {
	addEdge(parent, j);
	children--;
	if (children == 0) {
	  children = randomValue(3)+1;
	  parent++;
	}
      }
    }
  }
}


void addRemainingEdges() {
  int r = _maxE - _E;
  int tries = TRY_FACTOR*r;
  
  while (r > 0) {
    // Select Network
    int net = randomValue(_nNetworks);
    
    if (_netVert[net] == 1) continue;
    
    int u = randomValue(_netVert[net]) + _initNetID[net];
    int v = randomValue(_netVert[net]) + _initNetID[net];
    
    // Do not allow self-loops
    if (u == v) continue;
    
    //Need to check if edge already exists...
    if (!existsEdge(u, v)) {
      addEdge(u, v);
      r--;
    }
    else {
      tries--;
      if (tries == 0) break;	
    }
  }
}


void shuffleVertexIDs() {
  for (int i = 0; i < _V; i++) {
    int u = randomValue(_V);
    int v = randomValue(_V);
    int aux = _vertexMap[u];
    _vertexMap[u] = _vertexMap[v];
    _vertexMap[v] = aux;
  }
}


void printGraph() {
	printf("%d,%d\n", _V, _E);
	// print grades
	for (int i = 0; i < _V; i++) {
		printf("%d\n", randomValue(20));
	}
	// print network
	for (int i = 0; i < _V; i++) {
		for (list<int>::iterator iter = _g[i].begin(); iter != _g[i].end(); iter++) {
			if (_vertexMap[i] < _vertexMap[(*iter)])
			printf("%d %d\n", _vertexMap[i]+1, _vertexMap[(*iter)]+1);
		}
	}
}


int main(int argc, char *argv[]) {
  parseCommandLine(argc, argv);
  
  initDataStructures();
  
  vertexDistribution();
  
  addInternalSCCEdges();
  
  addRemainingEdges();
  
  shuffleVertexIDs();
  
  printGraph();
  
  return 0;
}
