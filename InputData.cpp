/**
 * InputData.cpp
 *
 * Вспомогательный класс InputData, упрощающий работу с
 * входными данными (векторами последовательностей)
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include "InputData.h"

using namespace std;

/**
 * Заполнение последовательностей случайными значениями
 *
 * @param n – длина последовательности
 * @param m – число последовательностей
 */
void InputData::generateData(int n, int m) {
    nSequences = m;
    sequenceLength = n;
    for (unsigned i = 0; i < nSequences; ++i) {
        sequences.emplace_back(sequenceLength);
        sequences[i].generate();
    }
}

/**
 * Заполнение последовательностей данными из входного файла
 *
 * @param filename – имя входного файла с последовательностями
 * @returns Код ошибки, в случае нормальной работы возвращает 0
 */
int InputData::loadFromFile(const string &filename) {
    ifstream inputFile(filename);
    if (inputFile.fail()) {
        cerr << "File open error: " << strerror(errno) << endl;
        return -4;
    }
    inputFile >> sequenceLength;
    inputFile >> nSequences;
    sequences.reserve(nSequences);

    auto dataPosition = inputFile.tellg();

    for (std::string line; std::getline(inputFile, line);) {
        if (line.empty() || line == "\n" || line == "\r")
            continue;
        sequences.emplace_back(sequenceLength);
    }
    inputFile.clear();
    inputFile.seekg(dataPosition);

    for (unsigned i = 0; i < nSequences; ++i)
        for (unsigned j = 0; j < sequenceLength; ++j) {
            string value;
            inputFile >> value;
            try {
                for (char c: value)
                    if (!isdigit(c) && c != '-' && c != ',' && c != '.')
                        throw invalid_argument("Invalid symbol");
                sequences[i].data[j] = stof(value);
            } catch (invalid_argument &error) {
                cerr << "Invalid data format: " << i + 1 << " " << j + 1 << endl;
                inputFile.close();
                return -7;
            }
        }
    inputFile.close();
    return 0;
}

/**
 * Равномерное распределение количества входных последовательностей между процессами
 *
 * @param nProcesses – число MPI-процессов
 * @returns массив отражающий распределение последовательностей по MPI-процессам
 */
int* InputData::getDataPerProcess(int nProcesses) const {
    int *dataPerProcess = safeAllocate<int>(nProcesses);
	for (unsigned i = 0; i < nProcesses; ++i) {
        dataPerProcess[i] = nSequences / nProcesses;
        if (nSequences % nProcesses != 0 && i < nSequences % nProcesses)
            ++dataPerProcess[i];
    }
	return dataPerProcess;
}
