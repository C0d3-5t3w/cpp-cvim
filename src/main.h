#ifndef CVIM_MAIN_H
#define CVIM_MAIN_H

#include <string>
#include <vector>
#include "modules/editor.h"
#include "modules/terminal.h"
#include "config/config.h"

namespace cvim {

class CVim {
public:
    CVim(int argc, char** argv);
    ~CVim();

    int run();

private:
    bool initialize();
    void processArguments(int argc, char** argv);
    void openInitialFiles(const std::vector<std::string>& filePaths);
    void showHelp();
    void showVersion();
    void shutdown();

    Terminal terminal_;
    Editor editor_;
    Config config_;
    bool running_ = false;
};

} // namespace cvim

#endif // CVIM_MAIN_H
