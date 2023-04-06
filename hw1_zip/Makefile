FLAGS = -Wall -Wextra -Wpedantic -std=c11
PROGRAM = isjpgzip


$(PROGRAM): main.o utils.o constants.o
	gcc main.o utils.o constants.o -o isjpgzip


main.o: main.c
	gcc $(FLAGS) -c main.c 

utils.o: utils.c utils.h
	gcc $(FLAGS) -c utils.c

constants.o: constants.c constants.h
	gcc $(FLAGS) -c constants.c


clean:
	rm *.o $(PROGRAM)

