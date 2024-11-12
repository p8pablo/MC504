#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define LINE_LENGTH 101
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
#define TOTAL_LINES 100
#define SWAPS 50
#define MAX_INT 2147483647

#define O_CREAT  0x200
#define O_APPEND 0x400
#define O_EXCL   0x800
#define O_TRUNC  0x1000  


char lines_file[TOTAL_LINES][LINE_LENGTH];

int total_file_time = 0;

void my_itoa(int value, char *str) {
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    // Process individual digits
    while (value != 0) {
        int rem = value % 10;
        str[i++] = rem + '0';
        value /= 10;
    }

    // If number is negative, append '-'
    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

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
    
    // Construct lock filename
    int len = 0;
    // Assuming strcpy_custom appends strings with offset
    // Implemented here manually
    for (; filename[len] != '\0' && len < 255; len++) {
        lock_filename[len] = filename[len];
    }
    // Append ".lock"
    char *lock_suffix = ".lock";
    int suffix_len = 0;
    while (lock_suffix[suffix_len] != '\0' && (len + suffix_len) < 255) {
        lock_filename[len + suffix_len] = lock_suffix[suffix_len];
        suffix_len++;
    }
    lock_filename[len + suffix_len] = '\0';

    int fd_lock;
    int retries = 0;
    char pid_str[12]; // Increased size to accommodate larger PIDs

    // Get current PID
    int pid = getpid();
    my_itoa(pid, pid_str);
    printf(1, "lock filename: %s\n", lock_filename);

    while ((fd_lock = open(lock_filename, O_WRONLY | O_CREAT | O_EXCL)) < 0) {
        // Lock file exists, attempt to read existing PID
        int fd_existing = open(lock_filename, O_RDONLY);
        if (fd_existing >= 0) {
            char existing_pid_str[12] = {'\0'};
            int n = read(fd_existing, existing_pid_str, sizeof(existing_pid_str) - 1);
            close(fd_existing);

            if (n > 0) {
                int existing_pid = atoi(existing_pid_str);
                if (existing_pid <= 0) {
                    // Invalid PID, assume stale lock
                    if (unlink(lock_filename) == 0) {
                        printf(1, "Stale lock removed (invalid PID: %d).\n", existing_pid);
                    } else {
                        printf(1, "Failed to remove stale lock. Retrying...\n");
                    }
                } else {
                    // Check if process with existing_pid is alive
                    if (kill(existing_pid) < 0) {
                        // Process doesn't exist, remove stale lock
                        if (unlink(lock_filename) == 0) {
                            printf(1, "Stale lock removed successfully (PID %d).\n", existing_pid);
                        } else {
                            printf(1, "Failed to remove stale lock. Retrying...\n");
                        }
                    } else {
                        // Process is still running, wait and retry
                        printf(1, "Lock held by PID %d. Retrying...\n", existing_pid);
                    }
                }
            } else {
                // Unable to read PID, assume stale lock
                if (unlink(lock_filename) == 0) {
                    printf(1, "Stale lock removed (unable to read PID).\n");
                } else {
                    printf(1, "Failed to remove stale lock. Retrying...\n");
                }
            }
        } else {
            // Unable to open existing lock file, possibly a race condition, retry
            printf(1, "Failed to open existing lock file. Retrying...\n");
        }

        sleep(2);
        retries++;
        if (retries >= 100) {
            printf(1, "Error: Timeout while waiting for the lock.\n");
            exit();
        }
    }

    fd_lock = open(lock_filename, O_WRONLY | O_CREAT | O_EXCL);
    if (fd_lock < 0) {
        // Handle lock acquisition failure
    } else {
        // Successfully acquired the lock, write PID directly
        if (write(fd_lock, pid_str, strlen_custom(pid_str)) < 0) {
            printf(1, "Error: Failed to write PID to lock file.\n");
            close(fd_lock);
            unlink(lock_filename); // Remove the lock file since writing failed
            exit();
        }
        printf(1, "Lock acquired successfully (PID %d).\n", pid);
        close(fd_lock);
        // Proceed with critical section
}


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

    printf(1, "total time: %d\n", total_file_time);
    return total_file_time;
}
