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
double benchmark_single_matrix(const std::size_t m, const std::size_t n, const std::size_t k, int l, int u, bool benchmarking) {

    std::uniform_int_distribution<int> dis(l, u);
    auto generator = [&]() { return dis(gen); };

    const auto start = std::chrono::high_resolution_clock::now();

    sequential_matrix<int> first_matrix(m, n);
    hpx::ranges::generate(first_matrix.matrix, generator);

    sequential_matrix<int> second_matrix(n, k);
    hpx::ranges::generate(second_matrix.matrix, generator);

    sequential_matrix<int> product = first_matrix * second_matrix;

    const auto end = std::chrono::high_resolution_clock::now();
    const auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    return static_cast<double>(timeTaken.count()) * 1e-9; // return in seconds
}

// Benchmark asynchronous Multi_Matrix multiplication
double benchmark_multi_matrix(const std::size_t m, const std::size_t n, const std::size_t k, int l, int u, bool benchmarking) {

    std::uniform_int_distribution<int> dis(l, u);
    auto generator = [&]() { return dis(gen); };

    const auto start = std::chrono::high_resolution_clock::now();

    parallel_matrix<int> first_matrix(m, n);
    hpx::ranges::generate(first_matrix.matrix, generator);

    parallel_matrix<int> second_matrix(n, k);
    hpx::ranges::generate(second_matrix.matrix, generator);

    hpx::future<parallel_matrix<int>> product_future = first_matrix * second_matrix;

    parallel_matrix<int> product = product_future.get();

    const auto end = std::chrono::high_resolution_clock::now();
    const auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    if (!benchmarking) {
        std::cout << "\nFirst Matrix:\n";
        first_matrix.print_matrix();
        std::cout << "\nSecond Matrix:\n";
        second_matrix.print_matrix();

        std::cout << "\nProduct Matrix:\n";
        product.print_matrix();
    }

    return static_cast<double>(timeTaken.count()) * 1e-9; // return in seconds
}

void print_pretty(std::size_t n, std::size_t m, std::size_t k, unsigned int num_threads, double time_taken, double speedup,
                            double efficiency, double throughput, double latency_per_task) {
    std::cout << "\nTime taken: " << time_taken << " sec" << std::endl;
    std::cout << "Speedup: " << speedup << std::endl;
    std::cout << "Number of threads: " << num_threads << std::endl;
    std::cout << "Efficiency: " << efficiency * 100 << "%" << std::endl;
    std::cout << "Throughput: " << throughput << std::endl;
    std::cout << "Latency per task: " << latency_per_task << std::endl;
}

void print_benchmarks_csv(std::size_t n, std::size_t m, std::size_t k, unsigned int num_threads, double time_taken, double speedup,
                            double efficiency, double throughput, double latency_per_task) {
    std::cout << n << ","
              << m << ','
              << k << ','
              << num_threads << ","
              << time_taken << ","
              << speedup << ","
              << efficiency << ","
              << throughput << ","
              << latency_per_task << std::endl;
}

int my_hpx_main(const hpx::program_options::variables_map& vm) {
    std::size_t n = vm["n"].as<std::size_t>();
    std::size_t m = vm["m"].as<std::size_t>();
    std::size_t k = vm["k"].as<std::size_t>();
    const int l = vm["l"].as<int>();
    const int u = vm["u"].as<int>();
    const int benchmarking = vm["benchmarking"].as<int>();
    assert(l <= u);

    unsigned int seed = std::random_device{}();
    if (vm.count("s"))
        seed = vm["s"].as<unsigned int>();

    gen.seed(seed);

    const double parallel_time = benchmark_multi_matrix(m, n, k, l, u, benchmarking);
    const double sequential_time = benchmark_single_matrix(m, n, k, l, u, benchmarking);

    const double speedup = calculate_speedup(sequential_time, parallel_time);
    std::size_t num_of_threads = hpx::get_num_worker_threads();
    const double efficiency = calculate_efficiency(speedup, num_of_threads);
    const size_t total_tasks = m*k;
    const double throughput = calculate_throughput(total_tasks, parallel_time);
    const double latency_per_task = 1/throughput;

    if (benchmarking) {
        print_benchmarks_csv(n, m, k, num_of_threads, parallel_time,
                        speedup, efficiency, throughput, latency_per_task);
    }
    else {
        print_pretty(n, m, k, num_of_threads, parallel_time,
                        speedup, efficiency, throughput, latency_per_task);
    }

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
            ("s", hpx::program_options::value<unsigned int>(), "Random seed")
            ("benchmarking", hpx::program_options::value<int>()->default_value(0), "Benchmarking?");
    // clang-format on
    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::local::init(my_hpx_main, argc, argv, init_args);
}