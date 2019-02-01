#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
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
#define BUFFER_SIZE 10

struct keys
{
    key_t keyd;
    key_t keyt;
};

void *sensorDistancia(void *args);
float box_muller(float m, float s);

void capturaSenalReinicio(int sig);    //Handler para la senal SIGUSR1
void capturaSenalTerminacion(int sig); //Handler para la senal SIGUSR2

int main(int argc, char *argv[])
{

    int i;
    if (signal(SIGUSR1, capturaSenalReinicio) == SIG_IGN) //Ignora la senal y la redirige a la funcion de captura
        signal(SIGUSR1, SIG_IGN);

    if (signal(SIGUSR2, capturaSenalTerminacion) == SIG_IGN) //Ignora la senal y la redirige a la funcion de captura
        signal(SIGUSR2, SIG_IGN);

    pthread_t sensores[3];    //Se generan n hilos que crean cada uno un proceso para leer los sensores
    pthread_attr_t attr;      //Atributo de los hilos
    pthread_attr_init(&attr); //Inicializar atributo por defecto
    struct keys keys[3] = {0, 0};

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
            keys[atoi(num)].keyd = atoi(d);
            keys[atoi(num)].keyt = atoi(t);
        }
    }

    if (ferror(config))
    {
        perror("El siguiente error ocurrio");
    }

    fclose(config);

    for (i = 0; i < 3; i++)
    {

        pthread_create(&sensores[i], &attr, sensorDistancia, (void *)&keys[i]);
    }

    for (i = 0; i < 3; i++)
    {
        pthread_join(sensores[i], NULL);
    }

    return 0; //Termina el programa principal
}

void *sensorDistancia(void *args) //Funcion creadora de procesos
{
    struct keys *keys = args;
    printf("Escribiendo memoria compartida con llave: %d\n", keys->keyd);
    printf("Escribiendo memoria compartida con llave: %d\n", keys->keyt);
    pid_t pid = fork(); //Crea una copia del proceso padre
    char c;
    int shmidd, shmidt, shmids;
    key_t keysec;
    char *shmd, *shmt, *shms;
    char tmps[SHMSZ];
    int i, j;
    float distances[MAX_SAMPLES];
    float angles[MAX_SAMPLES_THETA];
    float anglesD[MAX_SAMPLES];
    float mu, sigma, delta_theta;

    struct timespec tim, tim2;

    if (pid < 0) //Si ocurrio un error creando el proceso
    {
        perror("fork()");
        exit(1); //Termina con error.
    }
    else if (pid == 0) //Si se crea el proceso satisfactoriamente
    {
        printf("Sensores activados con PID:  %d\n", (int)getpid()); //Alerta al usuario y muestra el PID del proceso creado

        if ((shmidd = shmget(keys->keyd, SHMSZ, IPC_CREAT | 0666)) < 0)
        {
            perror("shmget");
            exit(1);
        }
        if ((shmd = shmat(shmidd, NULL, 0)) == (char *)-1)
        {
            perror("shmat");
            exit(1);
        }
        if ((shmidt = shmget(keys->keyt, SHMSZ, IPC_CREAT | 0666)) < 0)
        {
            perror("shmget");
            exit(1);
        }
        if ((shmt = shmat(shmidt, NULL, 0)) == (char *)-1)
        {
            perror("shmat");
            exit(1);
        }
        keysec = 1928;
        if ((shmids = shmget(keysec, SHMSZ, IPC_CREAT | 0666)) < 0)
        {
            perror("shmget");
            exit(1);
        }
        if ((shms = shmat(shmids, NULL, 0)) == (char *)-1)
        {
            perror("shmat");
            exit(1);
        }

        mu = 0;
        sigma = 25;

        sleep(3);

        for (i = 0; i < MAX_SAMPLES_THETA; i++)
        {
            angles[i] = box_muller(mu, sigma);
        }

        j = -1;
        for (i = 0; i < MAX_SAMPLES_THETA - 1; i++)
        {
            if (j++ < MAX_SAMPLES)
                anglesD[j] = angles[i];
            if (j++ < MAX_SAMPLES)
            {
                delta_theta = abs(angles[i + 1] - angles[i]) / 2;
                if (angles[i + 1] > angles[i])
                    anglesD[j] = angles[i] + delta_theta;
                else
                    anglesD[j] = angles[i] - delta_theta;
            }
        }

        for (i = 0; i < j; i++)
        {
            distances[i] = DIST / cos(anglesD[i] / 180 * PI);
        }

        for (i = 0; i < j; i++)
        {
            strcpy(tmps, shms);

            tim.tv_sec = atoi(tmps);
            tim.tv_nsec = 0;
            if (nanosleep(&tim, &tim2) < 0)
            {
                printf("Nano sleep failed \n");
                exit(-1);
            }
            sprintf(shmd, "%f", distances[i]);
            if (i % 2 == 0)
            {
                sprintf(shmt, "%f", anglesD[i]);
            }
            else
            {
                strcpy(shmt, "--");
            }
        }
    }
    else //Regresando al proceso padre
    {
        int status;                                  //Status del proceso hijo
        pid_t return_pid = waitpid(pid, &status, 0); //Espera por el status del proceso hijo

        printf("\nSensores de distancia apagado con status:  0x%.4X (%d)\n", status, status); //Muestra el status del proceso hijo
        if (WIFEXITED(status))                                                                //Si termino normalmente usando exit
        {
            printf("Sensor de distancia apagado con status: 0x%.2X (%d)\n",
                   WEXITSTATUS(status), WEXITSTATUS(status)); //Muestra el estatus de salida
            if (WEXITSTATUS(status) == 1)                     //Si el estatus es 1 (termino de forma abrupta)
            {
                struct keys keys;
                keys.keyd = 1234;
                keys.keyt = 5678;
                printf("\nReiniciando sensor de distancia... \n");

                sensorDistancia((void *)&keys); //Se reinicia el proceso, creando uno nuevo
            }
        }

        if (WIFSIGNALED(status))                                                                          //Si termina por alguna otra senal no manejada
            printf("Sensor de distancia apagado debido a la senal no manejada:  %d\n", WTERMSIG(status)); //Muestra la senal no manejada que causo que termine el proceso
    }
    pthread_exit(0); //Termina el hilo.
}

void capturaSenalReinicio(int sig) //Handler de senal SIGUSR1
{
    printf("\nSeñal de reinicio recibida. \n"); //Alerta al usuario que se terminara abruptamente el proceso
    exit(1);                                    //Termina el proceso abruptamente con un status 1 (EXIT_FAILURE)
}

void capturaSenalTerminacion(int sig) //Handler de senal SIGUSR2
{
    printf("\nSeñal de apagado recibida. \n"); //Alerta al usuario que se terminara correctamente el proceso
    exit(0);                                   //Termina el proceso correctamente con un status 0 (EXIT_SUCCESS)
}

float box_muller(float m, float s) /* normal random variate generator */
{                                  /* mean m, standard deviation s */
    float x1, x2, w, y1;
    static float y2;
    static int use_last = 0;

    if (use_last) /* use value from previous call */
    {
        y1 = y2;
        use_last = 0;
    }
    else
    {
        do
        {
            x1 = 2.0 * ((double)(rand()) / RAND_MAX) - 1.0;
            x2 = 2.0 * ((double)(rand()) / RAND_MAX) - 1.0;
            w = x1 * x1 + x2 * x2;
        } while (w >= 1.0);

        w = sqrt((-2.0 * log(w)) / w);
        y1 = x1 * w;
        y2 = x2 * w;
        use_last = 1;
    }

    return (m + y1 * s);
}
