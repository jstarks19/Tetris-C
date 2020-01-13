define welcome
TETRIS                                                                                                                   
endef

define bye
BYE
endef

export welcome
export bye

all: tetris
# This is a rule
# tetris is a target
tetris: 
	gcc tetris.c -o tetris `sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm
# $@ = tetris = "target"
# So what this does is says to make an executable with its given target
# $^ = $(obj_bt) = "prerequisite"
# This says take all of the prequeistes, and links them all together

# .PHONY is for when you want the Makefile to run commands that don't represent physical files
# This will make it commands like 'clean' commands that are constantly out-of-date, so they don't
# depend on the  state of the file system to run.
.PHONY: clean all

clean: 
	rm tetris

# Naming Conventions
# Rule = each of these blocks
# Rule Format:
# target: prerequisites ...
# 	recipe
# 	.
#	.
#	.

