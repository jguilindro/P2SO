# Sistema para detectar Vehículos

> Un programa para poder detectar obstaculos y vehículos usando sensores de distancia laser y giroscopios.


## Requerimientos

- [GNU Compiler Collection](https://gcc.gnu.org/) 

Como hacer build al programa:
Ejecutar una terminal en la raiz del programa e ingresar los siguientes comandos.
```shell
make

```
Luego de hacer build, correr el programa:
```shell
#Crea el directorio donde se guardaran los ejecutables
mkdir bin/
# Mover al directorio de ejecutables
cd bin/
# Correr el sistema
./run

```
## Edición de archivos de configuración
Se tienen 2 archivos de configuración: config.txt y parameters.txt

El archivo config.txt se encarga de guardar la configuración de los sensores, los espacios de memoria compartida que se van a usar para poder comunicarse con los procesos lectores.

Se tienen 2 espacios de memoria compartidos por cada sensor, uno para pushear los datos del sensor laser y otro para pushear los datos del sensor giroscopio.

Estructura:
```shell
#Archivo config.txt
0:[Key de la memoria compartida sensor Distancia],[Key de la memoria compartida sensor Giroscopio]
1:[Key de la memoria compartida sensor Distancia],[Key de la memoria compartida sensor Giroscopio]
2:[Key de la memoria compartida sensor Distancia],[Key de la memoria compartida sensor Giroscopio]

```

El archivo parameters.txt guardará la informacion de los parametros de Frecuencia de lectura de sensores (I), de Número de muestras a tomar por promedio (Q), determinador de Obstaculos (W), determinador de Vehículos (T).

Estructura:
```shell
#Archivo parameters.txt
i:[Valor],[Key de la memoria compartida]
q:[Valor],[Key de la memoria compartida]
w:[Valor],[Key de la memoria compartida]
t:[Valor],[Key de la memoria compartida]

```

## Descripción de la arquitectura

El programa consiste en 3 pares de sensores laser/giroscopio, los cuales son generados por un programa generador de Sensores por medio de hilos.
Se crea un hilo para cada uno de los sensores, los hilos leen el archivo de configuracion "config.txt" el cual contiene las direcciones de memoria compartida donde estos sensores van a pushear sus datos.

Se crean 3 procesos lectores, uno para cada sensor, estos se encargan de leer los datos pusheados a la memoria compartida. Realizan el respectivo procesamiento de los datos para poder hayar la distancia real y generar las muestras para realizar promedios. Los procesos lectores leen el archivo de configuracion "parameters.txt" el cual les dice cuantas muestras Q tomar. Una vez tomadas las Q muestras, este realiza el promedio de ellas y pushea el resultado por una memoria compartida para ser leidas por el proceso visor.

El proceso visor recibe los valores promediados de cada uno de los sensores (3 promedios). Toma estos promedios y los procesa hayando la desviación estandar (sigma) entre ellos, además de comparar sus diferencias con los parametros W*sigma y T*sigma para poder determinar si existe algún tipo de obstacúlo o vehículo, respectivamente, al frente del carro.

Por último tiene un proceso controlador, el cual es un menú que permite modificar los parámetros de Frecuencia de lectura de sensores (I), de Número de muestras a tomar por promedio (Q), determinador de Obstaculos (W), determinador de Vehículos (T) en tiempo de ejecución, mostrando una diferencia en la lectura de los datos, cálculo de promedios y determinación de obstaculos inmediatamente, sin necesidad de reiniciar alguno de los procesos.


