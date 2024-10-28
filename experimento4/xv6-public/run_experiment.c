// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"

#define INF 1e9 // Representing infinite distance
#define MAX_INT 2147483647

// Flags to help calculate i/o latency system calls
int min_io_latency = INF;
int max_io_latency = 0;
int sum_latencies = 0;

int completed_count = 0;
int max_throughput = 0;
int min_throughput = MAX_INT;

void print_float(int x){
    // Integer and fractional parts for display
    int norm_factor = 1000;
    int integer_part = x / norm_factor;
    int fractional_part = x % norm_factor;

    // Print results without formatted padding
    printf(1, "%d.%d\n", integer_part, fractional_part);
}

int calculate_io_latency(int sum_latencies, int count_latencies, int min_io_latency, int max_io_latency){

    int norm_factor = 1000;

    int avg_latency_scaled = (sum_latencies * norm_factor) / (count_latencies);

    int norm_io_latency = norm_factor - ((avg_latency_scaled - (min_io_latency * norm_factor)) / (max_io_latency - min_io_latency));

    return norm_io_latency;
    // printf(1, "sum: %d, min: %d, max: %d\n", sum_latencies, min_io_latency, max_io_latency);
}

int calculate_throughput(int max_throughput, int min_throughput, int current_throughput){
    int normalized_throughput = 1;
    int norm_factor = 1000;
    if (max_throughput != min_throughput) {
        normalized_throughput = norm_factor - (current_throughput - min_throughput) * norm_factor / (max_throughput - min_throughput);
    }
    return normalized_throughput;
}


void run_experiment(int cpu_count, int io_count)
{
    printf(1, "=============== CPU ===============\n");

    int start_time = uptime();
    int current_throughput = 0;

    // Executando o experimento de CPU
    for (int i = 0; i < cpu_count; i++)
    {
        if (fork() == 0)
        {
            cpu_bound_task();
            exit();
        }
        wait();
        completed_count++;

        // Check if one second has passed (for throughput analysis)
        if (uptime() - start_time >= 100) { // 100 ticks are, approximately, 1 second in xv6

            int current_throughput = completed_count;
            if (current_throughput > max_throughput) max_throughput = current_throughput;
            if (current_throughput < min_throughput) min_throughput = current_throughput;

            // Reset for the next interval
            completed_count = 0;
            start_time = uptime();
        }
    }

    printf(1, "Vazao Normalizada:\n");
    printf(1, "max: %d, min: %d, current: %d\n", max_throughput, min_throughput, current_throughput);
    print_float(calculate_throughput(max_throughput, min_throughput, current_throughput));


    // Executando o experimento do I/O bound
    printf(1, "=============== I/O bound ===============\n");

    completed_count = 0;
    max_throughput = 0;
    min_throughput = MAX_INT;
    start_time = uptime();
    current_throughput = 0;
    int sum_throughput = 0;
    int iterations_throughput = 0;

    for (int i = 0; i < io_count; i++) {
        int start_io_uptime = uptime();

        // printf(1, "chegou aq\n");
        if (fork() == 0) {
            io_bound_task();
            exit();
        }

        int end_io_uptime = uptime();
        int diff = end_io_uptime - start_io_uptime;
        sum_latencies += diff;

        if (diff > max_io_latency) max_io_latency = diff;
        if (diff < min_io_latency) min_io_latency = diff;

        // printf(1, "esperando outras acabarem\n");
        wait();
        completed_count++;

        // Check if one second has passed (for throughput analysis)
        if (uptime() - start_time >= 100) { // 100 ticks are, approximately, 1 second in xv6
            // printf(1, "mais um tick\n");


            iterations_throughput += 1;
            int current_throughput = completed_count;
            sum_throughput += completed_count;
            if (current_throughput > max_throughput) max_throughput = current_throughput;
            if (current_throughput < min_throughput) min_throughput = current_throughput;

            // Reset for the next interval
            completed_count = 0;
            start_time = uptime();
        }
    }


    // Print results
    printf(1, "Latencia de I/O Normalizada:\n");
    print_float(calculate_io_latency(sum_latencies, io_count, min_io_latency, max_io_latency));

    printf(1, "Vazao Normalizada:\n");
    printf(1, "max: %d, min: %d, current: %d\n", max_throughput, min_throughput, sum_throughput/iterations_throughput);
    print_float(calculate_throughput(max_throughput, min_throughput, sum_throughput/iterations_throughput));

    // Wait for all processes to finish
    for (int i = 0; i < cpu_count + io_count; i++)
    {
        wait();
    }
}

// passing command line arguments

int main(int argc, char *argv[])
{
    printf(1, "Comecando os Experimentos: \n");
    run_experiment(14, 20);
    exit();
}

// This code is contributed by sambhav228
