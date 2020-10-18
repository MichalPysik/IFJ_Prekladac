#zde bude MAKEFILE (toto je jen test, nefunguje)

name=testProgram
SRCFOLDER=src
CC=gcc
CFLAGS= -std=c99 -pedantic -Wall

all: $(NAME)

$(NAME): $(SRCFOLDER)/*.o
	$(CC) $(CFLAGS) -o *.o

$(SRCFOLDER)/*.o: $(SRCFOLDER)/*.c
	$(CC) $(CFLAGS) -c *.c
