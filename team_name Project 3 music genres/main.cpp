#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>

#include "Genre.h"
#include "GraphSearch.h"

using namespace::std;

string dataSelect() {

	//gets what data sets the user wants to load
	string dataChoice;
	cout << "Select which data sets to load: \n";
	cout << " 1. Croatia\n 2. Romania\n 3. Hungary\n 4. All\n";
	getline(cin, dataChoice);
	while (dataChoice != "1" && dataChoice != "2" && dataChoice != "3" && dataChoice != "4") {
		cout << "Please enter a valid number\n";
		getline(cin, dataChoice);
	}
	return dataChoice;
}

string operationSelect() {

	//recieves operation from user
	string opChoice;
	cout << "Select operation:\n";
	cout << " 1. Most popular genre\n 2. Least Popular Genre\n 3. Enter a user's genres\n 4. View stats of a given genre\n 5. List genres\n 6. Exit\n";
	getline(cin, opChoice);
	return opChoice;
}

void listGenres(map<string, Genre>& initializedGenres) {

	//list all the genres
	cout << "Listing genres in alphabetical order..." << endl;
	auto iter = initializedGenres.begin();
	while (iter != initializedGenres.end()) {
		cout << iter->first << ", ";
		iter++;
	}
	cout << "\n\n";
}

void genrePopularity(map<string, Genre>& initializedGenres, int mostLeast, int dataPoints) {

	//returns either the most popular genre or least popular genre depending on what is desired
	string maxGenre = "";
	int genres = 0;
	auto iter = initializedGenres.begin();
	int max = iter->second.returnPopularity();
	if (mostLeast == 1) {
		while (iter != initializedGenres.end()) {
			if (iter->second.returnPopularity() > max) {
				max = iter->second.returnPopularity();
				maxGenre = iter->second.returnGenre();
			}
			iter++;
		}
		map<string, int> maxGenreCorrelations = *initializedGenres[maxGenre].returnCorrelations();
		cout << "Most popular genre: " << maxGenre << " with " << initializedGenres[maxGenre].returnListeners() << " listeners (" << (float)initializedGenres[maxGenre].returnListeners() / (float)dataPoints * 100 <<  "%)\n";
	}

	else {
		while (iter != initializedGenres.end()) {
			if (iter->second.returnPopularity() < max) {
				max = iter->second.returnPopularity();
				maxGenre = iter->second.returnGenre();
			}
			iter++;
		}
		map<string, int> maxGenreCorrelations = *initializedGenres[maxGenre].returnCorrelations();
		cout << "Least popular genre: " << maxGenre << " with " << initializedGenres[maxGenre].returnListeners() << " listeners (" << (float)initializedGenres[maxGenre].returnListeners() / (float)dataPoints * 100.0 << "%)\n";
	}

}

vector<string> sortMostRecommended(map<string, float>& recommendedGenres, int amount) {

	//sorts the recommended genres into the top 5 genres
	vector<string> orderedRecommended;
	vector<float> weightRecommended;
	for (int i = 0; i < amount; i++) {
		orderedRecommended.push_back("");
		weightRecommended.push_back(0);
	}
	int minLocation = 0;
	float min = 0;
	auto iter = recommendedGenres.begin();
	while (iter != recommendedGenres.end()) {
		min = weightRecommended[0];
		minLocation = 0;
		for (int i = 0; i < amount; i++) {
			if (weightRecommended[i] < min) {
				min =weightRecommended[i];
				minLocation = i;
			}
		}
		if (iter->second > min) {
			weightRecommended[minLocation] = iter->second;
			orderedRecommended[minLocation] = iter->first;
		}
		iter++;
	}

	//sorting algorithm to organize that list so it is inorder
	float max = 0;
	int maxLocation = 0;
	for (int i = 0; i < amount; i++) {
		max = weightRecommended[i];
		maxLocation = i;
		for (int j = i; j < amount; j++) {
			if (max < weightRecommended[j]) {
				max = weightRecommended[j];
				maxLocation = j;
			}
			float temp = weightRecommended[i];
			string tempString = orderedRecommended[i];
			weightRecommended[i] = max;
			orderedRecommended[i] = orderedRecommended[maxLocation];
			weightRecommended[maxLocation] = temp;
			orderedRecommended[maxLocation] = tempString;
		}
	}

	return orderedRecommended;
}

