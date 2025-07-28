#pragma once
#include <vector>
#include <functional>

class Matrix {
public:
    Matrix();
    Matrix(int rows, int cols);

    void print() const;
    
    static Matrix multiply(const Matrix& a, const Matrix& b);

    Matrix add(const Matrix& other) const;
    Matrix transpose() const;
    Matrix apply(std::function<double(double)> func) const;

    int rows;
    int cols;
    std::vector<std::vector<double>> data;
};
