/**
 * Sequence.h
 *
 * Заголовочный файл класса Sequence
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#ifndef PARTIAL_SUM_SEQUENCE_H
#define PARTIAL_SUM_SEQUENCE_H

#include <iostream>
#include <mpi.h>


class Sequence {
public:
    float *data = nullptr;
    int length = 0;

    void free() const;
    explicit Sequence(int length);
    void generate() const;
    void print() const;
    Sequence shiftRight(int shift) const;
    Sequence operator+=(const Sequence& other);
};

template <typename T>
T* safeAllocate(unsigned length) {
    T* data;
    try {
        data = new T[length];
    } catch (std::bad_alloc &error) {
        std::cerr << "Memory allocation error" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, -6);
        MPI_Finalize();
        exit(-1);
    }
    return data;
}


#endif //PARTIAL_SUM_SEQUENCE_H