map<string, float> bFS(vector<string>* userGenresPointer, map<string, Genre>& initializedGenres) {
	//BFS
	cout << "Beginning breadth first search...\n";
	float factor = 1;
	vector<string> userGenres = *userGenresPointer;
	map<string, float> userRecommendations;
	//BFS Search and time calculations
	auto startBFS = chrono::high_resolution_clock::now();
	for (int i = 0; i < userGenres.size(); i++) {
		GraphSearch rootGenre = GraphSearch(&initializedGenres[userGenres[i]]);
		auto babyIter = initializedGenres.begin();
		while (babyIter != initializedGenres.end()) {
			float rCF = 1.0 / rootGenre.breadthFirstSearch(babyIter->first);
			if (rCF < 0)
				rCF = -0.2;
			else if (rCF == 0)
				rCF = 0.01;
			userRecommendations[babyIter->first] = userRecommendations[babyIter->first] + rCF * pow(0.9, i);
			babyIter++;
		}
	}
	auto stopBFS = chrono::high_resolution_clock::now();
	auto durationBFS = chrono::duration_cast<chrono::milliseconds>(stopBFS - startBFS);
	cout << "BFS duration: " << durationBFS.count() << "ms" << endl;

	//generates calculations for each genre correlation
	auto recommendedIter = userRecommendations.begin();
	while (recommendedIter != userRecommendations.end()) {
		userRecommendations[recommendedIter->first] = userRecommendations[recommendedIter->first] / ((float)userGenres.size() * pow(0.95, userGenres.size()));
		recommendedIter++;
	}
	factor = factor * 0.95;
	cout << endl;
	return userRecommendations;
}

map<string, float> dFS(vector<string>* userGenresPointer, map<string, Genre>& initializedGenres) {
	//DFS
	vector<string> userGenres = *userGenresPointer;
	float factor = 1;
	cout << "Beginning depth first search...\n";
	map<string, float> userRecommendations;

	//DFS search and time calculations
	auto startDFS = chrono::high_resolution_clock::now();
	for (int i = 0; i < userGenres.size(); i++) {
		GraphSearch rootGenre = GraphSearch(&initializedGenres[userGenres[i]]);
		auto babyIter = initializedGenres.begin();
		while (babyIter != initializedGenres.end()) {
			float rCF = 1.0 / rootGenre.depthFirstSearch(babyIter->first);
			if (rCF < 0)
				rCF = -0.2;
			else if (rCF == 0)
				rCF = 0.01;
			userRecommendations[babyIter->first] = userRecommendations[babyIter->first] + rCF * factor;
			babyIter++;
		}
		factor = factor * 0.95;
	}
	auto stopDFS = chrono::high_resolution_clock::now();
	auto durationDFS = chrono::duration_cast<chrono::milliseconds>(stopDFS - startDFS);
	cout << "DFS duration: " << durationDFS.count() << "ms" << endl;

	//generates calculations
	auto recommendedIter = userRecommendations.begin();
	while (recommendedIter != userRecommendations.end()) {
		userRecommendations[recommendedIter->first] = userRecommendations[recommendedIter->first] / ((float)userGenres.size());
		recommendedIter++;
	}
	return userRecommendations;
}

