CC = g++
FLAGS = -std=c++17 -Wall
all: my_matrix
test:
	./my_matrix
my_matrix: matrix.o  main.o
	$(CC) $(FLAGS) -o my_matrix matrix.o main.o
main.o: main.cpp matrix.hpp
	$(CC) $(FLAGS) -c main.cpp
matrix.o: matrix.cpp matrix.hpp
	$(CC) $(FLAGS) -c matrix.cpp
clean:
	rm -rf *.o my_matrix