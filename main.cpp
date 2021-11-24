#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <bits/stdc++.h>
#include "CmdParser.h"
#include "InputData.h"

#define ROOT_RANK 0

using namespace std;

Sequence prepareData(int argc, char *argv[]) {
    CmdParser cmd(argc, argv);
    string inputFilename;
    bool testingMode;
    InputData data;

    try {
        testingMode = cmd.getMode();
    } catch (invalid_argument &error) {
        cerr << error.what();
        MPI_Finalize();
        exit(-1);
    }

    if (testingMode) {
        inputFilename = cmd.getOption("-f");
        data.loadFromFile(inputFilename);
    } else
        data.generateData(stoi(cmd.getOption("-n")),
                          stoi(cmd.getOption("-m")),
                          true);

    cout << (testingMode ? "Testing" : "Experiment") << " mode" << endl;
    if (testingMode)
        cout << "Filename: " << inputFilename << endl;
    data.print();

    MPI_Bcast(&data.sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    bool* types = new bool[data.nSequences];
    for (unsigned i = 0; i < data.nSequences; ++i)
        types[i] = data.sequences[i].type;
    MPI_Scatter(types,
                data.nSequences,
                MPI_CXX_BOOL,
                &data.sequences[0].type,
                1,
                MPI_CXX_BOOL,
                ROOT_RANK,
                MPI_COMM_WORLD);
    delete[] types;
    for (int i = 1; i < data.nSequences; ++i) {
        MPI_Request request;
        MPI_Isend(data.sequences[i].data,
                  data.sequenceLength,
                  (data.sequences[i].type)?MPI_INT:MPI_FLOAT,
                  i,
                  0,
                  MPI_COMM_WORLD,
                  &request);
    }
    return data.sequences[0];
}

Sequence getDataFromRoot() {
    int sequenceLength;
    bool dataType;

    MPI_Bcast(&sequenceLength,
              1,
              MPI_INT,
              ROOT_RANK,
              MPI_COMM_WORLD);
    MPI_Scatter(nullptr,
                0,
                MPI_CXX_BOOL,
                &dataType,
                1,
                MPI_CXX_BOOL,
                ROOT_RANK,
                MPI_COMM_WORLD);
    Sequence sequence(sequenceLength, dataType);

    MPI_Status status;
    MPI_Recv(sequence.data,
             sequenceLength,
             (dataType)?MPI_INT:MPI_FLOAT, ROOT_RANK,
             0,
             MPI_COMM_WORLD,
             &status);
    return sequence;
}

void partialSum(Sequence sequence) {
    omp_set_num_threads(int(log2(sequence.length)));
    #pragma omp parallel for default(none) shared(sequence)
    for (unsigned i = 0; i < sequence.length; ++i) {
        int shift = int(pow(2, i));
        Sequence Q = sequence.shiftRight(shift);
        sequence += Q;
        Q.free();
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    Sequence sequence;

    if (rank == ROOT_RANK)
        sequence = prepareData(argc, argv);
    else
        sequence = getDataFromRoot();

    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << " (input): ";
    sequence.print();
    cout << endl;

    partialSum(sequence);

    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << " (output): ";
    sequence.print();
    cout << endl;

    sequence.free();
    MPI_Finalize();
    return 0;
}
