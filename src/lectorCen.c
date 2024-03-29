/*
*   Un sistema para evitar choques basado en laser y giroscopio (Version Concurrente) - Proyecto primer parcial
*   Creado por Julio Cesar Guilindro Ordonez para la materia de
*   Sistemas Operativos (2018-2S)
*
*   Senal usada para reiniciar sensores: SIGUSR1
*   Senal usada para matar sensores: SIGUSR2
*   
*   Modo de uso:
*   Mandar una senal por medio de una terminal
*   kill -SIGUSR1 [PID]
*   kill -SIGUSR2 [PID]
*   
*
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <string.h>
#include <math.h>
#include <wait.h>
#include <time.h>

#define SHMSZ 27
#define MAX_SAMPLES 100
#define DIST 10
#define PI 3.14159265

int shmidd, shmidt, shmidq, shmidcen;
int numSamples = 0, iSamples = 0;
float samples[MAX_SAMPLES];
struct keys
{
    key_t keyd;
    key_t keyt;
    key_t keyq;
};
key_t keyCen;
clock_t ti, tf;
char *shmd, *shmt, *shmq, *shmcen;

//Funcion que crea procesos
void *lectorSensor();

void *calcularDistancia(void *param);

float ang, dist;

int main(int argc, char *argv[])
{
    int i = 0;
    pthread_t lector;         //Se generan n hilos que crean cada uno un proceso para leer los sensores
    pthread_attr_t attr;      //Atributo de los hilos
    pthread_attr_init(&attr); //Inicializar atributo por defecto

    struct keys keys;

    FILE *config = fopen("../config/config.txt", "r");

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
            sscanf(buffer, "%[^:]:%[^,],%s", num, d, t);
            if (atoi(num) == 0)
            {
                keys.keyd = atoi(d);
                keys.keyt = atoi(t);
            }
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

    FILE *config2 = fopen("../config/parameters.txt", "r");

    char param[10], value[10], mem[10];

    if (config2 == NULL)
    {
        printf("No se puede abrir el archivo de configuracion");
    }
    else
    {

        while (fgets(buffer, 100, config2) != NULL)
        {
            sscanf(buffer, "%[^:]:%[^,],%s", param, value, mem);

            if (strcmp(param, "q") == 0)
            {
            
                keys.keyq = atoi(mem);
            }
        }
    }

    if (ferror(config2))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config2);

    pthread_create(&lector, &attr, lectorSensor, (void *)&keys);

    pthread_join(lector, NULL);

    return 0; //Termina el programa principal
}

void *lectorSensor(void *args)
{
    struct keys *keys = args;
    //printf("Leyendo memoria compartida con llave: %d\n", keys->keyd);
    //printf("Leyendo memoria compartida con llave: %d\n", keys->keyt);
    //printf("Leyendo memoria compartida con llave: %d\n", keys->keyq);
    char tmpd[SHMSZ];
    char oldt[SHMSZ];
    char tmpt[SHMSZ];
    int distanciaExiste = 0, anguloExiste = 0;

    if ((shmidd = shmget(keys->keyd, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmd = shmat(shmidd, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    if ((shmidt = shmget(keys->keyt, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmt = shmat(shmidt, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    if ((shmidq = shmget(keys->keyq, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    if ((shmq = shmat(shmidq, NULL, 0)) == (char *)-1)
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
    sprintf(shmcen, "%f", 0.0f);

    while (1)
    {

        strcpy(tmpt, shmt);
        if ((strcmp(tmpt, "--") != 0) && (strcmp(oldt, tmpt) != 0))
        {
            pthread_mutex_t lock;
            pthread_mutex_lock(&lock);
            ti = clock();
            pthread_mutex_unlock(&lock);
            strcpy(oldt, tmpt);
            anguloExiste = 1;
        }
        if (strcmp(shmd, tmpd) != 0)
        {

            distanciaExiste = 1;
        }

        strcpy(tmpd, shmd);
        if (distanciaExiste == 1 && anguloExiste == 1 && strcmp(tmpt, "--"))
        {
            pthread_t tcalcularDistancia;
            pthread_attr_t attr;
            pthread_mutex_t lock;
            pthread_mutex_lock(&lock);
            dist = atof(shmd); //distancia
            ang = atof(tmpt);  //angulo

            //fprintf(stdout, "giroscopio: %s\n", tmpt);
            //fprintf(stderr, "distancia %s\n", shmd);
            pthread_mutex_unlock(&lock);
            pthread_attr_init(&attr); //Inicializar atributo por defecto

            pthread_create(&tcalcularDistancia, &attr, calcularDistancia, NULL);
            distanciaExiste = 0;
            anguloExiste = 0;
        }

        /*
        strcpy(tmpt,shmt);
		if ((strcmp(tmpt,"--")!=0)&&(strcmp(oldt,tmpt)!=0)){
			fprintf(stdout,"giroscopio: %s\n",tmpt);
			strcpy(oldt,tmpt);
        }
		if (strcmp(shmd,tmpd)!=0)
			fprintf(stderr,"distancia %s\n",shmd);
		strcpy(tmpd,shmd);*/
    }
}

void *calcularDistancia(void *param)
{
    char tmpq[SHMSZ];
    float acumSamples = 0.0f, meanSamples = 0.0f;
    strcpy(tmpq, shmq);

    if (iSamples > atoi(tmpq) - 1)
    {
        for (int i = 0; i < iSamples; i++)
        {
            acumSamples = acumSamples + samples[i];
        }
        meanSamples = acumSamples / iSamples;
        sprintf(shmcen, "%f", meanSamples); //Pushea los datos a la memoria compartida
        printf("\nMedia: %f\n\n", meanSamples);
        pthread_mutex_t lock;
        pthread_mutex_lock(&lock);
        iSamples = 0;
        pthread_mutex_unlock(&lock);
    }
    float distanciaReal = dist * cos(ang / 180 * PI);
    samples[iSamples] = distanciaReal;
    printf("Muestra #%d\n", iSamples + 1);
    printf("giroscopio: %f\n", ang);
    printf("distancia %f\n\n", dist);
    pthread_mutex_t lock;
    pthread_mutex_lock(&lock);
    iSamples++;
    pthread_mutex_unlock(&lock);

    pthread_exit(0);
}
