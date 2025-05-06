#include "config.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Check if yaml-cpp is available
#ifdef HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace cvim {

Config::Config() {
    loadDefaults();
}

Config::~Config() {}

void Config::loadDefaults() {
    // General settings
    settings_["tabSize"] = "4";
    settings_["expandTab"] = "true";
    settings_["lineNumbers"] = "true";
    settings_["relativeLine"] = "false";
    settings_["wrapText"] = "false";
    settings_["syntax"] = "true";
    settings_["autoIndent"] = "true";
    settings_["showStatusLine"] = "true";
    settings_["theme"] = "default";
    
    // Default color scheme
    colorScheme_.foreground = 7;   // White
    colorScheme_.background = 0;   // Black
    colorScheme_.selection = 4;    // Blue
    colorScheme_.lineNumber = 2;   // Green
    colorScheme_.statusLine = 3;   // Cyan
    colorScheme_.commandLine = 5;  // Magenta
    colorScheme_.errorMessage = 1; // Red
    colorScheme_.keyword = 5;      // Magenta
    colorScheme_.string = 2;       // Green
    colorScheme_.comment = 6;      // Brown
    colorScheme_.number = 3;       // Cyan
    
    // Default key bindings are managed by the Editor class
}

bool Config::load(const std::string& configPath) {
    if (!configPath.empty()) {
        configPath_ = configPath;
    } else {
        configPath_ = getUserConfigPath();
    }
    
    if (fileExists(configPath_)) {
        return loadFromFile(configPath_);
    }
    
    return false;
}

bool Config::save(const std::string& configPath) {
    std::string path = configPath.empty() ? configPath_ : configPath;
    if (path.empty()) {
        path = getUserConfigPath();
    }
    
    return saveToFile(path);
}

bool Config::loadFromFile(const std::string& path) {
#ifdef HAS_YAML_CPP
    try {
        YAML::Node config = YAML::LoadFile(path);
        
        // General settings
        if (config["settings"]) {
            for (const auto& setting : config["settings"]) {
                std::string key = setting.first.as<std::string>();
                std::string value = setting.second.as<std::string>();
                settings_[key] = value;
            }
        }
        
        // Color scheme
        if (config["colors"]) {
            YAML::Node colors = config["colors"];
            if (colors["foreground"]) colorScheme_.foreground = colors["foreground"].as<int>();
            if (colors["background"]) colorScheme_.background = colors["background"].as<int>();
            if (colors["selection"]) colorScheme_.selection = colors["selection"].as<int>();
            if (colors["lineNumber"]) colorScheme_.lineNumber = colors["lineNumber"].as<int>();
            if (colors["statusLine"]) colorScheme_.statusLine = colors["statusLine"].as<int>();
            if (colors["commandLine"]) colorScheme_.commandLine = colors["commandLine"].as<int>();
            if (colors["errorMessage"]) colorScheme_.errorMessage = colors["errorMessage"].as<int>();
            if (colors["keyword"]) colorScheme_.keyword = colors["keyword"].as<int>();
            if (colors["string"]) colorScheme_.string = colors["string"].as<int>();
            if (colors["comment"]) colorScheme_.comment = colors["comment"].as<int>();
            if (colors["number"]) colorScheme_.number = colors["number"].as<int>();
        }
        
        // Key bindings
        keyBindings_.clear();
        if (config["keybindings"]) {
            for (const auto& binding : config["keybindings"]) {
                KeyBinding kb;
                kb.mode = binding["mode"].as<std::string>();
                kb.key = binding["key"].as<std::string>();
                kb.action = binding["action"].as<std::string>();
                keyBindings_.push_back(kb);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
        return false;
    }
#else
    // Fallback simple loading when yaml-cpp is not available
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value parsing
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            settings_[key] = value;
        }
    }
    
    return true;
#endif
}

