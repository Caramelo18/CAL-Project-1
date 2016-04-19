/*
 * Graph.h
 */
#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <queue>
#include <list>
#include <limits>
#include <cmath>
#include <iostream>

using namespace std;

template <class T, class U> class Edge;
template <class T, class U> class Graph;

const int NOT_VISITED = 0;
const int BEING_VISITED = 1;
const int DONE_VISITED = 2;
const int INT_INFINITY = 2147483647;

/*
 * ================================================================================================
 * Class Vertex
 * ================================================================================================
 */
template <class T, class U>
class Vertex {
	T info;
	vector<Edge<T, U>  > adj;
	bool visited;
	bool processing;
	int indegree;
	int dist;
public:

	Vertex(T in);
	friend class Graph<T, U>;

	void addEdge(Vertex<T, U> *dest, double w);
	bool removeEdgeTo(Vertex<T, U> *d);

	T getInfo() const;
	void setInfo(T info);

	int getDist() const;
	int getIndegree() const;

	Vertex* path;
};


template <class T, class U>
struct vertex_greater_than {
    bool operator()(Vertex<T, U> * a, Vertex<T, U> * b) const {
        return a->getDist() > b->getDist();
    }
};


template <class T, class U>
bool Vertex<T, U>::removeEdgeTo(Vertex<T, U> *d) {
	d->indegree--; //adicionado do exercicio 5
	typename vector<Edge<T, U> >::iterator it= adj.begin();
	typename vector<Edge<T, U> >::iterator ite= adj.end();
	while (it!=ite) {
		if (it->dest == d) {
			adj.erase(it);
			return true;
		}
		else it++;
	}
	return false;
}

//atualizado pelo exerc�cio 5
template <class T, class U>
Vertex<T, U>::Vertex(T in): info(in), visited(false), processing(false), indegree(0), dist(0) {
	path = NULL;
}


template <class T, class U>
void Vertex<T, U>::addEdge(Vertex<T, U> *dest, double w) {
	Edge<T, U> edgeD(dest,w);
	adj.push_back(edgeD);
}


template <class T, class U>
T Vertex<T, U>::getInfo() const {
	return this->info;
}

template <class T, class U>
int Vertex<T, U>::getDist() const {
	return this->dist;
}


template <class T, class U>
void Vertex<T, U>::setInfo(T info) {
	this->info = info;
}

template <class T, class U>
int Vertex<T, U>::getIndegree() const {
	return this->indegree;
}




/* ================================================================================================
 * Class Edge
 * ================================================================================================
 */
template <class T, class U>
class Edge {
	Vertex<T, U> * dest;
	double weight;
	U data;
public:
	Edge(Vertex<T, U> *d, double w);
	friend class Graph<T, U>;
	friend class Vertex<T, U>;
	bool operator< (Edge<T, U> e) const;
};

template <class T, class U>
Edge<T, U>::Edge(Vertex<T, U> *d, double w): dest(d), weight(w){}

template <class T, class U>
bool Edge<T, U>::operator< (Edge<T, U> e) const{
	return weight < e.weight;
}



/* ================================================================================================
 * Class Graph
 * ================================================================================================
 */
template <class T, class U>
class Graph {
	vector<Vertex<T, U> *> vertexSet;
	void dfs(Vertex<T, U> *v, vector<T, U> &res) const;

	//exercicio 5
	int numCycles;
	void dfsVisit(Vertex<T, U> *v);
	void dfsVisit();
	void getPathTo(Vertex<T, U> *origin, list<T, U> &res);

	vector<vector<double> > W;
	vector<vector<double> > P;


public:
	bool addVertex(const T &in);
	bool addEdge(const T &sourc, const T &dest, double w);
	bool removeVertex(const T &in);
	bool removeEdge(const T &sourc, const T &dest);
	vector<T, U> dfs() const;
	vector<T, U> bfs(Vertex<T, U> *v) const;
	int maxNewChildren(Vertex<T, U> *v, T &inf) const;
	vector<Vertex<T, U> * > getVertexSet() const;
	int getNumVertex() const;

	//exercicio 5
	Vertex<T, U>* getVertex(const T &v) const;
	void resetIndegrees();
	vector<Vertex<T, U>*> getSources() const;
	int getNumCycles();
	vector<T, U> topologicalOrder();
	vector<T, U> getPath(const T &origin, const T &dest);
	void unweightedShortestPath(const T &v);
	bool isDAG();

	void bellmanFordShortestPath(const T &s);
	void dijkstraShortestPath(const T &s);
	void floydWarshallShortestPath();

