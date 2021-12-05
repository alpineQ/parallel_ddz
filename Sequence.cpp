/*–––––––––––––––––––––––––––––––––––––----––––––––––––––––––*/
/* Sequence.cpp                                              */
/*                                                           */
/* Вспомогательный класс Sequence, упрощающий работу с       */
/* последовательностями	                                     */
/* Автор: Казанцев М.А., 2021 г.                             */
/*                                                           */
/* –––––––––––––––––––––––––––––––––––––---––––––––––––––––––*/
#include <random>
#include <bits/stdc++.h>
#include "Sequence.h"

using namespace std;

/*------------------------------------------------------------*/
/* Конструктор класса Sequence(int length, bool type)		  */
/* Описание: Выделение памяти под последовательность длины    */
/* length и типа type                                          */
/* Параметры:                								  */
/* length – длина последовательности                          */
/* type – тип данных последовательности:                      */
/* - true - для целочисленных значений,                       */
/* - false - для значений с плавающей точкой                  */
/* Возвращаемое значение:            						  */
/* Нет                                                        */
/* Внешние эффекты:              							  */
/* Нет                                                        */
/*------------------------------------------------------------*/
Sequence::Sequence(int length, bool type) {
    this->length = length;
    this->type = type;
    if (type)
        data = safeAllocate<int>(length);
    else
        data = safeAllocate<float>(length);
}

/*------------------------------------------------------------*/
/* Метод Sequence::free()                       			  */
/* Описание: освобождение памяти, занятой последовательностью */
/* значениями                                                 */
/* Параметры:                								  */
/* Нет                                                        */
/* Возвращаемое значение:            						  */
/* Нет                                                        */
/* Внешние эффекты:              							  */
/* Нет                                                        */
/* Примечание:                                                */
/* Деструктор не использовался из-за особенностей передачи    */
/* объектов класса как результата работы функции              */
/*------------------------------------------------------------*/
void Sequence::free() const {
    if (type)
        delete[] (int *) data;
    else
        delete[] (float *) data;
}

/*-----------------------------------------------------------*/
/* Метод Sequence::generate()                    			 */
/* Описание: заполнение последовательности случайными        */
/* значениями                                                */
/* Параметры:                								 */
/* Нет                                                       */
/* Возвращаемое значение:            						 */
/* Нет                                                       */
/* Внешние эффекты:              							 */
/* Нет                                                       */
/*-----------------------------------------------------------*/
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

/*-----------------------------------------------------------*/
/* Метод Sequence::shiftRight(int shift)         			 */
/* Описание: сдвиг элементов последовательности Sequence     */
/* вправо на shift элементов. Значения последовательности    */
/* слева заполняются нулями	                                 */
/* Параметры:                								 */
/* shift – величина сдвига последовательности                */
/* Возвращаемое значение:            						 */
/* Последовательность Sequence, сдвинутая вправо на shift    */
/* элементов                                                 */
/* Внешние эффекты:              							 */
/* Нет                                                       */
/*-----------------------------------------------------------*/
Sequence Sequence::shiftRight(int shift) const {
    Sequence shiftedSequence(length, type);
    for (unsigned j = 0; j < length; ++j)
        if (type)
            ((int *) shiftedSequence.data)[j] = (j < shift) ? 0 : ((int *) data)[j - shift];
        else
            ((float *) shiftedSequence.data)[j] = (j < shift) ? 0 : ((float *) data)[j - shift];
    return shiftedSequence;
}

/*------------------------------------------------------------*/
/* Метод Sequence::operator+=(const Sequence &other)		  */
/* Описание: операция сложения с присвоением двух объектов    */
/* класса Sequence	                                          */
/* Параметры:                								  */
/* other – константная ссылка на второе слагаемое в           */
/* операции сложения двух объектов класса Sequence            */
/* Возвращаемое значение:            						  */
/* сумма операции сложения двух объектов класса Sequence      */
/* Внешние эффекты:              							  */
/* Объекту класса Sequence присваивается значение суммы       */
/* объектов this и other                  					  */
/*------------------------------------------------------------*/
Sequence Sequence::operator+=(const Sequence &other) {
    for (unsigned j = 0; j < length; ++j)
        if (type)
            ((int *) data)[j] += ((int *) other.data)[j];
        else
            ((float *) data)[j] += ((float *) other.data)[j];
    return *this;
}

/*-----------------------------------------------------------*/
/* Метод Sequence::print()           						 */
/* Описание: выводит на экран последовательность Sequence	 */
/* Параметры:				                				 */
/* Нет									                	 */
/* Возвращаемое значение:            						 */
/* Нет               										 */
/* Внешние эффекты:              							 */
/* На экран выводится значение последовательности Sequence	 */
/*-----------------------------------------------------------*/
void Sequence::print() const {
    for (unsigned j = 0; j < length; ++j)
        if (type)
            cout << ((int *) data)[j] << " ";
        else
            cout << ((float *) data)[j] << " ";
    cout << endl;
}


