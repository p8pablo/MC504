// MC504 Simple-Shell
// Alunos:
//  Pablo Henrique Almeida Mendes RA:230977
//  Luiz Felipe Corradini Rego Costa: RA:230613

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_PATHS 1024
#define MAX_COMMAND_LENGTH 1024

void parse_directories(char *path, char **directories)
// Função para dividir os diretorios da entrada
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
// Função que itera pelos diretórios passados como parâmetro, procurando o comando desejado
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
    // Se tiver menos que 2 argumentos, então os diretórios não foram passados
    if (argc != 2)
    {
        printf("Please insert directories\n");
        return 1;
    }

    // Separando os diretórios
    char *directories[MAX_PATHS];
    parse_directories(argv[1], directories);

    // Loop do terminal, para que só termine com o exit
    while(1){
        
        // Mostrando o shell e definindo a linha de comando
        char command_line[MAX_COMMAND_LENGTH];
        printf("simple-shell$: ");

        // Recebe os comandos
        if (fgets(command_line, sizeof(command_line), stdin) == NULL)
        {
            printf("fgets failed");
            return 1;
        }

        // Tirando o '\n' da linha de comando
        command_line[strcspn(command_line, "\n")] = '\0';

        // Separa comando e argumentos
        char *command = strtok(command_line, " ");
        char *args[MAX_PATHS];
        int i = 0;
        
        // Itera pelos comandos passados, e coloca em "args"
        while (command != NULL)
        {
            args[i++] = command;
            command = strtok(NULL, " ");
        }

        // Seta o último args como null
        args[i] = NULL;

        // Procura o comando nas pastas dadas inicialmente, retorna PATH se achar e NULL se não
        char *command_path = find_command(directories, args[0]);

        // Se o comando não for exit (essa lógica tem que ser alterada)
        //      A ideia aqui é, caso seja != de exit, se cria o fork
        //      Porém, só o filho será executado, e o pai passará para a próxima alteração do while TRUE
        if (strcmp(args[0],"exit") != 0)
        {
            int id = fork();
            wait(NULL); // pai espera o filho acabar pra rodar
            
            if (execv(command_path, args) == -1)
            {
                printf("execv failed");
                return 1;
            }
        }
        else
        {
            break;
        
        }
        exit(0);
        
    }
    

    
    return 0;
}
