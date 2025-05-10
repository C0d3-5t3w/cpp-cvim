#include "hotkeys.h"
#include "editor.h" // Includes Mode enum definition
#include "buffer_utils.h"
#include <functional>

namespace cvim {

HotkeyManager::HotkeyManager() : editor_(nullptr) {}

HotkeyManager::HotkeyManager(Editor* editor) : editor_(editor) {
    setupDefaultBindings();
}

HotkeyManager::~HotkeyManager() {}

void HotkeyManager::setEditor(Editor* editor) {
    editor_ = editor;
    setupDefaultBindings();
}

void HotkeyManager::addNormalModeBinding(Key key, std::function<void()> action) {
    keyBindings_[static_cast<int>(Mode::NORMAL)][key] = action;
}

void HotkeyManager::addNormalModeBinding(char key, std::function<void()> action) {
    charBindings_[static_cast<int>(Mode::NORMAL)][key] = action;
}

void HotkeyManager::addVisualModeBinding(Key key, std::function<void()> action) {
    keyBindings_[static_cast<int>(Mode::VISUAL)][key] = action;
}

void HotkeyManager::addVisualModeBinding(char key, std::function<void()> action) {
    charBindings_[static_cast<int>(Mode::VISUAL)][key] = action;
}

void HotkeyManager::addInsertModeBinding(Key key, std::function<void()> action) {
    keyBindings_[static_cast<int>(Mode::INSERT)][key] = action;
}

void HotkeyManager::addInsertModeBinding(char key, std::function<void()> action) {
    charBindings_[static_cast<int>(Mode::INSERT)][key] = action;
}

void HotkeyManager::addCommandModeBinding(Key key, std::function<void()> action) {
    keyBindings_[static_cast<int>(Mode::COMMAND)][key] = action;
}

void HotkeyManager::addCommandModeBinding(char key, std::function<void()> action) {
    charBindings_[static_cast<int>(Mode::COMMAND)][key] = action;
}

bool HotkeyManager::handleKey(Mode mode, const KeyInput& input) {
    if (!editor_) return false;
    
    int modeInt = static_cast<int>(mode);
    
    // First check key bindings
    auto modeKeyBindings = keyBindings_.find(modeInt);
    if (modeKeyBindings != keyBindings_.end()) {
        auto binding = modeKeyBindings->second.find(input.key);
        if (binding != modeKeyBindings->second.end()) {
            binding->second();
            return true;
        }
    }
    
    // Then check character bindings if this is a normal character
    if (input.key == Key::NORMAL) {
        auto modeCharBindings = charBindings_.find(modeInt);
        if (modeCharBindings != charBindings_.end()) {
            auto binding = modeCharBindings->second.find(input.character);
            if (binding != modeCharBindings->second.end()) {
                binding->second();
                return true;
            }
        }
    }
    
    return false;
}

void HotkeyManager::setupDefaultBindings() {
    if (!editor_) return;
    
    // Normal mode movement bindings
    addNormalModeBinding('h', [this]() {
        editor_->getCursor().moveLeft(1);
    });
    
    addNormalModeBinding('j', [this]() {
        editor_->getCursor().moveDown(1);
    });
    
    addNormalModeBinding('k', [this]() {
        editor_->getCursor().moveUp(1);
    });
    
    addNormalModeBinding('l', [this]() {
        editor_->getCursor().moveRight(1);
    });
    
    addNormalModeBinding('0', [this]() {
        editor_->getCursor().moveToLineStart();
    });
    
    addNormalModeBinding('$', [this]() {
        editor_->getCursor().moveToLineEnd();
    });
    
    addNormalModeBinding('w', [this]() {
        editor_->moveToNextWord();
    });
    
    addNormalModeBinding('b', [this]() {
        editor_->moveToPrevWord();
    });
    
    addNormalModeBinding('e', [this]() {
        editor_->moveToWordEnd();
    });
    
    // Normal mode mode switching
    addNormalModeBinding('i', [this]() {
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('a', [this]() {
        editor_->getCursor().moveRight(1);
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('A', [this]() {
        editor_->getCursor().moveToLineEnd();
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('I', [this]() {
        editor_->getCursor().moveToLineStart();
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('o', [this]() {
        editor_->insertLineBelow();
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('O', [this]() {
        editor_->insertLineAbove();
        editor_->setMode(Mode::INSERT);
    });
    
    addNormalModeBinding('v', [this]() {
        editor_->setMode(Mode::VISUAL);
    });
    
    addNormalModeBinding('V', [this]() {
        editor_->setMode(Mode::VISUAL_LINE);
    });
    
    addNormalModeBinding(':', [this]() {
        editor_->setMode(Mode::COMMAND);
    });
    
    // Insert mode bindings
    addInsertModeBinding(Key::ESCAPE, [this]() {
        editor_->setMode(Mode::NORMAL);
    });
    
    addInsertModeBinding(Key::ENTER, [this]() {
        auto& cursor = editor_->getCursor();
        auto buffer = editor_->getCurrentBuffer();
        if (buffer) {
            BufferUtils::insertLineBreak(*buffer, cursor.getRow(), cursor.getCol());
            cursor.setCol(0);
            cursor.moveDown(1);
        }
    });
    
    addInsertModeBinding(Key::BACKSPACE, [this]() {
        auto& cursor = editor_->getCursor();
        auto buffer = editor_->getCurrentBuffer();
        if (buffer) {
            if (cursor.getCol() > 0) {
                BufferUtils::deleteCharAtPosition(*buffer, cursor.getRow(), cursor.getCol() - 1);
                cursor.moveLeft(1);
            } else if (cursor.getRow() > 0) {
                int prevLineLength = BufferUtils::getLineLength(*buffer, cursor.getRow() - 1);
                BufferUtils::joinLines(*buffer, cursor.getRow() - 1);
                cursor.setRow(cursor.getRow() - 1);
                cursor.setCol(prevLineLength);
            }
        }
    });
    
    // Visual mode bindings
    addVisualModeBinding(Key::ESCAPE, [this]() {
        editor_->setMode(Mode::NORMAL);
        editor_->clearSelection();
    });
    
    // Command mode bindings
    addCommandModeBinding(Key::ESCAPE, [this]() {
        editor_->setMode(Mode::NORMAL);
        editor_->clearCommandBuffer();
    });
    
    addCommandModeBinding(Key::ENTER, [this]() {
        editor_->executeCommand();
        editor_->setMode(Mode::NORMAL);
    });
    
    addCommandModeBinding(Key::BACKSPACE, [this]() {
        editor_->backspaceCommandBuffer();
    });
}

} // namespace cvim
