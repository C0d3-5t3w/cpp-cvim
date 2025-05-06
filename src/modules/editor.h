#ifndef CVIM_EDITOR_H
#define CVIM_EDITOR_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "terminal.h"
#include "cursor.h"

namespace cvim {

// Forward declarations
class Config;
class CommandProcessor;
class FileTree;
class TabManager;
class HotkeyManager;

enum class Mode {
    NORMAL,
    INSERT,
    VISUAL,
    VISUAL_LINE,
    COMMAND
};

class Buffer {
public:
    Buffer(const std::string& filePath = "");
    ~Buffer();

    bool load();
    bool save();
    bool saveAs(const std::string& filePath);
    
    void insertChar(char c);
    void insertLine(const std::string& line);
    void deleteLine(int line);
    void deleteChar(int pos, int line);
    
    const std::vector<std::string>& getLines() const;
    const std::string& getFilePath() const;
    bool isModified() const;
    void setModified(bool modified);
    
private:
    std::string filePath_;
    std::vector<std::string> lines_;
    bool modified_;
};

struct EditorState {
    Mode mode;
    std::string commandBuffer;
    std::string statusMessage;
    bool quit;
    
    EditorState() : mode(Mode::NORMAL), quit(false) {}
};

class Editor {
public:
    Editor();
    ~Editor();
    
    void initialize(Terminal* terminal, Config* config);
    void handleInput(const KeyInput& input);
    
    bool openFile(const std::string& filePath);
    bool saveFile();
    bool saveFileAs(const std::string& filePath);
    
    ViewData getViewData() const;
    bool shouldQuit() const;
    
    // Added accessor methods
    Cursor& getCursor() { return cursor_; }
    const Cursor& getCursor() const { return cursor_; }
    std::shared_ptr<Buffer> getCurrentBuffer();
    
    // Mode handling
    void setMode(Mode newMode);
    std::string getModeString(Mode mode) const;
    
    // Command handling
    void executeCommand();
    void clearCommandBuffer();
    void backspaceCommandBuffer();
    void appendToCommandBuffer(char c);
    
    // Selection
    void clearSelection();
    
    // Word movement
    void moveToNextWord();
    void moveToPrevWord();
    void moveToWordEnd();
    
    // Line operations
    void insertLineBelow();
    void insertLineAbove();
    
    // Resource cleanup
    void cleanup();
    
private:
    void handleNormalMode(const KeyInput& input);
    void handleInsertMode(const KeyInput& input);
    void handleVisualMode(const KeyInput& input);
    void handleCommandMode(const KeyInput& input);
    
    void executeCommand(const std::string& command);
    void updateStatusLine();
    
    void setupNormalModeBindings();
    void setupInsertModeBindings();
    void setupVisualModeBindings();
    void setupCommandModeBindings();
    
    void handleMovement(const KeyInput& input);
    void handleOperation(const KeyInput& input);
    
    Terminal* terminal_;
    Config* config_;
    TabManager* tabManager_;
    Cursor cursor_;
    CommandProcessor* commandProcessor_;
    FileTree* fileTree_;
    EditorState state_;
    HotkeyManager* hotkeyManager_;
};

} // namespace cvim

#endif // CVIM_EDITOR_H
