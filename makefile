OBJS	= main.o fileParser.o
SOURCE	= main.c fileParser.c
HEADER	= filerParser.h
OUT	= prova
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

main.o: main.c 
	$(CC) $(FLAGS) main.c 

fileParser.o: fileParser.c
	$(CC) $(FLAGS) fileParser.c
	
clean:
	sudo rm -f main.o fileParser.o
