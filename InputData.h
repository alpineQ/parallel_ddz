/*–––––––––––––––––––––––––––––––––––––----––––––––––––––––––*/
/* InputData.h                                               */
/*                                                           */
/* Заголовочный файл класса InputData                        */
/* Автор: Казанцев М.А., 2021 г.                             */
/*                                                           */
/* –––––––––––––––––––––––––––––––––––––---––––––––––––––––––*/
#ifndef PARTIAL_SUM_INPUT_DATA_H
#define PARTIAL_SUM_INPUT_DATA_H

#include <vector>
#include <string>
#include "Sequence.h"

using namespace std;

class InputData {
public:
    vector<Sequence> sequences;
    int sequenceLength{}; // m
    int nSequences{}; // n
	
    void loadFromFile(const string &filename);
    void generateData(int n, int m, bool type);
	int* getDataPerProcess(int nProcesses) const;
};



#endif //PARTIAL_SUM_INPUT_DATA_H
