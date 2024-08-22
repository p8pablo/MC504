// MC504 Simple-Shell
// Alunos:
//  Pablo Henrique Almeida Mendes RA:230977
//  Luiz Felipe Corradini Rego Costa: RA:230613
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_PATHS 1024
#define MAX_COMMAND_LENGTH 1024

void parse_directories(char *path, char **directories)
{
    int i = 0;
    char *dir = strtok(path, ":");
    while (dir != NULL)
    {
        directories[i++] = dir;
        dir = strtok(NULL, ":");
    }
    directories[i] = NULL; // Null-terminate the array
}

char *find_command(char **directories, char *command)
{
    static char path[MAX_COMMAND_LENGTH];
    for (int i = 0; directories[i] != NULL; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", directories[i], command);
        if (access(path, X_OK) == 0)
        {
            return path; // Command found
        }
    }
    return NULL; // Command not found
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <directories>\n", argv[0]);
        return 1;
    }

    char *directories[MAX_PATHS];
    parse_directories(argv[1], directories);

    char command_line[MAX_COMMAND_LENGTH];
    printf("simple-shell$: ");
    if (fgets(command_line, sizeof(command_line), stdin) == NULL)
    {
        perror("fgets failed");
        return 1;
    }

    // Remove newline character
    command_line[strcspn(command_line, "\n")] = '\0';

    // Separate command and arguments
    char *command = strtok(command_line, " ");
    char *args[MAX_PATHS];
    int i = 0;
    while (command != NULL)
    {
        args[i++] = command;
        command = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Find the command in the provided directories
    char *command_path = find_command(directories, args[0]);
    if (command_path != NULL)
    {
        if (execv(command_path, args) == -1)
        {
            perror("execv failed");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Command not found: %s\n", args[0]);
        return 1;
    }

    return 0;
}
