/**
 * DataSharing.h
 *
 * Файл вспомогательных функций передачи данных между MPI-процессами
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#include <mpi.h>
#include "DataSharing.h"

#define ROOT_RANK 0


/**
 * Приём необходимых данных для решения задачи MPI-процессом
 * @param data данные последовательностей
 * @param dataPerProcess количество последовательностей
 * @param sequenceLength длина последовательности
 */
void getDataFromRoot(float** data, int &dataPerProcess, int &sequenceLength) {
    MPI_Bcast(&sequenceLength,
              1,
              MPI_INT,
              ROOT_RANK,
              MPI_COMM_WORLD);
    MPI_Scatter(nullptr,
                0,
                MPI_INT,
                &dataPerProcess,
                1,
                MPI_INT,
                ROOT_RANK,
                MPI_COMM_WORLD);
    data = new float*[dataPerProcess];
    for (unsigned i = 0; i < dataPerProcess; ++i)
        data[i] = new float[sequenceLength];
    MPI_Reduce_scatter(nullptr, data, nullptr, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

/**
 * Отправка всех необходимых данных для решения задачи всем MPI-процессам
 *
 * @param sequences – последовательности чисел с плавающей запятой
 * @param nSequences – число последовательностей
 * @param sequenceLength – длина последовательностей
 * @param nProcesses – число MPI-процессов
 */
void sendDataToProcesses(float** sequences, int &nSequences, int sequenceLength, int nProcesses) {
    MPI_Bcast(&sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    int *dataPerProcess;
    dataPerProcess = new int[nProcesses];
    getDataPerProcess(nProcesses, nSequences, dataPerProcess);
    MPI_Scatter(dataPerProcess,
                1,
                MPI_INT,
                nullptr,
                0,
                MPI_UNSIGNED,
                ROOT_RANK,
                MPI_COMM_WORLD);
    float** rootData;
    rootData = new float*[nSequences];
    for (unsigned i = 0; i < nSequences; ++i)
        rootData[i] = new float[sequenceLength];
    MPI_Reduce_scatter(sequences, rootData, dataPerProcess, MPI_FLOAT, 0, MPI_COMM_WORLD);

    nSequences = dataPerProcess[ROOT_RANK];
    delete[] dataPerProcess;
    delete[] sequences;
    sequences = rootData;
}
