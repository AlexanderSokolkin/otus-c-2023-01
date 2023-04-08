FLAGS = -Wall -Wextra -Wpedantic -std=c11
PROGRAM = utfconvert

$(PROGRAM): main.o convert.o utils.o
	gcc $^ -o $@

main.o: main.c constants.h
	gcc $(FLAGS) -c main.c

utils.o: utils.c utils.h constants.h
	gcc $(FLAGS) -c utils.c

convert.o: convert.c convert.h constants.h
	gcc $(FLAGS) -c convert.c

clean:
	rm *.o $(PROGRAM)
