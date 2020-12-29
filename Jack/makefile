.PHONY : clean.PHONY : clean

OBJS	= mainJack.o fileParser.o socket.o semaphore_v2.o imatgeUtils.o lloyd.o
SOURCE	= mainJack.c ../fileParser.c ../connectionUtils/socket.c ../semaphore_v2.c ../ImatgeUtils/imatgeUtils.c ../Lloyd/lloyd.c
HEADER	= ../filerParser.h ../connectionUtils/socket.h ../semaphore_v2.h ../ImatgeUtils/imatgeUtils.h ../Lloyd/lloyd.h
OUT	= jack
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

imatgeUtils.o: ../ImatgeUtils/imatgeUtils.c
	$(CC) $(FLAGS) ../ImatgeUtils/imatgeUtils.c

main.o: mainJack.c
	$(CC) $(FLAGS) mainJack.c

fileParser.o: ../fileParser.c
	$(CC) $(FLAGS) ../fileParser.c

semaphore_v2.o: ../semaphore_v2.c
	$(CC) $(FLAGS) ../semaphore_v2.c

socket.o: ../connectionUtils/socket.c
	$(CC) $(FLAGS) ../connectionUtils/socket.c

lloyd.o: ../Lloyd/lloyd.c
		$(CC) $(FLAGS) ../Lloyd/lloyd.c



clean :
	-rm $(OBJS)
