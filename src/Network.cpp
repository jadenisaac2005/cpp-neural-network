#include "Network.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cstring>


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

void Network::predict_with_layers(const Matrix& input, std::vector<Matrix>& layer_outputs) {
    Matrix current_output = input;
    for (size_t i = 0; i < layers.size(); ++i) {
        current_output = Matrix::multiply(layers[i].weights, current_output);
        current_output = current_output.add(layers[i].biases);

        if (i < layers.size() - 1) {
            current_output = current_output.apply(relu);
        }
        layer_outputs.push_back(current_output);
    }
}

size_t Network::load_weights(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open weights file.");
    }
    std::string line;
    int current_layer = 0;
    size_t params_read = 0;

    // This expects 4 lines: fc1_weights, fc1_biases, fc2_weights, fc2_biases
    while (std::getline(file, line) && current_layer < layers.size() * 2) {
        std::stringstream ss(line);
        if (current_layer % 2 == 0) { // It's a weights line
            Matrix& weights = layers[current_layer / 2].weights;
            for (int i = 0; i < weights.rows; ++i) {
                for (int j = 0; j < weights.cols; ++j) {
                    if (ss >> weights.data[i][j]) ++params_read;
                }
            }
        } else { // It's a biases line
            Matrix& biases = layers[current_layer / 2].biases;
            for (int i = 0; i < biases.rows; ++i) {
                if (ss >> biases.data[i][0]) ++params_read;
            }
        }
        current_layer++;
    }
    return params_read;
}

namespace {
    void write_u32(std::ostream& out, uint32_t v) {
        out.write(reinterpret_cast<const char*>(&v), sizeof(v));
    }
    uint32_t read_u32(std::istream& in) {
        uint32_t v = 0;
        in.read(reinterpret_cast<char*>(&v), sizeof(v));
        return v;
    }
}

size_t Network::load_weights_binary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open binary weights file.");
    }

    char magic[4];
    file.read(magic, 4);
    bool is_double;
    if (std::memcmp(magic, "NNWF", 4) == 0) {
        is_double = false;
    } else if (std::memcmp(magic, "NNWD", 4) == 0) {
        is_double = true;
    } else {
        throw std::runtime_error("Invalid binary weights file: bad magic bytes.");
    }

    uint32_t num_layers = read_u32(file);
    if (num_layers != layers.size()) {
        throw std::runtime_error("Binary weights file layer count does not match network.");
    }

    std::vector<uint32_t> file_layer_sizes(num_layers + 1);
    for (uint32_t i = 0; i <= num_layers; ++i) {
        file_layer_sizes[i] = read_u32(file);
    }
    for (uint32_t i = 0; i < num_layers; ++i) {
        if (static_cast<int>(file_layer_sizes[i]) != layers[i].weights.cols ||
            static_cast<int>(file_layer_sizes[i + 1]) != layers[i].weights.rows) {
            throw std::runtime_error("Binary weights file layer sizes do not match network.");
        }
    }

    uint32_t param_count = read_u32(file);
    size_t params_read = 0;

    auto read_scalar = [&]() -> double {
        if (is_double) {
            double v = 0.0;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            return v;
        } else {
            float v = 0.0f;
            file.read(reinterpret_cast<char*>(&v), sizeof(v));
            return static_cast<double>(v);
        }
    };

    for (size_t l = 0; l < layers.size(); ++l) {
        Matrix& weights = layers[l].weights;
        for (int i = 0; i < weights.rows; ++i) {
            for (int j = 0; j < weights.cols; ++j) {
                if (!file) break;
                weights.data[i][j] = read_scalar();
                ++params_read;
            }
        }
        Matrix& biases = layers[l].biases;
        for (int i = 0; i < biases.rows; ++i) {
            if (!file) break;
            biases.data[i][0] = read_scalar();
            ++params_read;
        }
    }

    (void)param_count; // header value is informational; params_read is authoritative
    return params_read;
}

void Network::save_weights_binary(const std::string& filename, bool use_double) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open binary weights file for writing.");
    }

    file.write(use_double ? "NNWD" : "NNWF", 4);
    write_u32(file, static_cast<uint32_t>(layers.size()));

    write_u32(file, static_cast<uint32_t>(layers.front().weights.cols));
    for (const auto& layer : layers) {
        write_u32(file, static_cast<uint32_t>(layer.weights.rows));
    }

    size_t param_count = 0;
    for (const auto& layer : layers) {
        param_count += static_cast<size_t>(layer.weights.rows) * layer.weights.cols;
        param_count += layer.biases.rows;
    }
    write_u32(file, static_cast<uint32_t>(param_count));

    auto write_scalar = [&](double v) {
        if (use_double) {
            file.write(reinterpret_cast<const char*>(&v), sizeof(v));
        } else {
            float f = static_cast<float>(v);
            file.write(reinterpret_cast<const char*>(&f), sizeof(f));
        }
    };

    for (const auto& layer : layers) {
        const Matrix& weights = layer.weights;
        for (int i = 0; i < weights.rows; ++i) {
            for (int j = 0; j < weights.cols; ++j) {
                write_scalar(weights.data[i][j]);
            }
        }
        const Matrix& biases = layer.biases;
        for (int i = 0; i < biases.rows; ++i) {
            write_scalar(biases.data[i][0]);
        }
    }
}

void Network::train(const Matrix& input, const Matrix& label,
                    double learning_rate) {
    // Forward pass
    Matrix current = input;
    for (size_t i = 0; i < layers.size(); ++i) {
        current = layers[i].forward(current);
        if (i < layers.size() - 1) {
            current = current.apply(relu);
        }
    }
    
    // Compute loss gradient (output - label)
    Matrix dL_dZ = current.subtract(label);
    
    // Backward pass
    for (int i = layers.size() - 1; i >= 0; --i) {
        dL_dZ = layers[i].backward(dL_dZ, learning_rate);
    }
}

