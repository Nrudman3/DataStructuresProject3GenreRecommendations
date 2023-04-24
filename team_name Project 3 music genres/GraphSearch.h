#include <iomanip>;
#include <vector>;
#include <queue>;
#include <stack>;
#include <string>;

#include "Genre.h"

#pragma once
class GraphSearch
{
	//simple class to perform search operations
public:
	GraphSearch();
	GraphSearch(Genre* rootGenre);
	float breadthFirstSearch(string targetGenre);
	float depthFirstSearch(string targetGenre);
	
private:
	Genre rootGenre;
	string targetGenre;
};

