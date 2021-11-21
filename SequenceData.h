#ifndef PARALLEL_DDZ_SEQUENCE_DATA_H
#define PARALLEL_DDZ_SEQUENCE_DATA_H

#include <vector>

using namespace std;

class SequenceData {
public:
    vector<vector<pair<int, float>>> sequences;
    vector<bool> types; // true = int, false = float
    int nSequences{}; // n
    int sequenceLength{}; // m
    void print() {
        for (unsigned i = 0; i < nSequences; ++i) {
            for (unsigned j = 0; j < sequenceLength; ++j)
                if (types[i])
                    cout << sequences[i][j].first << " ";
                else
                    cout << sequences[i][j].second << " ";
            cout << endl;
        }
    }
};


#endif //PARALLEL_DDZ_SEQUENCE_DATA_H
