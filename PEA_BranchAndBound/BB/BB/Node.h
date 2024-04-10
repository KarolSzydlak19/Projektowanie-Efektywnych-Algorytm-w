#include <vector>

class Node {
public:
    int nodeCnt;
    std::vector<std::pair<int, int>> path;
    int** matrix_reduced;
    int cost;
    int vertex;
    int level;
    Node();
    ~Node();
    void reduce_row(int** matrix, int* row);
    void reduce_column(int** matrix, int* col);
    int cost_calculation(int** matrix);
    Node* newNode(int** matrixP, std::vector<std::pair<int, int>> const& path, int level, int i, int j);
    void print(std::vector<std::pair<int, int>>& path);
    int solve(int** matrix);
    void deleteNode(Node* node);
};
