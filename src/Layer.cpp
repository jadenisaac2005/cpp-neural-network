#include "Layer.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()

Layer::Layer(int input_size, int output_size)
    : weights(output_size, input_size), biases(output_size, 1) {

    // Seed the random number generator
    srand(time(NULL));

    // Initialize weights with small random values
    for (int i = 0; i < weights.rows; ++i) {
        for (int j = 0; j < weights.cols; ++j) {
            // Random value between -0.5 and 0.5
            weights.data[i][j] = ((double)rand() / RAND_MAX) - 0.5;
        }
    }
}

Matrix Layer::forward(const Matrix& input) {
    last_input = input;
    return Matrix::multiply(weights, input).add(biases);
}

Matrix Layer::backward(const Matrix& dL_dZ, double learning_rate) {
    Matrix dL_dW = Matrix::multiply(dL_dZ, last_input.transpose());
    Matrix dL_dX = Matrix::multiply(weights.transpose(), dL_dZ);
    
    // update weights and biases
    weights = weights.subtract(dL_dW.scale(learning_rate));
    biases = biases.subtract(dL_dZ.scale(learning_rate));
    
    return dL_dX; // pass gradient to previous layer
}

