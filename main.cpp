#include <iostream>
#include "CmdParser.h"

using namespace std;

int main(int argc, char* argv[]) {
    CmdParser input(argc, argv);
    string inputFilename;
    size_t nSequences; // n
    size_t sequenceLength; // m
    if(input.cmdOptionExists("-n")){
        stoi(input.getCmdOption("-n"), &nSequences);
    } else {
        cout << "No amount of sequences parameter" << endl;
        return -1;
    }
    if(input.cmdOptionExists("-m")){
        stoi(input.getCmdOption("-m"), &sequenceLength);
    } else {
        cout << "No sequence length parameter" << endl;
        return -1;
    }
    if(input.cmdOptionExists("-f")){
        inputFilename = input.getCmdOption("-f");
    }

    return 0;
}
