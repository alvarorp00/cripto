CC = gcc
CFLAGS =  -Wall -g
LDFLAGS = -pthread -lpthread -lm -lcrypto -lgmp

INC=inc
OUT=out
OBJ=obj
SRC=src
SRCTEST=srctest
TEST=test

MODEL=FOO

VG_OPTIONS=--leak-check=full --show-leak-kinds=all --track-origins=yes
DEBUG_MODE=__DEBUG__

SOURCEFILES := $(shell find $(SRC) -name '*.c')

default: clean ## Equivalent to 'make all'
				@ echo "Compiling all..."
				@ $(CC) -I $(INC) $(CFLAGS) -D $(MODEL) -o $(OUT)/$(subst __%__,%,$(MODEL)) $(SOURCEFILES) $(LDFLAGS)
				@ echo "Executable generated successfully!"

help:	## This guide
		@@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

all: default ## Produces agent compiled executable

valgrind: all ## Compiles and executes using valgrind mode
		@ valgrind $(VG_OPTIONS) ./$(OUT)/$(MODEL)

debug: clean ## Compiles using -D __DEBUG__ option
		@ echo "Compiling in debug mode..."
		@ $(CC) -I $(INC) $(CFLAGS) -D $(DEBUG_MODE) -o $(OUT)/debug $(SOURCEFILES) $(LDFLAGS)

calclib_test: clean
		@ $(CC) -I $(INC) $(CFLAGS) -o $(TEST)/calclib_test $(SRCTEST)/calcLibTest.c $(SRC)/calclib.c $(LDFLAGS)

affine: MODEL=__AFFINE__ ## Compiles with Affine cipher
affine: default

hill: MODEL=__HILL__ ## Compiles with Hill cipher
hill: default

vigenere: MODEL=__VIGENERE__ ## Compiles with Vigenere cipher
vigenere: default

flow: MODEL=__FLOW__ ## Compiles with Flow cipher
flow: default

.PHONY: clean

clean: ## Cleans output and object files
				@ rm -f $(OBJ)/*
				@ rm -f $(OUT)/*
				@ rm -f $(COMPILED)
				@ rm -f $(TEST)/*
