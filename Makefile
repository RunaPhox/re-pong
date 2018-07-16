MAIN = main.c
LFLAGS = -lSDL2
CFLAGS = -Wall -Wextra -Winit-self -Wuninitialized -Wmissing-declarations -fmax-errors=5 -Wstrict-aliasing -Wunreachable-code -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Wlogical-op -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option -ansi -Werror -pedantic -ggdb3

all: $(MAIN)
	gcc $(LFLAGS) $(CFLAGS) $(MAIN)
