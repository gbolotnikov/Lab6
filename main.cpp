#include <cassert>
#include <iomanip>
#include <iostream>
#include "Matrix.hpp"

class CMatrix {
public:
    class CRow {
        friend class CMatrix;
    public:
        int& operator[](int col)
        {
            return parent.arr[row][col];
        }
    private:
        CRow(CMatrix &parent_, int row_) : 
            parent(parent_),
            row(row_)
        {}

        CMatrix& parent;
        int row;
    };

    CRow operator[](int row)
    {
        return CRow(*this, row);
    }
private:
    int rows, cols;
    int **arr;
};

int main() {
    Matrix<int16_t, -1> matrix;

    auto a = matrix[0][0];
    assert(a == -1); 
    assert(matrix.size() == 0); 
    // создание эл-та
    matrix[100][100] = 314; 
    assert(matrix[100][100] == 314); 
    assert(matrix.size() == 1);
    // удаление эл-та
    matrix[100][100] = -1; 
    assert(matrix.size() == 0);

    // заполнение матрицы
    for (uint8_t i = 0; i < 9; ++i) {
        matrix[i][i] = i + 1;
        matrix[i][8 - i] = 9 - i;
    }

    std::cout << "======Размер матрицы=====" << std::endl;
    std::cout << matrix.size() << std::endl;

    std::cout << "======Вывод блока матрицы=====" << std::endl;
    for (uint8_t i = 1; i < 8; ++i) {
        for(uint8_t j = 1; j < 8; ++j) {
            std::cout << std::setw(4) << matrix[i][j];
        }
        std::cout << std::endl;
    }
    
    std::cout << "======Вывод заполненных элементов=====" << std::endl;
     for (auto tuple = matrix.begin(); tuple != matrix.end(); ++tuple) {
        int16_t x; size_t i; size_t j;
        std::tie(x, i, j) = *tuple;
        std::cout << "value = " << x << ", i = " << i << ", j = " << j << std::endl;
     }

    return 0;
}