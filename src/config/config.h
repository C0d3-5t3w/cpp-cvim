#ifndef CVIM_CONFIG_H
#define CVIM_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "../utils/utils.h"

namespace cvim {

enum class ColorTheme {
    DEFAULT,
    LIGHT,
    DARK,
    SOLARIZED_LIGHT,
    SOLARIZED_DARK,
    MONOKAI,
    CUSTOM
};

struct ColorScheme {
    int foreground;
    int background;
    int selection;
    int lineNumber;
    int statusLine;
    int commandLine;
    int errorMessage;
    int keyword;
    int string;
    int comment;
    int number;
};

struct KeyBinding {
    std::string mode;
    std::string key;
    std::string action;
};

class Config {
public:
    Config();
    ~Config();
    
    bool load(const std::string& configPath = "");
    bool save(const std::string& configPath = "");
    
    // General settings
    bool getBoolean(const std::string& key, bool defaultValue = false) const;
    int getInteger(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    void setBoolean(const std::string& key, bool value);
    void setInteger(const std::string& key, int value);
    void setString(const std::string& key, const std::string& value);
    
    // Color scheme
    ColorScheme getColorScheme() const;
    void setColorScheme(const ColorScheme& scheme);
    void setColorTheme(ColorTheme theme);
    
    // Key bindings
    std::vector<KeyBinding> getKeyBindings() const;
    void addKeyBinding(const KeyBinding& binding);
    void removeKeyBinding(const std::string& mode, const std::string& key);
    
    // Paths
    std::string getConfigPath() const;
    std::string getUserConfigPath() const;
    
private:
    void loadDefaults();
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);
    
    std::map<std::string, std::string> settings_;
    std::vector<KeyBinding> keyBindings_;
    ColorScheme colorScheme_;
    std::string configPath_;
};

} // namespace cvim

#endif // CVIM_CONFIG_H
