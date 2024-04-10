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
std::string fileName = "ga.ini";
LARGE_INTEGER startTime, endTime, diffTime, frequency, improvementTime, currentTime, improvementDiff;
std::string pom = "PomiaryGA/";
PROCESS_MEMORY_COUNTERS_EX pmc;


int* solution;
int nodeCnt;
int** matrix = nullptr;
std::vector<std::pair<std::vector<int>, int>> population;
std::vector<std::pair<std::vector<int>, int>> offspring;
float pk,pm;    // prawdopodobieństwo krzyżowania i mutacji
int iterations;   //liczba iteracji dla danej populacji
int N;
float n;

int selectionCnt; //liczba osobników biorących udział w selekcji turniejowej
int offspringSize; //rozmiar wektora potomków
int successionRate; //% populacji przechodzącej do następnej generacji (sukcesja elitarna)
std::vector<int> bestSol;//najlepsza ścieżka
int bestCost = INF;//najlepszy koszt


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
                int dij = (int)(RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
                
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
    /*for (int i = 0; i < nodeCnt; i++) {
        for (int j = 0; j < nodeCnt; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl << "-------------------" << std::endl;
    }*/

    return matrix;
}
int randINT(int size) {
    //std::random_device dev;
    //std::mt19937 rng(dev());
    //std::uniform_int_distribution<std::mt19937::result_type> dist(0, size);
    std::random_device rd; // Urządzenie do generowania ziarna
    std::mt19937 gen(rd()); // Generator (Mersenne Twister)

    // Rozkład jednostajny w zadanym zakresie
    std::uniform_int_distribution<> distrib(0, size);

    // Generowanie losowego int
    int randomInt = distrib(gen);
    return randomInt;
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
                else if (current_section == "PopulationSize") {
                    try {
                        n = std::stof(line);
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji PopulationSize" << std::endl;
                    }
                }
                else if (current_section == "pk") {
                    try {
                        pk = std::stof(line);
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji PopulationSize" << std::endl;
                    }
                }
                else if (current_section == "pm") {
                    try {
                        pm = std::stof(line);
                        
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji pm" << std::endl;
                    }
                }
                else if (current_section == "iterations") {
                    try {
                        iterations = std::stoi(line);
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji iterations" << std::endl;
                    }
                }
                else if (current_section == "SuccessionRate") {
                    try {
                        successionRate = std::stoi(line);
                    }
                    catch (const std::invalid_argument& e) {
                        std::cout << "Nieprawidlowa wartosc w sekcji SuccesionRate" << std::endl;
                    }
                }
                else if (current_section == "TLimit") {
                    try {
                        
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

// Funkcja obliczająca koszt rozwiązania (do zaimplementowania)
int calculateSol(const std::vector<int>& sol) {
    int cost = 0;
    for (int i = 0; i < nodeCnt-1; i++) {
        cost += matrix[sol[i]][sol[i + 1]];
    }
    cost += matrix[sol[nodeCnt - 1]][sol[0]];

    return cost; // tymczasowa wartość
}

float randFloat(float P) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0.0f, P);
    return dist(rng);
}

void generateRandSolution(std::vector<int>& sol) {
    sol.clear();
    sol.resize(nodeCnt);
    for (int i = 0; i < nodeCnt; i++) {
        sol[i] = i;
    }
    std::random_shuffle(sol.begin(), sol.end());
}

void orderCrossover(const std::vector<int>& parent0, const std::vector<int>& parent1, std::vector<int>& child) {   //Krzyżowanie zamienne
    int parent_C = randINT(1);
    int firstNode = randINT(nodeCnt - 1);
    int secondNode = randINT(nodeCnt - 1);

    //stworzenie mapy aby pamietać które z miast w znajdują się w chromosomie dziecka
    int* map = new int[nodeCnt];
    for (int i = 0; i < nodeCnt; i++) {
        map[i] = 0;
    }

    if (secondNode < firstNode) {
        int bufor = secondNode;
        secondNode = firstNode;
        firstNode = bufor;
    }
    //std::cout << "Rand1 " << firstNode << std::endl;
    //std::cout << "Rand2 " << secondNode << std::endl;
    for (int i = firstNode; i < secondNode+1; i++) {
            child[i] = parent0[i];
            map[parent0[i]] = 1;
    }
    for (int i = 0; i < nodeCnt; i++) {
        if (i<firstNode || i>secondNode) {
            int node;
            for (int j = 0; j < nodeCnt; j++) {
                node = parent1[j];
                if (map[node] == 0) {
                    break;
                }
            }
            child[i] = node;
            map[node] = 1;
        }
    }
    delete[] map;
}
void swapMutate(std::vector<int>& obj) {
    int firstNode = randINT(nodeCnt - 1);
    int secondNode;
    do {
        secondNode = randINT(nodeCnt - 1);
    } while (firstNode == secondNode);
    int bufor = obj[firstNode];
    obj[firstNode] = obj[secondNode];
    obj[secondNode] = bufor;
   /* int size = obj.size();

    // Sprawdzenie, czy wektor ma wystarczającą liczbę elementów do mutacji
    if (size < 2) {
        return; // Nie można przeprowadzić mutacji, jeśli jest tylko jeden element lub wektor jest pusty
    }

    // Wybór losowego indeksu elementu do przeniesienia
    int indexToMove = rand() % size;

    // Wybór losowego indeksu docelowego (różnego od indeksu początkowego)
    int targetIndex;
    do {
        targetIndex = rand() % size;
    } while (targetIndex == indexToMove);

    // Przeniesienie elementu
    int elementToMove = obj[indexToMove];
    obj.erase(obj.begin() + indexToMove);
    obj.insert(obj.begin() + targetIndex, elementToMove);*/
    
}
// Funkcja generująca początkową populację
void generateFirstBatch(std::vector<std::pair<std::vector<int>, int>>& population) {
    population.clear();
    population.resize(N);

    for (int i = 0; i < N; i++) {
        generateRandSolution(population[i].first);
        population[i].second = calculateSol(population[i].first);
        if (population[i].second < bestCost) {
            bestCost = population[i].second;
            bestSol = population[i].first;
            QueryPerformanceCounter(&improvementTime);
        }
    }
}

int* createTab() {
    int* tab = new int[N];
    for (int i = 0; i < N; i++) {
        tab[i] = i;
    }
    return tab;
}
int reduceTAB(int* newTAB, int index, int size) {
    
    for (int i = 0; i < index; i++) {
        newTAB[i] = newTAB[i];
    }
    for (int i = index; i < size; i++) {
        newTAB[i] = newTAB[i + 1];
    }
    
    return 0;
}

int tournamentSelection(int* selTAB, int* selTABSize) {
    int chosen = -1;
    int lowestCost = INF;
    int obj;
    for (int i = 0; i < selectionCnt; i++) {
        obj = randINT(*selTABSize - 1);
        if (population[selTAB[obj]].second < lowestCost) {
            lowestCost = population[selTAB[obj]].second;
            chosen = selTAB[obj];
        }
    }
    //std::cout << ": " << *selTABSize << std::endl;
    --(*selTABSize);
    //std::cout << ": " << *selTABSize << std::endl;
    reduceTAB(selTAB, obj, *selTABSize);
    //std::cout << "Wybrano: " << chosen << std::endl;
    return chosen;
}

int main()
{
    float standardPm;
    srand(static_cast <unsigned> (time(0)));
    auto result = read_ini(fileName);
    std::vector<std::string> file_names = result.first;
    std::vector<int> numbers = result.second;
    std::ofstream oFile;
    oFile.open("PomiaryGA/pomiary.csv");
    for (int i = 0; i < file_names.size(); i++) {
        
        std::string tmp = file_names[i];
        std::string roz;
        std::cout << file_names[i] << std::endl;
        if (tmp.length() >= 4) {
            roz = tmp.substr(tmp.length() - 4);
        }
        tmp.erase(0, 7);
        if (roz == ".tsp") {
            tmp.resize(tmp.size() - 4);
            tmp += "tsp";
        }
        if (roz == "atsp") {
            tmp.resize(tmp.size() - 5);
            tmp += "atsp";
        }
        std::string outName = pom + "OUT_" + tmp + ".csv";
        //oFile.open(outName);
        if (roz == ".tsp") {
            matrix = loadFromTSPlib(file_names[i]);
        }
        else
            matrix = loadFromFile(file_names[i]);
        //N = nodeCnt * n;
        N = static_cast<int>(n*nodeCnt);
        oFile << tmp << std::endl;
        for (int j = 0; j < numbers[i]; j++) {
            bestCost = INF;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime);
        start:
            //std::cout << "++++++++++++++++++NEW BATCH+++++++++++++++++++++" << std::endl;
            int noImprovementIterations = 0;
            //std::cout << j << std::endl;
            offspringSize = 0;
            float succesionR = successionRate / 100.0;
            if (N > 50) {
               // N = 50;
            }
            succesionR = N * succesionR;
            
            
            

            generateFirstBatch(population);

            // Wyświetlanie populacji i kosztów
            /*for (const auto& pair : population) {
                for (int node : pair.first) {
                    std::cout << node << " ";
                }
                std::cout << " - Koszt: " << pair.second << std::endl;
            }*/


            selectionCnt = (int)N * 0.1;
            standardPm = pm;
            int* selTAB = createTab();
            int selTABSizeVal = N;
            int* selTABSize = &selTABSizeVal;
            
            //poniżej selekcja osobników do krzyżowania, wybiera się indeksy w tablicy population
            int p1, p2;
            while (true) {
                if (*selTABSize < 2) {
                    delete[] selTAB;
                    selTAB = createTab();
                    *selTABSize = N;
                }
                while (offspring.size() <  2*N) {
                    p1 = tournamentSelection(selTAB,selTABSize);
                    do {

                        if (*selTABSize < 2) {
                            delete[] selTAB;
                            selTAB = createTab();
                            *selTABSize = N;
                        }
                        p2 = tournamentSelection(selTAB,selTABSize);
                    } while (p1 == p2);
                    
                    //krzyżowanie 1 potomek
                    float produce = randFloat(1.0);
                    if (produce < pk) {
                        offspringSize++;
                        offspring.resize(offspringSize);
                        offspring[offspringSize - 1].first.resize(nodeCnt);
                        orderCrossover(population[p1].first, population[p2].first, offspring[offspringSize - 1].first);
                        offspring[offspringSize - 1].second = calculateSol(offspring[offspringSize - 1].first);
                        if (offspring[offspringSize - 1].second < bestCost) {
                            bestCost = offspring[offspringSize - 1].second;
                            bestSol = offspring[offspringSize - 1].first;
                            QueryPerformanceCounter(&improvementTime);
                            noImprovementIterations = 0;
                            pm = standardPm;
                        }
                        //mutajca
                        float mutate = randFloat(1.0);
                        if (mutate < pm) {
                            swapMutate(offspring[offspringSize - 1].first);
                            offspring[offspringSize - 1].second = calculateSol(offspring[offspringSize - 1].first);
                            if (offspring[offspringSize - 1].second < bestCost) {
                                bestCost = offspring[offspringSize - 1].second;
                                bestSol = offspring[offspringSize - 1].first;
                                QueryPerformanceCounter(&improvementTime);
                                noImprovementIterations = 0;
                                pm = standardPm;
                            }
                        }
                        //krzyżownie drugi potomek
                        //produce = randFloat(1.0);
                        //if (produce < pk) {
                            offspringSize++;
                            offspring.resize(offspringSize);
                            offspring[offspringSize - 1].first.resize(nodeCnt);
                            orderCrossover(population[p2].first, population[p1].first, offspring[offspringSize - 1].first);
                            offspring[offspringSize - 1].second = calculateSol(offspring[offspringSize - 1].first);
                            if (offspring[offspringSize - 1].second < bestCost) {
                                bestCost = offspring[offspringSize - 1].second;
                                bestSol = offspring[offspringSize - 1].first;
                                QueryPerformanceCounter(&improvementTime);
                                noImprovementIterations = 0;
                                pm = standardPm;
                            }
                            //mutajca
                            //float mutate = randFloat(1.0);
                            if (mutate < pm) {
                                swapMutate(offspring[offspringSize - 1].first);
                                offspring[offspringSize - 1].second = calculateSol(offspring[offspringSize - 1].first);
                                if (offspring[offspringSize - 1].second < bestCost) {
                                    bestCost = offspring[offspringSize - 1].second;
                                    bestSol = offspring[offspringSize - 1].first;
                                    QueryPerformanceCounter(&improvementTime);
                                    noImprovementIterations = 0;
                                    pm = standardPm;
                                }
                            }
                        //}
                    }
                    
                }



                    // Sortowanie wektorów
                    std::sort(population.begin(), population.end(), [](const auto& a, const auto& b) {
                        return a.second < b.second; // Sortowanie według kosztu
                    });
                    std::sort(offspring.begin(), offspring.end(), [](const auto& a, const auto& b) {
                        return a.second < b.second; // Sortowanie według kosztu
                    });
                    int offspNum = 0;
                    for (int i = succesionR; i < N; i++) {
                        population[i].first = offspring[offspNum].first;
                        population[i].second = offspring[offspNum].second;
                    }
                    
                    offspring.clear();
                    offspringSize = 0;
                    //std::cout << "COST: " << bestCost << std::endl;

                    QueryPerformanceCounter(&endTime);
                    diffTime.QuadPart = endTime.QuadPart - startTime.QuadPart;
                    float time = ((float)diffTime.QuadPart * 1000) / frequency.QuadPart;

                    if (time >= 600000) {
                        std::cout <<"END_TIME: " << time << std::endl;
                        break;
                    }

                    if (noImprovementIterations > 1000) {   
                        
                        
                        std::cout <<  "Najlepszy dotychczas: " << bestCost << std::endl;
                        noImprovementIterations = 0;
                    }
                    noImprovementIterations++;
                    std::cout << "Best: " << bestCost << std::endl;
                    //std::cout << "Najlepszy dotychczas: " << bestCost << std::endl;
                    
            }
            


            
            

            



            





            
            improvementDiff.QuadPart = improvementTime.QuadPart - startTime.QuadPart;
            float bestTime = ((float)improvementDiff.QuadPart * 1000) / frequency.QuadPart;
            //poniżej odczytanie używanej pamięci
            MEMORYSTATUSEX memInfo;
            memInfo.dwLength = sizeof(MEMORYSTATUSEX);
            GlobalMemoryStatusEx(&memInfo);
            DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
            SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
            SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
            SIZE_T inMb = physMemUsedByMe / (1024 * 1024);
            std::cout << "COST: " << bestCost << std::endl;
            std::cout << "File: " << file_names[i] << std::endl;
            std::cout << "TIME: " << bestTime << std::endl;
            //for (int i = 0; i < nodeCnt; i++) {
             //   std::cout << solution[i] << " ";
            //}
            //std::cout << std::endl;
            //std::cout << "COST: " << cost << std::endl;
            //std::cout << "Memory: " << inMb << "mb" << std::endl;
            if (oFile.is_open()) {
                oFile << bestCost << "," << bestTime << "," << pk<< "," << pm << "," << N << "," << n << "," << inMb << ",";
                for (int i = 0; i < nodeCnt-1; i++) {
                    oFile << bestSol[i] << "-";
                }
                oFile << bestSol[nodeCnt - 1] << std::endl;
            }
        }
        

    }
    if (oFile.is_open()) {
        oFile.close();
    }
}
