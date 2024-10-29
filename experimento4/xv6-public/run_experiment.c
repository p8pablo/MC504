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

// Flags to help calculate memory overhead
int m_over_min = INF;
int m_over_max = 0;


int sum_exec_times = 0;      // Soma dos tempos de execução (∑x_i)
int sum_exec_times_squared = 0; // Soma dos quadrados dos tempos de execução (∑x_i^2)

void print_float(int x){
    // Integer and fractional parts for display
    int norm_factor = 1000;
    int integer_part = x / norm_factor;
    int fractional_part = x % norm_factor;

    // Print results without formatted padding
    printf(1, "%d.%d\n", integer_part, fractional_part);
}

// Função para calcular o overhead de gerenciamento de memória
int calculate_memory_overhead(int t_access, int t_alloc, int t_free) {
    return t_access + t_alloc + t_free;
}

// Função para calcular o overhead normalizado
int calculate_normalized_overhead(int m_over, int m_over_min, int m_over_max) {
    int norm_factor = 1000;
    if (m_over_max != m_over_min) {
        return norm_factor - ((m_over - m_over_min) * norm_factor) / (m_over_max - m_over_min);
    } else {
        return 1; // evita divisão por zero
    }
}
// Função para alocação de memória
int allocate_memory(int size) {
    // Aloca 'size' bytes de memória
    char *ptr = sbrk(size);
    if (ptr == (char*) -1) {
        // Falha na alocação
        return -1;
    }
    return 0; // Sucesso
}

// Função para a desalocação de memória
int deallocate_memory(int size) {
    // Desaloca 'size' bytes de memória
    char *ptr = sbrk(-size);
    if (ptr == (char*) -1) {
        // Falha na desalocação
        return -1;
    }
    return 0; // Sucesso
}
// Função principal de calculo de overhead de memória
void measure_memory_overhead(const char* label) {
    int start_alloc = uptime();
    if (allocate_memory(1024) == -1) { // Aloca 1024 bytes
        printf(1, "Falha na alocação de memória\n");
    }
    int t_alloc = uptime() - start_alloc;

    int start_free = uptime();
    if (deallocate_memory(1024) == -1) { // Desaloca os mesmos 1024 bytes
        printf(1, "Falha na desalocação de memória\n");
    }
    int t_free = uptime() - start_free;

    // Calcula o overhead de memória
    int m_over = calculate_memory_overhead(0, t_alloc, t_free);
    
    // Atualiza valores de m_over_min e m_over_max
    if (m_over < m_over_min) m_over_min = m_over;
    if (m_over > m_over_max) m_over_max = m_over;

    // Calcula o overhead normalizado
    int m_over_norm = calculate_normalized_overhead(m_over, m_over_min, m_over_max);

    // Exibe os resultados
    printf(1, "Overhead Normalizado de Memória (%s):\n", label);
    print_float(m_over_norm);
}


int calculate_j_cpu(int sum_exec_times, int sum_exec_times_squared, int process_count) {
    if (process_count == 0) return 0; // Evitar divisão por zero
    int norm_factor = 1000;

    int numerator = (sum_exec_times * sum_exec_times) / process_count;
    int denominator = sum_exec_times_squared;

    if (denominator == 0) return 0; // Outra proteção contra divisão por zero
    int j_cpu = (numerator * norm_factor) / denominator;
    return j_cpu;
}

// Função para calcular J_cpu e exibir o resultado
void display_j_cpu(int process_count) {
    int j_cpu = calculate_j_cpu(sum_exec_times, sum_exec_times_squared, process_count);
    printf(1, "\n");
    printf(1, "Justica entre Processos (J_cpu):\n");
    print_float(j_cpu);
}

int calculate_io_latency(int diff, int min_io_latency, int max_io_latency){

    int norm_factor = 1000;

    int avg_latency_scaled = diff * norm_factor;

    int norm_io_latency;
    if(max_io_latency != min_io_latency) norm_io_latency = norm_factor - ((avg_latency_scaled - (min_io_latency * norm_factor)) / (max_io_latency - min_io_latency));
    else return 1; // handle division by zero
    
    // printf(1, "sum: %d, min: %d, max: %d\n", sum_latencies, min_io_latency, max_io_latency);
    return norm_io_latency;
}

