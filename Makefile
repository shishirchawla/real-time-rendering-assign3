
CC = gcc
LD = gcc

CFLAGS = -ansi -Wall -c
#LFLAGS = `sdl-config --libs` -lGL -lm
LFLAGS = `sdl-config --libs` -framework OpenGL -lm

OBJS = sdl-base.o util.o shape.o arena.o brick.o level.o paddle.o game.o breakout.o

PROG = breakout

default: $(PROG)

$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

sdl-base.o: sdl-base.c sdl-base.h
	$(CC) $(CFLAGS) sdl-base.c

util.o: util.c util.h
	$(CC) $(CFLAGS) util.c

shape.o: shape.c shape.h
	$(CC) $(CFLAGS) shape.c

arena.o: arena.c arena.h	
	$(CC) $(CFLAGS) arena.c

brick.o: brick.c brick.h
	$(CC) $(CFLAGS) brick.c

level.o: level.c level.h
	$(CC) $(CFLAGS) level.c

paddle.o: paddle.c paddle.h
	$(CC) $(CFLAGS) paddle.c

game.o: game.c game.h
	$(CC) $(CFLAGS) game.c

breakout.o: breakout.c game.h sdl-base.h
	$(CC) $(CFLAGS) breakout.c

clean:
	rm -rf *.o

real_clean:
	 rm -rf *.o $(PROG)

