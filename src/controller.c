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
    int shmids, shmidq, shmidw, shmidt, valor;
    char *shms, *shmq, *shmw, *shmt;
    key_t keys, keyq, keyw, keyt;
    //system("pwd");

    FILE *config = fopen("../config/parameters1.txt", "r");

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
                printf("Leyendo el valor de I=%s en el espacio de mem compartida %s\n", value, mem);
                
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
                printf("Leyendo el valor de Q=%s en el espacio de mem compartida %s\n", value, mem);
                
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
                printf("Leyendo el valor de W=%s en el espacio de mem compartida %s\n", value, mem);
                
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
                printf("Leyendo el valor de T=%s en el espacio de mem compartida %s\n", value, mem);
                
            }
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

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
            scanf("%d", &valor);
            sprintf(shms, "%d", valor);
            system("clear");
            break;
        case 2:
            printf("\nIngresar el nuevo valor de Q: ");
            scanf("%d", &valor);
            sprintf(shmq, "%d", valor);
            system("clear");
            break;
        case 3:
            printf("\nIngresar el nuevo valor de W: ");
            scanf("%d", &valor);
            sprintf(shmw, "%d", valor);
            system("clear");
            break;
        case 4:
            printf("\nIngresar el nuevo valor de T: ");
            scanf("%d", &valor);
            sprintf(shmt, "%d", valor);
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