.PHONY : clean

OBJS	= main.o fileParser.o socket.o semaphore_v2.o imatgeUtils.o
SOURCE	= main.c fileParser.c ./connectionUtils/socket.c ./semaphore_v2.c  ./ImatgeUtils/imatgeUtils.c
HEADER	= filerParser.h ./connectionUtils/socket.h ./semaphore_v2.h ./ImatgeUtils/imatgeUtils.h
OUT	= danny
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

imatgeUtils.o: ./ImatgeUtils/imatgeUtils.c
	$(CC) $(FLAGS) ./ImatgeUtils/imatgeUtils.c

main.o: main.c
	$(CC) $(FLAGS) main.c

fileParser.o: fileParser.c
	$(CC) $(FLAGS) fileParser.c

semaphore_v2.o: ./semaphore_v2.c
	$(CC) $(FLAGS) ./semaphore_v2.c

socket.o: ./connectionUtils/socket.c
	$(CC) $(FLAGS) ./connectionUtils/socket.c



clean :
	-rm $(OBJS)
