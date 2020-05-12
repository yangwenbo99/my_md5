SRC_DIR = ./src
CC = g++
FLAGS = -march=athlon64 -O3 -pipe

default: md5

md5: md5.o main.o

md5.o: $(SRC_DIR)/md5.cpp
	$(CC) $(FLAGS) -c -o md5.o $(SRC_DIR)/md5.cpp 

main.o: $(SRC_DIR)/main.cpp
	$(CC) $(FLAGS) -c -o main.o $(SRC_DIR)/main.cpp 

clean:
	rm -f *.o

