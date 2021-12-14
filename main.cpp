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
 * @note В переменную sequence записывается результат вычисления
 * частных сумм
 */
void partialSum(Sequence sequence) {
    unsigned nIterations = floor(log2(sequence.length));
    #pragma omp parallel for default(none) firstprivate(nIterations) shared(sequence)
    for (unsigned i = 0; i <= nIterations; ++i) {
        int shift = int(pow(2, i));
        Sequence Q = sequence.shiftRight(shift);
        sequence += Q;
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
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    CmdParser cmd(argc, argv);
    vector<Sequence> sequences;

    int flag;
    if (rank == ROOT_RANK) {
        flag = cmd.checkArguments();
        if (flag != 0) {
            MPI_Bcast(&flag, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
            MPI_Finalize();
            return flag;
        }
        InputData input;
        if (cmd.getMode()) {
            flag = input.loadFromFile(cmd.getOption("-f"));
            if (flag != 0) {
                MPI_Bcast(&flag, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
                MPI_Finalize();
                return flag;
            }
        } else
            input.generateData(stoi(cmd.getOption("-n")),
                               stoi(cmd.getOption("-m")));
        MPI_Bcast(&flag, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
        sequences = sendDataToProcesses(input, nProcesses);
    } else {
        MPI_Bcast(&flag, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
        if (flag != 0) {
            MPI_Finalize();
            return flag;
        }
        sequences = getDataFromRoot();
    }


    steady_clock::time_point begin, end;

    begin = steady_clock::now();
    for (const Sequence &sequence: sequences)
        partialSum(sequence);
    end = steady_clock::now();

    for (unsigned i = 0; i < nProcesses; ++i) {
        if (rank == i)
            for (Sequence sequence: sequences) {
                if (cmd.getMode())
                    sequence.print();
                sequence.free();
            }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (!cmd.getMode()) {
        unsigned sec = duration_cast<seconds>(end - begin).count();
        unsigned ms = duration_cast<milliseconds>(end - begin).count();
        double delta = sec + double(ms) / 1000;
        double maxDelta;
        MPI_Reduce(&delta, &maxDelta, 1, MPI_DOUBLE,
                   MPI_MAX, ROOT_RANK, MPI_COMM_WORLD);
        cout << "Elapsed time: " << maxDelta << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
