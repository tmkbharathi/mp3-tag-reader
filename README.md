# MP3 Tag Reader

A simple C-based utility to read and update MP3 (ID3v1 and ID3v2) tags.

## Project Structure
- `inc/`: Header files
- `src/`: Source files
- `bin/`: Compiled binaries
- `obj/`: Object files

## Build Instructions

### Prerequisites
- GCC compiler
- Make utility (e.g., `mingw32-make` on Windows)

### Commands
To build the project:
```bash
make
```

To run automated tests:
```bash
make test
```

To clean build artifacts:
```bash
make clean
```

## Usage
The binary is generated in the `bin/` directory.

### Windows
```cmd
bin\mp3_tag_reader.exe <filename.mp3>
```

### Linux
```bash
bin/a.out <filename.mp3>
```
