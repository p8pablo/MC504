#include "types.h"
#include "stat.h"
#include "user.h"

#define LINE_LENGTH 101
#define MAX_LINES 100
#define RAND_MAX_32 ((1U << 30) - 1)
#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2
#define O_CREAT  0x200
#define O_APPEND  0x400
#define TOTAL_LINES 100
#define SWAPS 50
#define MAX_INT 2147483647

char lines_file[TOTAL_LINES][LINE_LENGTH];

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
void write_random_line_to_file(const char *filename, int fd)
{
    char line[LINE_LENGTH];

    generate_random_string(line, 100);

    // Append a newline character at the end
    line[100] = '\n';

    if (write(fd, line, LINE_LENGTH) != LINE_LENGTH) {
        printf(1, "Could not write on file.\n");
        return;
    }

}

// Function to update file with 50 new permutations
void permute_lines(const char *filename)
{
 
    // Open the file for reading and writing
    int fd = open(filename, O_RDWR | O_CREAT);
    if(fd < 0){
        printf(1, "Could not open file for permutation\n");
        return;
    }

    // Read all lines into an array
    for(int i = 0; i < TOTAL_LINES; i++) read(fd, lines_file[i], LINE_LENGTH);

    // Perform 50 swaps
    for(int i = 0; i < SWAPS; i++) {
        int a = randomrange(0, 100);
        int b = randomrange(0, 100);
        
        char temp[LINE_LENGTH];
        for(int j = 0; j < LINE_LENGTH; j++) temp[j] = lines_file[a][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines_file[a][j] = lines_file[b][j];
        for(int j = 0; j < LINE_LENGTH; j++) lines_file[b][j] = temp[j];
    }

    // Reopen the file to rewrite all changed lines
    int fd_write = open(filename, O_WRONLY | O_CREAT);
    if(fd_write < 0){
        printf(1, "Error: Cannot open file for writing\n");
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

    // Close the file descriptor
    close(fd);
}

void io_bound_task()
{
    char filename[] = "testfile.txt";

    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND);
    if (fd < 0) {
        printf(1, "Could open file\n");
        return;
    }

    // Write 100 random lines to file
    for (int i = 0; i < 100; i++)
    {
        write_random_line_to_file(filename, fd);
    }

    close(fd);

    // Permutating lines
    permute_lines(filename);

    // Delete file
    unlink(filename);
}