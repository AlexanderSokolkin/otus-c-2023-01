FLAGS = -Wall -Wextra -Wpedantic -std=c11
PROGRAM = weather

$(PROGRAM): main.o utils.o
	gcc $^ -lcurl -L$(cJSON_DIR)/lib -Wl,-Bstatic -lcjson -Wl,-Bdynamic -o $@

main.o: main.c constants.h
	gcc $(FLAGS) -c main.c

utils.o: utils.c utils.h constants.h
	gcc $(FLAGS) -c utils.c -I $(cJSON_DIR)/include

clean:
	rm *.o $(PROGRAM)
