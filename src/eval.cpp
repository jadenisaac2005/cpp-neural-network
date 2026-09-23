#include <iostream>
#include <iomanip>
#include <array>
#include "Network.h"
#include "MNISTReader.h"

// Loads the network with the weights currently deployed to the web demo
// (docs/network.data) and reports accuracy on the MNIST test set.
// Usage: eval [weights_path] [images_path] [labels_path] [--binary]
int main(int argc, char** argv) {
    std::string weights_path = argc > 1 ? argv[1] : "docs/network.data";
    std::string images_path = argc > 2 ? argv[2] : "data/t10k-images-idx3-ubyte";
    std::string labels_path = argc > 3 ? argv[3] : "data/t10k-labels-idx1-ubyte";
    bool use_binary = argc > 4 && std::string(argv[4]) == "--binary";

    try {
        Network nn({784, 128, 10});
        size_t params_read = use_binary
            ? nn.load_weights_binary(weights_path)
            : nn.load_weights(weights_path);
        std::cout << "Loaded " << params_read << " parameters from " << weights_path << std::endl;

        std::vector<MNISTImage> test_data = read_mnist_dataset(images_path, labels_path);
        std::cout << "Loaded " << test_data.size() << " test images from " << images_path << std::endl;

        std::array<int, 10> per_digit_correct{};
        std::array<int, 10> per_digit_total{};
        int correct = 0;

        for (const auto& img : test_data) {
            Matrix input(img.pixels.size(), 1);
            for (size_t i = 0; i < img.pixels.size(); ++i) {
                input.data[i][0] = img.pixels[i];
            }

            Matrix output;
            nn.predict(input, output);

            int predicted = 0;
            double best = output.data[0][0];
            for (int i = 1; i < output.rows; ++i) {
                if (output.data[i][0] > best) {
                    best = output.data[i][0];
                    predicted = i;
                }
            }

            per_digit_total[img.label]++;
            if (predicted == img.label) {
                correct++;
                per_digit_correct[img.label]++;
            }
        }

        double overall = 100.0 * correct / test_data.size();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nOverall accuracy: " << correct << "/" << test_data.size()
                  << " (" << overall << "%)\n\n";

        std::cout << "Digit  Correct/Total  Accuracy\n";
        for (int d = 0; d < 10; ++d) {
            double acc = per_digit_total[d] > 0
                ? 100.0 * per_digit_correct[d] / per_digit_total[d]
                : 0.0;
            std::cout << "  " << d << "    " << per_digit_correct[d] << "/" << per_digit_total[d]
                      << "         " << acc << "%\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
