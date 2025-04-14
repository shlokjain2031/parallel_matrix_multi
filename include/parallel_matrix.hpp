//
// Created by Shlok Jain on 12/04/25.
//

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>
#include <iostream>
#include <vector>
#include <cassert>
#include <hpx/parallel/algorithms/for_loop.hpp>

#endif //MATRIX_HPP

template <typename T>
class parallel_matrix {
public:
    const std::size_t rows, cols;
    std::vector<T> matrix;

    parallel_matrix(const std::size_t rows, const std::size_t cols) : rows(rows), cols(cols), matrix(rows * cols, 0) {}

    ~parallel_matrix() = default;

    T& entry(std::size_t i, std::size_t j) {
        assert(i < rows && j < cols);
        return matrix[i * cols + j];
    }

    // Asynchronous matrix multiplication with fixed data race issue
    hpx::future<parallel_matrix<T>> operator*(const parallel_matrix<T>& multiplicand) const {
        assert(multiplicand.rows == cols);
        auto product = std::make_shared<parallel_matrix<T>>(rows, multiplicand.cols);

        return hpx::async([=]() mutable {
            hpx::experimental::for_loop(hpx::execution::par, 0, rows, [&](std::size_t i) {
                for (std::size_t k = 0; k < multiplicand.cols; k++) {
                    T sum = 0;
                    for (std::size_t j = 0; j < cols; j++) {
                        sum += matrix[i * cols + j] * multiplicand.matrix[j * multiplicand.cols + k];
                    }
                    product->entry(i, k) = sum;
                }
            });
            return *product;
        });
    }

    void print_matrix() const {
        for (std::size_t i = 0; i < rows; i++) {
            for (std::size_t j = 0; j < cols; j++)
                std::cout << matrix[i * cols + j] << " ";
            std::cout << "\n";
        }
    }
};

