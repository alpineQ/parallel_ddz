#include <iostream>
#include <vector>
#include <random>
#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>
#include "CmdParser.h"
#include "InputParser.h"

#define ROOT_RANK 0

using namespace std;

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int sequenceLength; // m
    int nSequences; // n
    int *sendData;

    if (rank == ROOT_RANK) {
        CmdParser cmd(argc, argv);
        string inputFilename;
        bool testingMode;

        if (cmd.optionExists("-n") && cmd.optionExists("-m") && !cmd.optionExists("-f")) {
            testingMode = false;
            nSequences = stoi(cmd.getOption("-n"));
            sequenceLength = stoi(cmd.getOption("-m"));
        } else if (!cmd.optionExists("-n") && !cmd.optionExists("-m") && cmd.optionExists("-f")) {
            testingMode = true;
            inputFilename = cmd.getOption("-f");
        } else {
            cerr << "Invalid arguments" << endl;
            return -1;
        }

        vector<vector<pair<int, float>>> data;
        vector<bool> types;

        if (testingMode) {
            InputParser input(inputFilename);
            nSequences = input.nSequences;
            sequenceLength = input.sequenceLength;
            data = input.data;
            types = input.types;
        } else {
            random_device rd;     // only used once to initialise (seed) engine
            mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
            uniform_int_distribution<int> uni(floor(INT_MIN / 8), floor(INT_MAX / 8)); // guaranteed unbiased
            for (unsigned i = 0; i < nSequences; ++i) {
                data.emplace_back();
                types.push_back(true);
                for (unsigned j = 0; j < sequenceLength; ++j)
                    data[i].push_back(pair(uni(rng), 0.0f));
            }
        }
        cout << (testingMode ? "Testing mode" : "Experiment mode") << endl;
        cout << "Filename: " << inputFilename << endl;
        cout << "Amount of sequences: " << nSequences << endl;
        cout << "Sequence length: " << sequenceLength << endl;
        cout << "Data:" << endl;
        for (unsigned i = 0; i < nSequences; ++i) {
            for (unsigned j = 0; j < sequenceLength; ++j)
                if (types[i])
                    cout << data[i][j].first << " ";
                else
                    cout << data[i][j].second << " ";
            cout << endl;
        }
        sendData = new int[nSequences * sequenceLength];
        for (unsigned i = 0; i < nSequences; ++i)
            for (unsigned j = 0; j < sequenceLength; ++j)
                sendData[i * sequenceLength + j] = data[i][j].first;
    }
    MPI_Bcast(&sequenceLength, 1, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    int *recvData = new int[sequenceLength];
    MPI_Scatter(sendData, sequenceLength, MPI_INT, recvData,
                sequenceLength, MPI_INT, ROOT_RANK, MPI_COMM_WORLD);

    if (rank == ROOT_RANK)
        delete[] sendData;

    cout << "Process " << rank << ": source data is ";
    for (unsigned i = 0; i < sequenceLength; ++i)
        cout << recvData[i] << " ";
    cout << endl;

    omp_set_num_threads(int(log2(sequenceLength)));
    #pragma omp parallel for default(none) firstprivate(sequenceLength) shared(recvData) shared(cout)
    for (unsigned i = 0; i < sequenceLength; ++i) {
        int *Q = new int[sequenceLength];
        for (unsigned j = 0; j < sequenceLength; ++j)
            Q[j] = 0;
        int shift = int(pow(2, i));
        for (unsigned j = 0; j < sequenceLength; ++j)
            Q[j] = (j < shift) ? 0 : recvData[j - shift];
        for (unsigned j = 0; j < sequenceLength; ++j)
            recvData[j] += Q[j];
        delete[] Q;
    }
    cout << "Process " << rank << ": result data is ";
    for (unsigned i = 0; i < sequenceLength; ++i)
        cout << recvData[i] << " ";
    cout << endl;

    delete[] recvData;
    MPI_Finalize();
    return 0;
}
