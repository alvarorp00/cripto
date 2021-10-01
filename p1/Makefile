CC = gcc
CFLAGS =  -Wall
LDFLAGS = -pthread -lpthread -lm -lcrypto -lgmp

INC=inc
OUT=out
OBJ=obj
SRC=src

VG_OPTIONS=--leak-check=full --show-leak-kinds=all --track-origins=yes
DEBUG_MODE=__DEBUG__

COMPILED=exe

SOURCEFILES := $(shell find $(SRC) -name '*.c')

default: clean ## Equivalent to 'make all'
				@ echo "Compiling all..."
				@ $(CC) -I $(INC) $(CFLAGS) -o $(COMPILED) $(SOURCEFILES) $(LDFLAGS)
				@ echo "Executable generated successfully!"

help:	## This guide
		@@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

all: default ## Produces agent compiled executable

valgrind: all ## Compiles and executes using valgrind mode
		@ valgrind $(VG_OPTIONS) ./exe

debug: clean ## Compiles using -D __DEBUG__ option
		@ echo "Compiling in debug mode..."
		@ $(CC) -I $(INC) $(CFLAGS) -D $(DEBUG_MODE) -o $(COMPILED) $(SOURCEFILES) $(LDFLAGS)

.PHONY: clean

clean: ## Cleans output and object files
				rm -f $(OBJ)/*
				rm -f $(OUT)/*
				rm -f $(COMPILED)
