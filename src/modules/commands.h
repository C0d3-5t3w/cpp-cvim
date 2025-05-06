#ifndef CVIM_COMMANDS_H
#define CVIM_COMMANDS_H

#include <string>
#include <map>
#include <functional>  // Added for std::function
#include <vector>

namespace cvim {

class Editor;

class CommandProcessor {
public:
    CommandProcessor();
    CommandProcessor(Editor* editor);
    ~CommandProcessor();
    
    void setEditor(Editor* editor);
    
    bool executeCommand(const std::string& command);
    std::vector<std::string> getCompletions(const std::string& partial);
    
private:
    // Register built-in commands
    void registerBuiltInCommands();
    
    // Command handlers
    bool cmdQuit(const std::vector<std::string>& args);
    bool cmdWrite(const std::vector<std::string>& args);
    bool cmdWriteQuit(const std::vector<std::string>& args);
    bool cmdEdit(const std::vector<std::string>& args);
    bool cmdNext(const std::vector<std::string>& args);
    bool cmdPrev(const std::vector<std::string>& args);
    bool cmdNumber(const std::vector<std::string>& args);
    bool cmdSet(const std::vector<std::string>& args);
    bool cmdHelp(const std::vector<std::string>& args);
    
    // Command parsing
    std::vector<std::string> parseCommand(const std::string& command);
    
    // The editor instance
    Editor* editor_;
    
    // Map of command names to handler functions
    std::map<std::string, std::function<bool(const std::vector<std::string>&)> > commands_;
};

} // namespace cvim

#endif // CVIM_COMMANDS_H
