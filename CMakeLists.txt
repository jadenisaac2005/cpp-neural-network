cmake_minimum_required(VERSION 3.10)
project(NeuralNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable and specify ALL its source files from the 'src' directory
add_executable(main
    src/main.cpp
    src/MNISTReader.cpp
    src/Matrix.cpp
    src/Layer.cpp
    src/Network.cpp
)

# Tell CMake to look for headers in the 'include' directory
target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/include")

# Evaluates a trained weights file against the MNIST test set
add_executable(eval
    src/eval.cpp
    src/MNISTReader.cpp
    src/Matrix.cpp
    src/Layer.cpp
    src/Network.cpp
)
target_include_directories(eval PUBLIC "${CMAKE_SOURCE_DIR}/include")

# One-off converter: text weights format -> binary weights format
add_executable(convert_weights
    src/convert_weights.cpp
    src/Matrix.cpp
    src/Layer.cpp
    src/Network.cpp
)
target_include_directories(convert_weights PUBLIC "${CMAKE_SOURCE_DIR}/include")
