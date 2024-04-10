#include <iostream>
#include <vector>
#include <string>
#include <pugixml.hpp>
using std::vector;
class TSP {
public:
	int nodeCnt;
	int** matrix;
	TSP();
	~TSP();

	int loadFromFile(std::string FileName);
	
	int loadFromTSPlib(std::string FileName);

	void printTSP();

	void bruteForceRec(int* vis, int path, int lastVis, int& shortestPath, vector<int>& shortestPathNodes, vector<int>& currentPathNodes, int &num);

	float bruteForce();

	void generateMatrix(int size);

	void BruteMeasure();

	void saveMatrix();
};
