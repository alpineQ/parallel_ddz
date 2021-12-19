/**
 * CmdParser.h
 *
 * Вспомогательный класс CmdParser, упрощающий работу с
 * аргументами командной строки, вводимых пользователем
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#ifndef PARALLEL_DDZ_CPP_CMD_PARSER_H
#define PARALLEL_DDZ_CPP_CMD_PARSER_H

#include <iostream>
#include <unordered_map>
#include <mpi.h>

using namespace std;

class CmdParser {
    unordered_map<string, string> tokens;
public:
    /**
     * Последовательный разбор аргументов командной
     * строки в словарь tokens
     * @param argc – количество аргументов командной строки
     * @param argv – значения аргументов командной строки
     */
    CmdParser(int argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            const string key = argv[i];
            const string value = argv[++i];
            tokens[key] = value;
        }
    }

    /**
     * Получение значение параметра option
     * @param option – имя параметра
     * @returns Значение параметра option
     */
    string getOption(const string &option) const {
        return tokens.at(option);
    }

    /**
     * Проверка наличия параметра option
     * @param option – имя параметра
     * @returns: true - аргумент присутсвует
     *           false - аргумент отсутсвует
     */
    bool optionExists(const string &option) const {
        return tokens.find(option) != tokens.end();
    }

    /**
     * Получение режима работы программы
     * @returns: true - режим тестирования
     *           false - режим вычислительного эксперимента
     */
    bool isTestingMode() const {
        return !optionExists("-n") && !optionExists("-m") && optionExists("-f");
    }

    /**
     * Проверка наличия обязательных аргументов
     * @returns Код ошибки, в случае наличия всех аргументов возвращает 0
     */
    int checkArguments() const {
        int n = -1;
        int m = -1;
        if (optionExists("-n") && optionExists("-m")) {
            for (char c: getOption("-n"))
                if (!isdigit(c)) {
                    cerr << "Invalid sequence length" << endl;
                    return -2;
                }
            for (char c: getOption("-m"))
                if (!isdigit(c)) {
                    cerr << "Invalid amount of sequences" << endl;
                    return -3;
                }
            n = stoi(getOption("-n"));
            m = stoi(getOption("-m"));
        }
        if ((n < 0 || m < 0) && !optionExists("-f")) {
            cerr << "Invalid parameters" << endl;
            return -1;
        }
        return 0;
    }
};

#endif //PARALLEL_DDZ_CPP_CMD_PARSER_H
