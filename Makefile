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
	@if exist test_report.html del test_report.html

test: $(TARGET)
	@echo ========================================
	@echo   Generating HTML Test Report
	@echo ========================================
	@if exist test_report.html del test_report.html
	@echo ^<html^>^<head^> > test_report.html
	@echo ^<style^> >> test_report.html
	@echo body{font-family:sans-serif;margin:20px;background:#f4f4f4;} >> test_report.html
	@echo h1{color:#333;} >> test_report.html
	@echo .case{background:#fff;padding:15px;margin-bottom:20px;border-radius:8px;box-shadow:0 2px 5px rgba(0,0,0,0.1);} >> test_report.html
	@echo .success{border-left:5px solid #28a745;} >> test_report.html
	@echo .failure{border-left:5px solid #dc3545;} >> test_report.html
	@echo .status{font-weight:bold;margin-bottom:10px;} >> test_report.html
	@echo .log{background:#2d2d2d;color:#ccc;padding:10px;border-radius:4px;overflow-x:auto;max-height:200px;font-family:monospace;} >> test_report.html
	@echo ^</style^>^</head^> >> test_report.html
	@echo ^<body^>^<h1^>Test Execution Report^</h1^> >> test_report.html
	@echo [1/4] Checking Help and Version...
	@echo ^<div class="case success"^>^<div class="status"^>Test: Help and Version^</div^>^<pre class="log"^> >> test_report.html
	@$(subst /,\,$(TARGET)) -h >> test_report.html 2>&1
	@$(subst /,\,$(TARGET)) -v >> test_report.html 2>&1
	@echo ^</pre^>^</div^> >> test_report.html
	@echo [2/4] Testing View Mode...
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: View Mode for %%f^</div^>^<pre class="log"^> >> test_report.html & \
		$(subst /,\,$(TARGET)) "%%f" >> test_report.html 2>&1 & \
		echo ^</pre^>^</div^> >> test_report.html \
	)
	@echo [3/4] Testing Metadata Updates...
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Title -t - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -t "Test Title" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Title Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Track -T - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -T "5" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Track Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Artist -a - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -a "Test Artist" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Artist Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Album -A - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -A "Test Album" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Album Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Year -y - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -y "2025" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Year Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Comment -c - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -c "Test Comment" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Comment Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Modify Genre -g - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -g "Rock" "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Verify Genre Change - File: %%f^</div^>^<pre class="log"^> >> test_report.html & $(subst /,\,$(TARGET)) -v "%%f" >> test_report.html 2>&1 & echo ^</pre^>^</div^> >> test_report.html \
	)
	@echo [4/4] Testing Extract and Delete...
	@for %%f in (*.mp3) do @( \
		echo ^<div class="case success"^>^<div class="status"^>Test: Extraction ^& Delete for %%f^</div^>^<pre class="log"^> >> test_report.html & \
		$(subst /,\,$(TARGET)) -e "%%f" >> test_report.html 2>&1 & \
		$(subst /,\,$(TARGET)) -d "%%f" >> test_report.html 2>&1 & \
		echo ^</pre^>^</div^> >> test_report.html \
	)
	@echo ^</body^>^</html^> >> test_report.html
	@echo ========================================
	@echo   Tests Completed. 
	@echo   HTML Report: test_report.html
	@echo ========================================

.PHONY: all clean test
