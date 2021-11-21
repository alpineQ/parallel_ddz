#ifndef PARALLEL_DDZ_CPP_INPUTPARSER_H
#define PARALLEL_DDZ_CPP_INPUTPARSER_H

#include <string>
#include <fstream>

using namespace std;

class InputParser {
    ifstream inputFile;
    InputParser(const string& filename, size_t n, size_t m) {
        inputFile = ifstream(filename);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                string value;
                inputFile >> value;
            }
        }
    }

private:
    bool isFloatNumber(const string& input){
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
