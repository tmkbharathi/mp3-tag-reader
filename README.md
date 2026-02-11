# MP3 Tag Reader

A professional C-based command-line utility designed for robust viewing and modification of MP3 metadata (supporting ID3v1 and ID3v2 tags).

## Features

- **Dynamic Tag Editing**: Modify Title, Track, Artist, Album, Year, Comment, and Genre.
- **Technical Analysis**: View MPEG layer details, bitrate, sampling frequency, and duration.
- **Album Art Management**: Extract embedded album art images (JPG/PNG).
- **Metadata Scrubbing**: Quickly delete all tag information from a file.
- **Automated Verification**: Built-in test suite with professional HTML report generation.
- **Cross-Platform Readiness**: Designed with portability in mind (Windows/Linux).

---

## Project Structure

- `src/`: Core implementation files (`main.c`, `id3_v2.c`, etc.).
- `inc/`: Header definitions for modular architecture.
- `bin/`: Output directory for the executable.
- `obj/`: Temporary object files for compilation.
- `documents/`: Supplemental documentation and design specifications.

---

## Build & Test Instructions

### Prerequisites
- **Compiler**: GCC
- **Build Tool**: Make (e.g., `mingw32-make` on Windows)

### Commands
| Command | Action |
| :--- | :--- |
| `make` | Compiles the project and generates `bin/mp3tag.exe`. |
| `make test` | Executes full test suite and generates `test_report.html`. |
| `make clean` | Removes all temporary build artifacts and object files. |

---

## Usage

The primary binary is located in `bin/mp3tag.exe`.

### Basic Operations
- **View Metadata**: `bin\mp3tag.exe <filename.mp3>`
- **Display Help**: `bin\mp3tag.exe -h`
- **Show Version/View Only**: `bin\mp3tag.exe -v <filename.mp3>`

### Metadata Modification
Use the following flags followed by a value in quotes:
```cmd
bin\mp3tag.exe -t "New Title" "song.mp3"  # Title
bin\mp3tag.exe -a "New Artist" "song.mp3" # Artist
bin\mp3tag.exe -A "New Album" "song.mp3"  # Album
bin\mp3tag.exe -y "2024" "song.mp3"       # Year
bin\mp3tag.exe -T "1" "song.mp3"          # Track
bin\mp3tag.exe -c "Comment" "song.mp3"    # Comment
bin\mp3tag.exe -g "Genre" "song.mp3"      # Genre
```

### Advanced Features
- **Extract Album Art**: `bin\mp3tag.exe -e <filename.mp3>`
- **Delete All Tags**: `bin\mp3tag.exe -d <filename.mp3>`

---

## Testing & Verification
The `make test` command follows a specialized **"Modify then Verify"** sequence. For every modification performed, the suite immediately runs a verification step using the `-v` flag to confirm the change was physically applied to the file. All results are logged in a clean, card-based **HTML Report** (`test_report.html`).
