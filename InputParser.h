#ifndef PARALLEL_DDZ_CPP_INPUTPARSER_H
#define PARALLEL_DDZ_CPP_INPUTPARSER_H

#include <string>
#include <fstream>
#include <vector>

using namespace std;

class InputParser {
public:
    vector<vector<pair<int, float>>> data;
    vector<bool> types; // true = int, false = float
    size_t nSequences{}; // n
    size_t sequenceLength{}; // m

    explicit InputParser(const string& filename) {
        ifstream inputFile(filename);
        if (!inputFile.is_open())
            throw runtime_error("Unable to open file '" + filename +"'");
        inputFile >> nSequences;
        inputFile >> sequenceLength;
        data.reserve(nSequences);
        types.reserve(nSequences);

        auto dataPosition = inputFile.tellg();

        for (std::string line; std::getline(inputFile, line); ) {
            if (line.empty())
                continue;
            types.push_back(line.find(',') == -1 && line.find('.') == -1);
            data.emplace_back();
        }
        inputFile.clear();
        inputFile.seekg(dataPosition);

        for (unsigned i = 0; i < nSequences; ++i)
            for (unsigned j = 0; j < sequenceLength; ++j) {
                pair<int, float> value;
                if (types[i])
                    inputFile >> value.first;
                else
                    inputFile >> value.second;
                data[i].push_back(value);
            }
        inputFile.close();
    }
};


#endif //PARALLEL_DDZ_CPP_INPUTPARSER_H
