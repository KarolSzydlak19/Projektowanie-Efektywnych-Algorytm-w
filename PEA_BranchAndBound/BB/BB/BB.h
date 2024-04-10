#include <iostream>
#include <vector>
#include <string>
class BB {
public:
	int nodeCnt;
	int** matrix;
	BB();
	~BB();

	int loadFromFile(std::string FileName);

	void printMatrix();

	int loadFromTSPlib(std::string FileName);

	void BranchNBound();

};
