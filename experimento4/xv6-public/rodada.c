// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"

#define INF 1e9 // Representing infinite distance
#define MAX_INT 2147483647
#define MAX_ITERATIONS 30
int rodada = 0;
int files_result;
int memory_result;

// VAZÃO
int completed_count = 0;
int max_throughput = 0;
int min_throughput = MAX_INT;
int sum_throughput = 0;

// MEMORY OVERHEAD
int sum_m_over = 0;

// EFICIÊNCIA DO SISTEMA DE ARQUIVOS
int sum_file_time = 0;
int min_file_time = MAX_INT;
int max_file_time = 0;

// JUSTIÇA ENTRE PROCESSOS
int sum_exec_times = 0;      // Soma dos tempos de execução (∑x_i)
int sum_exec_times_squared = 0; // Soma dos quadrados dos tempos de execução (∑x_i^2)


void print_float(int x){
    // Função para printar um float, recebendo-o como o valor original multiplicado por 1000
    int norm_factor = 1000;
    int integer_part = x / norm_factor;
    int fractional_part = x % norm_factor;

    if (x < 100){
        printf(1, "%d.0%d\n", integer_part, fractional_part);
    }else{
        printf(1, "%d.%d\n", integer_part, fractional_part);
    }
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


int calculate_throughput(int max_throughput, int min_throughput, int sum_throughput, int num_execs){

    // printf(1, "max: %d, min: %d, sum: %d\n", max_throughput, min_throughput, sum_throughput);
    int current_throughput = sum_throughput / num_execs;   // Esperança da vazão
    int normalized_throughput = 1;
    int norm_factor = 1000;
    if (max_throughput != min_throughput) {
        normalized_throughput = norm_factor - (current_throughput - min_throughput) * norm_factor / (max_throughput - min_throughput);
    }
    return normalized_throughput;
}

int max(int a, int b){
    if (a > b) return a;
    else return b;
}

void run_experiment(int cpu_count, int io_count)
{
    
    // VAZÃO
    int start_time_throughput = uptime();
    int iterations_throughput = 0;

    // JUSTIÇA ENTRE PROCESSOS
    int start_time_justica = uptime();

    // SALVAR RESULTADO DA EFICIÊNCIA
    int files_result = 0;

    // SALVAR TEMPOS TOTAIS
    int (*fd_memory)[2] = malloc(MAX_ITERATIONS * sizeof(int[2]));
    int (*fd_files)[2] = malloc(MAX_ITERATIONS * sizeof(int[2]));

    // INÍCIO DA EXECUÇÃO
    for(int i = 0; i < max(cpu_count, io_count); i++){

        // ITERAÇÃO CPU
        if(i < cpu_count){ // Ainda está no número de acessos da CPU

            if (pipe(fd_memory[i]) < 0) {
                printf(1, "Criacao do %d Pipe falhou! (1)\n", i);
                free(fd_memory);
                exit();
            }

            if (fork() == 0){
                
                close(fd_memory[i][0]);

                int total_cpu_time = cpu_bound_task();

                write(fd_memory[i][1], &total_cpu_time, sizeof(total_cpu_time));
                close(fd_memory[i][1]);
                exit();
            }
            close(fd_memory[i][1]);

            // Read I/O time
            if (read(fd_memory[i][0], &memory_result, sizeof(memory_result)) < 0) {
                printf(1, "Leitura do %d Pipe de Arquivos falhou\n", i);
                exit();
            }
            close(fd_memory[i][0]); // Close read end after reading

            completed_count ++; // Para a vazão

        }

        // ITERAÇÃO I/O
        if(i < io_count){

            if (pipe(fd_files[i]) < 0) {
                printf(1, "Criacao do %d Pipe falhou! (2)\n", i);
                free(fd_files);
                exit();
            }

            if (fork() == 0){
                
                close(fd_files[i][0]);

                int total_io_time = io_bound_task(i % 10);
                
                write(fd_files[i][1], &total_io_time, sizeof(total_io_time));
                close(fd_files[i][1]);
                exit();
            }

            close(fd_files[i][1]);

            // Read I/O time
            if (read(fd_files[i][0], &files_result, sizeof(files_result)) < 0) {
                printf(1, "Leitura do %d Pipe de Arquivos falhou\n", i);
                exit();
            }
            close(fd_files[i][0]); // Close read end after reading

            completed_count ++ ;// Para a vazão
        }

        // VAZÃO
        if (uptime() - start_time_throughput >= 100) {

            iterations_throughput += 1;

            int current_throughput = completed_count;
            if (current_throughput > max_throughput) max_throughput = current_throughput;
            if (current_throughput < min_throughput) min_throughput = current_throughput;

            sum_throughput += completed_count;

            completed_count = 0;
            start_time_throughput = uptime();

        }

        // JUSTIÇA ENTRE PROCESSOS
        int exec_time = uptime() - start_time_justica;

        sum_exec_times += exec_time;
        sum_exec_times_squared += exec_time * exec_time;

        // EFICIÊNCIA DO SISTEMA DE ARQUIVOS
        int current_total_time = files_result;
        sum_file_time += current_total_time;

        // MEMORY OVERHEAD
        sum_m_over += memory_result;

    }

    // Esperar para todos os processos terminarem
    for (int i = 0; i < cpu_count + io_count; i++)
    {
        wait();
    }

    // VAZAO
    printf(1, "Vazao: ");
    int final_throughput = calculate_throughput(max_throughput, min_throughput, sum_throughput, iterations_throughput);
    print_float(final_throughput);
    printf(1, "\n");
    
    // JUSTIÇA ENTRE PROCESSOS
    printf(1, "Justica entre processos: ");
    int final_justice = calculate_j_cpu(sum_exec_times, sum_exec_times_squared, cpu_count + io_count);
    print_float(final_justice);
    printf(1, "\n");

    // EFICIÊNCIA DO SISTEMA DE ARQUIVOS
    printf(1, "Eficiencia do sistema de arquivos: ");
    print_float(io_count * 1000 / sum_file_time);
    printf(1, "\n");

    // MEMORY OVERHEAD
    printf(1, "Memory overhead: ");
    print_float(cpu_count * 1000 / sum_m_over);
    printf(1, "\n");
    
    // DESEMPENHO TOTAL DO SISTEMA
    printf(1, "DESEMPENHO GERAL DO SISTEMA: ");
    int total_ans = (final_throughput * 250)/1000 + (final_justice * 250)/1000 + (250 * io_count/sum_file_time) + (250 * cpu_count/sum_m_over);
    print_float(total_ans);
    printf(1, "\n");


    // REINICIANDO VARIÁVEIS DA SOMA
    sum_throughput = 0;
    sum_m_over = 0;
    sum_file_time = 0;
    sum_exec_times = 0;
    sum_exec_times_squared = 0;

    free(fd_memory);
    free(fd_files);

}
