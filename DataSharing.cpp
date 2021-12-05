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
 * Отправка типов данных последовательностей всем MPI-процессам
 *
 * @param nProcesses – число MPI-процессов
 * @param dataPerProcess – массив распределения количества данных по процессам
 */
void sendTypes(int nProcesses, int* dataPerProcess, InputData input) {
    bool *types = safeAllocate<bool>(nProcesses);
    int *displacements = safeAllocate<int>(nProcesses);
    for (unsigned i = 0; i < nProcesses; ++i) {
        types[i] = input.sequences[i].type;
        displacements[i] = 0;
    }
    MPI_Scatterv(types,
                 dataPerProcess,
                 displacements,
                 MPI_CXX_BOOL,
                 nullptr,
                 0,
                 MPI_CXX_BOOL,
                 ROOT_RANK,
                 MPI_COMM_WORLD);
    delete[] types;
    delete[] displacements;
}

/**
 * Принятие типов данных последовательностей MPI-процессом
 *
 * @param nSequences – число последовательностей
 * @returns типы данных последовательностей
 */
bool* recvTypes(int nSequences) {
    bool *types = safeAllocate<bool>(nSequences);
    MPI_Scatterv(nullptr,
                 nullptr,
                 nullptr,
                 MPI_CXX_BOOL,
                 types,
                 nSequences,
                 MPI_CXX_BOOL,
                 ROOT_RANK,
                 MPI_COMM_WORLD);
    return types;
}

/**
 * Отправка данных последовательностей всем MPI-процессам
 *
 * @param dataPerProcess – массив распределения количества данных по процессам
 * @param input – данные последовательностей
 * @returns данные последовательностей, предназначенные главному MPI-процессу
 */
vector<Sequence> sendData(const int* dataPerProcess, InputData input) {
    vector<Sequence> rootData;
    for (int process = 0, sequence = 0; sequence < input.nSequences; ++process) {
        MPI_Request request = MPI_REQUEST_NULL;
        for (int i = 0; i < dataPerProcess[process]; ++i, ++sequence) {
            if (process == ROOT_RANK)
                rootData.push_back(input.sequences[sequence]);
            else
                MPI_Isend(input.sequences[sequence].data,
                          input.sequenceLength,
                          (input.sequences[sequence].type) ? MPI_INT : MPI_FLOAT,
                          process,
                          i,
                          MPI_COMM_WORLD,
                          &request);
        }
    }
    return rootData;
}

/**
 * Приём данных последовательностей MPI-процессом
 *
 * @param nSequences - число последовательностей
 * @param sequenceLength – длина последовательностей
 * @param types – типы данных последовательностей
 * @returns данные последовательностей, предназначенные главному MPI-процессу
 */
vector<Sequence> recvData(int nSequences, int sequenceLength, const bool* types) {
    vector<Sequence> sequences;
    for (int i = 0; i < nSequences; ++i) {
        sequences.emplace_back(sequenceLength, types[i]);
        MPI_Recv(sequences[i].data,
                 sequenceLength,
                 (types[i]) ? MPI_INT : MPI_FLOAT,
                 ROOT_RANK,
                 i,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
    }
    return sequences;
}

/**
 * Приём необходимых данных для решения задачи MPI-процессом
 *
 * @returns данные последовательностей
 */
vector<Sequence> getDataFromRoot() {
    int sequenceLength;
    int dataPerProcess;

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
    bool* types = recvTypes(dataPerProcess);
    vector<Sequence> data = recvData(dataPerProcess,sequenceLength, types);
    delete[] types;
    return data;
}

/**
 * Отправка всех необходимых данных для решения задачи всем MPI-процессам
 *
 * @param input – данные последовательностей
 * @param nProcesses – число MPI-процессов
 * @returns данные последовательностей, предназначенные главному MPI-процессу
 */
vector<Sequence> sendDataToProcesses(InputData input, int nProcesses) {
    int *dataPerProcess = input.getDataPerProcess(nProcesses);
    MPI_Bcast(&input.sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    MPI_Scatter(dataPerProcess,
                1,
                MPI_INT,
                nullptr,
                0,
                MPI_INT,
                ROOT_RANK,
                MPI_COMM_WORLD);
    sendTypes(nProcesses, dataPerProcess, input);
    vector<Sequence> sequences = sendData(dataPerProcess, input);

    delete[] dataPerProcess;
    return sequences;
}
