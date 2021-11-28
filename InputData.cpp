#include <iostream>
#include <fstream>
#include <stdexcept>
#include <mpi.h>
#include <cstring>
#include "InputData.h"

using namespace std;

void InputData::generateData(int n, int m, bool type) {
    this->nSequences = n;
    this->sequenceLength = m;
    for (unsigned i = 0; i < n; ++i) {
        sequences.emplace_back(m, type);
        sequences[i].generate();
    }
}

void InputData::loadFromFile(const string &filename) {
    ifstream inputFile(filename);
    if (inputFile.fail()) {
        cerr << "File open error: " << strerror(errno) << endl;
        MPI_Abort(MPI_COMM_WORLD, -4);
        MPI_Finalize();
        exit(-4);
    }
    inputFile >> sequenceLength;
    inputFile >> nSequences;
    sequences.reserve(nSequences);

    auto dataPosition = inputFile.tellg();

    for (std::string line; std::getline(inputFile, line);) {
        if (line.empty())
            continue;
        sequences.emplace_back(sequenceLength, line.find(',') == -1 && line.find('.') == -1);
    }
    inputFile.clear();
    inputFile.seekg(dataPosition);

    for (unsigned i = 0; i < nSequences; ++i)
        for (unsigned j = 0; j < sequenceLength; ++j) {
            string value;
            inputFile >> value;
            try {
                for (char c: value)
                    if (!isdigit(c) && c != '-' && (sequences[i].type || c != ',' && c != '.' && !sequences[i].type))
                        throw invalid_argument("Invalid symbol");
                if (sequences[i].type)
                    ((int *) sequences[i].data)[j] = stoi(value);
                else
                    ((float *) sequences[i].data)[j] = stof(value);
            } catch (invalid_argument &error) {
                cerr << "Invalid data format: " << i + 1 << " " << j + 1 << endl;
                inputFile.close();
                MPI_Abort(MPI_COMM_WORLD, -7);
                MPI_Finalize();
                exit(-7);
            }
        }
    inputFile.close();
}
int* InputData::getDataPerProcess(int nProcesses) const {
    int *dataPerProcess = safeAllocate<int>(nProcesses);
	for (unsigned i = 0; i < nProcesses; ++i)
		if (nSequences % nProcesses == 0)
			dataPerProcess[i] = nSequences / nProcesses;
		else
			dataPerProcess[i] = nSequences / nProcesses + ((i < nSequences % nProcesses) ? 1 : 0);
	return dataPerProcess;
}