void recommendedGenres(map<string, Genre>& initializedGenres) {

	//generates recommended genre after recieving a list from the user
	vector<string> userGenres;
	string currGenre = "start";
	cout << "We kindly ask you offer us your favorite genres. Please order them most favorite to least favorite and we recommend a max of 10 genres.\n";
	cout << "Enter a genre or type \"done\" if you are done entering genres!\n";
	getline(cin, currGenre);
	while (currGenre != "done") {
		cout << "Genre added: " << currGenre << endl;
		userGenres.push_back(currGenre);
		getline(cin, currGenre);
	}
	cout << "\nCalculating recommendations...\n\n";
	dFS(&userGenres, initializedGenres);
	map<string, float> userRecommendations = bFS(&userGenres, initializedGenres);

	//eliminate repeats
	auto iter = userRecommendations.begin();
	while (iter != userRecommendations.end()) {
		for (int i = 0; i < userGenres.size(); i++) {
			userRecommendations[userGenres[i]] = -1.0;
		}
		iter++;
	}

	//sort and display the top 5 recomendations, done from BFS data as it is more accurate
	cout << "We have 5 recommendations for you!" << endl;
	vector<string> recommendedGenres = sortMostRecommended(userRecommendations, 5);
	for (int i = 0; i < recommendedGenres.size(); i++) {
		cout << i + 1 << ". " << recommendedGenres[i] << endl;
	}
	cout << endl;
	
}

void viewGenreStats(map<string, Genre>& initializedGenres, int dataPoints) {

	//view some stats (listeners and related genres) for a given genre
	string genreChoice;
	cout << "Select genre: \n";
	getline(cin, genreChoice);
	cout << genreChoice << " has " << initializedGenres[genreChoice].returnListeners() << " listeners which is " << (float)initializedGenres[genreChoice].returnListeners() / (float)dataPoints * 100.0 << "% of all listeners." << endl;
	
	//uses similar code as BFS and DFS as it is just executing a BFS search to find the 5 closest neighbors,
	vector<string> userGenres;
	userGenres.push_back(genreChoice);
	map<string, float> userRecommendations = bFS(&userGenres, initializedGenres);
	auto iter = userRecommendations.begin();
	while (iter != userRecommendations.end()) {
		for (int i = 0; i < userGenres.size(); i++) {
			userRecommendations[userGenres[i]] = -1.0;
		}
		iter++;
	}

	//display the 5 most related genres, done with a BFS to show performance but could be done by simply recieving the vector of neighbors to any given genres 5 closest neigbors
	cout << "Top 5 related genres" << endl;
	vector<string> recommendedGenres = sortMostRecommended(userRecommendations, 5);
	for (int i = 0; i < recommendedGenres.size(); i++) {
		cout << i + 1 << ". " << recommendedGenres[i] << endl;
	}
	cout << endl;
}

void depthVSBreadth(map<string, Genre>& initializedGenres) {

	//hidden method used for debugging, can be accessed by entering DnB on operation select, simply returns the distance between two genres using BFS and DFS, BFS is more accurate, DFS finds as close of a path as possible
	//but it can mark certain nodes as "visited" which can actually block the best path, so it finds the best path it can.
	string startGenre;
	string endGenre;
	cout << "Enter Starting Genre: \n";
	getline(cin, startGenre);
	cout << "Enter Ending Genre: \n";
	getline(cin, endGenre);
	GraphSearch dnb = GraphSearch(&initializedGenres[startGenre]);
	auto startBFS = chrono::high_resolution_clock::now();
	cout << "BFS: " << dnb.breadthFirstSearch(endGenre) << endl;
	auto stopBFS = chrono::high_resolution_clock::now();
	auto durationBFS = chrono::duration_cast<chrono::milliseconds>(stopBFS - startBFS);
	cout << "BFS duration : " << durationBFS.count() << endl;
	auto startDFS = chrono::high_resolution_clock::now();
	cout << "DFS: " << dnb.depthFirstSearch(endGenre) << endl;
	auto stopDFS = chrono::high_resolution_clock::now();
	auto durationDFS = chrono::duration_cast<chrono::milliseconds>(stopDFS - startDFS);
	cout << "DFS duration : " << durationDFS.count() << endl;


}

void insertGenres(vector<string>& userGenres, map<string, Genre> &initializedGenres, unordered_map<string, bool> &recognizedGenres) {

	//simple method for inserting genres
	for (int i = 0; i < userGenres.size(); i++) {
		initializedGenres[userGenres[i]].addListener();
		if (recognizedGenres[userGenres[i]] != true) {
			initializedGenres[userGenres[i]] = Genre(userGenres[i]);
			recognizedGenres[userGenres[i]] = true;
		}
		for (int j = 0; j < userGenres.size(); j++) {
			initializedGenres[userGenres[i]].AddCorrelation(userGenres[j]);
		}
	}
}

