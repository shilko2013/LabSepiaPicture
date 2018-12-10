CC=gcc
NASMC=nasm
NASMFLAGS =-g -f elf64
CFLAGS=-no-pie
LIBS=-lm
TARGET=main
NASMFILES =sse.asm
CFILES =main.c bmp.c

mainmake:
	$(NASMC) $(NASMFLAGS) $(NASMFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(CFILES) $(LIBS) sse.o
clean:
	rm *.o main
