#ifndef PARALLEL_DDZ_CPP_CMD_PARSER_H
#define PARALLEL_DDZ_CPP_CMD_PARSER_H

#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

class CmdParser {
    unordered_map<string, string> tokens;
public:
    CmdParser(int argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            if (argv[i][0] == '-') {
                const string key = argv[i];
                const string value = argv[++i];
                tokens[key] = value;
            }
            else {
                throw invalid_argument(argv[i]);
            }
    }

    string getOption(const string& option) const {
        return tokens.at(option);
    }

    bool optionExists(const string& option) const {
        return tokens.find(option) != tokens.end();
    }

    bool getMode() const {
        if (optionExists("-n") && optionExists("-m") && !optionExists("-f")) {
            return false;
        }
        if (!optionExists("-n") && !optionExists("-m") && optionExists("-f")) {
            return true;
        }
        cerr << "Invalid arguments" << endl;
        exit(-1);
    }
};

#endif //PARALLEL_DDZ_CPP_CMD_PARSER_H
