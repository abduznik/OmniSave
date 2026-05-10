# Windows Build (MinGW)
CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -O2 -mwindows
LIBS = -lshlwapi -lshell32

SRC = main.c config.c sync.c launch.c lock.c platform_win32.c
OBJ = $(SRC:.c=.o)
TARGET = OmniSave.exe

# Test Build (Native GCC)
TEST_CC = gcc
TEST_CFLAGS = -Wall -g -I. -Itests -Ivendor/unity/src
TEST_SRC = config.c sync.c launch.c lock.c platform_mock.c \
           tests/test_config.c tests/test_sync.c tests/test_lock.c \
           tests/test_launch.c tests/test_runner.c vendor/unity/src/unity.c
TEST_TARGET = test_runner

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c omnisave.h platform.h
	$(CC) $(CFLAGS) -c $< -o $@

test: vendor/unity/src/unity.c
	$(TEST_CC) $(TEST_CFLAGS) $(TEST_SRC) -o $(TEST_TARGET)
	./$(TEST_TARGET)

vendor/unity/src/unity.c:
	mkdir -p vendor
	git clone https://github.com/ThrowTheSwitch/Unity.git vendor/unity

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_TARGET)

test_clean:
	rm -rf vendor $(TEST_TARGET)

.PHONY: all clean test test_clean
