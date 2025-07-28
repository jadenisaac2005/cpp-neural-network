#include "Network.h"
#include "Matrix.h"
#include <emscripten/bind.h>

using namespace emscripten;

// A helper function to convert a JS array into our C++ Matrix class
// This helper function converts a JS array into our C++ Matrix class
Matrix js_array_to_matrix(const val& js_array) {
    // Use the modern .as<>() method for conversion
    std::vector<double> vec = js_array.as<std::vector<double>>();

    Matrix m(vec.size(), 1);
    for (size_t i = 0; i < vec.size(); ++i) {
        m.data[i][0] = vec[i];
    }
    return m;
}
// A helper function to convert our C++ Matrix back to a JS array
val matrix_to_js_array(const Matrix& matrix) {
    val js_array = val::array();
    for (int i = 0; i < matrix.rows; ++i) {
        for (int j = 0; j < matrix.cols; ++j) {
            js_array.call<void>("push", matrix.data[i][j]);
        }
    }
    return js_array;
}

// This wrapper class will be what JavaScript interacts with.
class NetworkWrapper {
public:
    NetworkWrapper(const std::vector<int>& layer_sizes) : nn(layer_sizes) {}

    void load_weights(const std::string& filename) {
        nn.load_weights(filename);
    }

    val predict(const val& input_js_array) {
        Matrix input_matrix = js_array_to_matrix(input_js_array);
        Matrix output_matrix = nn.predict(input_matrix);
        return matrix_to_js_array(output_matrix);
    }

private:
    Network nn;
};

// This is the binding code that exposes our C++ classes and functions to JavaScript.
EMSCRIPTEN_BINDINGS(my_module) {
    register_vector<int>("VectorInt");
    register_vector<double>("VectorDouble");

    class_<NetworkWrapper>("Network")
        .constructor<const std::vector<int>&>()
        .function("load_weights", &NetworkWrapper::load_weights)
        .function("predict", &NetworkWrapper::predict);
}
