all: lectorCen lectorIzq lectorDer sensores exec controller visor
lectorCen: src/lectorCen.c
	gcc -o bin/lectorCen src/lectorCen.c -lm -lpthread

lectorIzq: src/lectorIzq.c
	gcc -o bin/lectorIzq src/lectorIzq.c -lm -lpthread

lectorDer: src/lectorDer.c
	gcc -o bin/lectorDer src/lectorDer.c -lm -lpthread

sensores: src/sensores.c
	gcc -o bin/sensores src/sensores.c -lm -lpthread

controller: src/controller.c
	gcc -o bin/controller src/controller.c

visor: src/visor.c
	gcc -o bin/visor src/visor.c -lm	

exec: src/run.c
	gcc -o bin/run src/run.c

run: exec
	./bin/run 

.PHONY: clean
clean:
	rm bin/*
