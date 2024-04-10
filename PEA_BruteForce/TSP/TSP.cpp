//
//

#include <iostream>
#include<fstream>
#include<string.h>
#include <string>
#include "TSP.h"
#include <cstdlib>
#include <climits>
#include<Windows.h>
using namespace std;
TSP::TSP() {

}
TSP::~TSP() {

}
int TSP::loadFromFile(std::string FileName) { //wczytanie z pliku tekstowego
	if (matrix != nullptr) {
		for (int i = 0; i < nodeCnt; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
		matrix = nullptr;
	}
	std::ifstream iFile;
	iFile.open(FileName);
	if (!iFile.is_open()) {
		std::cout << "nofile" << endl;
		return 1;
	}
	iFile >> nodeCnt;
	int startNode;
	int endNode;
	int val;
	matrix = new int* [nodeCnt];
	for (int i = 0; i < nodeCnt; i++) {
		matrix[i] = new int[nodeCnt];
	}
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			matrix[i][j] = 0;
		}
	}
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			iFile >> val;
			matrix[i][j] = val;
		}
	}
}
void TSP::printTSP() { //wypisanie macierzy
	cout << "  ";
	for (int i = 0; i < nodeCnt; i++) {
		cout << i << " ";
	}
	/*cout << endl;
	for (int i = 0; i < nodeCnt+4; i++) {
		cout << "--";
	}*/
	cout << endl;
	for (int i = 0; i < nodeCnt; i++) {
		//cout << "| "<< i<<"|";
		cout << i << " ";
		for (int j = 0; j < nodeCnt; j++) {
			cout << matrix[i][j] << " ";
		}
		//cout << " |";
		cout << endl;
	}
}

int TSP::loadFromTSPlib(string FileName) {// wczytanie z pliku z współrzędnymi
	if (matrix != nullptr) {
		for (int i = 0; i < nodeCnt; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
		matrix = nullptr;
	}
	std::ifstream iFile;
	iFile.open(FileName);
	if (!iFile.is_open()) {
		std::cout << "nofile" << endl;
		return 1;
	}
	iFile >> nodeCnt;

	matrix = new int* [nodeCnt];
	for (int i = 0; i < nodeCnt; i++) {
		matrix[i] = new int[nodeCnt];
	}
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			matrix[i][j] = 0;
		}
	}

	int* xc = new int[nodeCnt];
	int* yc = new int[nodeCnt];
	int irr;
	float q, w;
	for (int i = 0; i < nodeCnt; i++) {
		iFile >> irr;
		iFile >> q;
		iFile >> w;
		q = static_cast<int>(q);
		w = static_cast<int>(w);
		xc[i] = q;
		yc[i] = w;
	}
	for (int i = 0; i < nodeCnt; i++) {
		for (int j = 0; j < nodeCnt; j++) {
			int a, b, c;
			a = abs(xc[i] - xc[j]);
			b = abs(yc[i] - yc[j]);
			c = sqrt(a * a + b * b);
			matrix[i][j] = c;
		}
	}
	return 0;
}
//Poniżej funkcja do rekurencyjnego rozwiązania TSP metodą Brute Force
void TSP::bruteForceRec(int* vis, int path, int lastVis, int& shortestPath, vector<int>& shortestPathNodes, vector<int>& currentPathNodes, int &num) {
	bool allVisited = true;
	for (int i = 0; i < nodeCnt; i++) {	//wykonanie dla każdego wierzchołka grafu
		if (vis[i] == -1) {
			allVisited = false;
			int newPath = path + matrix[i][lastVis];	//dodanie wagi między ostatnio odwiedzonym, a aktualnie odwiedzanym wierzchołkiem do wartośći ścieżki
			vis[i] = vis[lastVis] + 1;	//zapamiętanie ostatnio odwiedzonego wierzchołka

			
			currentPathNodes.push_back(i);//pamiętanie ścieżk w liście

			bruteForceRec(vis, newPath, i, shortestPath, shortestPathNodes, currentPathNodes, num);//rekurencyjne wywołanie funkcji

			
			currentPathNodes.pop_back();// po powrocie z rekurencji usunięcie wierzchołka z listy odwiedzonych, niżej odznaczenie go w tablicy

			vis[i] = -1;
		}
	}

	if (allVisited) {	//jeśli wszytkie wierzchołki zostały odwiedzone sprawdzenie długości drogi i ewentualne jej zapisanie
		int first = currentPathNodes.front();
		int last = currentPathNodes.back();
		currentPathNodes.push_back(first);
		path = path + matrix[first][last];
		//cout <<"--------------------- " << "PATH: " << path << "num: "<<num <<endl;
		//num++;
		//for (int node : currentPathNodes) {
		//	cout << node << " ";
		//}
		currentPathNodes.pop_back();
		//cout <<endl;
		if (path < shortestPath) {
			shortestPath = path;
			shortestPathNodes = currentPathNodes; // Update the shortest path nodes
		}
	}
}

