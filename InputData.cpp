#include <iostream>
#include <fstream>
#include "InputData.h"

using namespace std;

void InputData::generateData(int n, int m, bool type) {
    this->nSequences = n;
    this->sequenceLength = m;
    for (unsigned i = 0; i < n; ++i) {
        sequences.emplace_back(m, type);
    }
    for (unsigned i = 0; i < n; ++i)
        sequences[i].generate();
}

void InputData::loadFromFile(const string &filename) {
    ifstream inputFile(filename);
    if (!inputFile.is_open())
        throw runtime_error("Unable to open file '" + filename + "'");
    inputFile >> nSequences;
    inputFile >> sequenceLength;
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
            if (sequences[i].type)
                inputFile >> ((int *) sequences[i].data)[j];
            else
                inputFile >> ((float *) sequences[i].data)[j];
        }
    inputFile.close();

}

void InputData::print() {
    cout << "Amount of sequences: " << nSequences << endl;
    cout << "Sequence length: " << sequenceLength << endl;
    cout << "Data:" << endl;
    for (unsigned i = 0; i < nSequences; ++i) {
        sequences[i].print();
        cout << endl;
    }
}
