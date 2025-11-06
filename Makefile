#Compiler und Flags
CC := clang
CFLAGS := -Wall -Wextra -O2 -g
LDFLAGS :=

BINDIR := bin
BUILDDIR := build

SRCS_MAIN := $(wildcard main/*.c)
SRCS_INPUT := $(wildcard input/*.c)
SRCS_TERM := $(wildcard term/*.c)

OBJS_MAIN := $(patsubst %.c,$(BUILDDIR)/main/%.o,$(notdir $(SRCS_MAIN)))
OBJS_INPUT := $(patsubst %.c,$(BUILDDIR)/input/%.o,$(notdir $(SRCS_INPUT)))
OBJS_TERM := $(patsubst %.c,$(BUILDDIR)/term/%.o,$(notdir $(SRCS_TERM)))

OBJS := $(OBJS_MAIN) $(OBJS_INPUT) $(OBJS_TERM)

.PHONY: all clean main input term

all: $(BINDIR)/lert

$(BINDIR)/lert: $(OBJS) | $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Per-module targets (create static libs for each module)
main: $(BUILDDIR)/libmain.a
input: $(BUILDDIR)/libinput.a
term: $(BUILDDIR)/libterm.a

$(BUILDDIR)/libmain.a: $(OBJS_MAIN) | $(BUILDDIR)/main
	ar rcs $@ $^

$(BUILDDIR)/libinput.a: $(OBJS_INPUT) | $(BUILDDIR)/input
	ar rcs $@ $^

$(BUILDDIR)/libterm.a: $(OBJS_TERM) | $(BUILDDIR)/term
	ar rcs $@ $^

# Compile rule: build/<module>/<file>.o from <module>/<file>.c
$(BUILDDIR)/%/%.o: %/%.c | $(BUILDDIR)/%
	$(CC) $(CFLAGS) -c -o $@ $<

# Directories
$(BINDIR):
	mkdir -p $@

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/%:
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

