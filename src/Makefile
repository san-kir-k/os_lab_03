CXX = gcc
CXXFLAGS = -pedantic -Wall -Werror -lpthread -std=c99
NAME = lab3
DIR = ../src

all:
	$(CXX) $(CXXFLAGS) lab3.c matrixio.c -o $(NAME)
clean:
	rm -f *.o $(NAME)