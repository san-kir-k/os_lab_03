CXX = gcc
CXXFLAGS = -pedantic -Wall -Werror -std=c99
LDFLAGS = -lpthread
NAME = lab3
DIR = ../src

all:
	$(CXX) $(CXXFLAGS) lab3.c matrixio.c -o $(NAME) $(LDFLAGS)
clean:
	rm -f *.o $(NAME)