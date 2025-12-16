CC = cc
CFLAGS = -Wall -Wextra -g
BUILD_DIR = build
VPATH = src
OBJECTS = $(addprefix $(BUILD_DIR)/, main.o core.o globals.o)


all: $(OBJECTS)
	$(CC) $(CFLAGS) -o lert $(OBJECTS)

$(OBJECTS): globals.h | $(BUILD_DIR)
build/main.o build/core.o: core.h

$(BUILD_DIR)/%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir build

.PHONY: clean
clean:
	rm -f build/* lert
