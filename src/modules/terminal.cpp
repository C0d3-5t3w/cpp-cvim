#include "terminal.h"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <cstring>
#include <signal.h>
#include <stdexcept>

namespace cvim {

static Terminal* globalTerminalInstance = nullptr;

static void handleSigWinch(int sig) {
    if (globalTerminalInstance) {
        globalTerminalInstance->refreshScreen();
    }
}

Terminal::Terminal() : rawMode_(false), originalTerminalState_(-1) {
    globalTerminalInstance = this;
}

Terminal::~Terminal() {
    shutdown();
    globalTerminalInstance = nullptr;
}

bool Terminal::initialize() {
    setupTerminal();
    
    // Setup resize handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSigWinch;
    sigaction(SIGWINCH, &sa, NULL);
    
    // Get initial terminal size
    handleResize();
    clearScreen();
    return true;
}

void Terminal::setupTerminal() {
    if (rawMode_) return;

    // Save current terminal attributes
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        throw std::runtime_error("Could not get terminal attributes");
    }
    originalTerminalState_ = term.c_lflag;
    
    // Switch to raw mode
    struct termios raw = term;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
        throw std::runtime_error("Could not set terminal attributes");
    }
    
    rawMode_ = true;
}

void Terminal::restoreTerminal() {
    if (!rawMode_) return;
    
    // Restore original terminal settings
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) >= 0) {
        term.c_lflag = originalTerminalState_;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
    }
    
    rawMode_ = false;
}

void Terminal::shutdown() {
    restoreTerminal();
    clearScreen();
    setCursor(0, 0);
}

KeyInput Terminal::getInput() {
    KeyInput input;
    input.key = UNKNOWN;
    input.character = 0;
    input.shift = false;
    input.ctrl = false;
    input.alt = false;

    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == 27) { // ESC sequence
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) {
                input.key = ESCAPE;
                input.character = 27;
                return input;
            }
            if (read(STDIN_FILENO, &seq[1], 1) != 1) {
                input.key = ESCAPE;
                input.character = 27;
                return input;
            }
            
            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(STDIN_FILENO, &seq[2], 1) != 1) return input;
                    if (seq[2] == '~') {
                        switch (seq[1]) {
                            case '1': input.key = HOME; return input;
                            case '3': input.key = DELETE; return input;
                            case '4': input.key = END; return input;
                            case '5': input.key = PAGE_UP; return input;
                            case '6': input.key = PAGE_DOWN; return input;
                        }
                    }
                } else {
                    switch (seq[1]) {
                        case 'A': input.key = UP; return input;
                        case 'B': input.key = DOWN; return input;
                        case 'C': input.key = RIGHT; return input;
                        case 'D': input.key = LEFT; return input;
                        case 'H': input.key = HOME; return input;
                        case 'F': input.key = END; return input;
                    }
                }
            }
        } else {
            // Handle control keys
            if (c < 32) {
                switch (c) {
                    case 13: input.key = ENTER; input.character = c; return input;
                    case 9: input.key = TAB; input.character = c; return input;
                    case 127: input.key = BACKSPACE; input.character = c; return input;
                    case 1: input.key = CTRL_A; input.character = c; input.ctrl = true; return input;
                    case 2: input.key = CTRL_B; input.character = c; input.ctrl = true; return input;
                    case 3: input.key = CTRL_C; input.character = c; input.ctrl = true; return input;
                    case 4: input.key = CTRL_D; input.character = c; input.ctrl = true; return input;
                    case 5: input.key = CTRL_E; input.character = c; input.ctrl = true; return input;
                    case 6: input.key = CTRL_F; input.character = c; input.ctrl = true; return input;
                    // and so on for other control keys
                    default: input.key = NORMAL; input.character = c; return input;
                }
            } else {
                input.key = NORMAL;
                input.character = c;
                return input;
            }
        }
    }
    return input;
}

void Terminal::render(const ViewData& viewData) {
    clearScreen();
    
    // Render text content
    size_t visibleLines = size_.height - 2; // Reserve space for status and command line
    for (size_t i = 0; i < viewData.lines.size() && i < visibleLines; i++) {
        std::cout << viewData.lines[i] << "\r\n";
    }
    
    // Render status line
    setCursor(size_.height - 2, 0);
    std::cout << viewData.statusLine;
    
    // Render command line
    setCursor(size_.height - 1, 0);
    std::cout << viewData.mode << " " << viewData.commandLine;
    
    // Set cursor to editing position
    setCursor(viewData.cursorRow, viewData.cursorCol);
    
    refreshScreen();
}

Size Terminal::getSize() const {
    return size_;
}

void Terminal::setCursor(int row, int col) {
    std::cout << "\x1b[" << (row + 1) << ";" << (col + 1) << "H";
}

void Terminal::clearScreen() {
    std::cout << "\x1b[2J\x1b[H";
}

void Terminal::refreshScreen() {
    std::cout.flush();
    handleResize();
}

void Terminal::handleResize() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        // Default if we can't get terminal size
        size_ = {24, 80};
    } else {
        size_ = {ws.ws_row, ws.ws_col};
    }
}

} // namespace cvim
