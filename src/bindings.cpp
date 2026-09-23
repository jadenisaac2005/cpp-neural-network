#include "Network.h"
#include "Matrix.h"
#include <emscripten/bind.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace emscripten;

std::unique_ptr<Network> nn;

// Converts raw output-layer scores into a probability distribution.
std::vector<double> softmax(const std::vector<double>& scores) {
    if (scores.empty()) return scores;
    double max_score = *std::max_element(scores.begin(), scores.end());

    std::vector<double> exp_scores(scores.size());
    double sum = 0.0;
    for (size_t i = 0; i < scores.size(); ++i) {
        exp_scores[i] = std::exp(scores[i] - max_score);
        sum += exp_scores[i];
    }
    for (size_t i = 0; i < exp_scores.size(); ++i) {
        exp_scores[i] /= sum;
    }
    return exp_scores;
}

// --- C++ functions that will be called from JavaScript ---
void init_network(const std::vector<int>& layer_sizes) {
    nn = std::make_unique<Network>(layer_sizes);
}

void load_network_weights(const std::string& filename) {
    if (nn) {
        nn->load_weights(filename.c_str());
    }
}

// The new predict function takes a raw pointer to the data from JavaScript
std::vector<double> predict_from_memory(uintptr_t input_ptr, size_t input_size) {
    if (!nn) {
        return {};
    }

    // Create a C++ vector by viewing the memory from JavaScript
    double* input_array = reinterpret_cast<double*>(input_ptr);
    std::vector<double> input_vec(input_array, input_array + input_size);

    // 1. Convert input vector to a Matrix
    Matrix input_matrix(input_vec.size(), 1);
    for (size_t i = 0; i < input_vec.size(); ++i) {
        input_matrix.data[i][0] = input_vec[i];
    }

    // 2. Run prediction
    Matrix output_matrix;
    nn->predict(input_matrix, output_matrix);

    // 3. Convert output Matrix back to a simple vector
    std::vector<double> output_vec;
    if (output_matrix.cols > 0) {
        output_vec.reserve(output_matrix.rows);
        for (int i = 0; i < output_matrix.rows; ++i) {
            output_vec.push_back(output_matrix.data[i][0]);
        }
    }

    // 4. Convert raw scores to a probability distribution over all 10 digits
    return softmax(output_vec);
}

// --- Binding code ---
EMSCRIPTEN_BINDINGS(my_module) {
    register_vector<int>("VectorInt");
    register_vector<double>("VectorDouble");

    function("init_network", &init_network);
    function("load_network_weights", &load_network_weights);
    function("predict", &predict_from_memory); // Bind the new memory-based function
}
