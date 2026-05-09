CC = clang
CFLAGS = -Wall -Wextra -std=c11 -O2

WIN_CC = x86_64-w64-mingw32-gcc
WIN_EXEC = OmniSave.exe

SRC = main.c config_parser.c path_utils.c sync_engine.c process_manager.c
OBJ = $(SRC:.c=.o)
EXEC = OmniSave

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

windows:
	$(WIN_CC) $(CFLAGS) -o $(WIN_EXEC) $(SRC)

%.o: %.c omnisave.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC) $(WIN_EXEC)
