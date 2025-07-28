#pragma once
#include <vector>
#include <string>

// A struct to hold a single MNIST image and its label
struct MNISTImage {
    std::vector<double> pixels; // 784 pixels (28x28), normalized to 0.0-1.0
    int label;                  // The correct digit (0-9)
};

// Function to load the dataset
std::vector<MNISTImage> read_mnist_dataset(const std::string& image_path, const std::string& label_path);
