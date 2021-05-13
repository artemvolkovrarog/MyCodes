CC = g++
FLAGS = -std=c++17 -Wall
all: my_parser
test:
	./my_parser
my_parser: parser.o  main.o
	$(CC) $(FLAGS) -o my_parser parser.o main.o
main.o: main.cpp parser.hpp
	$(CC) $(FLAGS) -c main.cpp
parser.o: parser.cpp parser.hpp
	$(CC) $(FLAGS) -c parser.cpp
clean:
	rm -rf *.o my_parser