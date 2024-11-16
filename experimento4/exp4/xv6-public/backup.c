// void run_experiment(int cpu_count, int io_count)
// {
//     printf(1, "=============== CPU ===============\n");
    
//     int start_time = uptime();
//     int current_throughput = 0;
//     int iterations_throughput = 0;

//     // Executando o experimento de CPU
//     for (int i = 0; i < cpu_count; i++)
//     {
//         int fd[2]; // file descriptors
   
//         if (pipe(fd) < 0) { // Pipe to obtain information that were previously exclusive to the child process
//             printf(1, "Criação do Pipe falhou!\n");
//             exit();
//         }

//         if (fork() == 0)
//         {
//             // Close the read end of the pipe; child only writes
//             close(fd[0]);

//             // Perform the CPU-bound task
//             int current_m_over = cpu_bound_task();

//             // Write the result to the pipe
//             write(fd[1], &current_m_over, sizeof(current_m_over));

//             // Close the write end after writing
//             close(fd[1]);
//             exit();
//         }

//         // Processo Pai
//         close(fd[1]); // fechar write inutilizado

//         int child_result = 0;

//         if (read(fd[0], &child_result, sizeof(child_result)) < 0) {
//             printf(1, "Leitura do Pipe falhou\n");
//             exit();
//         }

//         close(fd[0]); // fechar o read após ler

//         wait();
        
//         sum_m_over += measure_memory_overhead(child_result); // atualizando variavel global        
        
//         completed_count++;

//         // Adiciona o tempo de execução para cálculo de J_cpu
//         int exec_time = uptime() - start_time;
//         sum_exec_times += exec_time;
//         sum_exec_times_squared += exec_time * exec_time;

//         // Check if one second has passed (for throughput analysis)
//         if (uptime() - start_time >= 100) { // 100 ticks are, approximately, 1 second in xv6

//             iterations_throughput += 1;

//             current_throughput = completed_count;
//             if (current_throughput > max_throughput) max_throughput = current_throughput;
//             if (current_throughput < min_throughput) min_throughput = current_throughput;

//             // Reset f´or the next interval
//             completed_count = 0;
//             start_time = uptime();

//             sum_throughput += calculate_throughput(max_throughput, min_throughput, current_throughput);
//         }
        
//     }

//     printf(1, "Vazao normalizada: \n");
//     if (iterations_throughput == 0) printf(1, "0\n");
//     else print_float(sum_throughput / iterations_throughput);

//     // Exibir J_cpu após o experimento de CPU
//     display_j_cpu(cpu_count);

//     // Exibindo a média dos overheads de memória
//     printf(1, "\nOverhead de Gerenciamento de memoria: \n");
//     print_float(sum_m_over / cpu_count);

//     // Executando o experimento do I/O bound
//     printf(1, "\n=============== I/O bound ===============\n");

//     completed_count = 0;
//     max_throughput = 0;
//     min_throughput = MAX_INT;
//     start_time = uptime();
//     current_throughput = 0;
//     sum_throughput = 0;
//     iterations_throughput = 0;

//     for (int i = 0; i < io_count; i++) {

//         int fd[2]; // file descriptors
   
//         if (pipe(fd) < 0) { // Pipe to obtain information that were previously exclusive to the child process
//             printf(1, "Criação do Pipe falhou!\n");
//             exit();
//         }


//         // printf(1, "chegou aq\n");
//         if (fork() == 0) {
//                         // Close the read end of the pipe; child only writes
//             close(fd[0]);

//             // Perform the CPU-bound task
//             int current_m_over = io_bound_task();


//             // Write the result to the pipe
//             write(fd[1], &current_m_over, sizeof(current_m_over));

//             // Close the write end after writing
//             close(fd[1]);
//             exit();
//         }

//         // Processo Pai
//         close(fd[1]); // fechar write inutilizado

//         int child_result = 0;

//         if (read(fd[0], &child_result, sizeof(child_result)) < 0) {
//             printf(1, "Leitura do Pipe falhou\n");
//             exit();
//         }

//         close(fd[0]); // fechar o read após ler

//         wait();

//         sum_file_calc += measure_memory_overhead(child_result);


//         // printf(1, "esperando outras acabarem\n");
//         completed_count++;

//         // Check if one second has passed (for throughput analysis)
    //     if (uptime() - start_time >= 100) { // 100 ticks are, approximately, 1 second in xv6
    //         // printf(1, "mais um tick\n");

    //         iterations_throughput += 1;
    //         int current_throughput = completed_count;
    //         sum_throughput += completed_count;
    //         if (current_throughput > max_throughput) max_throughput = current_throughput;
    //         if (current_throughput < min_throughput) min_throughput = current_throughput;

    //         // Reset for the next interval
    //         completed_count = 0;
    //         start_time = uptime();

    //         sum_throughput += (calculate_throughput(max_throughput, min_throughput, current_throughput));
            
    //     }
    // }

//     // Print results
//     printf(1, "\nVazao normalizada: \n");
//     print_float(sum_throughput / iterations_throughput);

//     printf(1, "\nEficiencia do sistema de arquivos: \n");
//     print_float(sum_file_calc / io_count);

//     // Wait for all processes to finish
//     for (int i = 0; i < cpu_count + io_count; i++)
//     {
//         wait();
//     }
// }