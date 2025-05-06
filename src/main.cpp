#include "main.h"
#include "modules/terminal.h"
#include "modules/editor.h"
#include "modules/commands.h"
#include "config/config.h"
#include <iostream>
#include <stdexcept>
#include <string>

namespace cvim {

// Version information
const char* VERSION = "0.1.0";

CVim::CVim(int argc, char** argv) {
    try {
        if (!initialize()) {
            std::cerr << "Failed to initialize application" << std::endl;
            exit(1);
        }
        processArguments(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << "Runtime error during initialization: " << e.what() << std::endl;
        shutdown();
        exit(1);
    } catch (const std::logic_error& e) {
        std::cerr << "Logic error during initialization: " << e.what() << std::endl;
        shutdown();
        exit(1);
    } catch (const std::exception& e) {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
        shutdown();
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error during initialization" << std::endl;
        shutdown();
        exit(1);
    }
}

CVim::~CVim() {
    shutdown();
}

bool CVim::initialize() {
    // Load configuration first
    if (!config_.load()) {
        std::cerr << "Warning: Failed to load configuration, using defaults" << std::endl;
        // Continue with defaults
    }
    
    // Initialize terminal
    if (!terminal_.initialize()) {
        return false;
    }

    // Initialize editor with terminal and config references
    editor_.initialize(&terminal_, &config_);
    
    // Set application as running
    running_ = true;
    return true;
}

void CVim::processArguments(int argc, char** argv) {
    std::vector<std::string> filePaths;
    
    // Process arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle options/flags
        if (arg == "--help" || arg == "-h") {
            showHelp();
            exit(0);
        }
        else if (arg == "--version" || arg == "-v") {
            showVersion();
            exit(0);
        }
        else if (arg == "--config" || arg == "-c") {
            // Load specific config file
            if (i + 1 < argc) {
                config_.load(argv[++i]);
            } else {
                std::cerr << "Error: --config option requires a file path" << std::endl;
                exit(1);
            }
        }
        else {
            // Treat as file path
            filePaths.push_back(arg);
        }
    }
    
    if (!filePaths.empty()) {
        openInitialFiles(filePaths);
    }
}

void CVim::openInitialFiles(const std::vector<std::string>& filePaths) {
    for (int i = 0; i < static_cast<int>(filePaths.size()); ++i) {
        if (!editor_.openFile(filePaths[i])) {
            std::cerr << "Warning: Failed to open file: " << filePaths[i] << std::endl;
        }
    }
}

void CVim::showHelp() {
    std::cout << "Usage: cvim [options] [file ...]\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help          Show this help message and exit\n"
              << "  -v, --version       Show version information and exit\n"
              << "  -c, --config FILE   Use specified config file\n"
              << std::endl;
}

void CVim::showVersion() {
    std::cout << "cvim version " << VERSION << std::endl;
    std::cout << "Copyright (c) 2025-2030 C0D3-5T3W" << std::endl;
    std::cout << "Licensed under the MIT License" << std::endl;
}

int CVim::run() {
    while (running_) {
        // Render current editor state
        terminal_.render(editor_.getViewData());
        
        // Get user input
        KeyInput input = terminal_.getInput();
        
        // Process the input
        editor_.handleInput(input);
        
        // Check if we should exit
        if (editor_.shouldQuit()) {
            running_ = false;
        }
    }
    return 0;
}

void CVim::shutdown() {
    // Save any configuration changes
    config_.save();
    
    // Clean up editor resources first
    editor_.cleanup();
    
    // Shut down terminal
    terminal_.shutdown();
    
    // Additional cleanup if needed
    running_ = false;
}

} // namespace cvim

int main(int argc, char** argv) {
    cvim::CVim app(argc, argv);
    return app.run();
}