	int edgeCost(int i, int j);
	vector<T, U> getfloydWarshallPath(const T &origin, const T &dest);
};


template <class T, class U>
int Graph<T, U>::getNumVertex() const {
	return vertexSet.size();
}
template <class T, class U>
vector<Vertex<T, U> * > Graph<T, U>::getVertexSet() const {
	return vertexSet;
}

template <class T, class U>
int Graph<T, U>::getNumCycles() {
	numCycles = 0;
	dfsVisit();
	return this->numCycles;
}

template <class T, class U>
bool Graph<T, U>::isDAG() {
	return (getNumCycles() == 0);
}

template <class T, class U>
bool Graph<T, U>::addVertex(const T &in) {
	typename vector<Vertex<T, U>*>::iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::iterator ite= vertexSet.end();
	for (; it!=ite; it++)
		if ((*it)->info == in) return false;
	Vertex<T, U> *v1 = new Vertex<T, U>(in);
	vertexSet.push_back(v1);
	return true;
}

template <class T, class U>
bool Graph<T, U>::removeVertex(const T &in) {
	typename vector<Vertex<T, U>*>::iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::iterator ite= vertexSet.end();
	for (; it!=ite; it++) {
		if ((*it)->info == in) {
			Vertex<T, U> * v= *it;
			vertexSet.erase(it);
			typename vector<Vertex<T, U>*>::iterator it1= vertexSet.begin();
			typename vector<Vertex<T, U>*>::iterator it1e= vertexSet.end();
			for (; it1!=it1e; it1++) {
				(*it1)->removeEdgeTo(v);
			}

			typename vector<Edge<T, U> >::iterator itAdj= v->adj.begin();
			typename vector<Edge<T, U> >::iterator itAdje= v->adj.end();
			for (; itAdj!=itAdje; itAdj++) {
				itAdj->dest->indegree--;
			}
			delete v;
			return true;
		}
	}
	return false;
}

template <class T, class U>
bool Graph<T, U>::addEdge(const T &sourc, const T &dest, double w) {
	typename vector<Vertex<T, U>*>::iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::iterator ite= vertexSet.end();
	int found=0;
	Vertex<T, U> *vS, *vD;
	while (found!=2 && it!=ite ) {
		if ( (*it)->info == sourc )
			{ vS=*it; found++;}
		if ( (*it)->info == dest )
			{ vD=*it; found++;}
		it ++;
	}
	if (found!=2) return false;
	vD->indegree++;
	vS->addEdge(vD,w);

	return true;
}

template <class T, class U>
bool Graph<T, U>::removeEdge(const T &sourc, const T &dest) {
	typename vector<Vertex<T, U>*>::iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::iterator ite= vertexSet.end();
	int found=0;
	Vertex<T, U> *vS, *vD;
	while (found!=2 && it!=ite ) {
		if ( (*it)->info == sourc )
			{ vS=*it; found++;}
		if ( (*it)->info == dest )
			{ vD=*it; found++;}
		it ++;
	}
	if (found!=2)
		return false;

	vD->indegree--;

	return vS->removeEdgeTo(vD);
}




template <class T, class U>
vector<T, U> Graph<T, U>::dfs() const {
	typename vector<Vertex<T, U>*>::const_iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::const_iterator ite= vertexSet.end();
	for (; it !=ite; it++)
		(*it)->visited=false;
	vector<T, U> res;
	it=vertexSet.begin();
	for (; it !=ite; it++)
	    if ( (*it)->visited==false )
	    	dfs(*it,res);
	return res;
}

template <class T, class U>
void Graph<T, U>::dfs(Vertex<T, U> *v,vector<T, U> &res) const {
	v->visited = true;
	res.push_back(v->info);
	typename vector<Edge<T, U> >::iterator it= (v->adj).begin();
	typename vector<Edge<T, U> >::iterator ite= (v->adj).end();
	for (; it !=ite; it++)
	    if ( it->dest->visited == false ){
	    	dfs(it->dest, res);
	    }
}

template <class T, class U>
vector<T, U> Graph<T, U>::bfs(Vertex<T, U> *v) const {
	vector<T, U> res;
	queue<Vertex<T, U> *> q;
	q.push(v);
	v->visited = true;
	while (!q.empty()) {
		Vertex<T, U> *v1 = q.front();
		q.pop();
		res.push_back(v1->info);
		typename vector<Edge<T, U> >::iterator it=v1->adj.begin();
		typename vector<Edge<T, U> >::iterator ite=v1->adj.end();
		for (; it!=ite; it++) {
			Vertex<T, U> *d = it->dest;
			if (d->visited==false) {
				d->visited=true;
				q.push(d);
			}
		}
	}
	return res;
}

