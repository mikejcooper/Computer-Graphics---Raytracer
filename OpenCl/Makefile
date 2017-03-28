# Makefile

EXE=d2q9-bgk

CC=g++
CFLAGS= -Wall -O3
LIBS = -lm

PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	LIBS += -framework OpenCL
else
	LIBS += -lOpenCL
endif

all: $(EXE)

$(EXE): $(EXE).c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


.PHONY: all check clean

clean:
	rm -f $(EXE)
