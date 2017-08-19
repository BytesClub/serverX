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
CC = gcc

# Flags for Compiler
CFLAGS = -ggdb3 -O0 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wshadow -U__STRICT_ANSI__

# Flags for Linker
LFLAGS = -lm

# Flags for Directories
DIR = -I$(INC)
VPATH = $(SRC)

# Executable
EXE = serverX

# Directories
BIN = bin
INC = inc
LIB = src/lib
SRC = src
TST = public

# Header(s)
HEADER = $(wildcard $(INC)/*.h)

# Source(s)
SOURCE = $(wildcard $(SRC)/*.c)

# Windows_NT Support
ifeq ($(OS),Windows_NT)
	VPATH += $(LIB)
	SOURCE += $(wildcard $(LIB)/*.c)
	LFLAGS += -lws2_32 -lwsock32
else
	# POSIX Multi-thread
	LFLAGS += -pthread
endif

# Object File(s)
OBJECT = $(addprefix $(BIN)/, $(notdir $(SOURCE:.c=.o)))

# Default Target
$(BIN)/$(EXE): $(OBJECT)
	$(CC) $^ -o $@ $(LFLAGS)

$(BIN)/%.o: %.c $(HEADER)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(DIR) -c $< -o $@

# Test Option
test: $(BIN)/$(EXE) $(TST)
	@echo "Testing of application started."
	$^

# Help Option
help:
	@echo "Header: $(HEADER)"
	@echo "Source: $(SOURCE)"
	@echo "Object: $(OBJECT)"

# House-keeping
clean:
	@rm -rf core $(EXE) bin *.o *.exe

.PHONY: test clean
