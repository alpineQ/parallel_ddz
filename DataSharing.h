#ifndef PARTIAL_SUM_DATA_SHARING_H
#define PARTIAL_SUM_DATA_SHARING_H

#include "InputData.h"

vector<Sequence> sendDataToProcesses(InputData input, int nProcesses);
vector<Sequence> getDataFromRoot();

#endif //PARTIAL_SUM_DATA_SHARING_H
