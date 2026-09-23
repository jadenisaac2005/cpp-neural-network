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

    // Binary weights format: 4-byte magic ("NNWF" = float32, "NNWD" =
    // float64), uint32 LE num_layers, (num_layers+1) uint32 LE layer sizes,
    // uint32 LE param_count, then param_count scalars in the same order as
    // the text format (per layer: weights row-major, then biases).
    size_t load_weights_binary(const std::string& filename);
    void save_weights_binary(const std::string& filename, bool use_double = false) const;

    void train(const Matrix& input, const Matrix& label, double learning_rate);


private:
    std::vector<Layer> layers;
};
