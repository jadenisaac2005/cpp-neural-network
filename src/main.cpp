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
        nn.load_weights("weights.txt");
        std::cout << "Weights loaded successfully." << std::endl;

        // 2. Load the test data
        std::vector<MNISTImage> test_data = read_mnist_dataset("data/t10k-images-idx3-ubyte", "data/t10k-labels-idx1-ubyte");
        std::cout << "Test data loaded successfully." << std::endl;

        // 3. Predict the first image
        MNISTImage first_image = test_data[0];
        Matrix input = vectorToMatrix(first_image.pixels);

        Matrix output = nn.predict(input);

        std::cout << "\nPrediction for first image (Correct Label: " << first_image.label << "):" << std::endl;
        output.print();

        // Find the index of the highest value in the output matrix
        auto& output_data = output.data[0]; // Get reference to the vector
        int predicted_label = std::distance(output_data.begin(), std::max_element(output_data.begin(), output_data.end()));

        std::cout << "\nPredicted Digit: " << predicted_label << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
