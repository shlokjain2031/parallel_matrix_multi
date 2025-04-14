  #!/bin/bash

  # Output CSV header
  echo "n,m,k,num_threads,time_taken,speedup,efficiency,throughput,latency_per_task,overhead" > benchmark_results.csv

  # Define matrix sizes and thread counts
  matrix_sizes=(128 256 512 1024 2048)
  thread_counts=(1 2 4 8)

  # Loop over matrix sizes and thread counts
  for size in "${matrix_sizes[@]}"; do
    for threads in "${thread_counts[@]}"; do
      # Run your executable, capture output
      result=$(/Users/shlokjain/CLionProjects/parallel_matrix_multi/cmake-build-release/parallel_matrix_multi --n $size --m $size --k $size --hpx:threads $threads --s 1000 --l 0 --u 10000)

      # Extract the CSV line from program output
      csv_line=$(echo "$result" | grep -E '^[0-9]+,')  # Assumes your program prints only one CSV line per run

      # Append to CSV file
      echo "$csv_line" >> benchmark_results.csv

      echo "Completed: Matrix Size $size, Threads $threads"
    done
  done

  echo "Benchmarking completed! Results saved to benchmark_results.csv"

 # Run the Python script to plot the results
 echo "Generating plots..."
 /Users/shlokjain/CLionProjects/parallel_matrix_multi/.venv/bin/python /Users/shlokjain/CLionProjects/parallel_matrix_multi/benchmarks/benchmark_plotter.py

 echo "Plots generated and saved!"