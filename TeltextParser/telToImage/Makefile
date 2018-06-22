CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -ansi -Wfloat-equal
INCS = neillsdl2.h

TARGET1 = base
TARGET2 = test
TARGET3 = printtest
SOURCES1 =  $(TARGET1).c neillsdl2.c
SOURCES2 = $(TARGET2).c neillsdl2.c
SOURCES3 = $(TARGET3).c neillsdl2.c
LIBS =  `sdl2-config --libs` -lm
MYLIB = `pkg`
CC = gcc


all: $(TARGET1)

base: $(SOURCES1) $(INCS)
	$(CC) $(SOURCES1) -o $(TARGET1) $(CFLAGS) $(LIBS) 




clean:
	rm -f $(TARGET1)

run: all
	./$(TARGET1) test.m7
	./$(TARGET1) panda.m7
	./$(TARGET1) lfc.m7

