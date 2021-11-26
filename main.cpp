#include <iostream>
#include <vector>
#include <mpi.h>
#include <omp.h>
#include <bits/stdc++.h>
#include "CmdParser.h"
#include "InputData.h"
#include "DataSharing.h"

#define ROOT_RANK 0

using namespace std;

vector<Sequence> prepareData(int argc, char *argv[]) {
    CmdParser cmd(argc, argv);
    bool testingMode;
    try {
        testingMode = cmd.getMode();
    } catch (invalid_argument &error) {
        cerr << error.what();
        MPI_Finalize();
        exit(-1);
    }

    InputData input;
    string inputFilename;
    if (testingMode) {
        inputFilename = cmd.getOption("-f");
        input.loadFromFile(inputFilename);
    } else
        input.generateData(stoi(cmd.getOption("-n")),
                           stoi(cmd.getOption("-m")),
                           true);

    cout << (testingMode ? "Testing" : "Experiment") << " mode" << endl;
    if (testingMode)
        cout << "Filename: " << inputFilename << endl;
    input.print();

    int nProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    vector<Sequence> sequences = sendDataToProcesses(input, nProcesses);

    return sequences;
}

void partialSum(Sequence sequence) {
    omp_set_num_threads(int(log2(sequence.length)));
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
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    vector<Sequence> sequences;

    if (rank == ROOT_RANK)
        sequences = prepareData(argc, argv);
    else
        sequences = getDataFromRoot();

    for (const Sequence &sequence: sequences) {
        cout << "Process " << rank << " (input): ";
        sequence.print();
    }

    for (const Sequence &sequence: sequences)
        partialSum(sequence);

    // MPI_Barrier(MPI_COMM_WORLD);
    for (const Sequence &sequence: sequences) {
        cout << "Process " << rank << " (output): ";
        sequence.print();
    }
    MPI_Finalize();
    return 0;
}
