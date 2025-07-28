#include <iostream>
#include "Matrix.h"
#include <vector>

int main() {
    // Create two matrices
    Matrix a(2, 3);
    a.data = {{1, 2, 3}, {4, 5, 6}};

    Matrix b(3, 2);
    b.data = {{7, 8}, {9, 10}, {11, 12}};

    std::cout << "Matrix A:" << std::endl;
    a.print();
    std::cout << "\nMatrix B:" << std::endl;
    b.print();

    // Multiply them
    try {
        Matrix c = Matrix::multiply(a, b);
        std::cout << "\nResult of A * B:" << std::endl;
        c.print();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
