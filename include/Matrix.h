#pragma once
#include <vector>
#include <functional> // Required for std::function

class Matrix {
public:
    Matrix();
    Matrix(int rows, int cols);

    void print() const;

    // New math functions
    static Matrix multiply(const Matrix& a, const Matrix& b);
    Matrix add(const Matrix& other) const;
    Matrix transpose() const;
    // Applies a function to every element of the matrix
    Matrix apply(std::function<double(double)> func) const;

    int rows;
    int cols;
    std::vector<std::vector<double>> data;
};
