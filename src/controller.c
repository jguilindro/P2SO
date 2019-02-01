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
    int shmids, shmidq, shmidw, shmidt, value;
    char *shms, *shmq, *shmw, *shmt;
    key_t keys, keyq, keyw, keyt;
    //system("pwd");
    keys = 1928;
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
    keyq = 1929;
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
    keyw = 1930;
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
    keyt = 1931;
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

    int opcion;

    do
    {
        printf("******** Controlador de Parámetros ********\n\n");
        printf("1. Cambiar el valor de I\n");
        printf("\n2. Cambiar el valor de Q\n");
        printf("\n3. Cambiar el valor de W\n");
        printf("\n4. Cambiar el valor de T\n");
        printf("\n5. Exit\n");
        printf("\nIngrese la opción: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
            printf("\nIngresar el nuevo valor de I: ");
            scanf("%d", &value);
            sprintf(shms, "%d", value);
            system("clear");
            break;
        case 2:
            printf("\nIngresar el nuevo valor de Q: ");
            scanf("%d", &value);
            sprintf(shmq, "%d", value);
            system("clear");
            break;
        case 3:
            printf("\nIngresar el nuevo valor de W: ");
            scanf("%d", &value);
            sprintf(shmw, "%d", value);
            system("clear");
            break;
        case 4:
            printf("\nIngresar el nuevo valor de T: ");
            scanf("%d", &value);
            sprintf(shmt, "%d", value);
            system("clear");
            break;
        case 5:
            printf("\nAdios. ");
            break;    
        default:
            printf("La opcion no es correcta. Intente de nuevo.\n");
            break;
        }

    } while (opcion != 5);

    return 0;
}