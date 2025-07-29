#include "Network.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>


double relu(double x) { return std::max(0.0, x); }

Network::Network(const std::vector<int>& layer_sizes) {
    // Create layers based on the provided sizes
    for (size_t i = 0; i < layer_sizes.size() - 1; ++i) {
        layers.emplace_back(layer_sizes[i], layer_sizes[i + 1]);
    }
}

void Network::predict(const Matrix& input, Matrix& output) {
    Matrix current_output = input;
    for (size_t i = 0; i < layers.size(); ++i) {
        current_output = Matrix::multiply(layers[i].weights, current_output);
        current_output = current_output.add(layers[i].biases);

        if (i < layers.size() - 1) {
            current_output = current_output.apply(relu);
        }
    }
    output = current_output;
}
void Network::load_weights(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open weights file.");
    }
    std::string line;
    int current_layer = 0;

    // This expects 4 lines: fc1_weights, fc1_biases, fc2_weights, fc2_biases
    while (std::getline(file, line) && current_layer < layers.size() * 2) {
        std::stringstream ss(line);
        if (current_layer % 2 == 0) { // It's a weights line
            Matrix& weights = layers[current_layer / 2].weights;
            for (int i = 0; i < weights.rows; ++i) {
                for (int j = 0; j < weights.cols; ++j) {
                    ss >> weights.data[i][j];
                }
            }
        } else { // It's a biases line
            Matrix& biases = layers[current_layer / 2].biases;
            for (int i = 0; i < biases.rows; ++i) {
                ss >> biases.data[i][0];
            }
        }
        current_layer++;
    }
}
