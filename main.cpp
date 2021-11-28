#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <mpi.h>
#include "CmdParser.h"
#include "InputData.h"
#include "DataSharing.h"

#define ROOT_RANK 0

using namespace std;
using namespace chrono;

void partialSum(Sequence sequence) {
#pragma omp parallel for default(none) shared(sequence)
    for (unsigned i = 0; i < sequence.length; ++i) {
        int shift = int(pow(2, i));
        Sequence Q = sequence.shiftRight(shift);
        sequence += Q;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    int nProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    CmdParser cmd(argc, argv);
    bool testingMode;
    try {
        testingMode = cmd.getMode();
    } catch (invalid_argument &error) {
        cerr << error.what();
        MPI_Finalize();
        exit(-1);
    }


    vector<Sequence> sequences;

    if (rank == ROOT_RANK) {
        InputData input;
        if (testingMode)
            input.loadFromFile(cmd.getOption("-f"));
        else
            input.generateData(stoi(cmd.getOption("-n")),
                               stoi(cmd.getOption("-m")),
                               true);
        sequences = sendDataToProcesses(input, nProcesses);
    } else
        sequences = getDataFromRoot();

    steady_clock::time_point begin = steady_clock::now();
    for (const Sequence &sequence: sequences)
        partialSum(sequence);
    steady_clock::time_point end = steady_clock::now();

    for (unsigned i = 0; i < nProcesses; ++i) {
        if (rank == i)
            for (Sequence sequence: sequences) {
                if (testingMode)
                    sequence.print();
                sequence.free();
            }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (!testingMode) {
        unsigned s = duration_cast<seconds>(end - begin).count();
        unsigned m = duration_cast<milliseconds>(end - begin).count();
        double delta = s + double(m) / 1000;
        if (rank == ROOT_RANK) {
            auto deltas = new double[nProcesses];
            MPI_Gather(&delta, 1, MPI_DOUBLE, deltas, nProcesses, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD);
            double max_delta = 0;
            for(int i = 0; i < nProcesses; ++i)
                if (deltas[i] > max_delta)
                    max_delta = deltas[i];
            cout << "Elapsed time: " << max_delta << " seconds" << endl;
            delete[] deltas;
        } else
            MPI_Gather(&delta, 1, MPI_DOUBLE, nullptr, 0, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
