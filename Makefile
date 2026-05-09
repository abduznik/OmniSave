CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -O2 -mwindows
LIBS = -lshlwapi -lshell32

SRC = main.c config.c sync.c launch.c lock.c
OBJ = $(SRC:.c=.o)
TARGET = OmniSave.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c omnisave.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
