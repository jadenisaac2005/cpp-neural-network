#pragma once
#include "Layer.h"
#include "Matrix.h"
#include <vector>
#include <string>

class Network {
public:
    Network(const std::vector<int>& layer_sizes);
    void predict(const Matrix& input, Matrix& output);
    // Same forward pass as predict(), but also returns every layer's
    // post-activation output (the last entry is the same as predict()'s
    // output; every entry before it is a hidden-layer activation).
    void predict_with_layers(const Matrix& input, std::vector<Matrix>& layer_outputs);
    // Returns the number of scalar values successfully parsed, so callers
    // can verify the file actually matched this network's shape.
    size_t load_weights(const std::string& filename);
    void train(const Matrix& input, const Matrix& label, double learning_rate);


private:
    std::vector<Layer> layers;
};
