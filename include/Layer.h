#pragma once
#include "Matrix.h"

class Layer {
public:
    // Constructor for a layer
    Layer(int input_size, int output_size);

    Matrix weights;
    Matrix biases;
    Matrix forward(const Matrix& input);
    Matrix backward(const Matrix& dL_dZ, double learning_rate);

private:
    Matrix last_input;

};
