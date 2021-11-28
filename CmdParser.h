#ifndef PARALLEL_DDZ_CPP_CMD_PARSER_H
#define PARALLEL_DDZ_CPP_CMD_PARSER_H

#include <iostream>
#include <unordered_map>
#include <mpi.h>

using namespace std;

class CmdParser {
    unordered_map<string, string> tokens;
public:
    CmdParser(int argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            const string key = argv[i];
            const string value = argv[++i];
            tokens[key] = value;
        }
    }

    string getOption(const string& option) const {
        return tokens.at(option);
    }

    bool optionExists(const string& option) const {
        return tokens.find(option) != tokens.end();
    }

    bool getMode() const {
        if (optionExists("-n") && optionExists("-m") && !optionExists("-f"))
            return false;
        return true;
    }
    void checkArguments() const {
        int n = -1;
        int m = -1;
        if (optionExists("-n") && optionExists("-m")) {
            for (char c: getOption("-n"))
                if (!isdigit(c)) {
                    cerr << "Invalid sequence length" << endl;
                    MPI_Abort(MPI_COMM_WORLD, -2);
                    MPI_Finalize();
                    exit(-2);
                }
            for (char c: getOption("-m"))
                if (!isdigit(c)) {
                    cerr << "Invalid amount of sequences" << endl;
                    MPI_Abort(MPI_COMM_WORLD, -3);
                    MPI_Finalize();
                    exit(-3);
                }
            n = stoi(getOption("-n"));
            m = stoi(getOption("-m"));
        }
        if ((n < 0 || m < 0 ) && !optionExists("-f")) {
            cerr << "Invalid parameters" << endl;
            MPI_Abort(MPI_COMM_WORLD, -1);
            MPI_Finalize();
            exit(-1);
        }
    }
};

#endif //PARALLEL_DDZ_CPP_CMD_PARSER_H