int calculate_average_io_latency(int sum, int count, int min_io_latency, int max_io_latency){

    int norm_factor = 1000;

    int avg_latency_scaled = sum * norm_factor / count;

    int norm_io_latency;
    if(max_io_latency != min_io_latency) norm_io_latency = norm_factor - ((avg_latency_scaled - (min_io_latency * norm_factor)) / (max_io_latency - min_io_latency));
    else return 1; // handle division by zero
    
    // printf(1, "sum: %d, min: %d, max: %d\n", sum_latencies, min_io_latency, max_io_latency);
    return norm_io_latency;
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
    int iterations_throughput = 0;

    // Executando o experimento de CPU
    for (int i = 0; i < cpu_count; i++)
    {
        if (fork() == 0)
        {
            cpu_bound_task();
            measure_memory_overhead("CPU");
            exit();
        }
        wait();
        completed_count++;

        // Adiciona o tempo de execução para cálculo de J_cpu
        int exec_time = uptime() - start_time;
        sum_exec_times += exec_time;
        sum_exec_times_squared += exec_time * exec_time;

        // Check if one second has passed (for throughput analysis)
        if (uptime() - start_time >= 100) { // 100 ticks are, approximately, 1 second in xv6

            iterations_throughput += 1;

            current_throughput = completed_count;
            if (current_throughput > max_throughput) max_throughput = current_throughput;
            if (current_throughput < min_throughput) min_throughput = current_throughput;

            // Reset for the next interval
            completed_count = 0;
            start_time = uptime();

            printf(1, "\n%d Vazao Normalizada:\n", iterations_throughput);
            // printf(1, "max: %d, min: %d, current: %d\n", max_throughput, min_throughput, sum_throughput/iterations_throughput);
            print_float(calculate_throughput(max_throughput, min_throughput, current_throughput));
        }
        
    }

    // Exibir J_cpu após o experimento de CPU
    display_j_cpu(cpu_count);


    // Executando o experimento do I/O bound
    printf(1, "\n=============== I/O bound ===============\n");

    completed_count = 0;
    max_throughput = 0;
    min_throughput = MAX_INT;
    start_time = uptime();
    current_throughput = 0;
    int sum_throughput = 0;
    iterations_throughput = 0;

    for (int i = 0; i < io_count; i++) {
        int start_io_uptime = uptime();

        // printf(1, "chegou aq\n");
        if (fork() == 0) {
            io_bound_task();
            measure_memory_overhead("I/O");
            exit();
        }
        wait();

        int end_io_uptime = uptime();
        int diff = end_io_uptime - start_io_uptime;
        sum_latencies += diff;

        if (diff > max_io_latency || min_io_latency == 0) max_io_latency = diff;
        if (diff < min_io_latency || max_io_latency == INF) min_io_latency = diff;

        // printf(1, "esperando outras acabarem\n");
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

            printf(1, "\n%d Vazao Normalizada:\n", iterations_throughput);
            // printf(1, "max: %d, min: %d, current: %d\n", max_throughput, min_throughput, sum_throughput/iterations_throughput);
            print_float(calculate_throughput(max_throughput, min_throughput, current_throughput));
            
        }
        printf(1, "\n\nExperimento %d)\n", i);

        // Print results
        printf(1, "Latencia de I/O Normalizada:\n");
        print_float(calculate_io_latency(diff, min_io_latency, max_io_latency));
        printf(1, "diff: %d, max: %d, min: %d\n", diff, max_io_latency, min_io_latency);

        // printf(1, "Vazao Normalizada:\n");
        // print_float(calculate_throughput(max_throughput, min_throughput, current_throughput));
    }

    // Print results
    printf(1, "Latencia media de I/O Normalizada:\n");
    print_float(calculate_average_io_latency(sum_latencies, io_count, min_io_latency, max_io_latency));

    // printf(1, "Vazao Normalizada:\n");
    // // printf(1, "max: %d, min: %d, current: %d\n", max_throughput, min_throughput, sum_throughput/iterations_throughput);
    // print_float(calculate_throughput(max_throughput, min_throughput, sum_throughput/iterations_throughput));

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
