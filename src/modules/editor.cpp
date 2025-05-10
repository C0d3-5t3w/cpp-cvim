#include "editor.h"
#include "buffer_utils.h"
#include "tabs.h"
#include "commands.h" // commands.h includes <functional>
#include "filetree.h"
#include "hotkeys.h"  // hotkeys.h includes <functional>
#include "../config/config.h"
#include "../utils/utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional> // Added as safeguard

namespace cvim {

// Buffer implementation
Buffer::Buffer(const std::string& filePath) : filePath_(filePath), modified_(false) {
    lines_.push_back("");
    if (!filePath.empty()) {
        load();
    }
}

Buffer::~Buffer() {}

bool Buffer::load() {
    if (filePath_.empty()) return false;
    
    std::ifstream file(filePath_);
    if (!file.is_open()) return false;
    
    lines_.clear();
    std::string line;
    while (std::getline(file, line)) {
        // Handle line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines_.push_back(line);
    }
    
    if (lines_.empty()) {
        lines_.push_back("");
    }
    
    modified_ = false;
    return true;
}

bool Buffer::save() {
    if (filePath_.empty()) return false;
    
    std::ofstream file(filePath_);
    if (!file.is_open()) return false;
    
    for (const std::string& line : lines_) {
        file << line << "\n";
    }
    
    modified_ = false;
    return true;
}

bool Buffer::saveAs(const std::string& filePath) {
    filePath_ = filePath;
    return save();
}

void Buffer::insertChar(char c) {
    if (lines_.empty()) {
        lines_.push_back("");
    }
    
    // Implementation would depend on cursor position being passed
    // This is simplified here
    lines_[0] += c;
    modified_ = true;
}

void Buffer::insertLine(const std::string& line) {
    lines_.push_back(line);
    modified_ = true;
}

void Buffer::deleteLine(int line) {
    if (line >= 0 && line < static_cast<int>(lines_.size())) {
        lines_.erase(lines_.begin() + line);
        modified_ = true;
        
        if (lines_.empty()) {
            lines_.push_back("");
        }
    }
}

void Buffer::deleteChar(int pos, int line) {
    if (line >= 0 && line < static_cast<int>(lines_.size())) {
        std::string& currentLine = lines_[line];
        if (pos >= 0 && pos < static_cast<int>(currentLine.size())) {
            currentLine.erase(pos, 1);
            modified_ = true;
        }
    }
}

const std::vector<std::string>& Buffer::getLines() const {
    return lines_;
}

const std::string& Buffer::getFilePath() const {
    return filePath_;
}

bool Buffer::isModified() const {
    return modified_;
}

void Buffer::setModified(bool modified) {
    modified_ = modified;
}

// Editor implementation
Editor::Editor() : terminal_(nullptr), config_(nullptr), tabManager_(nullptr), 
                   commandProcessor_(nullptr), fileTree_(nullptr), state_(), 
                   hotkeyManager_(nullptr) {}

Editor::~Editor() {
    delete tabManager_;
    delete commandProcessor_;
    delete fileTree_;
    delete hotkeyManager_;
}

void Editor::initialize(Terminal* terminal, Config* config) {
    terminal_ = terminal;
    config_ = config;
    
    // Initialize components
    tabManager_ = new TabManager();
    commandProcessor_ = new CommandProcessor(this);
    fileTree_ = new FileTree();
    hotkeyManager_ = new HotkeyManager(this);
    
    // Create empty buffer if none exists
    if (tabManager_->isEmpty()) {
        tabManager_->addTab(std::make_shared<Buffer>());
    }
}

void Editor::handleInput(const KeyInput& input) {
    // First try the hotkey manager
    if (hotkeyManager_->handleKey(state_.mode, input)) {
        // The input was handled by hotkeys
        updateStatusLine();
        return;
    }
    
    // Otherwise use the mode-specific handlers
    switch (state_.mode) { // Now uses plain enum Mode
        case NORMAL:
            handleNormalMode(input);
            break;
        case INSERT:
            handleInsertMode(input);
            break;
        case VISUAL:
        case VISUAL_LINE:
            handleVisualMode(input);
            break;
        case COMMAND:
            handleCommandMode(input);
            break;
    }
    
    updateStatusLine();
}

bool Editor::openFile(const std::string& filePath) {
    auto buffer = std::make_shared<Buffer>(filePath);
    if (buffer->load()) {
        tabManager_->addTab(buffer);
        return true;
    }
    return false;
}

bool Editor::saveFile() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        if (buffer->getFilePath().empty()) {
            // Need a file path first
            state_.statusMessage = "No filename. Use :w filename";
            return false;
        }
        return buffer->save();
    }
    return false;
}

