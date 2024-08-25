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

// Função para dividir os diretorios da entrada
void parse_directories(char *path, char **directories)
{
    int i = 0;
    char *dir = strtok(path, ":");
    while (dir != NULL)
    {
        directories[i++] = dir;
        dir = strtok(NULL, ":");
    }
    directories[i] = NULL;
}

char *find_command(char **directories, char *command)
{
    static char path[MAX_COMMAND_LENGTH];
    for (int i = 0; directories[i] != NULL; i++)
    {
        snprintf(path, sizeof(path), "%s/%s", directories[i], command);
        if (access(path, X_OK) == 0)
        {
            return path; // Comando achado!
        }
    }
    return NULL; // Comando não achado!
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Please insert directories\n");
        return 1;
    }

    char *directories[MAX_PATHS];
    parse_directories(argv[1], directories);

    char command_line[MAX_COMMAND_LENGTH];
    printf("simple-shell$: ");

    // Recebe os comandos
    if (fgets(command_line, sizeof(command_line), stdin) == NULL)
    {
        printf("fgets failed");
        return 1;
    }

    command_line[strcspn(command_line, "\n")] = '\0';

    // Separa comando e argumentos
    char *command = strtok(command_line, " ");
    char *args[MAX_PATHS];
    int i = 0;
    while (command != NULL)
    {
        args[i++] = command;
        command = strtok(NULL, " ");
    }
    args[i] = NULL;

    // Procura o comando nas pastas dadas inicialmente e executa caso achado
    char *command_path = find_command(directories, args[0]);
    if (command_path != NULL)
    {
        if (execv(command_path, args) == -1)
        {
            printf("execv failed");
            return 1;
        }
    }
    else
    {
        printf("Please insert command\n");
        return 1;
    }

    return 0;
}
