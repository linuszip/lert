CC = cc
DFLAGS = -Wall -Wextra -g -fsanitize=address
CFLAGS = -Wall -Wextra -O3
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

debug: $(OBJECTS)
	$(CC) $(DFLAGS) -o lert $(OBJECTS)


.PHONY: clean
clean:
	rm -f build/* lert
