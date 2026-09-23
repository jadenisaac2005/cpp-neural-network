#include "Network.h"
#include "Matrix.h"
#include <emscripten/bind.h>
#include <emscripten/val.h>
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

size_t load_network_weights(const std::string& filename) {
    if (!nn) return 0;
    return nn->load_weights(filename);
}

size_t load_network_weights_binary(const std::string& filename) {
    if (!nn) return 0;
    return nn->load_weights_binary(filename);
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

// Result of predict_with_activations: softmax probabilities, raw logits,
// and the hidden layer's post-ReLU activations, each as a plain JS array.
struct Activations {
    val probs;
    val logits;
    val hidden;
};

Activations predict_with_activations(uintptr_t input_ptr, size_t input_size) {
    if (!nn) {
        return Activations{ val::array(), val::array(), val::array() };
    }

    double* input_array = reinterpret_cast<double*>(input_ptr);
    std::vector<double> input_vec(input_array, input_array + input_size);

    Matrix input_matrix(input_vec.size(), 1);
    for (size_t i = 0; i < input_vec.size(); ++i) {
        input_matrix.data[i][0] = input_vec[i];
    }

    std::vector<Matrix> layer_outputs;
    nn->predict_with_layers(input_matrix, layer_outputs);

    // Every layer output except the last is a hidden activation; this
    // network has exactly one (128-wide, post-ReLU).
    std::vector<double> hidden_vec;
    if (layer_outputs.size() > 1) {
        const Matrix& hidden_matrix = layer_outputs.front();
        hidden_vec.reserve(hidden_matrix.rows);
        for (int i = 0; i < hidden_matrix.rows; ++i) {
            hidden_vec.push_back(hidden_matrix.data[i][0]);
        }
    }

    const Matrix& logits_matrix = layer_outputs.back();
    std::vector<double> logits_vec;
    logits_vec.reserve(logits_matrix.rows);
    for (int i = 0; i < logits_matrix.rows; ++i) {
        logits_vec.push_back(logits_matrix.data[i][0]);
    }

    std::vector<double> probs_vec = softmax(logits_vec);

    Activations result;
    result.probs = val::array(probs_vec.begin(), probs_vec.end());
    result.logits = val::array(logits_vec.begin(), logits_vec.end());
    result.hidden = val::array(hidden_vec.begin(), hidden_vec.end());
    return result;
}

// --- Binding code ---
EMSCRIPTEN_BINDINGS(my_module) {
    register_vector<int>("VectorInt");
    register_vector<double>("VectorDouble");

    value_object<Activations>("Activations")
        .field("probs", &Activations::probs)
        .field("logits", &Activations::logits)
        .field("hidden", &Activations::hidden);

    function("init_network", &init_network);
    function("load_network_weights", &load_network_weights);
    function("load_network_weights_binary", &load_network_weights_binary);
    function("predict", &predict_from_memory); // Bind the new memory-based function
    function("predict_with_activations", &predict_with_activations);
}
