CC = gcc
CFLAGS = -I. -Wall -Wshadow -g
LDFLAGS = -lpthread

RDIRS = util proto data com .
RSRCS = $(foreach d, $(RDIRS), $(wildcard $(d)/*.c))
ROBJS = $(patsubst %.c, %.o, $(RSRCS))
REXE  = sm.out

TDIR  = test
TSRCS = $(filter-out %main.c, $(RSRCS)) $(wildcard $(TDIR)/*.c)
TOBJS = $(patsubst %.c, %.o, $(TSRCS))
TEXE  = unit.out

DIRS  = $(RDIRS) $(TDIR)
SRCS  = $(foreach d, $(DIRS), $(wildcard $(d)/*.c))
OBJS  = $(patsubst %.c, %.o, $(SRCS))

.PHONY: all
all : $(REXE) $(TEXE)

$(REXE) : $(ROBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(TEXE) : $(TOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJS) : %.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: test
test :
	@echo 'Files in main program:'
	@echo 'Srcs: ' $(RSRCS)
	@echo 'Objs: ' $(ROBJS)
	@echo 'Files in test program:'
	@echo 'Srcs: ' $(TSRCS)
	@echo 'Objs: ' $(TOBJS)

.PHONY: clean
clean :
	rm -f $(OBJS) $(REXE) $(TEXE)
