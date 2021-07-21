CC = clang
CFLAGS = -Wall -Wextra -Werror -Wno-newline-eof -Wno-unused-parameter -pedantic -std=c99 -ferror-limit=1
LDFLAGS = -lm

PROGRAM = program
SOURCE_FILES = $(shell find . -type f -name '*.c')
HEADER_FILES = $(shell find . -type f -name '*.h')
OBJECT_FILES = $(SOURCE_FILES:.c=.o)

.PHONY: all build clean run

all: build

build: $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJECT_FILES)

run: build
	./$(PROGRAM)

$(PROGRAM): $(OBJECT_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)