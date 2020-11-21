.PHONY : clean

OBJS	= main.o fileParser.o socket.o
SOURCE	= main.c fileParser.c ./connectionUtils/socket.c
HEADER	= filerParser.h ./connectionUtils/socket.h
OUT	= prova
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

main.o: main.c
	$(CC) $(FLAGS) main.c

fileParser.o: fileParser.c
	$(CC) $(FLAGS) fileParser.c

socket.o: ./connectionUtils/socket.c
		$(CC) $(FLAGS) ./connectionUtils/socket.c


clean :
	-rm $(OBJS)