bool Editor::saveFileAs(const std::string& filePath) {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        return buffer->saveAs(filePath);
    }
    return false;
}

ViewData Editor::getViewData() const {
    ViewData viewData;
    viewData.mode = getModeString(state_.mode);
    viewData.statusLine = state_.statusMessage;
    viewData.commandLine = state_.commandBuffer;
    viewData.cursorRow = cursor_.getRow();
    viewData.cursorCol = cursor_.getCol();
    
    auto buffer = tabManager_->getCurrentBuffer();
    if (buffer) {
        viewData.lines = buffer->getLines();
    }
    
    return viewData;
}

bool Editor::shouldQuit() const {
    return state_.quit;
}

void Editor::handleNormalMode(const KeyInput& input) {
    if (input.key == Key::ESCAPE) { // Key is now plain enum
        // Clear any pending operation
        state_.statusMessage = "";
    } else if (input.key == Key::NORMAL && input.character == ':') {
        setMode(COMMAND);
    } else if (input.key == Key::NORMAL && input.character == 'i') {
        setMode(INSERT);
    } else if (input.key == Key::NORMAL && input.character == 'v') {
        setMode(VISUAL);
    } else if (input.key == Key::NORMAL && input.character == 'V') {
        setMode(VISUAL_LINE);
    } else {
        // Check for movement keys
        handleMovement(input);
        
        // Check for operation keys
        handleOperation(input);
    }
}

void Editor::handleInsertMode(const KeyInput& input) {
    if (input.key == Key::ESCAPE) {
        setMode(NORMAL);
    } else if (input.key == Key::NORMAL) {
        auto buffer = getCurrentBuffer();
        if (buffer) {
            // Insert character at cursor position
            BufferUtils::insertCharAtPosition(*buffer, cursor_.getRow(), cursor_.getCol(), input.character);
            cursor_.moveRight(1);
        }
    } else if (input.key == Key::ENTER) {
        auto buffer = getCurrentBuffer();
        if (buffer) {
            // Handle line break
            BufferUtils::insertLineBreak(*buffer, cursor_.getRow(), cursor_.getCol());
            cursor_.setCol(0);
            cursor_.moveDown(1);
        }
    } else if (input.key == Key::BACKSPACE) {
        auto buffer = getCurrentBuffer();
        if (buffer) {
            // Handle backspace
            if (cursor_.getCol() > 0) {
                BufferUtils::deleteCharAtPosition(*buffer, cursor_.getRow(), cursor_.getCol() - 1);
                cursor_.moveLeft(1);
            } else if (cursor_.getRow() > 0) {
                // Join with previous line
                int prevLineLength = BufferUtils::getLineLength(*buffer, cursor_.getRow() - 1);
                BufferUtils::joinLines(*buffer, cursor_.getRow() - 1);
                cursor_.setRow(cursor_.getRow() - 1);
                cursor_.setCol(prevLineLength);
            }
        }
    }
}

void Editor::handleVisualMode(const KeyInput& input) {
    if (input.key == Key::ESCAPE) {
        setMode(NORMAL);
        clearSelection();
    } else {
        // Visual mode movements and operations
    }
}

void Editor::handleCommandMode(const KeyInput& input) {
    if (input.key == Key::ESCAPE) {
        setMode(NORMAL);
        clearCommandBuffer();
    } else if (input.key == Key::ENTER) {
        executeCommand(state_.commandBuffer);
        clearCommandBuffer();
        setMode(NORMAL);
    } else if (input.key == Key::BACKSPACE) {
        backspaceCommandBuffer();
    } else if (input.key == Key::NORMAL) {
        appendToCommandBuffer(input.character);
    }
}

void Editor::executeCommand(const std::string& command) {
    if (commandProcessor_) {
        commandProcessor_->executeCommand(command);
    } else if (command == "q" || command == "quit") {
        state_.quit = true;
    } else if (command == "w" || command == "write") {
        if (saveFile()) {
            state_.statusMessage = "File saved";
        } else {
            state_.statusMessage = "Save failed";
        }
    } else if (command.substr(0, 2) == "w ") {
        std::string filename = command.substr(2);
        if (saveFileAs(filename)) {
            state_.statusMessage = "File saved as " + filename;
        } else {
            state_.statusMessage = "Save failed";
        }
    } else if (command == "wq") {
        if (saveFile()) {
            state_.quit = true;
        } else {
            state_.statusMessage = "Save failed, not quitting";
        }
    } else if (command.substr(0, 1) == "e" && command.length() > 2) {
        std::string filename = command.substr(2);
        if (openFile(filename)) {
            state_.statusMessage = "Opened " + filename;
        } else {
            state_.statusMessage = "Could not open " + filename;
        }
    } else {
        state_.statusMessage = "Unknown command: " + command;
    }
}

