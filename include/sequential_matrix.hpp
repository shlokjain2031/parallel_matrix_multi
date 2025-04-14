//
// Created by Shlok Jain on 14/04/25.
//

#ifndef SINGLE_MATRIX_HPP
#define SINGLE_MATRIX_HPP

#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
#include <cassert>

#endif //SINGLE_MATRIX_HPP

template <typename T>
class sequential_matrix {
public:
    const std::size_t rows, cols;
    std::vector<T> matrix;

    sequential_matrix(const std::size_t rows, const std::size_t cols) : rows(rows), cols(cols), matrix(rows * cols, 0) {}

    ~sequential_matrix() = default;

    T& entry(std::size_t i, std::size_t j) {
        assert(i < rows && j < cols);
        return matrix[i * cols + j];
    }

    // Asynchronous matrix multiplication with fixed data race issue
    sequential_matrix<T> operator*(const sequential_matrix<T>& multiplicand) const {
        assert(multiplicand.rows == cols);
        auto product = std::make_shared<sequential_matrix<T>>(rows, multiplicand.cols);

        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < multiplicand.cols; ++j) {
                T sum = 0;
                for (std::size_t k = 0; k < cols; ++k) {
                    sum += this->matrix[i * cols + k] * multiplicand.matrix[k * multiplicand.cols + j];
                }
                product->entry(i, j) = sum;
            }
        }

        return *product;
    }

    void print_matrix() const {
        for (std::size_t i = 0; i < rows; i++) {
            for (std::size_t j = 0; j < cols; j++)
                std::cout << matrix[i * cols + j] << " ";
            std::cout << "\n";
        }
    }
};