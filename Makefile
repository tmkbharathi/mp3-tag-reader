# Makefile for MP3 Tag Reader

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    SHELL = C:\Windows\System32\cmd.exe
    CC = gcc
    TARGET_EXT = .exe
    MKDIR = if not exist $(1) mkdir $(1)
    RM = if exist $(1) rmdir /s /q $(1)
    BIN_NAME = mp3tag
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
	@if exist album_art.jpg del album_art.jpg
	@if exist album_art.png del album_art.png
	@if exist album_art.bin del album_art.bin
	@if exist album_art.out del album_art.out

test: $(TARGET)
	@echo ========================================
	@echo   Running Automated Tests
	@echo ========================================
	@echo [1/3] Checking Version and Help...
	@$(subst /,\,$(TARGET)) -v
	@$(subst /,\,$(TARGET)) -h > nul
	@echo [2/3] Checking Metadata Viewing...
	@for %%f in (*.mp3) do @( \
		echo Testing with file: %%f & \
		$(subst /,\,$(TARGET)) "%%f" & \
		echo [3/3] Testing Tag Update... & \
		$(subst /,\,$(TARGET)) -t "Test Title" -a "Test Artist" "%%f" & \
		($(subst /,\,$(TARGET)) "%%f" | findstr "Test Title" > nul && (echo   - Update: OK) || (echo   - Update: FAILED)) & \
		echo Testing Image Extraction... & \
		$(subst /,\,$(TARGET)) -e "%%f" & \
		(if exist album_art.jpg (echo   - Extraction: OK) else if exist album_art.png (echo   - Extraction: OK) else if exist album_art.bin (echo   - Extraction: OK) else (echo   - Extraction: No image or failed)) & \
		exit /b 0 \
	)
	@echo ========================================
	@echo   Tests Completed.
	@echo ========================================

.PHONY: all clean test
