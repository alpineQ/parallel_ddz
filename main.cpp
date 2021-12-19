/**
 * main.cpp
 *
 * Программная реализация параллельного алгоритма нахождения
 * частных сумм
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <mpi.h>
#include "DataSharing.h"
#include "CmdParser.h"

#define ROOT_RANK 0

using namespace std;
using namespace chrono;

/**
 * Вычисление частных сумм последовательности sequence
 *
 * @param sequence – заданная последовательность
 * @param sequenceLength – длина последовательности
 * @note В переменную sequence записывается результат вычисления
 * частных сумм
 */
void partialSum(float* sequence, int sequenceLength) {
    unsigned nIterations = floor(log2(sequenceLength));
    #pragma omp parallel for default(none) firstprivate(nIterations, sequenceLength) shared(sequence)
    for (unsigned i = 0; i <= nIterations; ++i) {
        int shift = int(pow(2, i));
        for (unsigned j = 0; j < sequenceLength; ++j)
            #pragma omp atomic
            sequence[j] += (j < shift) ? 0 : sequence[j - shift];
    }
}

/**
 * Точка входа в программу
 *
 * @param argc количество аргументов командной строки
 * @param argv значения аргументов командной строки
 * @return код возврата (ошибки)
 */
int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    int nProcesses;
    int nSequences;
    int sequenceLength;
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    CmdParser cmd(argc, argv);
    float** sequences;

    int err;
    if (rank == ROOT_RANK) {
        err = cmd.checkArguments();
        if (err != 0) {
            MPI_Bcast(&err, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
            MPI_Finalize();
            return err;
        }
        if (cmd.isTestingMode()) {
            err = loadFromFile(cmd.getOption("-f"), sequences, nSequences, sequenceLength);
            if (err != 0) {
                MPI_Bcast(&err, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
                MPI_Finalize();
                return err;
            }
        } else {
            sequenceLength = stoi(cmd.getOption("-n"));
            nSequences = stoi(cmd.getOption("-m"));
            err = generateData(sequenceLength, nSequences, sequences);
            if (err != 0) {
                MPI_Bcast(&err, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
                MPI_Finalize();
                return err;
            }
        }
        MPI_Bcast(&err, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
        sendDataToProcesses(sequences, nSequences, sequenceLength, nProcesses);
    } else {
        MPI_Bcast(&err, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
        if (err != 0) {
            MPI_Finalize();
            return err;
        }
        getDataFromRoot(sequences, nSequences, sequenceLength);
    }


    steady_clock::time_point begin, end;

    begin = steady_clock::now();
    for (int i = 0; i < nSequences; ++i)
        partialSum(sequences[i], sequenceLength);
    end = steady_clock::now();

    for (unsigned process = 0; process < nProcesses; ++process) {
        if (rank == process)
            for (int i = 0; i < nSequences; ++i) {
                if (cmd.isTestingMode()) {
                    for (unsigned j = 0; j < sequenceLength; ++j)
                        cout << sequences[i][j] << " ";
                    cout << endl;
                }
            }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (!cmd.isTestingMode()) {
        unsigned sec = duration_cast<seconds>(end - begin).count();
        unsigned ms = duration_cast<milliseconds>(end - begin).count();
        double delta = sec + double(ms) / 1000;
        double maxDelta;
        MPI_Reduce(&delta, &maxDelta, 1, MPI_DOUBLE,
                   MPI_MAX, ROOT_RANK, MPI_COMM_WORLD);
        cout << "Elapsed time: " << maxDelta << " seconds" << endl;
    }

    delete[] sequences;
    MPI_Finalize();
    return 0;
}
