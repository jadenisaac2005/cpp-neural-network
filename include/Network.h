#pragma once
#include "Layer.h"
#include "Matrix.h"
#include <vector>
#include <string>

class Network {
public:
    Network(const std::vector<int>& layer_sizes);
    Matrix predict(Matrix input); // Renamed for clarity
    void load_weights(const std::string& filename);

private:
    std::vector<Layer> layers;
};
