#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>
#include <hpx/init.hpp>
#include <hpx/local/algorithm.hpp>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
#include <cassert>

std::default_random_engine gen;

template <typename T>
class Matrix {
public:
    const std::size_t rows, cols;
    std::vector<T> matrix;

    Matrix(const std::size_t rows, const std::size_t cols) : rows(rows), cols(cols), matrix(rows * cols, 0) {}

    ~Matrix() = default;

    T& entry(std::size_t i, std::size_t j) {
        assert(i < rows && j < cols);
        return matrix[i * cols + j];
    }

    // Asynchronous matrix multiplication with fixed data race issue
    hpx::future<Matrix<T>> operator*(const Matrix<T>& multiplicand) const {
        assert(multiplicand.rows == cols);
        auto product = std::make_shared<Matrix<T>>(rows, multiplicand.cols);

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

    void printMatrix() const {
        for (std::size_t i = 0; i < rows; i++) {
            for (std::size_t j = 0; j < cols; j++)
                std::cout << matrix[i * cols + j] << " ";
            std::cout << "\n";
        }
    }
};

int my_hpx_main(hpx::program_options::variables_map& vm) {
    std::size_t n = vm["n"].as<std::size_t>();
    std::size_t k = vm["k"].as<std::size_t>();
    std::size_t m = vm["m"].as<std::size_t>();
    int l = vm["l"].as<int>();
    int u = vm["u"].as<int>();

    assert(l <= u);

    unsigned int seed = std::random_device{}();
    if (vm.count("s"))
        seed = vm["s"].as<unsigned int>();

    gen.seed(seed);
    std::cout << "Using seed: " << seed << std::endl;

    std::uniform_int_distribution<int> dis(l, u);
    auto generator = [&]() { return dis(gen); };

    auto start = std::chrono::high_resolution_clock::now();

    Matrix<int> firstMatrix(m, n);
    hpx::ranges::generate(firstMatrix.matrix, generator);
    Matrix<int> secondMatrix(n, k);
    hpx::ranges::generate(secondMatrix.matrix, generator);

    // Run multiplication asynchronously
    hpx::future<Matrix<int>> productFuture = firstMatrix * secondMatrix;

    std::cout << "\nFirst Matrix:\n";
    firstMatrix.printMatrix();
    std::cout << "\nSecond Matrix:\n";
    secondMatrix.printMatrix();

    // Wait for the result and print it
    const Matrix<int> product = productFuture.get();

    std::cout << "\nProduct Matrix:\n";
    product.printMatrix();

    auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now() - start);

    std::cout << "\nTime taken: " << static_cast<double>(timeTaken.count()) * 1e-9 << " sec" << std::endl;

    return hpx::local::finalize();
}

int main(int argc, char *argv[]) {
    using namespace hpx::program_options;
    options_description cmdline("usage: " HPX_APPLICATION_STRING " [options]");
    // clang-format off
    cmdline.add_options()
            ("n", hpx::program_options::value<std::size_t>()->default_value(2), "Number of rows of first matrix")
            ("m", hpx::program_options::value<std::size_t>()->default_value(3), "Number of columns of first matrix")
            ("k", hpx::program_options::value<std::size_t>()->default_value(2), "Number of columns of second matrix")
            ("l", hpx::program_options::value<int>()->default_value(0), "Lower limit of range of values")
            ("u", hpx::program_options::value<int>()->default_value(10), "Upper limit of range of values")
            ("s", hpx::program_options::value<unsigned int>(), "Random seed");
    // clang-format on
    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::local::init(my_hpx_main, argc, argv, init_args);
}
