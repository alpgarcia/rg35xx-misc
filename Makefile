# Compiler
CC = g++
CROSS_COMPILE = /opt/cdata/rg35xx/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-

CFLAGS = -fdata-sections -ffunction-sections -fPIC -flto -Wall
#-Ofast
LIBS = -lSDL -lSDL_image -lSDL_ttf #-lSDL_gfx -lz -lpthread -lm -lboost_locale# -lasound

# You can use Ofast too but it can be more prone to bugs, careful.
CFLAGS += -g -Iinclude/
LDFLAGS = -Wl,--start-group $(LIBS) -Wl,--end-group -Wl,--as-needed -Wl,--gc-sections -flto

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = output

# Source, object and binary files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
BINS := $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%,$(SOURCES))

# Remove command
rm = rm -f

# Targets
.PHONY: rg35xx
rg35xx: CC = $(CROSS_COMPILE)g++
rg35xx: all

.PHONY: all
all: remove prepare $(OBJECTS) $(BINS)

.PHONY: prepare
prepare:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	@echo "[PREPARE] Directories created!\n"

$(BINS): $(BINDIR)/%: $(OBJDIR)/%.o
	$(CC) $< $(LDFLAGS) $(LIBS) -o $@
	@echo "[BIN: "$@"] Linking complete from "$<"\n"

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[OBJ: "$@"] Compiled successfully from "$<"\n"

.PHONY: clean
clean:
	$(rm) $(OBJECTS)
	@echo "[CLEAN] Objects removed!\n"

.PHONY: remove
remove: clean
	$(rm) $(BINS)
	@echo "[REMOVE] Binary files removed!\n"
