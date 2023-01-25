CC=gcc
CFLAGS= -c -g -Wall
BUILD=.build

all: cups-wrapper

cups-wrapper: main.o
	$(CC) $(BUILD)/main.o -o $(BUILD)/cups_wrapper -lcups

main.o:
	$(CC) $(CFLAGS) main.c -o $(BUILD)/main.o

clean:
	rm -rf $(BUILD)/*.o $(BUILD)/cups_wrapper
