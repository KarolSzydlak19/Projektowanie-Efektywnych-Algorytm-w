#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <map>
#include <Windows.h>
#include <random>
#include <Psapi.h>
#define INF INT_MAX
#define PI 3.141592;
std::string fileName = "sa.ini";
LARGE_INTEGER startTime, endTime, diffTime, frequency;
std::string pom = "PomiarySA/";
PROCESS_MEMORY_COUNTERS_EX pmc;



int* solution;
int nodeCnt;
int** matrix = nullptr;
float T;
float lambda;
int age;
int coolingType = 1; // 0 oznacza chłodzenie geometryczne, 1 logarytmiczne
int neighberhoodC = 1; // 0 oznacza tworzenie sąsiedztwa przez zamianę, 1 przez odwrócenie sekwencji
float TLimit = 100; // po osiągnięciu tej temperatury algorytm kończy działanie
int a,b,c;

float geometricCool(float T, float lambda) {
    T = lambda * T;
    return T;
}
float linearCool(float T, float lambda) {
    return T-lambda;
}
//wczytanie macierzy z pliku
int** loadFromFile(std::string FileName) {
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

    /*for (int i = 0; i < nodeCnt; i++) {
        for (int j = 0; j < nodeCnt; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl << "-------------------" << std::endl;
    }*/
    return matrix;
}
int** loadFromTSPlib(std::string FileName) {// wczytanie z pliku z współrzędnymi
    if (matrix != nullptr) {
        for (int i = 0; i < nodeCnt; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
        matrix = nullptr;
    }
    int irr;
    int mode;
    std::string mod;
    std::ifstream iFile;
    iFile.open(FileName);
    if (!iFile.is_open()) {
        std::cout << "nofile" << std::endl;
        
    }
    iFile >> mod;
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
    float* geoX = new float[nodeCnt];
    float* geoY = new float[nodeCnt];
    float q, w;
    float* latitude = new float[nodeCnt];
    float* longitude = new float[nodeCnt];
    if (mod == "GEO") {
        std::cout << "GEO" << std::endl;
        for (int i = 0; i < nodeCnt; i++) {
            iFile >> irr;
            iFile >> q;
            iFile >> w;
            geoX[i] = q;
            geoY[i] = w;
            int deg = static_cast<int>(q);
            int min = q - deg;
            latitude[i] = 3.141592 * (deg + 5.0 * min / 3.0) / 180;
            deg = static_cast<int>(w);
            min = w - deg;
            longitude[i] = 3.141592 * (deg + 5.0 * min / 3.0) / 180;
        }
        for (int i = 0; i < nodeCnt; i++) {
            for (int j = 0; j < nodeCnt; j++) {
                float RRR = 6378.388;
                float q1, q2, q3;
                q1 = cos(longitude[i] - longitude[j]);
                q2 = cos(latitude[i] - latitude[j]);
                q3 = cos(latitude[i] + latitude[j]);
                int dij = (int)(RRR * acos(0.5 * ((1.0 + q1) * q2) - (1.0 - q1) * q3) + 1.0);
                matrix[i][j] = dij;
            }
        }
    }
    else {
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
    }
    

    return matrix;
}
int randINT(int size) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, size);
    return dist(rng);
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
                        // Obsługa błędów
                        std::cout << "Blad: " << std::endl;
                    }
                }
                else if (current_section == "Cooling") {
                    try {
                        coolingType = std::stoi(line);
                        std::cout << "CHLODZENIE: " << coolingType << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji cooling" << std::endl;
                    }
                }
                else if (current_section == "Neighberhood") {
                    try {
                        neighberhoodC = std::stoi(line);
                        std::cout << "SASIEDZTWO: " << neighberhoodC << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji Neighberhood" << std::endl;
                    }
                }
                else if (current_section == "Lambda") {
                    try {
                        lambda = std::stof(line);
                        std::cout << "lambad: " << lambda << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji Lamda" << std::endl;
                    }
                }
                else if (current_section == "T0") {
                    try {
                        a = std::stoi(line);
                        std::cout << "t0: " << a << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji T0" << std::endl;
                    }
                }
                else if (current_section == "age") {
                    try {
                        b = std::stoi(line);
                        std::cout << "age: " << b << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji age" << std::endl;
                    }
                }
                else if (current_section == "TLimit") {
                    try {
                        c = std::stoi(line);
                        std::cout << "limit: " << c << std::endl;
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji TLimit" << std::endl;
                    }
                }
            }
        }
        file.close();
    }
    else {
        std::cerr << "Nie udało się otworzyć pliku." << std::endl;
    }

    return { file_names, numbers };
}

void sol(int* solution) {
    int* visited = new int[nodeCnt];
    int temp;
    int lastNode = 0;
    int currNode = 0;
    for (int i = 0; i < nodeCnt; i++) {
        solution[i] = 0;
        visited[i] = 0;
    }
    visited[0] = 1;
    for (int i = 1; i < nodeCnt; i++) {
        temp = INF;
        for (int j = 0; j < nodeCnt; j++) {
            if (matrix[lastNode][j] < temp && visited[j] == 0) {
                temp = matrix[lastNode][j];
                currNode = j;
            }
        }
        solution[i] = currNode;
        visited[currNode] = 1;
    }
}
int calculateSol(int* sol) {
    int val = 0;
    for (int i = 0; i < nodeCnt-1; i++) {
        val += matrix[sol[i]][sol[i+1]];
    }
    val += matrix[sol[nodeCnt-1]][sol[0]];
    return val;
}

