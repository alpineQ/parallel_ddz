/**
 * DataSharing.h
 *
 * Заголовочный файл вспомогательных функций передачи
 * данных между MPI-процессами
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#ifndef PARTIAL_SUM_DATA_SHARING_H
#define PARTIAL_SUM_DATA_SHARING_H

#include "InputData.h"

void sendDataToProcesses(float** sequences, int &nSequences, int sequenceLength, int nProcesses);
void getDataFromRoot(float** data, int &dataPerProcess, int &sequenceLength);

#endif //PARTIAL_SUM_DATA_SHARING_H
