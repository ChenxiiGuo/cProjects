CFLAGS = `sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -ansi -Wfloat-equal
INCS = neillsdl2.h
TARGET1 = base
TARGET2 = ext
TARGET3 = printtest
SOURCES1 =  $(TARGET1).c neillsdl2.c
SOURCES2 = $(TARGET2).c neillsdl2.c
SOURCES3 = $(TARGET3).c neillsdl2.c
LIBS =  `sdl2-config --libs` -lm
MYLIB = `pkg`
CC = gcc


all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(SOURCES1) $(INCS)
	$(CC) $(SOURCES1) -o $(TARGET1) $(CFLAGS) $(LIBS) -lSDL2 -lSDL2_image

ext: $(SOURCES2) $(INCS)
	$(CC) $(SOURCES2) -o $(TARGET2) $(CFLAGS) $(LIBS) -lSDL2 -lSDL2_image


printtest: $(SOURCES3) $(INCS)
	$(CC) $(SOURCES3) -o $(TARGET3) $(CFLAGS) $(LIBS)  -lSDL2 -lSDL2_image
	
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3)

run: all
	./$(TARGET2) test_m7.gif 
	./$(TARGET1) rev.m7
	./$(TARGET3) rev.m7 test_m7.gif
