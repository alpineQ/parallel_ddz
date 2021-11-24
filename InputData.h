#ifndef PARTIAL_SUM_INPUTDATA_H
#define PARTIAL_SUM_INPUTDATA_H

#include <vector>
#include "Sequence.h"

using namespace std;

class InputData {
public:
    vector<Sequence> sequences;
    int sequenceLength{}; // m
    int nSequences{}; // n

    void loadFromFile(const string &filename);
    void generateData(int n, int m, bool type);
    void print();
};



#endif //PARTIAL_SUM_INPUTDATA_H
