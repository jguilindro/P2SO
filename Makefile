all: lectores sensores exec controller
lectores: src/lectores.c
	gcc -o bin/lectores src/lectores.c -lm -lpthread

sensores: src/sensores.c
	gcc -o bin/sensores src/sensores.c -lm -lpthread

controller: src/controller.c
	gcc -o bin/controller src/controller.c	

exec: src/run.c
	gcc -o bin/run src/run.c

run: exec
	./bin/run 

.PHONY: clean
clean:
	rm bin/*
