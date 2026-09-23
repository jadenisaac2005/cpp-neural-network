#include <iostream>
#include <string>
#include "Network.h"

// Converts the text weights format (docs/network.data) to the binary format.
// Usage: convert_weights <text_in> <binary_out> [--double]
int main(int argc, char** argv) {
    std::string text_path = argc > 1 ? argv[1] : "docs/network.data";
    std::string bin_path = argc > 2 ? argv[2] : "docs/network.bin";
    bool use_double = argc > 3 && std::string(argv[3]) == "--double";

    try {
        Network nn({784, 128, 10});
        size_t count = nn.load_weights(text_path);
        std::cout << "Read " << count << " parameters from " << text_path << std::endl;

        nn.save_weights_binary(bin_path, use_double);
        std::cout << "Wrote binary weights (" << (use_double ? "float64" : "float32")
                  << ") to " << bin_path << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
