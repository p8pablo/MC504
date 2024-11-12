#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define LINE_LENGTH 101
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
#define O_CREAT 0x200
#define O_APPEND 0x400
#define O_EXCL 2048
#define TOTAL_LINES 100
#define SWAPS 50
#define MAX_INT 2147483647
#define O_TRUNC 0x400


char lines_file[TOTAL_LINES][LINE_LENGTH];

int total_file_time = 0;

// Função para gerar uma string aleatória
void generate_random_string(char *str, int length)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length - 1; i++)
    {
        int key = randomrange(0, sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0'; // Null-terminate a string
}

// Função para escrever uma linha aleatória no arquivo
void write_random_line_to_file(char *filename, int fd)
{
    char line[LINE_LENGTH];
    generate_random_string(line, 100);
    line[100] = '\n';

    int start = uptime();
    if (write(fd, line, LINE_LENGTH) != LINE_LENGTH)
    {
        printf(1, "Could not write on file.\n");
        return;
    }

    total_file_time += uptime() - start;
}

// Implementação de strlen personalizada
int strlen_custom(char *s)
{
    int len = 0;
    while (s[len] != '\0')
    {
        len++;
    }
    return len;
}

// Implementação de strcpy personalizada
void strcpy_custom(char *dest, char *src, int dest_pos)
{
    int i = 0;
    while (src[i] != '\0' && (dest_pos + i) < 255)
    { // Garantir limite de buffer
        dest[dest_pos + i] = src[i];
        i++;
    }
    dest[dest_pos + i] = '\0'; // Null-terminate
}

// Função para atualizar o arquivo com 50 novas permutações
void permute_lines(char *filename)
{
    int fd = open(filename, O_RDWR | O_CREAT);
    if (fd < 0)
    {
        printf(1, "Could not open file for permutation\n");
        return;
    }

    int start = uptime();
    for (int i = 0; i < TOTAL_LINES; i++)
        read(fd, lines_file[i], LINE_LENGTH);
    total_file_time += uptime() - start;

    // Realizar 50 permutações
    for (int i = 0; i < SWAPS; i++)
    {
        int a = randomrange(0, 100);
        int b = randomrange(0, 100);

        char temp[LINE_LENGTH];
        for (int j = 0; j < LINE_LENGTH; j++)
            temp[j] = lines_file[a][j];
        for (int j = 0; j < LINE_LENGTH; j++)
            lines_file[a][j] = lines_file[b][j];
        for (int j = 0; j < LINE_LENGTH; j++)
            lines_file[b][j] = temp[j];
    }

    char lock_filename[256];
    strcpy_custom(lock_filename, filename, 0);
    strcpy_custom(lock_filename, ".lock", strlen_custom(lock_filename));
    char* my_itoa(int value, char *str);

    int fd_lock;
    int max_retries = 100;
    int retries = 0;
    char pid_str[10];

    // Obtenha o PID do processo atual
    int pid = getpid();
    my_itoa(pid, pid_str);

    while ((fd_lock = open(lock_filename, O_WRONLY | O_CREAT | O_EXCL)) < 0)
    {
        // Tente ler o PID do lock file existente
        int fd_existing = open(lock_filename, O_RDONLY);
        if (fd_existing >= 0)
        {
            char existing_pid_str[10] = {'\0'};
            read(fd_existing, existing_pid_str, sizeof(existing_pid_str));
            close(fd_existing);

            int existing_pid = atoi(existing_pid_str);
            if (kill(existing_pid) < 0)
            {
                // O processo que criou o lock não existe mais, então removemos o lock file
                if (unlink(lock_filename) == 0)
                {
                    printf(1, "Stale lock removed successfully (PID %d).\n", existing_pid);
                }
                else
                {
                    printf(1, "Failed to remove stale lock. Retrying...\n");
                }
            }
        }

        sleep(2);
        retries++;
        if (retries >= max_retries)
        {
            printf(1, "Error: Timeout while waiting for the lock. Retrying after removing stale lock.\n");
            retries = 0; // Reset retries para continuar tentando
        }
    }

    // Lock adquirido, escreva o PID no lock file
    fd_lock = open(lock_filename, O_WRONLY | O_CREAT | O_TRUNC);
    write(fd_lock, pid_str, strlen_custom(pid_str));
    close(fd_lock);

    int fd_write = open(filename, O_WRONLY | O_CREAT);
    if (fd_write < 0)
    {
        printf(1, "Error: Cannot open file for writing\n");
        close(fd_lock);
        unlink(lock_filename);
        return;
    }

    for (int i = 0; i < TOTAL_LINES; i++)
    {
        if (write(fd_write, lines_file[i], LINE_LENGTH) != LINE_LENGTH)
        {
            printf(1, "Error: Failed to write line %d\n", i + 1);
            close(fd_write);
            return;
        }
    }

    unlink(lock_filename);
    close(fd_lock);
    close(fd_write);
    close(fd);
}

// Função itoa simplificada
char *my_itoa(int value, char *str)
{
    if (value < 0 || value > 30)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    if (value < 10)
    {
        str[0] = '0' + value;
        str[1] = '\0';
    }
    else
    {
        str[0] = '0' + (value / 10);
        str[1] = '0' + (value % 10);
        str[2] = '\0';
    }
    return str;
}

// Função para concatenar strings
char *my_strcat(char *dest, const char *src)
{
    while (*dest)
    {
        dest++;
    }

    while (*src)
    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';
    return dest;
}

// Função de tarefa de I/O
int io_bound_task(int rodada)
{
    char rodada_str[3] = {'\0'};
    my_itoa(rodada, rodada_str);

    char filename[256] = "testfile";
    my_strcat(filename, rodada_str);
    my_strcat(filename, ".txt");

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND);
    if (fd < 0)
    {
        printf(1, "Could not open file\n");
        return -1;
    }

    for (int i = 0; i < 100; i++)
    {
        write_random_line_to_file(filename, fd);
    }

    close(fd);
    permute_lines(filename);

    int start = uptime();
    unlink(filename);
    total_file_time += uptime() - start;

    return total_file_time;
}
