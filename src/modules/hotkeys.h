#ifndef CVIM_HOTKEYS_H
#define CVIM_HOTKEYS_H

#include <map>
#include <string>
#include <functional>
#include "terminal.h"
#include "editor.h"

namespace cvim {

class Editor;

class HotkeyManager {
public:
    HotkeyManager();
    HotkeyManager(Editor* editor);
    ~HotkeyManager();
    
    void setEditor(Editor* editor);
    
    // Register a normal mode key binding
    void addNormalModeBinding(Key key, std::function<void()> action);
    void addNormalModeBinding(char key, std::function<void()> action);
    
    // Register a visual mode key binding
    void addVisualModeBinding(Key key, std::function<void()> action);
    void addVisualModeBinding(char key, std::function<void()> action);
    
    // Register an insert mode key binding
    void addInsertModeBinding(Key key, std::function<void()> action);
    void addInsertModeBinding(char key, std::function<void()> action);
    
    // Register a command mode key binding
    void addCommandModeBinding(Key key, std::function<void()> action);
    void addCommandModeBinding(char key, std::function<void()> action);
    
    // Handle a key input in the given mode
    bool handleKey(Mode mode, const KeyInput& input);
    
    // Setup default bindings
    void setupDefaultBindings();
    
private:
    Editor* editor_;
    std::map<int, std::map<Key, std::function<void()> > > keyBindings_;
    std::map<int, std::map<char, std::function<void()> > > charBindings_;
};

} // namespace cvim

#endif // CVIM_HOTKEYS_H
