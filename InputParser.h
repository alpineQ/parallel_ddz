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
    size_t nSequences; // n
    size_t sequenceLength; // m

    InputParser(const string& filename) {
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
        inputFile.seekg(dataPosition, std::ifstream::beg);

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

private:
    static bool isFloatNumber(const string& input){
        std::string::const_iterator it = input.begin();
        bool decimalPoint = false;
        int minSize = 0;
        if(!input.empty() && (input[0] == '-' || input[0] == '+')){
            it++;
            minSize++;
        }
        while(it != input.end()){
            if(*it == '.'){
                if(!decimalPoint) decimalPoint = true;
                else break;
            }else if(!std::isdigit(*it) && ((*it!='f') || it+1 != input.end() || !decimalPoint)){
                break;
            }
            ++it;
        }
        return input.size() > minSize && it == input.end();
    }
};


#endif //PARALLEL_DDZ_CPP_INPUTPARSER_H
