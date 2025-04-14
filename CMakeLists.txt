cmake_minimum_required(VERSION 3.30)
project(parallel_matrix_multi)
find_package(HPX REQUIRED)
#set(CMAKE_CXX_STANDARD 20)

add_executable(parallel_matrix_multi src/main.cpp
        src/benchmarks.cpp
        include/Parallel_Matrix.hpp
        include/Sequential_Matrix.hpp
        include/benchmarks.hpp
)
target_link_libraries(parallel_matrix_multi
        PRIVATE
        HPX::hpx
        HPX::wrap_main
        HPX::iostreams_component
        /opt/homebrew/opt/hwloc/lib/libhwloc.15.dylib
        "-framework CoreFoundation"
        "-framework IOKit"
)