#ifndef PARALLEL_DDZ_CPP_CMDPARSER_H
#define PARALLEL_DDZ_CPP_CMDPARSER_H

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class CmdParser {
private:
    vector<string> tokens;
public:
    CmdParser(int &argc, char **argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.emplace_back(argv[i]);
    }

    const string &getCmdOption(const string &option) const {
        auto itr = find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const string empty_string;
        return empty_string;
    }

    bool cmdOptionExists(const string &option) const {
        return find(this->tokens.begin(), this->tokens.end(), option)
               != this->tokens.end();
    }
};

#endif //PARALLEL_DDZ_CPP_CMDPARSER_H
