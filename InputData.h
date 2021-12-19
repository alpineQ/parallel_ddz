/**
 * InputData.h
 *
 * Заголовочный файл вспомогательных функций упрощающих работу с
 * входными данными (векторами последовательностей)
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#ifndef PARTIAL_SUM_INPUT_DATA_H
#define PARTIAL_SUM_INPUT_DATA_H

#include <vector>
#include <string>

using namespace std;

int loadFromFile(const string &filename, float** data, int &nSequences, int &sequenceLength);
int generateData(int sequenceLength, int nSequences, float** data);
void getDataPerProcess(int nProcesses, int nSequences, int *dataPerProcess);


#endif //PARTIAL_SUM_INPUT_DATA_H
