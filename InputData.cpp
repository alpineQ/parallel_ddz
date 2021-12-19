/**
 * InputData.cpp
 *
 * Файл вспомогательных функций упрощающих работу с
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
 * @param sequenceLength – длина последовательности
 * @param nSequences – число последовательностей
 * @param data – область памяти выделенная под последовательности
 * @returns Код ошибки, в случае нормальной работы возвращает 0
 */
int generateData(int sequenceLength, int nSequences, float** data) {
    srand(time(NULL));
    try {
        data = new float*[nSequences];
        for (unsigned i = 0; i < nSequences; ++i) {
            data[i] = new float[sequenceLength];
            for (unsigned j = 0; j < nSequences; ++j)
                data[i][j] = rand();
        }
    } catch (std::bad_alloc &error) {
        std::cerr << "Memory allocation error" << std::endl;
        return -1;
    }
    return 0;
}

/**
 * Заполнение последовательностей данными из входного файла
 *
 * @param filename – имя входного файла с последовательностями
 * @param data – область памяти выделенная под последовательности
 * @param nSequences – число последовательностей
 * @param sequenceLength – длина последовательностей
 * @returns Код ошибки, в случае нормальной работы возвращает 0
 */
int loadFromFile(const string &filename, float** data, int &nSequences, int &sequenceLength) {
    ifstream inputFile(filename);
    if (inputFile.fail()) {
        cerr << "File open error: " << strerror(errno) << endl;
        return -4;
    }
    inputFile >> sequenceLength;
    inputFile >> nSequences;

    data = new float*[nSequences];
    for (unsigned i = 0; i < nSequences; ++i) {
        data[i] = new float[sequenceLength];
        for (unsigned j = 0; j < sequenceLength; ++j)
            try {
                inputFile >> data[i][j];
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
 * @param nSequences – число последовательностей
 * @param dataPerProcess массив отражающий распределение последовательностей по MPI-процессам
 */
void getDataPerProcess(int nProcesses, int nSequences, int *dataPerProcess) {
	for (unsigned i = 0; i < nProcesses; ++i) {
        dataPerProcess[i] = nSequences / nProcesses;
        if (nSequences % nProcesses != 0 && i < nSequences % nProcesses)
            ++dataPerProcess[i];
    }
}
