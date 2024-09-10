# SimPlayer Base

CC   = cc
OPT  = 
INC  = `sdl2-config --cflags`
LIBS = -lSDL2 -lSDL2_ttf
OBJS = main.o player.o track_list.o visual.o visual/demo.o visual/waves.o

all: $(OBJS)
	$(CC) $(OPT) $(OBJS) -o SimPlayer $(LIBS)

debug: OPT=-g -Wall -Werror
debug: all

main.o: main.c player.h visual.h
	$(CC) $(OPT) $(INC) -c $<

player.o: player.c player.h track_list.h visual.h
	$(CC) $(OPT) $(INC) -c $<

track_list.o: track_list.c track_list.h
	$(CC) $(OPT) $(INC) -c $<
	
visual.o: visual.c visual.h player.h
	$(CC) $(OPT) $(INC) -c $<

visual/demo.o: visual/demo.c visual.h
	cd visual &&\
		$(CC) $(OPT) $(INC) -c $(subst visual/, ,$<)

visual/waves.o: visual/waves.c visual.h
	cd visual &&\
		$(CC) $(OPT) $(INC) -c $(subst visual/, ,$<)

.PHONY: clear
clear:
	rm -rf *.o
	rm -rf visual/*.o
	rm -rf SimPlayer
