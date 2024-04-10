// BB.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <fstream>
#include "BB.h"
using namespace std;
BB::BB() {

}
BB::~BB() {

}
int BB::loadFromFile(std::string FileName) { //wczytanie z pliku tekstowego
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
void BB::printMatrix() {
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
