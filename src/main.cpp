#include <iostream>
#include <vector>
#include <algorithm> // For max_element
#include "Network.h"
#include "MNISTReader.h"

// Helper function to create a Matrix from a flat vector
Matrix vectorToMatrix(const std::vector<double>& vec) {
    Matrix m(vec.size(), 1);
    for (size_t i = 0; i < vec.size(); ++i) {
        m.data[i][0] = vec[i];
    }
    return m;
}

int main() {
    try {
        // 1. Setup the network
        std::vector<int> layer_sizes = {784, 128, 10};
        Network nn(layer_sizes);

        // 2. Load training data
        std::vector<MNISTImage> train_data = read_mnist_dataset(
            "data/train-images-idx3-ubyte",
            "data/train-labels-idx1-ubyte"
        );
        std::cout << "Training data loaded: " << train_data.size() << " images." << std::endl;

        // 3. Train
        double learning_rate = 0.01;
        int epochs = 3;

        for (int epoch = 0; epoch < epochs; ++epoch) {
            for (size_t i = 0; i < train_data.size(); ++i) {
                Matrix input = vectorToMatrix(train_data[i].pixels);

                // Create one-hot label
                Matrix label(10, 1);
                label.data[train_data[i].label][0] = 1.0;

                nn.train(input, label, learning_rate);
            }
            std::cout << "Epoch " << epoch + 1 << " complete." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
