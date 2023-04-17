FLAGS = -Wall -Wextra -Wpedantic -std=c11
PROGRAM = hash

$(PROGRAM): main.o hash.o utils.o
	gcc $^ -o $@

main.o: main.c constants.h hash.h
	gcc $(FLAGS) -c main.c

utils.o: utils.c utils.h constants.h hash.h
	gcc $(FLAGS) -c utils.c

hash.o: hash.c 
	gcc $(FLAGS) -c hash.c

clean:
	rm *.o $(PROGRAM)
