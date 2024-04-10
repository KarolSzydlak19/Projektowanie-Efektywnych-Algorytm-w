
#include <iostream>
#include "Node.h"
#include <queue>
#include <string>
#include <fstream>
#include <map>
#include <Windows.h>
#define INF INT_MAX
int** matrix;
int nodeCnt;
int iterations;
std::string fileName = "bb.ini";
LARGE_INTEGER startTime, endTime, diffTime, frequency;
std::string pom = "PomiaryBB/";



//wczytanie macierzy z pliku
int** loadFromFile(std::string FileName, int &nodeCnt) {
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
        std::cout << "nofile" << std::endl;
        return NULL;
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
    return matrix;
}


// Funkcja do wczytania pliku .ini
std::pair<std::vector<std::string>, std::vector<int>> read_ini(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::vector<std::string> file_names;
    std::vector<int> numbers;
    std::string line;
    std::string current_section;
    int default_key_counter = 1;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                if (line[0] == '[' && line[line.size() - 1] == ']') {
                    current_section = line.substr(1, line.size() - 2);
                }
                else if (current_section == "Section1") {
                    file_names.push_back(line);
                }
                else if (current_section == "Section2") {
                    try {
                        numbers.push_back(std::stoi(line));
                    }
                    catch (const std::invalid_argument& e) {
                        // Obs³uga b³êdów
                        std::cout << "Blad: " << std::endl;
                    }
                }
            }
        }
        file.close();
    }
    else {
        std::cerr << "Nie uda³o siê otworzyæ pliku." << std::endl;
    }

    return { file_names, numbers };
}

int main() {
    auto result = read_ini(fileName);
    std::vector<std::string> file_names = result.first;
    std::vector<int> numbers = result.second;

    for (int i = 0; i < file_names.size(); i++) {
        std::ofstream oFile;
        std::string tmp = file_names[i];
        tmp.erase(0, 6);
        std::string outName = pom + "OUT_" + tmp;
        oFile.open(outName);
        matrix = loadFromFile(file_names[i],nodeCnt);
        Node node;
        node.nodeCnt = nodeCnt;
        node.matrix_reduced = matrix;
        if (oFile.is_open()) {
            
            //oFile << node.solve(matrix);
        }
        for (int j = 0; j < numbers[i]; j++) {
            
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime);
            std::cout << "Length: " << node.solve(matrix) << "\n";
            QueryPerformanceCounter(&endTime);
            diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
            float time = ((float)diffTime.QuadPart * 1000) / frequency.QuadPart;
            if (oFile.is_open()) {
                oFile << time << "\t" << nodeCnt << std::endl;
            }
        }
        if (oFile.is_open()) {
            oFile.close();
        }
    }
    
}

