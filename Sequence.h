#ifndef PARTIAL_SUM_SEQUENCE_H
#define PARTIAL_SUM_SEQUENCE_H


class Sequence {
public:
    void *data;
    int length; // m
    bool type;

    Sequence();
    ~Sequence();
    Sequence(int length, bool type);
    void generate() const;
    void print() const;
    Sequence shiftRight(int shift) const;
    Sequence operator+=(const Sequence& other);
};



#endif //PARTIAL_SUM_SEQUENCE_H
