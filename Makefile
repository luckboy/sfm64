CC = cl65
CFLAGS = -Os
LDFLAGS =
C1541 = c1541
SYS = c64

OBJS = cmd_channel.o dir_panel.o main.o main_menu.o screen.o util.o

.c.o:
	$(CC) -c -t $(SYS) $(CFLAGS) -o $@ $<
	
all: sfm64

sfm64: $(OBJS)
	$(CC) -t $(SYS) $(LDFLAGS) -o $@ $^

sfm64.d64: all
	$(C1541) -format sfm64,AA  d64 $@
	$(C1541) -attach $@ -write sfm64 sfm64

clean:
	rm -f sfm64 $(OBJS) *.d64 *~

cmd_channel.o: cmd_channel.c cmd_channel.h
dir_panel.o: dir_panel.c dir_panel.h cmd_channel.h screen.h util.h
main.o: main.c cmd_channel.h main_menu.h screen.h
main_menu.o: main_menu.c main_menu.h dir_panel.h screen.h util.h
screen.o: screen.c screen.h
util.o: util.c util.h
