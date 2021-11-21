#include <iostream>
#include <vector>
#include "CmdParser.h"
#include "InputParser.h"

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


//    cout << (testingMode ? "Testing mode" : "Experiment mode") << endl;
//    cout << "Filename: " << inputFilename << endl;
//    cout << "Amount of sequences: " << nSequences << endl;
//    cout << "Sequence length: " << sequenceLength << endl;


    if (testingMode) {
        InputParser input(inputFilename);
        for (unsigned i = 0; i < input.nSequences; ++i) {
            for (unsigned j = 0; j < input.sequenceLength; ++j)
                if (input.types[i])
                    cout << input.data[i][j].first << " ";
                else
                    cout << input.data[i][j].second << " ";
            cout << endl;
        }
    } else {

    }

    return 0;
}
