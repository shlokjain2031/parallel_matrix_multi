# Parallel Matrix Multiplication using HPX

## Description
This project implements **parallel matrix multiplication** using **HPX**, an **asynchronous many-task runtime**.  
The code utilizes **HPX parallel algorithms** to distribute computation efficiently across multiple cores.

### **Features**
- Uses **HPX’s experimnetal for loop and parallel execution policies** (` hpx::experimental::for_loop`) (`hpx::execution::par`)
- Implements **asynchronous execution** with `hpx::future<T>`.
- Dynamically generates random matrices.
- Accepts **custom matrix sizes** via command-line arguments.
- Outputs execution time for benchmarking along with speedup, throughput, efficiency and latency

---

## **Build & Run Instructions**
### **Dependencies**
- **HPX (C++ parallel runtime)**
- **CMake (Build system)**
- **vcpkg (for dependency management, optional)**

### **Installation**
- Install HPX using a package manager
- Clone the repository
- Build the project using CMake
- Run the program with the following command
  
  `./parallel_matrix_multi --m=3 --n=3 --k=3 --l=0 --u=10`

### **Sample Output**
```
First Matrix:
0 7 
3 10 
0 1 
Second Matrix:
8 0 
9 9 
Product Matrix:
63 63 
114 90 
9 9 
Time taken: 0.000514375 seconds
```

### **Benchmark Results**

### **Implementation Details**
- Used a generic template named `Matrix<T>` to accomadate all data types without any runtime overhead. Implemented the multiplication 'operator' function for the same.
- The multiplication is implemented using the basic `hpx::experimental::for_loop` with wrapped in a `hpx::async` task. All race conditions were considered and solved.
- All matrices were built randomly using the popular `std::default_random_engine`
- Different benchmarks are calculated to test the result against the basic `O(n^3)` solution and others as well.
