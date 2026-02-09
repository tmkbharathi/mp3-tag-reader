# Makefile for MP3 Tag Reader

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    SHELL = C:\Windows\System32\cmd.exe
    CC = gcc
    TARGET_EXT = .exe
    MKDIR = if not exist $(1) mkdir $(1)
    RM = if exist $(1) rmdir /s /q $(1)
    BIN_NAME = mp3_tag_reader
else
    # Linux/Mac settings
    CC = gcc
    TARGET_EXT = 
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
    # User requested a.out for Linux
    BIN_NAME = a.out
endif

CFLAGS = -Wall -Wextra -Iinc
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/$(BIN_NAME)$(TARGET_EXT)

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	$(call MKDIR,$(BIN_DIR))

$(OBJ_DIR):
	$(call MKDIR,$(OBJ_DIR))

clean:
	$(call RM,$(OBJ_DIR))
	$(call RM,$(BIN_DIR))

.PHONY: all clean
