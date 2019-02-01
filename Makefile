all: lectores sensores exec
lectores: src/lectores.c
	gcc -o bin/lectores src/lectores.c -lm -lpthread

sensores: src/sensores.c
	gcc -o bin/sensores src/sensores.c -lm -lpthread

exec: src/run.c
	gcc -o bin/run src/run.c

run: exec
	./bin/run 

.PHONY: clean
clean:
	rm bin/*
