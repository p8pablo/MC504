// A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{

    int numero_rodadas = 30; // trocar para 30
    for (int rodada = 0; rodada < numero_rodadas; rodada++)
    {
        printf(1, "=============== RODADA %d ===============\n", rodada + 1);
        run_experiment(15, 15);
    }

    exit();
}
