#include <mpi.h>
#include "DataSharing.h"
#include "InputData.h"

#define ROOT_RANK 0


void sendTypes(int nProcesses, int* dataPerProcess, InputData input) {
    bool *types = new bool[nProcesses];
    int *displacements = new int[nProcesses];
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

vector<Sequence> sendData(const int* dataPerProcess, InputData input) {
    vector<Sequence> rootData;
    for (int process = 0, sequence = 0; sequence < input.nSequences; ++process) {
        MPI_Request request;
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

vector<Sequence> recvData(int nSequences, int sequenceLength, const bool* types) {
    vector<Sequence> sequences;
    for (int i = 0; i < nSequences; ++i) {
        Sequence sequence(sequenceLength, types[i]);
        MPI_Status status;
        MPI_Recv(sequence.data,
                 sequenceLength,
                 (types[i]) ? MPI_INT : MPI_FLOAT,
                 ROOT_RANK,
                 i,
                 MPI_COMM_WORLD,
                 &status);
        sequences.push_back(sequence);
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
    vector<Sequence> sequences = sendData(dataPerProcess, input);

    delete[] dataPerProcess;
    return sequences;
}
