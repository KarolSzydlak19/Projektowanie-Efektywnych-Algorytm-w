#include <iostream>
#include "Node.h"
#include <vector>
#include <queue>
#define INF INT_MAX
Node::Node() {

}
Node::~Node() {

}

class compare {
public:
    bool operator()(const Node* l, const Node* r)const {    //porównanie granicy
        return l->cost > r->cost;
    }
};
void Node::reduce_row(int** matrix, int* row) {             
    
    for (int i = 0; i < nodeCnt; i++) {
        row[i] = INF;
    }

    for (int i = 0; i < nodeCnt; i++)
        for (int j = 0; j < nodeCnt; j++)
            if (matrix[i][j] < row[i])
                row[i] = matrix[i][j];

    for (int i = 0; i < nodeCnt; i++)
        for (int j = 0; j < nodeCnt; j++)
            if (matrix[i][j] != INF && row[i] != INF)
                matrix[i][j] -= row[i];
    
}


void Node::reduce_column(int** matrix, int* col) {
    for (int i = 0; i < nodeCnt; i++) {
        col[i] = INF;
    }

    for (int i = 0; i < nodeCnt; i++)
        for (int j = 0; j < nodeCnt; j++)
            if (matrix[i][j] < col[j])
                col[j] = matrix[i][j];

    for (int i = 0; i < nodeCnt; i++)
        for (int j = 0; j < nodeCnt; j++)
            if (matrix[i][j] != INF && col[j] != INF)
                matrix[i][j] -= col[j];
   
}
int Node::cost_calculation(int** matrix) {      //obliczenie kosztu drogi
    int cost = 0;
    int* row = new int[nodeCnt];
    reduce_row(matrix, row);
    int* col = new int[nodeCnt];
    reduce_column(matrix,col);

    for (int i = 0; i < nodeCnt; i++) {
        if (row[i] != INF) {
            cost += row[i];
        }
        if (col[i] != INF) {
            cost += col[i];
        }
    }
    delete[] row;
    delete[] col;
    return cost;
}
Node* Node::newNode(int** matrixP, std::vector<std::pair<int, int>> const& path, int level, int i, int j) {
    Node* node = new Node;
    node->path = path;
    node->matrix_reduced = new int* [nodeCnt];
    for (int i = 0; i < nodeCnt; i++) {
        node->matrix_reduced[i] = new int[nodeCnt];
    }
    if (level != 0) {
        node->path.push_back(std::make_pair(i, j));
    }
    for (int r = 0; r < nodeCnt; r++) {
        for (int c = 0; c < nodeCnt; c++) {
            node->matrix_reduced[r][c] = matrixP[r][c];
        }
    }
  
    for (int k = 0; level != 0 && k < nodeCnt; k++)     //odwiedzone wierzcho³ki s¹ zaznaczne w macierzy 
    {
        node->matrix_reduced[i][k] = INF;
        node->matrix_reduced[k][j] = INF;
    }
    node->matrix_reduced[j][0] = INF;
    node->level = level;
    node->vertex = j;
    return node;
}
void Node::print(std::vector<std::pair<int, int>>& path) {
    std::cout << "Path: ";
    for (int i = 0; i < path.size(); i++) {
        std::cout << path[i].first << " ";
    }
    std::cout << std::endl;
}

void Node::deleteNode(Node* node) {
    for (int i = 0; i < nodeCnt; i++) {
        delete[] node->matrix_reduced[i];
    }
    delete[] node->matrix_reduced;
    delete node;
}

int Node::solve(int** matrix) {
    std::priority_queue<Node*, std::vector<Node*>, compare> pq;
    std::vector<std::pair<int, int>> v;
    Node* root = newNode(matrix, v, 0, -1, 0);
    root->cost = cost_calculation(root->matrix_reduced);
    pq.push(root);
    while (!pq.empty()) {
        Node* min = pq.top();
        pq.pop();                                       //z kolejki pobierany jest najbardziej obiecuj¹cy wierzcho³ek
        int i = min -> vertex;
        if (min->level == nodeCnt - 1) {                //jeœli doszliœmy do liœcia otrzymaliœmy rozwi¹zanie
            while (!pq.empty()) {                       //usuniêcie kolejki priorytetowej
                Node* tmp = pq.top();
                pq.pop();
                deleteNode(tmp);
            }
            min->path.push_back(std::make_pair(i, 0));
            print(min->path);
            return min->cost;
        }
        for (int j = 0; j < nodeCnt; j++) {             //rozwiniêcie potomków obiecuj¹cego wierzcho³ka
            if (min->matrix_reduced[i][j] != INF) {
                Node* child = newNode(min->matrix_reduced, min->path, min->level + 1, i, j);
                child->cost = min->cost + min->matrix_reduced[i][j] + cost_calculation(child->matrix_reduced);
                pq.push(child);
                
            }
        }
        deleteNode(min);
    }
}
