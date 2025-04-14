import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Read the CSV file
data = pd.read_csv('benchmark_results.csv')

# Set the plotting style
sns.set(style="whitegrid")
plt.rcParams.update({'figure.max_open_warning': 0})  # Avoid warnings

# Define the metrics and labels
metrics = {
    'speedup': 'Speedup',
    'efficiency': 'Efficiency',
    'throughput': 'Throughput',
    'latency_per_task': 'Latency per Task (s)',
}

# Create a directory to store the graphs if it doesn't exist
output_dir = 'graphs'
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# Function to plot metric vs number of threads for each matrix size
def plot_metric_vs_threads(metric):
    plt.figure(figsize=(8, 6))
    for size in sorted(data['n'].unique()):
        subset = data[data['n'] == size]
        plt.plot(subset['num_threads'], subset[metric], marker='o', label=f'Size {size}x{size}')
    plt.xlabel('Number of Threads')
    plt.ylabel(metrics[metric])
    plt.title(f'{metrics[metric]} vs Number of Threads')
    plt.legend()
    plt.xscale('log', base=2)  # Optional: Log scale for clarity
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.tight_layout()
    # Save the plot inside the 'graphs' directory
    plt.savefig(os.path.join(output_dir, f'{metric}_vs_threads.png'))
    plt.close()

# Function to plot metric vs matrix size for each thread count
def plot_metric_vs_size(metric):
    plt.figure(figsize=(8, 6))
    for threads in sorted(data['num_threads'].unique()):
        subset = data[data['num_threads'] == threads]
        plt.plot(subset['n'], subset[metric], marker='o', label=f'{threads} Threads')
    plt.xlabel('Matrix Size (n = m = k)')
    plt.ylabel(metrics[metric])
    plt.title(f'{metrics[metric]} vs Matrix Size')
    plt.legend()
    plt.xscale('log', base=2)  # Optional: Log scale for clarity
    plt.grid(True, which='both', linestyle='--', linewidth=0.5)
    plt.tight_layout()
    # Save the plot inside the 'graphs' directory
    plt.savefig(os.path.join(output_dir, f'{metric}_vs_size.png'))
    plt.close()

# Generate all plots
for metric in metrics.keys():
    plot_metric_vs_threads(metric)
    plot_metric_vs_size(metric)