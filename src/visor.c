#include "stdlib.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define SHMSZ 27
float calcularDS(float data1, float data2, float data3);
int main(void)
{
    int shmidw, shmidt, shmidder, shmidizq, shmidcen;
    char *shmw, *shmt, *shmder, *shmizq, *shmcen;
    key_t keyw, keyt, keyCen, keyDer, keyIzq;
    char tmpt[SHMSZ];
    char tmpw[SHMSZ];
    char tmpcen[SHMSZ];
    char tmpder[SHMSZ];
    char tmpizq[SHMSZ];
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
                //printf("Leyendo el valor de W en el espacio de mem compartida %s\n", mem);
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
                //printf("Leyendo el valor de T en el espacio de mem compartida %s\n", mem);
            }
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

    keyDer = 6746;
    if ((shmidder = shmget(keyDer, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmder = shmat(shmidder, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }
    keyCen = 6745;
    if ((shmidcen = shmget(keyCen, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmcen = shmat(shmidcen, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }
    keyIzq = 6747;
    if ((shmidizq = shmget(keyIzq, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmizq = shmat(shmidizq, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }
    printf("***** Visor de datos y Obstaculos ******\n\n");
    while (1)
    {
        strcpy(tmpt, shmt);
        strcpy(tmpw, shmw);
        strcpy(tmpcen, shmcen);
        strcpy(tmpizq, shmizq);
        strcpy(tmpder, shmder);

        if (strcmp(tmpcen, "0.000000") != 0 && strcmp(tmpizq, "0.000000") != 0 && strcmp(tmpder, "0.000000") != 0)
        {

            //system("clear");
            
            float sigma= calcularDS(atof(tmpcen), atof(tmpder), atof(tmpizq));

            printf("\n*********\nCentro: %s\nIzquierda: %s\nDerecha: %s\nCentro-Izq: %f\nCentro-Der: %f\nSigma: %f\nT*sigma: %f\nW*sigma: %f\n*********\n", tmpcen, tmpizq, tmpder,fabs(atof(tmpcen) - atof(tmpizq)),fabs(atof(tmpcen) - atof(tmpder)), sigma, atof(tmpt)*sigma, atof(tmpw)*sigma);
            //printf("\n\nCentro-Der (bool T): %d\nCentro-Izq (bool T): %d\nCentro-Der (bool W): %d\nCentro-Izq (bool W): %d\nDer-Izq (bool W): %d\n", ((atof(tmpcen) - atof(tmpder))<=(atof(tmpt)*sigma)), (fabs(atof(tmpcen) - atof(tmpizq))<=(atof(tmpt)*sigma)),(fabs(atof(tmpcen) - atof(tmpder))<=(atof(tmpw)*sigma)),(fabs(atof(tmpcen) - atof(tmpizq))<=(atof(tmpw)*sigma)),(fabs(atof(tmpder) - atof(tmpizq))<=(atof(tmpw)*sigma)));
            
            if ((fabs(atof(tmpcen) - atof(tmpder))<=(atof(tmpt)*sigma)) && (fabs(atof(tmpcen) - atof(tmpizq))<=(atof(tmpt)*sigma)) ){
                printf("Existe un carro al frente de su carro.\n");
            }

            if ((fabs(atof(tmpcen) - atof(tmpder))==(atof(tmpw)*sigma)) || (fabs(atof(tmpcen) - atof(tmpizq))==(atof(tmpw)*sigma)) || (fabs(atof(tmpder) - atof(tmpizq))==(atof(tmpw)*sigma))){
                printf("Existe un obstaculo al frente de su carro.\n");
            }
            
            sprintf(shmcen, "%f", 0.0f); //Inicializa en cero
            sprintf(shmizq, "%f", 0.0f); //Inicializa en cero
            sprintf(shmder, "%f", 0.0f); //Inicializa en cero
            
        }
    }

    return 0;
}

float calcularDS(float data1, float data2, float data3)
{
    float sum = 0.0, media, sd = 0.0;
    float data[3];
    int i;
    sum = data1 + data2 + data3;
    data[0] = data1;
    data[1] = data2;
    data[2] = data3;
    media = sum / 3;

    for (i = 0; i < 3; i++)
    {
        sd += pow(data[i] - media, 2);
    }
    return sqrt(sd / 3);
}