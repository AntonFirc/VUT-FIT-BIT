#
#  Makefile
#  IFJ 2017 - 1. projekt
#  
#  Autoři:  xfolty15 (Martin Foltýn), xfiloj01 (Jakub Filo),
#           xfirca00 (Anton Firc), xliska13 (Jakub Liška)
#

CC=gcc
CFLAGS=-D_GNU_SOURCE -O0 -std=c99 -Wall -g
#-lm -rdynamic
SHELL=bash
OBJ_FILES=$(patsubst %.c, %.c.o, $(shell ls *.c))

# build
all: ifj

ifj: $(OBJ_FILES)
		$(CC) $(CFLAGS) *.o -o ifj

%.c.o: %.c
		$(CC) $(CFLAGS) -c $< -o $(subst , , $<.o)

# clean
clean:
	rm -rf *.o
	rm -rf ifj
