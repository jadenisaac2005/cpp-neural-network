cmake_minimum_required(VERSION 3.10)
project(NeuralNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake to look for headers in the 'include' directory
target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/include")

# Add the executable and specify ALL its source files from the 'src' directory
add_executable(main
    src/main.cpp
    src/MNISTReader.cpp
    src/Matrix.cpp
    src/Layer.cpp
    src/Network.cpp
)
