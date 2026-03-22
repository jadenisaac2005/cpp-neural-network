#pragma once
#include "Layer.h"
#include "Matrix.h"
#include <vector>
#include <string>

class Network {
public:
    Network(const std::vector<int>& layer_sizes);
    void predict(const Matrix& input, Matrix& output);
    void load_weights(const std::string& filename);
    void train(const Matrix& input, const Matrix& label, double learning_rate);


private:
    std::vector<Layer> layers;
};
