#pragma once
#include <map>;
#include <string>;
#include <chrono>;

using namespace::std;

class Genre
	//this class holds a given genre and its relationships to all other genres
{
public:
	//most methods and variable are self explanatory and do not need an explanation
	Genre();
	Genre(string genre);
	void addListener();
	void AddCorrelation(string linkedGenre);
	void GenerateCorrelationFactors(map<string, Genre>& initializedGenres);
	void GenerateRankedCorrelationFactors();
	void initializeCompatibleGenre(Genre* compatibleGenre);
	void initializeRecommendationCorrelationFactors(string targetGenre, float weight);
	map<string, int>* returnCorrelations();
	map<string, float>* returnCorrelationFactors();
	map<string, float>* returnMostLinkedGenres();
	map<string, float>* returnRecommendedationCorrelationFactors();
	vector<Genre*> returnMostCompatibleGenres();
	string returnGenre();
	int returnListeners();
	int returnPopularity();

private:
	string genre;
	int popularity;
	int listeners;
	bool visited;
	map<string, int> correlationMap;
	map<string, float> correlationFactors;
	map<string, float> mostRelatedGenres;
	//final map utilized to suggest recommendations
	map<string, float> recommendationCorrelationFactors;
	vector<Genre*> mostCompatibleGenres;
};