template <class T, class U>
int Graph<T, U>::maxNewChildren(Vertex<T, U> *v, T &inf) const {
	vector<T, U> res;
	queue<Vertex<T, U> *> q;
	queue<int> level;
	int maxChildren=0;
	inf =v->info;
	q.push(v);
	level.push(0);
	v->visited = true;
	while (!q.empty()) {
		Vertex<T, U> *v1 = q.front();
		q.pop();
		res.push_back(v1->info);
		int l=level.front();
		level.pop(); l++;
		int nChildren=0;
		typename vector<Edge<T, U> >::iterator it=v1->adj.begin();
		typename vector<Edge<T, U> >::iterator ite=v1->adj.end();
		for (; it!=ite; it++) {
			Vertex<T, U> *d = it->dest;
			if (d->visited==false) {
				d->visited=true;
				q.push(d);
				level.push(l);
				nChildren++;
			}
		}
		if (nChildren>maxChildren) {
			maxChildren=nChildren;
			inf = v1->info;
		}
	}
	return maxChildren;
}


template <class T, class U>
Vertex<T, U>* Graph<T, U>::getVertex(const T &v) const {
	for(unsigned int i = 0; i < vertexSet.size(); i++)
		if (vertexSet[i]->info == v) return vertexSet[i];
	return NULL;
}

template<class T, class U>
void Graph<T, U>::resetIndegrees() {
	//colocar todos os indegree em 0;
	for(unsigned int i = 0; i < vertexSet.size(); i++) vertexSet[i]->indegree = 0;

	//actualizar os indegree
	for(unsigned int i = 0; i < vertexSet.size(); i++) {
		//percorrer o vector de Edges, e actualizar indegree
		for(unsigned int j = 0; j < vertexSet[i]->adj.size(); j++) {
			vertexSet[i]->adj[j].dest->indegree++;
		}
	}
}


template<class T, class U>
vector<Vertex<T, U>*> Graph<T, U>::getSources() const {
	vector< Vertex<T, U>* > buffer;
	for(unsigned int i = 0; i < vertexSet.size(); i++) {
		if( vertexSet[i]->indegree == 0 ) buffer.push_back( vertexSet[i] );
	}
	return buffer;
}


template <class T, class U>
void Graph<T, U>::dfsVisit() {
	typename vector<Vertex<T, U>*>::const_iterator it= vertexSet.begin();
	typename vector<Vertex<T, U>*>::const_iterator ite= vertexSet.end();
	for (; it !=ite; it++)
		(*it)->visited=false;
	it=vertexSet.begin();
	for (; it !=ite; it++)
	    if ( (*it)->visited==false )
	    	dfsVisit(*it);
}

template <class T, class U>
void Graph<T, U>::dfsVisit(Vertex<T, U> *v) {
	v->processing = true;
	v->visited = true;
	typename vector<Edge<T, U> >::iterator it= (v->adj).begin();
	typename vector<Edge<T, U> >::iterator ite= (v->adj).end();
	for (; it !=ite; it++) {
		if ( it->dest->processing == true) numCycles++;
	    if ( it->dest->visited == false ){
	    	dfsVisit(it->dest);
	    }
	}
	v->processing = false;
}

template<class T, class U>
vector<T, U> Graph<T, U>::topologicalOrder() {
	//vector com o resultado da ordenacao
	vector<T, U> res;

	//verificar se � um DAG
	if( getNumCycles() > 0 ) {
		cout << "Ordenacao Impossivel!" << endl;
		return res;
	}

	//garantir que os "indegree" estao inicializados corretamente
	this->resetIndegrees();

	queue<Vertex<T, U>*> q;

	vector<Vertex<T, U>*> sources = getSources();
	while( !sources.empty() ) {
		q.push( sources.back() );
		sources.pop_back();
	}

	//processar fontes
	while( !q.empty() ) {
		Vertex<T, U>* v = q.front();
		q.pop();

		res.push_back(v->info);

		for(unsigned int i = 0; i < v->adj.size(); i++) {
			v->adj[i].dest->indegree--;
			if( v->adj[i].dest->indegree == 0) q.push( v->adj[i].dest );
		}
	}

	//testar se o procedimento foi bem sucedido
	if ( res.size() != vertexSet.size() ) {
		while( !res.empty() ) res.pop_back();
	}

	//garantir que os "indegree" ficam atualizados ao final
	this->resetIndegrees();

	return res;
}



