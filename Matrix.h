#pragma once
#include <vector>

class Matrix {
public:
    // Default constructor
    Matrix();
    // Constructor to create a matrix of a given size, initialized to 0
    Matrix(int rows, int cols);

    void print() const; // For debugging

// Making these public for easier access in this project
// In a real library, you'd use getter/setter methods
    int rows;
    int cols;
    std::vector<std::vector<double>> data;
};
