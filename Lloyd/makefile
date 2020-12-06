.PHONY : clean

OBJS	= lloyd.o fileParser.o semaphore_v2.o socket.o
SOURCE	= lloyd.c ../fileParser.c ../semaphore_v2.c ../connectionUtils/socket.c
HEADER	= ../filerParser.h ../semaphore_v2.h ../connectionUtils/socket.h
OUT	= lloyd
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

lloyd.o: lloyd.c
	$(CC) $(FLAGS) lloyd.c

fileParser.o: ../fileParser.c
	$(CC) $(FLAGS) ../fileParser.c

socket.o: ../connectionUtils/socket.c
	$(CC) $(FLAGS) ../connectionUtils/socket.c

semaphore_v2.o: ../semaphore_v2.c
	$(CC) $(FLAGS) ../semaphore_v2.c

clean :
	-rm $(OBJS)
