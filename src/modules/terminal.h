#ifndef CVIM_TERMINAL_H
#define CVIM_TERMINAL_H

#include <string>
#include <vector>
#include "../utils/utils.h"

namespace cvim {

enum Key { // Changed from enum class
    NORMAL,
    ESCAPE,
    ENTER,
    BACKSPACE,
    DELETE,
    TAB,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    HOME,
    END,
    PAGE_UP,
    PAGE_DOWN,
    CTRL_A,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    CTRL_H,
    CTRL_I,
    CTRL_J,
    CTRL_K,
    CTRL_L,
    CTRL_M,
    CTRL_N,
    CTRL_O,
    CTRL_P,
    CTRL_Q,
    CTRL_R,
    CTRL_S,
    CTRL_T,
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    UNKNOWN
};

struct KeyInput {
    Key key;
    char character;
    bool shift;
    bool ctrl;
    bool alt;
};

struct ViewData {
    std::vector<std::string> lines;
    int cursorRow;
    int cursorCol;
    std::string statusLine;
    std::string commandLine;
    std::string mode;
};

class Terminal {
public:
    Terminal();
    ~Terminal();

    bool initialize();
    void shutdown();

    KeyInput getInput();
    void render(const ViewData& viewData);
    
    Size getSize() const;
    void setCursor(int row, int col);
    void clearScreen();
    void refreshScreen();

private:
    void setupTerminal();
    void restoreTerminal();
    void handleResize();
    
    bool rawMode_;
    Size size_;
    int originalTerminalState_;
};

} // namespace cvim

#endif // CVIM_TERMINAL_H
