#include <iostream>
#include <vector>
#include "CmdParser.h"
#include "InputParser.h"
#include <mpi.h>
#include <omp.h>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    CmdParser cmd(argc, argv);
    string inputFilename;
    bool testingMode;
    size_t nSequences; // n
    size_t sequenceLength; // m

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

    }
    cout << (testingMode ? "Testing mode" : "Experiment mode") << endl;
    cout << "Filename: " << inputFilename << endl;
    cout << "Amount of sequences: " << nSequences << endl;
    cout << "Sequence length: " << sequenceLength << endl;

    for (unsigned i = 0; i < nSequences; ++i) {
        for (unsigned j = 0; j < sequenceLength; ++j)
            if (types[i])
                cout << data[i][j].first << " ";
            else
                cout << data[i][j].second << " ";
        cout << endl;
    }
    MPI_Finalize();

    return 0;
}
