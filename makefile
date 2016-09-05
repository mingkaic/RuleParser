CC = g++
DEBUG = -g
CFLAG = -std=c++11
SRCS = test.cpp src/*.cpp src/*/*.cpp

all:
	$(CC) $(CFLAG) $(SRCS) -o bin/test -D _TERMINAL_

clean:
	rm test