float TSP::bruteForce() {	//tworzy zmienne lokalne i wywołuje funkcję rekurencyjną do rozwiązania problemu
	LARGE_INTEGER startTime, endTime, diffTime, frequency;
	int* vis = new int[nodeCnt];
	for (int i = 0; i < nodeCnt; i++) {
		vis[i] = -1;
	}
	int shortestPath = INT_MAX;
	vector<int> shortestPathNodes;
	int num =0;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startTime);
	for (int i = 0; i < nodeCnt; i++) {	//wywołanie funkcji rekurencyjnej dla każdego wierzchołka
		int path = 0;
		vis[i] = 0;
		vector<int> currentPathNodes;
		currentPathNodes.push_back(i);
		bruteForceRec(vis, path, i, shortestPath, shortestPathNodes, currentPathNodes,num);
		vis[i] = -1;
	}
	QueryPerformanceCounter(&endTime);
	diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
	float time = ((float)diffTime.QuadPart * 1000) / frequency.QuadPart;
	int first = shortestPathNodes.front();
	shortestPathNodes.push_back(first);
	cout << "Shortest Path Length: " << shortestPath << endl;
	cout << "Shortest Path Sequence: ";
	for (int node : shortestPathNodes) {
		cout << node << " ";
	}
	cout << endl;

	delete[] vis;
	return time;
}

void TSP::generateMatrix(int size) { //generowanie macierzy
	if (matrix != nullptr) {
		for (int i = 0; i < nodeCnt; i++) {
			delete[] matrix[i];
		}
		delete[] matrix;
		matrix = nullptr;
	}
	nodeCnt = size;
	matrix = new int* [size];
	for (int i = 0; i < nodeCnt; i++) {
		matrix[i] = new int[nodeCnt];
	}
	int val;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			val = rand() % (200) + 1;
			if (i == j) {
				matrix[i][j] = 0;
			}
			else {
				matrix[i][j] = val;
			}
		}
	}
}
void TSP::BruteMeasure() { //funkcja do pomiarów
	string str = "PomiaryBrute/Brute_";
	ofstream oFile;
	//cout << "Podaj rozmiar: " << endl;
	//int size;
	//cin >> size;
	string name = str + to_string(nodeCnt);
	oFile.open(name);
	if (oFile.is_open()) {
		//oFile << "";
		//oFile << "Czas\tRozmiar" << endl;
	}
	for (int i = 0; i < 50; i++) {
		//generateMatrix(size);
		float time = bruteForce();
		if (oFile.is_open()) {
			oFile << time << "\t" << nodeCnt << endl;
		}
	}
	if (oFile.is_open()) {
		oFile.close();
	}
}
void TSP::saveMatrix() {
	string str = "Grafy/tsp_";
	ofstream oFile;
	string name = str + to_string(nodeCnt);
	oFile.open(name);
	if (oFile.is_open()) {
		oFile << nodeCnt << endl;
		for (int i = 0; i < nodeCnt; i++) {
			for (int j = 0; j < nodeCnt; j++) {
				oFile << matrix[i][j] << " ";
			}
			oFile << endl;
		}
		oFile.close();
	}
}



