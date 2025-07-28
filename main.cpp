#include <iostream>
#include "Network.h"

int main() {
    // Define the network architecture:
    // Input layer: 784 neurons (for 28x28 images)
    // Hidden layer: 128 neurons
    // Output layer: 10 neurons (for digits 0-9)
    std::vector<int> layer_sizes = {784, 128, 10};

    Network my_network(layer_sizes);

    std::cout << "Neural Network architecture created successfully!" << std::endl;

    return 0;
}
