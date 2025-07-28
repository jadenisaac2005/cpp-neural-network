#include "MNISTReader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// Helper function to reverse byte order for integers
int reverseInt(int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

std::vector<MNISTImage> read_mnist_dataset(const std::string& image_path, const std::string& label_path) {
    std::ifstream image_file(image_path, std::ios::binary);
    std::ifstream label_file(label_path, std::ios::binary);

    if (!image_file.is_open() || !label_file.is_open()) {
        throw std::runtime_error("Cannot open one of the dataset files.");
    }

    int magic_number_images = 0, num_images = 0, rows = 0, cols = 0;
    int magic_number_labels = 0, num_labels = 0;

    // Read image file header
    image_file.read((char*)&magic_number_images, sizeof(magic_number_images));
    image_file.read((char*)&num_images, sizeof(num_images));
    image_file.read((char*)&rows, sizeof(rows));
    image_file.read((char*)&cols, sizeof(cols));

    magic_number_images = reverseInt(magic_number_images);
    num_images = reverseInt(num_images);
    rows = reverseInt(rows);
    cols = reverseInt(cols);

    // Read label file header
    label_file.read((char*)&magic_number_labels, sizeof(magic_number_labels));
    label_file.read((char*)&num_labels, sizeof(num_labels));

    magic_number_labels = reverseInt(magic_number_labels);
    num_labels = reverseInt(num_labels);

    if (num_images != num_labels) {
        throw std::runtime_error("Image and label file counts do not match.");
    }

    std::vector<MNISTImage> dataset;
    dataset.reserve(num_images);

    for (int i = 0; i < num_images; ++i) {
        MNISTImage img;
        img.pixels.resize(rows * cols);

        // Read pixel data
        for (int r = 0; r < rows * cols; ++r) {
            unsigned char temp = 0;
            image_file.read((char*)&temp, sizeof(temp));
            img.pixels[r] = (double)temp / 255.0; // Normalize pixels to 0.0-1.0
        }

        // Read label data
        unsigned char label_temp = 0;
        label_file.read((char*)&label_temp, sizeof(label_temp));
        img.label = (int)label_temp;

        dataset.push_back(img);
    }

    return dataset;
}
