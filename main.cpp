#include <iostream>
#include <vector>
#include <random>
#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>
#include "CmdParser.h"
#include "InputParser.h"
#include "SequenceData.h"

#define ROOT_RANK 0

using namespace std;

void generateSequences(SequenceData &data) {
    random_device rd;     // only used once to initialise (seed) engine
    mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    uniform_int_distribution<int> uni(floor(INT_MIN / 8), floor(INT_MAX / 8)); // guaranteed unbiased
    for (unsigned i = 0; i < data.nSequences; ++i) {
        data.sequences.emplace_back();
        data.types.push_back(true);
        for (unsigned j = 0; j < data.sequenceLength; ++j)
            data.sequences[i].push_back(pair(uni(rng), 0.0f));
    }
}

int* prepareData(int argc, char *argv[], int &sequenceLength) {
    CmdParser cmd(argc, argv);
    string inputFilename;
    bool testingMode;
    int *sendData;
    SequenceData data;

    testingMode = cmd.getMode();
    if (testingMode) {
        inputFilename = cmd.getOption("-f");
        data = InputParser(inputFilename).data;
    } else {
        data.nSequences = stoi(cmd.getOption("-n"));
        data.sequenceLength = stoi(cmd.getOption("-m"));
        generateSequences(data);
    }
    sequenceLength = data.sequenceLength;

    cout << (testingMode ? "Testing mode" : "Experiment mode") << endl;
    if (testingMode)
        cout << "Filename: " << inputFilename << endl;
    cout << "Amount of sequences: " << data.nSequences << endl;
    cout << "Sequence length: " << data.sequenceLength << endl;
    cout << "Data:" << endl;
    data.print();
    sendData = new int[data.nSequences * sequenceLength];
    for (unsigned i = 0; i < data.nSequences * data.sequenceLength; ++i)
        sendData[i / data.sequenceLength + i % data.sequenceLength] = data.sequences[i / data.sequenceLength][i % data.sequenceLength].first;
    MPI_Bcast(&sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    int *recvData = new int[sequenceLength];
    MPI_Scatter(sendData, sequenceLength, MPI_INT, recvData,
                sequenceLength, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    delete[] sendData;
    return recvData;
}

int* getDataFromRoot(int &sequenceLength) {
    int *recvData;
    MPI_Bcast(&sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    recvData = new int[sequenceLength];
    MPI_Scatter(nullptr, sequenceLength, MPI_INT, recvData,
                sequenceLength, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);
    return recvData;
}

void partialSum(int* data, int sequenceLength) {
    omp_set_num_threads(int(log2(sequenceLength)));
    #pragma omp parallel for default(none) firstprivate(sequenceLength) shared(data) shared(cout)
    for (unsigned i = 0; i < sequenceLength; ++i) {
        int *Q = new int[sequenceLength];
        for (unsigned j = 0; j < sequenceLength; ++j)
            Q[j] = 0;
        int shift = int(pow(2, i));
        for (unsigned j = 0; j < sequenceLength; ++j)
            Q[j] = (j < shift) ? 0 : data[j - shift];
        for (unsigned j = 0; j < sequenceLength; ++j)
            data[j] += Q[j];
        delete[] Q;
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    int sequenceLength;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int *data;

    if (rank == ROOT_RANK)
        data = prepareData(argc, argv, sequenceLength);
    else
        data = getDataFromRoot(sequenceLength);


    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << ": source sequences is ";
    for (unsigned i = 0; i < sequenceLength; ++i)
        cout << data[i] << " ";
    cout << endl;

    partialSum(data, sequenceLength);

    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Process " << rank << ": result sequences is ";
    for (unsigned i = 0; i < sequenceLength; ++i)
        cout << data[i] << " ";
    cout << endl;

    delete[] data;
    MPI_Finalize();
    return 0;
}
