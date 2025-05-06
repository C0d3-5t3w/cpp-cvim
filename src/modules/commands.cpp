#include "commands.h"
#include "editor.h"
#include "../utils/utils.h"
#include <sstream>
#include <algorithm>
#include <functional> // Add this for std::function

namespace cvim {

CommandProcessor::CommandProcessor() : editor_(nullptr) {
    registerBuiltInCommands();
}

CommandProcessor::CommandProcessor(Editor* editor) : editor_(editor) {
    registerBuiltInCommands();
}

CommandProcessor::~CommandProcessor() {}

void CommandProcessor::setEditor(Editor* editor) {
    editor_ = editor;
}

void CommandProcessor::registerBuiltInCommands() {
    // Use simpler syntax without lambdas for older compilers
    commands_["q"] = std::bind(&CommandProcessor::cmdQuit, this, std::placeholders::_1);
    commands_["quit"] = std::bind(&CommandProcessor::cmdQuit, this, std::placeholders::_1);
    
    commands_["w"] = std::bind(&CommandProcessor::cmdWrite, this, std::placeholders::_1);
    commands_["write"] = std::bind(&CommandProcessor::cmdWrite, this, std::placeholders::_1);
    
    commands_["wq"] = std::bind(&CommandProcessor::cmdWriteQuit, this, std::placeholders::_1);
    commands_["writequit"] = std::bind(&CommandProcessor::cmdWriteQuit, this, std::placeholders::_1);
    commands_["x"] = std::bind(&CommandProcessor::cmdWriteQuit, this, std::placeholders::_1);
    
    commands_["e"] = std::bind(&CommandProcessor::cmdEdit, this, std::placeholders::_1);
    commands_["edit"] = std::bind(&CommandProcessor::cmdEdit, this, std::placeholders::_1);
    
    commands_["n"] = std::bind(&CommandProcessor::cmdNext, this, std::placeholders::_1);
    commands_["next"] = std::bind(&CommandProcessor::cmdNext, this, std::placeholders::_1);
    
    commands_["N"] = std::bind(&CommandProcessor::cmdPrev, this, std::placeholders::_1);
    commands_["prev"] = std::bind(&CommandProcessor::cmdPrev, this, std::placeholders::_1);
    
    commands_["set"] = std::bind(&CommandProcessor::cmdSet, this, std::placeholders::_1);
    
    commands_["help"] = std::bind(&CommandProcessor::cmdHelp, this, std::placeholders::_1);
}

bool CommandProcessor::executeCommand(const std::string& command) {
    if (command.empty()) return false;
    
    std::vector<std::string> args = parseCommand(command);
    if (args.empty()) return false;
    
    std::string cmd = args[0];
    args.erase(args.begin());
    
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        return it->second(args);
    }
    
    // Unknown command
    return false;
}

std::vector<std::string> CommandProcessor::getCompletions(const std::string& partial) {
    std::vector<std::string> completions;
    
    for (const auto& cmd : commands_) {
        if (cmd.first.find(partial) == 0) {
            completions.push_back(cmd.first);
        }
    }
    
    return completions;
}

std::vector<std::string> CommandProcessor::parseCommand(const std::string& command) {
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;
    
    for (char c : command) {
        if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else if (c == '"') {
            inQuotes = !inQuotes;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        args.push_back(current);
    }
    
    return args;
}

bool CommandProcessor::cmdQuit(const std::vector<std::string>& args) {
    if (!editor_) return false;
    // Implementation will depend on the Editor class
    // This is a placeholder
    return true;
}

bool CommandProcessor::cmdWrite(const std::vector<std::string>& args) {
    if (!editor_) return false;
    
    if (args.empty()) {
        return editor_->saveFile();
    } else {
        return editor_->saveFileAs(args[0]);
    }
}

bool CommandProcessor::cmdWriteQuit(const std::vector<std::string>& args) {
    if (!editor_) return false;
    
    bool result = cmdWrite(args);
    if (result) {
        return cmdQuit(std::vector<std::string>());
    }
    return false;
}

bool CommandProcessor::cmdEdit(const std::vector<std::string>& args) {
    if (!editor_ || args.empty()) return false;
    
    return editor_->openFile(args[0]);
}

bool CommandProcessor::cmdNext(const std::vector<std::string>& args) {
    // Implementation will depend on the Editor class
    return true;
}

bool CommandProcessor::cmdPrev(const std::vector<std::string>& args) {
    // Implementation will depend on the Editor class
    return true;
}

bool CommandProcessor::cmdNumber(const std::vector<std::string>& args) {
    // Implementation will depend on the Editor class
    return true;
}

bool CommandProcessor::cmdSet(const std::vector<std::string>& args) {
    // Implementation will depend on the Editor class
    return true;
}

bool CommandProcessor::cmdHelp(const std::vector<std::string>& args) {
    // Implementation will depend on the Editor class
    return true;
}

} // namespace cvim
