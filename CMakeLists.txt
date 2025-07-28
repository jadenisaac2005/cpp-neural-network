# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(NeuralNetwork)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add ALL source files to the executable
add_executable(main main.cpp MNISTReader.cpp Matrix.cpp Layer.cpp Network.cpp)
