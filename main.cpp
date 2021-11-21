#include <iostream>
#include <fstream>
#include <vector>
#include "CmdParser.h"

using namespace std;

int main(int argc, char* argv[]) {
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

    ifstream inputFile(inputFilename);
    inputFile >> nSequences;
    inputFile >> sequenceLength;

    cout << (testingMode ? "Testing mode" : "Experiment mode") << endl;
    cout << "Filename: " << inputFilename << endl;
    cout << "Amount of sequences: " << nSequences << endl;
    cout << "Sequence length: " << sequenceLength << endl;
    return 0;
}
