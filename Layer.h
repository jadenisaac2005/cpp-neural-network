#pragma once
#include "Matrix.h"

class Layer {
public:
    // Constructor for a layer
    Layer(int input_size, int output_size);

    Matrix weights;
    Matrix biases;
};
