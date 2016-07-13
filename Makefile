CC = gcc
CFLAGS = -Wall -Wshadow -g
LDFLAGS = -lpthread

EXE = sm.out

all : $(EXE)

Objects = $(patsubst %.c, %.o, $(wildcard *.c))
$(EXE) : $(Objects)
	$(CC) $(CFLAGS) $(Objects) -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

clean :
	rm -f *.o $(EXE)

.PHONY : all clean
