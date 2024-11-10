#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define LINE_LENGTH 101
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
// #define O_RDONLY  0
// #define O_WRONLY  1
// #define O_RDWR    2
#define O_CREAT  0x200
#define O_APPEND  0x400
#define O_EXCL 2048
#define TOTAL_LINES 100
#define SWAPS 50
#define MAX_INT 2147483647

char lines_file[TOTAL_LINES][LINE_LENGTH];

int total_file_time = 0;

// Function to generate a random string of characters
void generate_random_string(char *str, int length)
{
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length - 1; i++)
    {
        int key = randomrange(0, sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0'; // Null-terminate the string
}

// Function to write a random line to the file
void write_random_line_to_file( char *filename, int fd)
{
    char line[LINE_LENGTH];

    generate_random_string(line, 100);

    // Append a newline character at the end
    line[100] = '\n';

    int start = uptime();
    if (write(fd, line, LINE_LENGTH) != LINE_LENGTH) {
        printf(1, "Could not write on file.\n");
        return;
    }

    total_file_time += uptime() - start;

}

// Strlen implementation
int strlen_custom(char *s) {
    int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

// strcpy implementation
void strcpy_custom(char *dest, char *src, int dest_pos) {
    int i = 0;
    while (src[i] != '\0' && (dest_pos + i) < 255) { // Ensure buffer limit
        dest[dest_pos + i] = src[i];
        i++;
    }
    dest[dest_pos + i] = '\0'; // Null-terminate
}

// Function to update file with 50 new permutations
void permute_lines(char *filename)
{
 
    // Open the file for reading and writing
    int fd = open(filename, O_RDWR | O_CREAT);
    if(fd < 0){
        printf(1, "Could not open file for permutation\n");
        return;
    }

    // Read all lines into an array
    int start = uptime();
    for(int i = 0; i < TOTAL_LINES; i++) read(fd, lines_file[i], LINE_LENGTH);
    total_file_time += uptime() - start;

    // Perform 50 swaps
    for(int i = 0; i < SWAPS; i++) {
        int a = randomrange(0, 100);
        int b = randomrange(0, 100);
        
        char temp[LINE_LENGTH];
        for(int j = 0; j < LINE_LENGTH; j++) temp[j] = lines_file[a][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines_file[a][j] = lines_file[b][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines_file[b][j] = temp[j];
    }
    
    // Define the lock file name
    char lock_filename[256];

    // Copy the original filename to lock_filename
    strcpy_custom(lock_filename, filename, 0);

    // Append the ".lock" suffix
    strcpy_custom(lock_filename, ".lock", strlen_custom(lock_filename));

    int fd_lock;

    int max_retries = 100;
    int retries = 0;
    printf(1, "pre-loop\n");
    while ((fd_lock = open(lock_filename, O_WRONLY | O_CREATE | O_EXCL)) < 0) {
        sleep(2);
        retries++;
        printf(1, "%d esimo loop com fd_lock: %d\n", retries, fd_lock);
        if (retries >= max_retries) {
            printf(1, "Error: Timeout while waiting for the lock. Attempting to remove stale lock.\n");
            unlink(lock_filename);
            retries = 0; // Reset retries and attempt to acquire the lock again
        }
    }

    // Reopen the file to rewrite all changed lines
    int fd_write = open(filename, O_WRONLY | O_CREATE);
        
    if(fd_write < 0){
        printf(1, "Error: Cannot open file for writing\n");
        // Release the lock before returning
        close(fd_lock);                // Close the lock file descriptor
        unlink(lock_filename);         // Remove the lock file
        return;
    }

    // Rewriting all lines into the file
    for(int i = 0; i < TOTAL_LINES; i++) {
        if(write(fd_write, lines_file[i], LINE_LENGTH) != LINE_LENGTH){
            printf(1, "Error: Failed to write line %d\n", i+1);
            close(fd_write);
            return;
        }
    }

    unlink(lock_filename);
    close(fd_lock);
    // Close the file descriptor
    close(fd);
}


// Simple itoa function for numbers between 0 and 30
char* my_itoa(int value, char *str) {
    if (value < 0 || value > 30) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    if (value < 10) {
        str[0] = '0' + value;
        str[1] = '\0';
    } else {
        str[0] = '0' + (value / 10);
        str[1] = '0' + (value % 10);
        str[2] = '\0';
    }
    return str;
}

char* my_strcat(char *dest, const char *src) {
    // Move the dest pointer to the end of the current string
    while (*dest) {
        dest++;
    }
    
    // Copy characters from src to dest
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    
    // Null-terminate the concatenated string
    *dest = '\0';
    
    return dest;
}

int io_bound_task(int rodada)
{

    char *rodada_str = {'\0'};
    rodada_str = my_itoa(rodada, rodada_str);

    char filename[] = "testfile";
    my_strcat(filename, rodada_str);
    char end[] = ".txt";
    my_strcat(filename, end);

    printf(1, "filename: %s\n", filename);

    // printf(1, "chegou aq\n");
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND);
    if (fd < 0) {
        printf(1, "Could not open file\n");
        return -1;
    }

    // Write 100 random lines to file
    for (int i = 0; i < 100; i++)
    {
        write_random_line_to_file(filename, fd);
    }

    close(fd);

    // Permutating lines
    permute_lines(filename);

    int start = uptime();
    // Delete file
    unlink(filename);
    total_file_time += uptime() - start;

    return total_file_time;
}