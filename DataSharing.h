/*–––––––––––––––––––––––––––––––––––––----––––––––––––––––––*/
/* InputData.h                                               */
/*                                                           */
/* Заголовочный файл файла вспомогательных функций передачи  */
/* данных между MPI-процессами                               */
/* Автор: Казанцев М.А., 2021 г.                             */
/*                                                           */
/* –––––––––––––––––––––––––––––––––––––---––––––––––––––––––*/
#ifndef PARTIAL_SUM_DATA_SHARING_H
#define PARTIAL_SUM_DATA_SHARING_H

#include "InputData.h"

vector<Sequence> sendDataToProcesses(InputData input, int nProcesses);
vector<Sequence> getDataFromRoot();

#endif //PARTIAL_SUM_DATA_SHARING_H
