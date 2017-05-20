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
CFLAGS = -ggdb3 -O0 -Wall -Werror -Wextra -Wno-sign-compare -Wshadow 

# Flags for Directories
DIR = -I$(INC)

# Executable
EXE = serverX

# Directories
SRC = src
INC = inc
BIN = bin

# Source and Object File(s)
SOURCE = $(wildcard $(SRC)/*.c)
OBJECT = $(patsubst %,$(BIN)/%, $(notdir $(SOURCE:.c=.o)))

# Default Target
$(BIN)/$(EXE): $(OBJECT)
	$(CC) $^ -o $@

$(BIN)/%.o: $(SRC)/%.c
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(DIR) -c $< -o $@

# Help Option
help:
	@echo "src: $(SOURCE)"
	@echo "obj: $(OBJECT)"

# House-keeping
clean:
	rm -rf core $(EXE) bin/* *.o *.exe
