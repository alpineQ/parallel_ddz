/**
 * Sequence.cpp
 *
 * Вспомогательный класс Sequence, упрощающий работу с
 * последовательностями
 * Автор: Казанцев М.А., 2021 г.
 *
 */
#include <random>
#include <bits/stdc++.h>
#include "Sequence.h"

using namespace std;

/**
 * Выделение памяти под последовательность длины length и типа type
 *
 * @param length – длина последовательности
 */
Sequence::Sequence(int length) {
    this->length = length;
    data = safeAllocate<float>(length);
}

/**
 * Освобождение памяти, занятой последовательностью значениями
 *
 * Примечание:
 * Деструктор не использовался из-за особенностей передачи
 * объектов класса как результата работы функции
 */
void Sequence::free() const {
    delete[] data;
}

/**
 * Заполнение последовательности случайными значениями
 */
void Sequence::generate() const {
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> uni(floor(INT_MIN / 8), floor(INT_MAX / 8));
    for (unsigned j = 0; j < length; ++j)
        data[j] = float(uni(rng) + uni(rng) / 1000);
}

/**
 * Сдвиг элементов последовательности Sequence вправо на shift
 * элементов. Значения последовательности слева заполняются нулями
 *
 * @param shift – величина сдвига последовательности
 * @returns Последовательность Sequence, сдвинутая вправо на shift
 * элементов
 */
Sequence Sequence::shiftRight(int shift) const {
    Sequence shiftedSequence(length);
    for (unsigned j = 0; j < length; ++j)
        shiftedSequence.data[j] = (j < shift) ? 0 : data[j - shift];
    return shiftedSequence;
}

/**
 * Операция сложения с присвоением двух объектов класса Sequence
 *
 * @param other – константная ссылка на второе слагаемое в
 * операции сложения двух объектов класса Sequence
 * @returns сумма операции сложения двух объектов класса Sequence
 * @note Объекту класса Sequence присваивается значение суммы
 * объектов this и other
 */
Sequence Sequence::operator+=(const Sequence &other) {
    for (unsigned j = 0; j < length; ++j)
        #pragma omp atomic
        data[j] += other.data[j];
    return *this;
}

/**
 * Вывод на экран последовательность Sequence
 *
 * @note На экран выводится значение последовательности Sequence
 */
void Sequence::print() const {
    for (unsigned j = 0; j < length; ++j)
        cout << data[j] << " ";
    cout << endl;
}
