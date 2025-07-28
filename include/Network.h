#pragma once
#include "Layer.h"
#include <vector>

class Network {
public:
    // Constructor takes a vector specifying the size of each layer
    Network(const std::vector<int>& layer_sizes);

private:
    std::vector<Layer> layers;
};
