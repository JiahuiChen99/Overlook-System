.PHONY : clean

OBJS	= mainWendy.o fileParser.o socket.o imatgeUtils.o
SOURCE	= mainWendy.c ../fileParser.c ../connectionUtils/socket.c ../ImatgeUtils/imatgeUtils.c
HEADER	= ../filerParser.h ../connectionUtils/socket.h ../ImatgeUtils/imatgeUtils.h
OUT	= wendy
CC	 = gcc
FLAGS	 = -g -c -Wall -Wextra


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)
	make clean

main.o: mainJack.c
	$(CC) $(FLAGS) mainJack.c

fileParser.o: ../fileParser.c
	$(CC) $(FLAGS) ../fileParser.c

imatgeUtils.o: ../ImatgeUtils/imatgeUtils.c
	$(CC) $(FLAGS) ../ImatgeUtils/imatgeUtils.c

socket.o: ../connectionUtils/socket.c
	$(CC) $(FLAGS) ../connectionUtils/socket.c

clean :
	-rm $(OBJS)
