CC = g++
FLAGS = -std=c++17 -fsanitize=address,undefined -Wall
all: my_vec
test:
	./my_vec
my_vec: main.o
	$(CC) $(FLAGS) -o  my_vec main.o
main.o: main.cpp my_vec.hpp my_vec_iter.hpp my_vec_alloc.hpp
	$(CC) $(FLAGS) -c -g main.cpp
clean:
	rm -rf *.o my_vec