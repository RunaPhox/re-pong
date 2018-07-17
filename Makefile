MAIN = main.c
LFLAGS = -lSDL2
CFLAGS = -Wall -std=c11 -Wextra -Wuninitialized -Wmissing-declarations -Wunreachable-code -Wcast-align -Wcast-qual -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wundef -Wno-unused -Wno-parentheses -Werror -pedantic -ggdb3

all: $(MAIN)
	gcc $(LFLAGS) $(CFLAGS) $(MAIN)