bool Config::saveToFile(const std::string& path) {
#ifdef HAS_YAML_CPP
    try {
        YAML::Node config;
        
        // General settings
        for (const auto& [key, value] : settings_) {
            config["settings"][key] = value;
        }
        
        // Color scheme
        config["colors"]["foreground"] = colorScheme_.foreground;
        config["colors"]["background"] = colorScheme_.background;
        config["colors"]["selection"] = colorScheme_.selection;
        config["colors"]["lineNumber"] = colorScheme_.lineNumber;
        config["colors"]["statusLine"] = colorScheme_.statusLine;
        config["colors"]["commandLine"] = colorScheme_.commandLine;
        config["colors"]["errorMessage"] = colorScheme_.errorMessage;
        config["colors"]["keyword"] = colorScheme_.keyword;
        config["colors"]["string"] = colorScheme_.string;
        config["colors"]["comment"] = colorScheme_.comment;
        config["colors"]["number"] = colorScheme_.number;
        
        // Key bindings
        for (size_t i = 0; i < keyBindings_.size(); ++i) {
            config["keybindings"][i]["mode"] = keyBindings_[i].mode;
            config["keybindings"][i]["key"] = keyBindings_[i].key;
            config["keybindings"][i]["action"] = keyBindings_[i].action;
        }
        
        // Write to file
        std::ofstream fout(path);
        if (!fout.is_open()) {
            return false;
        }
        
        fout << config;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file: " << e.what() << std::endl;
        return false;
    }
#else
    // Simple key=value format when YAML is not available
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    for (std::map<std::string, std::string>::const_iterator it = settings_.begin(); 
         it != settings_.end(); ++it) {
        file << it->first << "=" << it->second << std::endl;
    }
    
    return true;
#endif
}

bool Config::getBoolean(const std::string& key, bool defaultValue) const {
    auto it = settings_.find(key);
    if (it == settings_.end()) {
        return defaultValue;
    }
    
    std::string value = it->second;
    return value == "true" || value == "yes" || value == "1";
}

int Config::getInteger(const std::string& key, int defaultValue) const {
    auto it = settings_.find(key);
    if (it == settings_.end()) {
        return defaultValue;
    }
    
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = settings_.find(key);
    if (it == settings_.end()) {
        return defaultValue;
    }
    
    return it->second;
}

void Config::setBoolean(const std::string& key, bool value) {
    settings_[key] = value ? "true" : "false";
}

void Config::setInteger(const std::string& key, int value) {
    settings_[key] = std::to_string(value);
}

void Config::setString(const std::string& key, const std::string& value) {
    settings_[key] = value;
}

ColorScheme Config::getColorScheme() const {
    return colorScheme_;
}

void Config::setColorScheme(const ColorScheme& scheme) {
    colorScheme_ = scheme;
}

void Config::setColorTheme(ColorTheme theme) {
    // Cast enum class to int for switch statement
    switch (static_cast<int>(theme)) {
        case static_cast<int>(ColorTheme::DEFAULT):
            // Reset to default colors
            loadDefaults();
            break;
        case static_cast<int>(ColorTheme::LIGHT):
            colorScheme_.foreground = 0;  // Black
            colorScheme_.background = 7;  // White
            // ... and other colors for light theme
            break;
        case static_cast<int>(ColorTheme::DARK):
            colorScheme_.foreground = 7;  // White
            colorScheme_.background = 0;  // Black
            // ... and other colors for dark theme
            break;
        // ... other themes
        default:
            break;
    }
    
    settings_["theme"] = "custom";
}

std::vector<KeyBinding> Config::getKeyBindings() const {
    return keyBindings_;
}

void Config::addKeyBinding(const KeyBinding& binding) {
    // Replace existing binding if it exists
    for (auto& kb : keyBindings_) {
        if (kb.mode == binding.mode && kb.key == binding.key) {
            kb.action = binding.action;
            return;
        }
    }
    
    // Add new binding
    keyBindings_.push_back(binding);
}

void Config::removeKeyBinding(const std::string& mode, const std::string& key) {
    keyBindings_.erase(
        std::remove_if(keyBindings_.begin(), keyBindings_.end(),
            [&](const KeyBinding& kb) {
                return kb.mode == mode && kb.key == key;
            }),
        keyBindings_.end()
    );
}

std::string Config::getConfigPath() const {
    return configPath_;
}

std::string Config::getUserConfigPath() const {
    return getHomeDirectory() + "/.cvimrc";
}

} // namespace cvim
