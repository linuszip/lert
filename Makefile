CC = cc
DFLAGS = -Wall -Wextra -ggdb
CFLAGS = -Wall -Wextra 
BUILD_DIR = build
DEBUG_DIR = debug
VPATH = src
OBJECTS = $(addprefix $(BUILD_DIR)/, conf.o main.o core.o globals.o)
DEBUG_OBJS = $(addprefix $(DEBUG_DIR)/, conf.o main.o core.o globals.o)


all: $(OBJECTS)
	$(CC) $(CFLAGS) -o lert $(OBJECTS)

$(OBJECTS): globals.h | $(BUILD_DIR)
build/main.o build/core.o: core.h
build/main.o: conf.h

$(BUILD_DIR)/%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@


$(DEBUG_OBJS): globals.h | $(DEBUG_DIR)
debug/main.o debug/core.o: core.h
debug/main.o: conf.h

$(DEBUG_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir build

$(DEBUG_DIR):
	mkdir debug

dbug: $(DEBUG_OBJS) 
	$(CC) $(DFLAGS) -o lert_debug $(DEBUG_OBJS)


.PHONY: clean
clean:
	rm -f build/* lert lert_debug debug/*
