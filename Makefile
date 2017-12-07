CC = g++
FLAGS = -std=c++14 -pthread -Wall

BIN_DIR = bin
SRC_DIR = src

all:
	$(CC) $(FLAGS) -o sendfile $(SRC_DIR)/sendfile.cpp 
	$(CC) $(FLAGS) -o recvfile $(SRC_DIR)/recvfile.cpp 

clean:
	rm -f sendfile
	rm -f recvfile
