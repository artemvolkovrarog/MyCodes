CC = g++
FLAGS = -std=c++17 -Wall
all: my_serializer
test:
	./my_serializer
my_serializer: serializer.o  main.o
	$(CC) $(FLAGS) -o my_serializer serializer.o main.o
main.o: main.cpp serializer.hpp
	$(CC) $(FLAGS) -c main.cpp
serializer.o: serializer.cpp serializer.hpp
	$(CC) $(FLAGS) -c serializer.cpp
clean:
	rm -rf *.o my_serializer