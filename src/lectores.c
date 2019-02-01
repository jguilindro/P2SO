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
#define MAX_SAMPLES_THETA 50
#define DIST 10
#define PI 3.14159265

int shmidd, shmidt;
struct keys
{
    key_t keyd;
    key_t keyt;
};
clock_t ti, tf;
char *shmd, *shmt;


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

    pthread_create(&lector, &attr, lectorSensor, (void *)&keys);

    pthread_join(lector, NULL);

    return 0; //Termina el programa principal
}

void *lectorSensor(void *args)
{
    //system("gnome-terminal");
    struct keys *keys = args;
    printf("%d\n", keys->keyd);
    printf("%d\n", keys->keyt);
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

            fprintf(stdout, "giroscopio: %s\n", tmpt);
            fprintf(stderr, "distancia %s\n", shmd);
            pthread_mutex_unlock(&lock);
            pthread_attr_init(&attr); //Inicializar atributo por defecto

            pthread_create(&tcalcularDistancia, &attr, calcularDistancia, NULL);
            distanciaExiste = 0;
            anguloExiste = 0;
        }
    }
}

void *calcularDistancia(void *param)
{
    float distanciaReal = dist * cos(ang / 180 * PI);

    printf("La distancia real es: %f", distanciaReal);
    pthread_mutex_t lock;
    pthread_mutex_lock(&lock);
    tf = clock();
    pthread_mutex_unlock(&lock);
    time_t t = tf - ti;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf(" || Tiempo de respuesta: %f segundos\n\n", time_taken);

    pthread_exit(0);
}
