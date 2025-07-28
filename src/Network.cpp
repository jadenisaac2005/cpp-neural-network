#include "Network.h"

Network::Network(const std::vector<int>& layer_sizes) {
    // Create layers based on the provided sizes
    for (size_t i = 0; i < layer_sizes.size() - 1; ++i) {
        layers.emplace_back(layer_sizes[i], layer_sizes[i + 1]);
    }
}
