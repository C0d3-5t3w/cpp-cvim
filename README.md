# cpp-cvim: A Vim Clone in C++

cpp-cvim is a lightweight implementation of Vim in C++, providing the core functionality of the popular text editor.

## Features

- Modal editing (normal, insert, visual modes)
- Basic movement commands (h, j, k, l, etc.)
- Text manipulation commands
- Command-line interface with `:` commands
- File operations (open, save)
- Syntax highlighting for common languages
- Customizable through configuration files

## Building

### Requirements

- C++11 compatible compiler (GCC 4.8+, Clang 3.3+, MSVC 2015+)
- ncurses library (for terminal interface)
- yaml-cpp library (optional, for configuration)

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get install build-essential libncurses5-dev libyaml-cpp-dev
```

#### macOS
```bash
brew install ncurses yaml-cpp
```

#### Windows
For Windows, consider using WSL (Windows Subsystem for Linux) or MSYS2.

### Building with Make

```bash
# Install dependencies if needed
make deps

# Build the project
make

# Debug build
make debug

# Run the application
make run
```

### Building with CMake

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
# Open a file
./bin/cvim path/to/file.txt

# Open multiple files
./bin/cvim file1.txt file2.txt
```

## Key Bindings

cpp-cvim follows Vim's key bindings:

### Normal Mode

- `h`, `j`, `k`, `l`: Move cursor left, down, up, right
- `w`, `b`: Move forward/backward by word
- `0`, `$`: Move to start/end of line
- `gg`, `G`: Move to start/end of file
- `i`: Enter insert mode
- `v`: Enter visual mode
- `V`: Enter visual line mode
- `:`: Enter command mode
- `/`: Search forward
- `?`: Search backward
- `n`, `N`: Navigate search results
- `d`, `y`, `p`: Delete, yank, paste

### Insert Mode

- `ESC`: Return to normal mode
- All other keys: Insert text

### Command Mode

- `:w`: Save file
- `:q`: Quit
- `:wq`: Save and quit
- `:e filename`: Edit file

## Troubleshooting

If you encounter build errors:

1. Make sure you have a C++11 compliant compiler
2. Check that ncurses is properly installed
3. If yaml-cpp is not available, the program will use simpler configuration loading

## Development

use "make help" to see all available commands for building.
