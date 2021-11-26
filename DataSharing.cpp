#include <mpi.h>
#include "DataSharing.h"
#include "InputData.h"

#define ROOT_RANK 0
#define DATA_SEND_TAG 0


void sendTypes(int nProcesses, int* dataPerProcess, InputData input) {
    bool *types = new bool[nProcesses];
    int *displacements = new int[nProcesses];
    for (unsigned i = 0; i < nProcesses; ++i) {
        types[i] = input.sequences[i].type;
        displacements[i] = (i == 0) ? 0 : displacements[i - 1] + dataPerProcess[i];
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

bool* recvTypes(int nSequences) {
    bool *types = new bool[nSequences];
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

void sendData(int nProcesses, const int* dataPerProcess, InputData input) {
    for (int process = 0, currentSequence = 0; process < nProcesses; ++process) {
        if (process == ROOT_RANK)
            continue;
        MPI_Request request;
        for (int j = 0; j < dataPerProcess[process]; ++j) {
            MPI_Isend(input.sequences[currentSequence].data,
                      input.sequenceLength,
                      (input.sequences[currentSequence].type) ? MPI_INT : MPI_FLOAT,
                      process,
                      DATA_SEND_TAG,
                      MPI_COMM_WORLD,
                      &request);
            ++currentSequence;
        }
    }
}

vector<Sequence> recvData(int nSequences, int sequenceLength, const bool* types) {
    vector<Sequence> sequences;
    for (int i = 0; i < nSequences; ++i) {
        MPI_Status status;
        MPI_Recv(sequences[i].data,
                 sequenceLength,
                 (types[i]) ? MPI_INT : MPI_FLOAT,
                 ROOT_RANK,
                 DATA_SEND_TAG,
                 MPI_COMM_WORLD,
                 &status);
    }
    return sequences;
}

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
    sendData(nProcesses, dataPerProcess, input);
    vector<Sequence> sequences;
    for (unsigned i = 0; i < dataPerProcess[ROOT_RANK]; ++i)
        sequences.push_back(sequences[i * nProcesses]);
    delete[] dataPerProcess;
    return sequences;
}
