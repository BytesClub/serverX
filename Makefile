# Copyright 2017 The serverX Authors.  All rights reserved.
# Maintainer: Bytes Club (https://bytesclub.github.io)<bytes-club@googlegroups.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

# Define Compiler
CC ?= gcc

# Flags for Compiler
CFLAGS = -ggdb3 -O0 -O3 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wshadow -U__STRICT_ANSI__ -pedantic

# Flags for Linker
LFLAGS = -lm -pthread

# Flags for Directories
DIR = -I$(INC)
VPATH = $(SRC)

# Executable
EXE = $(BIN)/$(PROG)
PROG = serverX

# PREFIX is environment variable, but if it is not set, then set default value
ifeq ($(prefix),)
	prefix = /usr/local
endif
ifeq ($(PREFIX),)
	PREFIX = $(prefix)
endif

# Directories
BIN = bin
INC = inc
LIB = src/lib
SRC = src
TST = test

# Header(s)
HEADER = $(INC)/serverX.h $(INC)/build.h

# Source(s)
SOURCE = $(wildcard $(SRC)/*.c)

# Windows_NT Support
ifeq ($(OS),Windows_NT)
	VPATH += $(LIB)
	SOURCE += $(wildcard $(LIB)/*.c)
	HEADER += $(INC)/winserverX.h
	LFLAGS += -lws2_32 -lwsock32
endif

# Object File(s)
OBJECT = $(addprefix $(BIN)/, $(notdir $(SOURCE:.c=.o)))

# Default Target
all: $(EXE)

$(EXE): $(OBJECT)
	@echo "  LD    $@    $(LFLAGS)"
	@$(CC) $^ -o $@ $(LFLAGS)

$(BIN)/%.o: %.c $(HEADER)
	@echo "  CC    $@"
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) $(DIR) -c $< -o $@

$(INC)/build.h: $(SOURCE)
	@./build.sh

.PHONY: all

# Install Option
install: $(EXE)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(PROG)

.PHONY: install

# Test Option
test: $(EXE) $(TST)
	@echo "Testing of application started."
	$^&

.PHONY: test

# Help Option
help:
	@echo "Header: $(HEADER)"
	@echo "Source: $(SOURCE)"
	@echo "Object: $(OBJECT)"

.PHONY: help

# House-keeping
clean:
	@echo "  CLEAN    $(EXE)"
	@rm -rf core $(EXE) $(BIN) *.o *.exe

.PHONY: clean
