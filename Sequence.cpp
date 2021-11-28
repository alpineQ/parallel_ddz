#include <random>
#include <bits/stdc++.h>
#include "Sequence.h"

using namespace std;

Sequence::Sequence(int length, bool type) {
    this->length = length;
    this->type = type;
    if (type)
        data = safeAllocate<int>(length);
    else
        data = safeAllocate<float>(length);
}

void Sequence::free() const {
    if (type)
        delete[] (int *) data;
    else
        delete[] (float *) data;
}

void Sequence::generate() const {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> uni(floor(INT_MIN / 8), floor(INT_MAX / 8));
    for (unsigned j = 0; j < length; ++j)
        if (type)
            ((int *) data)[j] = uni(rng);
        else
            ((float *) data)[j] = float(uni(rng) + uni(rng) / 1000);
}

Sequence Sequence::shiftRight(int shift) const {
    Sequence shiftedSequence(length, type);
    for (unsigned j = 0; j < length; ++j)
        if (type)
            ((int *) shiftedSequence.data)[j] = (j < shift) ? 0 : ((int *) data)[j - shift];
        else
            ((float *) shiftedSequence.data)[j] = (j < shift) ? 0 : ((float *) data)[j - shift];
    return shiftedSequence;
}

Sequence Sequence::operator+=(const Sequence &other) {
    for (unsigned j = 0; j < length; ++j)
        if (type)
            ((int *) data)[j] += ((int *) other.data)[j];
        else
            ((float *) data)[j] += ((float *) other.data)[j];
    return *this;
}

void Sequence::print() const {
    for (unsigned j = 0; j < length; ++j)
        if (type)
            cout << ((int *) data)[j] << " ";
        else
            cout << ((float *) data)[j] << " ";
    cout << endl;
}