template<class T, class U>
vector<T, U> Graph<T, U>::getPath(const T &origin, const T &dest){

	list<T, U> buffer;
	Vertex<T, U>* v = getVertex(dest);

	//cout << v->info << " ";
	buffer.push_front(v->info);
	while ( v->path != NULL &&  v->path->info != origin) {
		v = v->path;
		buffer.push_front(v->info);
	}
	if( v->path != NULL )
		buffer.push_front(v->path->info);


	vector<T, U> res;
	while( !buffer.empty() ) {
		res.push_back( buffer.front() );
		buffer.pop_front();
	}
	return res;
}


template<class T, class U>
void Graph<T, U>::unweightedShortestPath(const T &s) {

	for(unsigned int i = 0; i < vertexSet.size(); i++) {
		vertexSet[i]->path = NULL;
		vertexSet[i]->dist = INT_INFINITY;
	}

	Vertex<T, U>* v = getVertex(s);
	v->dist = 0;
	queue< Vertex<T, U>* > q;
	q.push(v);

	while( !q.empty() ) {
		v = q.front(); q.pop();
		for(unsigned int i = 0; i < v->adj.size(); i++) {
			Vertex<T, U>* w = v->adj[i].dest;
			if( w->dist == INT_INFINITY ) {
				w->dist = v->dist + 1;
				w->path = v;
				q.push(w);
			}
		}
	}
}

template<class T, class U>
void Graph<T, U>::bellmanFordShortestPath(const T &s){

	for(unsigned int i = 0; i < vertexSet.size(); i++) {
		vertexSet[i]->path = NULL;
		vertexSet[i]->dist = INT_INFINITY;
		vertexSet[i]-> visited = false;
	}

	Vertex<T, U>* v = getVertex(s);
	v->dist = 0;
	queue< Vertex<T, U>* > q;
	q.push(v);

	while( !q.empty() ) {
		v = q.front(); q.pop();
		for(unsigned int i = 0; i < v->adj.size(); i++) {
			Vertex<T, U>* w = v->adj[i].dest;
			if( w->dist > v->dist + v->adj[i].weight) {
				w->dist = v->dist + v->adj[i].weight;
				w->path = v;
				if(!w->visited)
					q.push(w);
					w->visited = true;
			}
		}
	}
}

template<class T, class U>
void Graph<T, U>::dijkstraShortestPath(const T &s){

	for(unsigned int i = 0; i < vertexSet.size(); i++) {
		vertexSet[i]->path = NULL;
		vertexSet[i]->dist = INT_INFINITY;
		vertexSet[i]-> visited = false;
	}

	Vertex<T, U>* v = getVertex(s);
	v->dist = 0;
	priority_queue< Vertex<T, U>* > q;
	q.push(v);

	while( !q.empty() ) {
		v = q.top(); q.pop();
		priority_queue<Edge<T, U> > order;
		for(unsigned int i = 0; i < v->adj.size(); i++)
			order.push(v->adj[i]);

		while(!order.empty()){
			Vertex<T, U>* w = order.top().dest;
			if( w->dist > v->dist + order.top().weight) {
				w->dist = v->dist + order.top().weight;
				w->path = v;
				if(!w->visited)
					q.push(w);
					w->visited = true;
			}
			order.pop();
		}
	}
}

template <class T, class U>
void Graph<T, U>::floydWarshallShortestPath(){
	W.resize(vertexSet.size());
	P.resize(vertexSet.size());
	for(unsigned int i=0; i<vertexSet.size(); i++){
		W[i].resize(vertexSet.size());
		P[i].resize(vertexSet.size());
	}

	for(unsigned int i=0; i < vertexSet.size(); i++){
		for(unsigned int j=0; j< vertexSet.size(); j++){
			W[i][j] = edgeCost(i,j);

		}
	}
}

template <class T, class U>
int Graph<T, U>::edgeCost(int i, int j){
	if(i==j)
		return 0;
	for(unsigned int m=0; m< vertexSet[i]->adj.size(); m++){
		if(vertexSet[i]->adj[m].dest == vertexSet[j])
			return vertexSet[i]->adj[m].weight;
	}
	return INT_INFINITY;
}

template <class T, class U>
vector<T, U> Graph<T, U>::getfloydWarshallPath(const T &origin, const T &dest){

}

#endif /* GRAPH_H_ */
