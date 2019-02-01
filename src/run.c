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
    int shmids, shmidq, value;
    char *shms, *shmq, param[10];
    key_t keys, keyq;
    //system("pwd");
    keys= 1928;
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
    keyq= 1929;
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

    FILE *config = fopen("../config/parameters.txt", "r");

    char buffer[100];
    char num[10], d[10], t[10];

    if (config == NULL)
    {
        printf("No se puede abrir el archivo de configuracion");
    }
    else
    {

        while (fgets(buffer, 100, config) != NULL)
        {
            sscanf(buffer, "%[^:]:%d", param, &value);
            if (strcmp (param, "i") == 0)
            {
                sprintf(shms, "%d", value);
            }
            if (strcmp (param, "q") == 0)
            {
                sprintf(shmq, "%d", value);
            }
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

    

    system("gnome-terminal -- ./sensores");
    system("gnome-terminal -- ./lectores");
    system("gnome-terminal -- ./controller");
    return 0;
}