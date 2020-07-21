TITLE   := pfs
VERSION := 1.0.0

CC      := g++
CFLAGS  := -std=c++17 \
           -O3 \
           -Wall \
           -DBUILD_TITLE='"$(TITLE)"' \
           -DBUILD_VERSION='"$(VERSION)"'
LIBS    := -lncurses

BINDIR  := bin
SRCDIR  := src
INCDIR  := include
OBJDIR  := obj
INSDIR  := /usr/local/bin

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.cpp=.o))

.PHONY: all clean init build

all: build

clean:
	@rm -rf $(BINDIR)
	@rm -rf $(OBJDIR)

init:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

build: init $(BINDIR)/$(TITLE)

$(BINDIR)/$(TITLE): $(OBJECTS)
	@$(CC) -o $(BINDIR)/$(TITLE) $^ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<
	@echo "CC  $@"

install: build
	@mkdir -p $(INSDIR)
	@cp ./$(BINDIR)/$(TITLE) $(INSDIR)/$(TITLE)
	@echo "Installed $(INSDIR)/$(TITLE)"

uninstall:
	@rm $(INSDIR)/$(TITLE)
	@echo "Uninstalled $(INSDIR)/$(TITLE)"
