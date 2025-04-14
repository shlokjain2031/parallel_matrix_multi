#include <hpx/execution.hpp>
#include <hpx/init.hpp>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
#include <cassert>
#include <hpx/algorithm.hpp>

#include </Users/shlokjain/CLionProjects/parallel_matrix_multi/include/parallel_matrix.hpp>
#include </Users/shlokjain/CLionProjects/parallel_matrix_multi/include/sequential_matrix.hpp>
#include </Users/shlokjain/CLionProjects/parallel_matrix_multi/include/benchmarks.hpp>

std::default_random_engine gen;

// Benchmark synchronous Single_Matrix multiplication
double benchmark_single_matrix(const std::size_t m, const std::size_t n, const std::size_t k, int l, int u, unsigned int seed) {

     gen.seed(seed);
     std::cout << "Using seed: " << seed << std::endl;

     std::uniform_int_distribution<int> dis(l, u);

     auto generator = [&]() { return dis(gen); };

    const auto start = std::chrono::high_resolution_clock::now();

    sequential_matrix<int> firstMatrix(m, n);
    hpx::ranges::generate(firstMatrix.matrix, generator);

    sequential_matrix<int> secondMatrix(n, k);
    hpx::ranges::generate(secondMatrix.matrix, generator);

    sequential_matrix<int> product = firstMatrix * secondMatrix;

    const auto end = std::chrono::high_resolution_clock::now();
    const auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    return static_cast<double>(timeTaken.count()) * 1e-9; // return in seconds
}

// Benchmark asynchronous Multi_Matrix multiplication
double benchmark_multi_matrix(const std::size_t m, const std::size_t n, const std::size_t k, int l, int u, unsigned int seed) {

     gen.seed(seed);
     std::cout << "Using seed: " << seed << std::endl;

     std::uniform_int_distribution<int> dis(l, u);

     auto generator = [&]() { return dis(gen); };

    const auto start = std::chrono::high_resolution_clock::now();

    parallel_matrix<int> firstMatrix(m, n);
    hpx::ranges::generate(firstMatrix.matrix, generator);

    parallel_matrix<int> secondMatrix(n, k);
    hpx::ranges::generate(secondMatrix.matrix, generator);

    hpx::future<parallel_matrix<int>> productFuture = firstMatrix * secondMatrix;

    parallel_matrix<int> product = productFuture.get();

    const auto end = std::chrono::high_resolution_clock::now();
    const auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "\nFirst Matrix:\n";
    firstMatrix.print_matrix();
    std::cout << "\nSecond Matrix:\n";
    secondMatrix.print_matrix();

    std::cout << "\nProduct Matrix:\n";
    product.print_matrix();

    std::cout << "\nTime taken: " << static_cast<double>(timeTaken.count()) * 1e-9 << " sec" << std::endl;

    return static_cast<double>(timeTaken.count()) * 1e-9; // return in seconds
}

int my_hpx_main(const hpx::program_options::variables_map& vm) {
    std::size_t n = vm["n"].as<std::size_t>();
    std::size_t k = vm["k"].as<std::size_t>();
    std::size_t m = vm["m"].as<std::size_t>();
    int l = vm["l"].as<int>();
    int u = vm["u"].as<int>();

    assert(l <= u);

    unsigned int seed = std::random_device{}();
    if (vm.count("s"))
        seed = vm["s"].as<unsigned int>();

    const double parallel_time = benchmark_multi_matrix(m, n, k, l, u, seed);
    const double sequential_time = benchmark_single_matrix(m, n, k, l, u, seed);

    const double speedup = calculate_speedup(sequential_time, parallel_time);
    std::cout << "Speedup: " << speedup << std::endl;

    const unsigned int num_of_threads = std::thread::hardware_concurrency();
    std::cout << "Number of threads: " << num_of_threads << std::endl;

    const double efficiency = calculate_efficiency(speedup, num_of_threads);
    std::cout << "Efficiency: " << efficiency * 100 << "%" << std::endl;

    const size_t total_tasks = m*k;
    const double throughput = calculate_throughput(total_tasks, parallel_time);
    std::cout << "Throughput: " << throughput << std::endl;

    const double latency_per_task = 1/throughput;
    std::cout << "Latency per task: " << latency_per_task << std::endl;

    const double overhead = calculate_overhead(sequential_time, parallel_time, speedup);
    std::cout << "Overhead: " << overhead << std::endl;

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