void listLinkedGenres(string genre, map<string, Genre> &initializedGenres) {
	//listen the most linked genres, primarily used for debugging
	cout << genre << ": " << initializedGenres[genre].returnMostLinkedGenres();
}

int main() {

	//menu
	bool cycle = true;
	string data = dataSelect();

	//setting up file to be opened
	string fileName;
	if (data == "1" || data == "4") {
		fileName = "data/HR_genres.json";
	}
	else if (data == "2") {
		fileName = "data/RO_genres.json";
	}
	else if (data == "3") {
		fileName = "data/HU_genres.json";
	}
	cout << "Loading " << fileName << "...\n";

	ifstream file;
	file.open(fileName);
	//setting up things to read
	int dataPoints = 0;
	string line;
	bool recording = false;
	bool genreRecording = false;
	int j = 0;
	string genre = "";
	vector<string> userGenres;
	//keep track of what genres have shown up
	unordered_map<string, bool> recognizedGenres;
	//holds all of our genre objects
	map<string, Genre> initializedGenres;

	//formatting of file
	//entire file enclosed within {}
	//"ID": ["Genre1", "Genre2", "Genre2"...], repeat ENTIRE thing is technically on one line, reading might be ram intensive
	//example "12345": ["Pop", "Rock", "Alternative"], "11111": ["Reggae", "Rock", "Alternative"], ...
	while (file.is_open()) {
		//take in the data, takes a while
		while (getline(file, line, '[')) {
			dataPoints++;
			getline(file, line, ']');
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == '"' && !genreRecording) {
					genreRecording = true;
				}
				else if (line[i] == '"') {
					userGenres.push_back(genre);
					genre.clear();
					j = 0;
					genreRecording = false;
				}
				else if (genreRecording) {
					genre = genre + line[i];
					j++;
				}
			}
			insertGenres(userGenres, initializedGenres, recognizedGenres);
			userGenres.clear();
		}
		if (data == "4" && fileName == "data/HR_genres.json") {
			fileName = "data/RO_genres.json";
			file.close();
			file.open(fileName);
			cout << "Loading " << fileName << "...\n";
		}
		else if (data == "4" && fileName == "data/RO_genres.json") {
			fileName = "data/HU_genres.json";
			file.close();
			file.open(fileName);
			cout << "Loading " << fileName << "...\n";
		}
		else {
			file.close();
			cout << "Loading complete.\n\n";
		}
		
	}

	//total data points processed
	cout << "Data Points: " << dataPoints << endl;
	cout << endl;
	cout << "Initializing Graph...\n\n";

	//Creating a graph with correlation factors
	auto iter = initializedGenres.begin();
	while (iter != initializedGenres.end()) {
		iter->second.GenerateCorrelationFactors(initializedGenres);
		iter->second.GenerateRankedCorrelationFactors();
		map<string, float> temp = *(iter->second.returnMostLinkedGenres());

		// create graph
		auto babyIter = temp.begin();
		while (babyIter != temp.end()) {
			iter->second.initializeCompatibleGenre(&initializedGenres[babyIter->first]);
			babyIter++;
		}
		iter++;
	}

	cout << "Graph initialized\n";

	//operations menu
	while (cycle) {
		string opChoice = operationSelect();
		if (opChoice == "1")
			genrePopularity(initializedGenres, 1, dataPoints);
		else if (opChoice == "2")
			genrePopularity(initializedGenres, -1, dataPoints);
		else if (opChoice == "3")
			recommendedGenres(initializedGenres);
		else if (opChoice == "4")
			viewGenreStats(initializedGenres, dataPoints);
		else if (opChoice == "5")
			listGenres(initializedGenres);
		else if (opChoice == "6") {
			cout << "We miss you already! :(" << endl;
			cycle = false;
		}
		else if (opChoice == "DnB")
			depthVSBreadth(initializedGenres);
		else
			cout << "Invalid input, please only enter the number of the operation you would like\n";
	}

}