float randFloat(float P) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.0f, P);
    return dist(rng);  
}
float beginT(float a, int cost) {
    return cost * a;
}

int main()
{
    srand(static_cast <unsigned> (time(0)));
    auto result = read_ini(fileName);
    std::vector<std::string> file_names = result.first;
    std::vector<int> numbers = result.second;
    for (int i = 0; i < file_names.size(); i++) {
        std::ofstream oFile;
        std::string tmp = file_names[i];
        std::string roz;
        std::cout << file_names[i] << std::endl;
        if (tmp.length() >= 4) {
            roz = tmp.substr(tmp.length() - 4);
        }
        tmp.erase(0, 7);
        std::string outName = pom + "OUT_" + tmp;
        oFile.open(outName);
        if (roz == ".tsp") {
            matrix = loadFromTSPlib(file_names[i]);
        }else
        matrix = loadFromFile(file_names[i]);
        solution = new int[nodeCnt];
        int* tempSol;
        int cost;
        tempSol = new int[nodeCnt];
        for (int j = 0; j < numbers[i]; j++) {
            std::cout << j << std::endl;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime);
            sol(solution);
            cost = calculateSol(solution);
            for (int i = 0; i < nodeCnt; i++) {
                tempSol[i] = solution[i];
            }
            T = beginT(a, nodeCnt);
            float P = T;
            age = nodeCnt * b;
            TLimit = pow(10, c);
            std::cout << "COST START: " << cost << std::endl;
            int* lowestSol = new int[nodeCnt];
            int lowestCost = INF;
            //int** neighb;
            //int* neighbCost = new int[age];
            //neighb = new int* [age];
            /*for (int i = 0; i < age; i++) {
                neighb[i] = new int[age];
            }*/
            int greedySol = INF;
            int greedySolIndex = 0;

            
            
            do {


                //std::cout << "Aktualna Temperatura: " << T << std::endl;
                //std::cout << "Temperatura koncowa: " << TLimit << std::endl;
                //std::cout << "Aktualny koszt: " << cost << std::endl;
                for (int i = 0; i < age; i++) {
                    
                    int node1, node2;
                    // losowanie indeksów
                    node1 = randINT(nodeCnt - 1);
                    do {
                        node2 = randINT(nodeCnt - 1);
                    } while (node1 == node2);
                    // tworzenie sąsiedztwa przez zamianę wierzchołków
                    if (neighberhoodC == 0) {
                        int buf = tempSol[node2];
                        tempSol[node2] = tempSol[node1];
                        tempSol[node1] = buf;
                    }
                    // tworzenie sąsiedztwa przez zmianę sekwencji
                    else if (neighberhoodC == 1) {
                        if (node1 > node2) {
                            while (node1 > node2) {
                                int buf = tempSol[node2];
                                tempSol[node2] = tempSol[node1];
                                tempSol[node1] = buf;
                                node1--;
                                node2++;
                            }
                        }
                        else {
                            while (node2 > node1) {
                                int buf = tempSol[node2];
                                tempSol[node2] = tempSol[node1];
                                tempSol[node1] = buf;
                                node1++;
                                node2--;
                            }
                        }
                    }
                    //std::cout << node1 << " " << node2 << std::endl;
                    int tempCost = calculateSol(tempSol);
                   // std::cout << "Prawdopodobienstwo przyjecia rozwiazania: " << T / P * 100 << std::endl;

                    float r = randFloat(P);
                    if (tempCost < cost || r < T) {
                        for (int i = 0; i < nodeCnt; i++) {
                            solution[i] = tempSol[i];
                            cost = tempCost;
                        }
                    }



                    if (cost < lowestCost) {
                        for (int i = 0; i < nodeCnt; i++) {
                            lowestSol[i] = solution[i];
                        }
                        lowestCost = cost;
                    }
                    for (int i = 0; i < nodeCnt; i++) {
                        tempSol[i] = solution[i];
                    }

                    

                   
                }
               

                
                
                if (coolingType == 0) {
                    T = geometricCool(T, lambda);
                }
                else  if(coolingType == 1){
                    T = linearCool(T, lambda);
                }
                std::cout << T << std::endl;
            } while (T > TLimit);
            if (lowestCost < cost) {
                for (int i = 0; i < nodeCnt; i++) {
                    solution[i] = lowestSol[i];
                }
            }
            QueryPerformanceCounter(&endTime);
            diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
            float time = ((float)diffTime.QuadPart * 1000) / frequency.QuadPart;
            //poniżej odczytanie używanej pamięci
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
            SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
            SIZE_T inMb = physMemUsedByMe / (1024 * 1024);
            std::cout << "File: " << file_names[i] << std::endl;
            std::cout << "PATH: ";
            for (int i = 0; i < nodeCnt; i++) {
                std::cout << solution[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "COST: " << cost << std::endl;
            std::cout << "Memory: " << inMb << "mb" << std::endl;
            if (oFile.is_open()) {
                oFile <<cost << "\t" << time << "\t" << lambda << "\t" << a << "\t" << b << "\t" << c << "\t" << inMb << std::endl;
            }
        }
        if (oFile.is_open()) {
            oFile.close();
        }
        
    }
        
    



}


