#include<conio.h>
#include<string>
#include<iostream>
#include"TSP.h"
#include<string.h>
#include<string>
#include<Windows.h>
using namespace std;


void displayMenu(string info)
{
	cout << endl;
	cout << info << endl;
	cout << "1.Wczytaj z pliku" << endl;
	cout << "2.Algorytm Djikstry" << endl;
	cout << "3.Generuj graf " << endl;
	cout << "4.Wyswietl" << endl;
	cout << "5.Pomiary" << endl;
	//cout << "6.Wyswietl" << endl;
	//cout << "7.Test (pomiary)" << endl;
	cout << "0.Powrot do menu" << endl;
	cout << "Podaj opcje:";
}

void displayMenuBrute(string info)
{
	cout << endl;
	cout << info << endl;
	cout << "1.Wczytaj z pliku tekstowego" << endl;
	cout << "2.Wczytaj z pliku ze wspolrzednymi" << endl;
	cout << "3.Brute Force" << endl;
	cout << "4.Generuj graf" << endl;
	cout << "5.Pomiary - Brute Force" << endl;
	cout << "6.Zapisz graf" << endl;
	//cout << "7.Test (pomiary)" << endl;
	cout << "0.Powrot do menu" << endl;
	cout << "Podaj opcje:";
}
TSP tsp;
void menu_brute()
{
	char opt;
	string fileName;
	int index, value;
	do {
		displayMenuBrute("--- Short Path ---");
		opt = _getch();
		cout << endl;
		switch (opt) {
		case '1': //tutaj wczytytwanie  tablicy z pliku
			cout << " Podaj nazwê zbioru:";
			cin >> fileName;
			tsp.loadFromFile(fileName);
			tsp.printTSP();
			break;

		case '2':
			cout << " Podaj nazwê zbioru:" << endl;
			cin >> fileName;
			tsp.loadFromTSPlib(fileName);
			tsp.printTSP();
			break;
		case '3':
			tsp.bruteForce();
			break;
		case '4':
			int size;
			cout << "Podaj liczbê wierzcholkow:" << endl;
			cin >> size;
			tsp.generateMatrix(size);
			tsp.printTSP();
			break;

		case '5':
			for (int i = 13; i < 15; i++) {
				string n = "Grafy/tsp_";
				string name = n + to_string(i);
				tsp.loadFromFile(name);
				tsp.printTSP();
				tsp.BruteMeasure();
			}
			break;
		case '6':
			tsp.saveMatrix();
		} 
	} while (opt != '0');
}
int main(int argc, char* argv[])
{

	char option;
	do {
		cout << endl;
		cout << "==== MENU GLOWNE ===" << endl;
		cout << "1.Brute Force" << endl;
		cout << "0.Wyjscie" << endl;
		cout << "Podaj opcje:";
		option = _getch();
		cout << endl;

		switch (option) {
		case '1':
			menu_brute();
			break;


		}


		
	} while (option != '0');
	return 0;
}