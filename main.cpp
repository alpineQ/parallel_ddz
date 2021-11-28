#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <mpi.h>
#include <iomanip>
#include "DataSharing.h"
#include "CmdParser.h"

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
    vector<Sequence> sequences;

    if (rank == ROOT_RANK) {
        cmd.checkArguments();
        InputData input;
        if (cmd.getMode())
            input.loadFromFile(cmd.getOption("-f"));
        else
            input.generateData(stoi(cmd.getOption("-n")), stoi(cmd.getOption("-m")), true);
        sequences = sendDataToProcesses(input, nProcesses);
    } else
        sequences = getDataFromRoot();

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
        if (rank == ROOT_RANK) {
            double *deltas;
            deltas = safeAllocate<double>(nProcesses);
            MPI_Gather(&delta, 1, MPI_DOUBLE, deltas, nProcesses, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD);
            double max_delta = 0;
            for(int i = 0; i < nProcesses; ++i)
                if (deltas[i] > max_delta)
                    max_delta = deltas[i];
            cout << "Elapsed time: " << setprecision(1) << max_delta << " seconds" << endl;
            delete[] deltas;
        } else
            MPI_Gather(&delta, 1, MPI_DOUBLE, nullptr, 0, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