void Editor::executeCommand() {
    executeCommand(state_.commandBuffer);
}

void Editor::clearCommandBuffer() {
    state_.commandBuffer.clear();
}

void Editor::backspaceCommandBuffer() {
    if (!state_.commandBuffer.empty()) {
        state_.commandBuffer.pop_back();
    }
}

void Editor::appendToCommandBuffer(char c) {
    state_.commandBuffer += c;
}

void Editor::clearSelection() {
    // Visual mode selection clearing implementation
}

void Editor::moveToNextWord() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        int newCol = BufferUtils::findNextWordStart(*buffer, cursor_.getRow(), cursor_.getCol());
        cursor_.setCol(newCol);
    }
}

void Editor::moveToPrevWord() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        int newCol = BufferUtils::findPrevWordStart(*buffer, cursor_.getRow(), cursor_.getCol());
        cursor_.setCol(newCol);
    }
}

void Editor::moveToWordEnd() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        int newCol = BufferUtils::findWordEnd(*buffer, cursor_.getRow(), cursor_.getCol());
        cursor_.setCol(newCol);
    }
}

void Editor::insertLineBelow() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        // Get current line index
        int row = cursor_.getRow();
        
        // Insert an empty line after the current line
        auto& lines = const_cast<std::vector<std::string>&>(buffer->getLines());
        if (row >= 0 && row < static_cast<int>(lines.size())) {
            lines.insert(lines.begin() + row + 1, "");
            buffer->setModified(true);
            
            // Move cursor to the beginning of the new line
            cursor_.setRow(row + 1);
            cursor_.setCol(0);
        }
    }
}

void Editor::insertLineAbove() {
    auto buffer = getCurrentBuffer();
    if (buffer) {
        // Get current line index
        int row = cursor_.getRow();
        
        // Insert an empty line before the current line
        auto& lines = const_cast<std::vector<std::string>&>(buffer->getLines());
        if (row >= 0 && row < static_cast<int>(lines.size())) {
            lines.insert(lines.begin() + row, "");
            buffer->setModified(true);
            
            // Keep the cursor at the same row (which is now the new empty line)
            cursor_.setCol(0);
        }
    }
}

void Editor::setMode(Mode newMode) {
    state_.mode = newMode;
}

std::string Editor::getModeString(Mode mode) const {
    // Use if-else instead of switch for enum class
    // Now that Mode is a plain enum, switch is fine, but if-else also works
    if (mode == NORMAL) return "NORMAL";
    else if (mode == INSERT) return "INSERT";
    else if (mode == VISUAL) return "VISUAL";
    else if (mode == VISUAL_LINE) return "VISUAL LINE";
    else if (mode == COMMAND) return "COMMAND";
    else return "UNKNOWN";
}

void Editor::updateStatusLine() {
    auto buffer = getCurrentBuffer();
    if (!buffer) return;
    
    std::stringstream ss;
    ss << "[" << getModeString(state_.mode) << "] ";
    
    if (!buffer->getFilePath().empty()) {
        ss << buffer->getFilePath();
    } else {
        ss << "[No Name]";
    }
    
    if (buffer->isModified()) {
        ss << " [+]";
    }
    
    ss << " - Line " << (cursor_.getRow() + 1) << "/" << buffer->getLines().size()
       << " Col " << (cursor_.getCol() + 1);
    
    if (!state_.statusMessage.empty()) {
        ss << " | " << state_.statusMessage;
    }
    
    state_.statusMessage = ss.str();
}

void Editor::setupNormalModeBindings() {
    // Now implemented in HotkeyManager class
}

void Editor::setupInsertModeBindings() {
    // Now implemented in HotkeyManager class
}

void Editor::setupVisualModeBindings() {
    // Now implemented in HotkeyManager class
}

void Editor::setupCommandModeBindings() {
    // Now implemented in HotkeyManager class
}

void Editor::handleMovement(const KeyInput& input) {
    // This is a placeholder; actual implementation is in HotkeyManager
}

void Editor::handleOperation(const KeyInput& input) {
    // This is a placeholder; actual implementation is in HotkeyManager
}

void Editor::cleanup() {
    // Save any unsaved buffers that need to be saved
    auto buffer = getCurrentBuffer();
    if (buffer && buffer->isModified()) {
        // Optionally show a warning or automatically save
        state_.statusMessage = "Warning: Unsaved changes in current buffer";
    }
    
    // Clear any selection
    clearSelection();
    
    // Reset state
    state_.mode = NORMAL;
    state_.commandBuffer.clear();
    state_.statusMessage.clear();
}

} // namespace cvim
