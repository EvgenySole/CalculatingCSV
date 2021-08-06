// Calc.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <vector>
#include <map>

using namespace std;

float calculate(vector <float> &numbers, vector <char> &symbols) {
	if (numbers.size() == symbols.size() && symbols.at(0) == '-') { // Проверка наличия минуса перед первой переменной
		numbers.insert(numbers.begin(), -numbers.at(0));
		numbers.erase(numbers.begin() + 1);
		symbols.erase(symbols.begin());
		return calculate(numbers, symbols);
	}
	bool multFound = false;
	for (int i = 0; i < symbols.size(); i++) { // Поиск действий умножения или деления, чтобы выполнить их первыми
		if (symbols.at(i) == '*' || symbols.at(i) == '/') {
			multFound = true;
		}
	}
	for (int i = 0; i < symbols.size(); i++) {
		if ((symbols.at(i) == '+' || symbols.at(i) == '-') && !multFound) { // Действие сложения и вычитания
			if (symbols.at(i) == '+') {
				numbers.insert(numbers.begin() + i, numbers.at(i) + numbers.at(i + 1));
			}
			else {
				numbers.insert(numbers.begin() + i, numbers.at(i) - numbers.at(i + 1));
			}
			numbers.erase(numbers.begin() + i + 1);
			numbers.erase(numbers.begin() + i + 1);
			symbols.erase(symbols.begin() + i);
			break;
		}
		if (symbols.at(i) == '*' || symbols.at(i) == '/') { // Действие умножения и деления
			if (symbols.at(i) == '*') {
				numbers.insert(numbers.begin() + i, numbers.at(i) * numbers.at(i + 1));
			}
			else {
				numbers.insert(numbers.begin() + i, numbers.at(i) / numbers.at(i + 1));
			}
			numbers.erase(numbers.begin() + i + 1);
			numbers.erase(numbers.begin() + i + 1);
			symbols.erase(symbols.begin() + i);
			break;
		}
	}
	if (numbers.size() == 1) { // Проверка окончания вычисления
		float result = numbers.at(0);
		delete &numbers;
		delete &symbols;
		return result;
	}
	else {
		return calculate(numbers, symbols); // Рекурсивный переход к следующему действию
	}
}



float parseExp(string function) {
	if (function.find('(') != string::npos) { // Проверка наличия скобок, если есть, то решение выражения внутри них
		int pos1 = int (function.find('(') + 1);
		int pos2 = int (function.find_last_of(')'));
		function = function.substr(0, pos1 - 1) + to_string(parseExp(function.substr(pos1, pos2 - pos1))) + function.substr(pos2 + 1, function.length() - pos2);
	}
	vector <char> *symbols = new vector <char>;
	vector <float> *numbers = new vector <float>;
	string temp;
	for (int i = 0; i < function.length(); i++) { // Создание массивов со знаками и числами
		if (i == function.length() - 1) {
			if (function[i] != ')') {
				temp += function[i];
			}
			numbers->push_back(stof(temp));
			temp = "";
		}
		if (function[i] == '-' || function[i] == '+' || function[i] == '*' ||
			function[i] == '/' || function[i] == '(' || function[i] == ')') {
			symbols->push_back(function[i]);
			if (temp != "") {
				numbers->push_back(stof(temp));
			}
			temp = "";
		}
		else {
			temp += function[i];
		}
	}
	return calculate(*numbers, *symbols);
}



string toNumbers(string function, map <string, float> &variables) {
	int notFound = 0;
	bool flag = true;
	while (flag == true) {  // Замена букв на числовые значения
		notFound = 0;
		for (const auto kv : variables) {
			if (function.find(kv.first) != string::npos) {
				function.replace(function.find(kv.first), 1, to_string(kv.second));
			}
			else {
				notFound++;
			}
			if (notFound == variables.size()) {
				flag = false;
			}
		}
	}
	return function;
}



string deleteSpaces(string &function) { // Удаление пробелов из выражения
	while (function.find(' ') != string::npos) {
		function.erase(function.find(' '), 1);
	}
	return function;
}



int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "RUS");
	string inputLine;
	string functionIn;
	string fileName;
	cout << "Enter filename: ";
	cin >> fileName;
	ifstream inputFile(fileName); 
	ofstream outputFile;
	string inName = "";
	cout << "Enter filename to write, 0 for copy filename: ";
	cin >> inName;
	if (inName == "0") {
		outputFile.open(fileName.substr(0, fileName.length() - 4) + "-Calculated.csv");
	}
	else {
		outputFile.open(inName);
	}
	vector <string> letters;
	map <string, float> variables;
	if (inputFile.is_open()) {
		getline(inputFile, inputLine);
		for (int i = 0; i < inputLine.length(); i++) {
			if (inputLine[i] != ';') {
				letters.push_back(string(1, inputLine[i]));
			}
		}
		cout << "Variables are available: ";
		for (int i = 0; i < letters.size(); i++) {
			if (i + 1 != letters.size()) {
				cout << letters.at(i) << ", ";
			}
			else {
				cout << letters.at(i) << endl;
			}
		}
		cout << "Available actions: +, -, *, /" << endl;
		cout << endl;
		cout << "Enter function...";
		cin >> fileName;
		getline(cin, functionIn);
		functionIn = deleteSpaces(fileName + functionIn);
		outputFile << functionIn << endl;
		while (getline(inputFile, inputLine)) {
			string tempFloat;
			int count = 0;
			for (int i = 0; i < inputLine.length(); i++) { // Запись в словарь чисел из каждой строки
				if (inputLine[i] != ';') {
					tempFloat += inputLine[i];
				}
				if (inputLine.length() == i + 1 || inputLine[i] == ';') {
					variables[letters.at(count)] = (stof(tempFloat));
					tempFloat = "";
					count++;
				}
			}
			float result = 0;
			result = parseExp(toNumbers(functionIn, variables));
			outputFile << result << endl;
		}
	}
	inputFile.close();
	outputFile.close();
	cout << endl << "Succesfully done" << endl << endl;
	system("pause");
    return 0;
}

