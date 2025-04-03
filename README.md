# Parallel Matrix Multiplication using HPX

## Description
This project implements **parallel matrix multiplication** using **HPX**, an **asynchronous many-task runtime**.  
The code utilizes **HPX parallel algorithms** to distribute computation efficiently across multiple cores.

### **Features**
- Uses **HPX’s parallel execution policies** (`hpx::execution::par`).
- Implements **asynchronous execution** with `hpx::future<T>`.
- Dynamically generates random matrices.
- Accepts **custom matrix sizes** via command-line arguments.
- Outputs execution time for benchmarking.

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

### **Sample input-output**

### **Benchmark Results**

### **Implementation Details**
