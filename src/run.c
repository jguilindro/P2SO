#include "stdlib.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#define SHMSZ 27
int main(void)
{
    int shmids, shmidq, shmidw, shmidt;
    char *shms, *shmq, *shmw, *shmt;
    key_t keys, keyq, keyw, keyt;
    //system("pwd");

    FILE *config = fopen("../config/parameters.txt", "r");

    char buffer[100];
    char param[10], value[10], mem[10];

    if (config == NULL)
    {
        printf("No se puede abrir el archivo de configuracion");
    }
    else
    {

        while (fgets(buffer, 100, config) != NULL)
        {
            sscanf(buffer, "%[^:]:%[^,],%s", param, value, mem);
            if (strcmp(param, "i") == 0)
            {
                //printf("%s %d %d\n",param, atoi(value), atoi(mem));
                keys = atoi(mem);
                if ((shmids = shmget(keys, SHMSZ, IPC_CREAT | 0666)) < 0)
                {
                    perror("shmget");
                    exit(1);
                }
                if ((shms = shmat(shmids, NULL, 0)) == (char *)-1)
                {
                    perror("shmat");
                    exit(1);
                }
                printf("Escribiendo el valor de I=%s en el espacio de mem compartida %s\n", value, mem);
                sprintf(shms, "%s", value);
            }

            if (strcmp(param, "q") == 0)
            {
                //printf("%s %d %d\n",param, atoi(value), atoi(mem));
                keyq = atoi(mem);
                if ((shmidq = shmget(keyq, SHMSZ, IPC_CREAT | 0666)) < 0)
                {
                    perror("shmget");
                    exit(1);
                }
                if ((shmq = shmat(shmidq, NULL, 0)) == (char *)-1)
                {
                    perror("shmat");
                    exit(1);
                }
                printf("Escribiendo el valor de Q=%s en el espacio de mem compartida %s\n", value, mem);
                sprintf(shmq, "%s", value);
            }

            if (strcmp(param, "w") == 0)
            {
                //printf("%s %d %d\n",param, atoi(value), atoi(mem));
                keyw = atoi(mem);
                if ((shmidw = shmget(keyw, SHMSZ, IPC_CREAT | 0666)) < 0)
                {
                    perror("shmget");
                    exit(1);
                }
                if ((shmw = shmat(shmidw, NULL, 0)) == (char *)-1)
                {
                    perror("shmat");
                    exit(1);
                }
                printf("Escribiendo el valor de W=%s en el espacio de mem compartida %s\n", value, mem);
                sprintf(shmw, "%s", value);
            }

            if (strcmp(param, "t") == 0)
            {
                //printf("%s %d %d\n",param, atoi(value), atoi(mem));
                keyt = atoi(mem);
                if ((shmidt = shmget(keyt, SHMSZ, IPC_CREAT | 0666)) < 0)
                {
                    perror("shmget");
                    exit(1);
                }
                if ((shmt = shmat(shmidt, NULL, 0)) == (char *)-1)
                {
                    perror("shmat");
                    exit(1);
                }
                printf("Escribiendo el valor de T=%s en el espacio de mem compartida %s\n", value, mem);
                sprintf(shmt, "%s", value);
            }
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

    

    system("gnome-terminal -- ./sensores");
    system("gnome-terminal -- ./lectorCen");
    system("gnome-terminal -- ./lectorDer");
    system("gnome-terminal -- ./lectorIzq");
    system("gnome-terminal -- ./controller");
    system("gnome-terminal -- ./visor");
    return 0;
}