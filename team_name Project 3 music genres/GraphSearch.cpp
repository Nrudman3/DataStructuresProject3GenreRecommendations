#include <iostream>;

#include "GraphSearch.h"

using namespace::std;

GraphSearch::GraphSearch() {
	
}
GraphSearch::GraphSearch(Genre* rootGenre) {
	this->rootGenre = *rootGenre;
}
float GraphSearch::breadthFirstSearch(string targetGenre) {

	//Conducts breadth first search
	if (rootGenre.returnGenre() == targetGenre) {
		return -1.0;
	}
	queue<Genre> q;
	map<string, float> visited;
	q.push(rootGenre);
	visited[rootGenre.returnGenre()] = 1.0;

	while (!q.empty()) {
		Genre curr = q.front();
		if (targetGenre == curr.returnGenre()) {
			return visited[curr.returnGenre()];
		}
		q.pop();
		vector<Genre*> mostCompatibleGenres = curr.returnMostCompatibleGenres();
		for (int i = 0; i < mostCompatibleGenres.size(); i++) {
			Genre adj = *mostCompatibleGenres[i];
			string adjString = adj.returnGenre();
			if (!visited[adjString]) {
				visited[adjString] = visited[curr.returnGenre()] + 1.0;
				q.push(adj);
			}
		}
	}
	return -1.0;
}
float GraphSearch::depthFirstSearch(string targetGenre) {

	//conducts depth first search
	float min = -1.0;
	stack<Genre> s;
	stack<float> depth;
	map<string, float> visited;
	s.push(rootGenre);
	depth.push(1.0);
	visited[rootGenre.returnGenre()] = 1.0;
	while (!s.empty()) {
		Genre curr = s.top();
		float currDepth = depth.top();
		if (targetGenre == curr.returnGenre()) {
			if (min == -1.0)
				min = currDepth;
			else if (visited[curr.returnGenre()] < min)
				min = currDepth;
		}
		depth.pop();
		s.pop();
		vector<Genre*> mostCompatibleGenres = curr.returnMostCompatibleGenres();
		for (int i = 0; i < mostCompatibleGenres.size(); i++) {
			Genre adj = *mostCompatibleGenres[i];
			string adjString = adj.returnGenre();
			if ((!visited[adjString] || adjString == targetGenre) && (visited[curr.returnGenre()] <= min || min == -1.0)) {
				visited[adjString] = visited[curr.returnGenre()] + 1.0;
				s.push(adj);
				depth.push(currDepth + 1.0);
			}
			
		}
		
	}
	return min;
}