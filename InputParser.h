#ifndef PARALLEL_DDZ_CPP_INPUTPARSER_H
#define PARALLEL_DDZ_CPP_INPUTPARSER_H

#include <string>
#include <fstream>
#include <vector>
#include "SequenceData.h"

using namespace std;

class InputParser {
public:
    SequenceData data;
    explicit InputParser(const string& filename) {
        ifstream inputFile(filename);
        if (!inputFile.is_open())
            throw runtime_error("Unable to open file '" + filename +"'");
        inputFile >> data.nSequences;
        inputFile >> data.sequenceLength;
        data.sequences.reserve(data.nSequences);
        data.types.reserve(data.nSequences);

        auto dataPosition = inputFile.tellg();

        for (std::string line; std::getline(inputFile, line); ) {
            if (line.empty())
                continue;
            data.types.push_back(line.find(',') == -1 && line.find('.') == -1);
            data.sequences.emplace_back();
        }
        inputFile.clear();
        inputFile.seekg(dataPosition);

        for (unsigned i = 0; i < data.nSequences; ++i)
            for (unsigned j = 0; j < data.sequenceLength; ++j) {
                pair<int, float> value;
                if (data.types[i])
                    inputFile >> value.first;
                else
                    inputFile >> value.second;
                data.sequences[i].push_back(value);
            }
        inputFile.close();
    }
};


#endif //PARALLEL_DDZ_CPP_INPUTPARSER_H
