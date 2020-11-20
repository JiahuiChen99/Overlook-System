.PHONY : clean

OBJS	= mainJack.o fileParser.o socket.o
SOURCE	= mainJack.c ../fileParser.c ../connectionUtils/socket.c
HEADER	= ../filerParser.h ../connectionUtils/socket.h
OUT	= jack
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

main.o: mainJack.c
	$(CC) $(FLAGS) mainJack.c

fileParser.o: ../fileParser.c
	$(CC) $(FLAGS) ../fileParser.c

socket.o: ../connectionUtils/socket.c
		$(CC) $(FLAGS) ../connectionUtils/socket.c


clean :
	-rm $(OBJS)
