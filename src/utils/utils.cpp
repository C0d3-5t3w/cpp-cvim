#include "utils.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

namespace cvim {

bool Range::contains(const Position& pos) const {
    // Check if position is within the range
    if (start.row < end.row) {
        return (pos.row > start.row || (pos.row == start.row && pos.col >= start.col)) &&
               (pos.row < end.row || (pos.row == end.row && pos.col <= end.col));
    } else if (start.row == end.row) {
        return pos.row == start.row && pos.col >= start.col && pos.col <= end.col;
    }
    return false;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i < parts.size() - 1) {
            result += delimiter;
        }
    }
    return result;
}

std::string trim(const std::string& str) {
    // Using non-auto syntax to avoid C++11 warnings
    std::string::const_iterator start = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
    std::string::const_iterator end = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
    
    return (start < end ? std::string(start, end) : std::string());
}

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string getFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return "";
}

std::string getFileName(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}

std::string getDirectoryPath(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        return path.substr(0, pos);
    }
    return "";
}

std::string normalizePath(const std::string& path) {
    std::vector<std::string> parts = split(path, '/');
    std::vector<std::string> normalized;
    
    // Use traditional for loop instead of range-based
    for (size_t i = 0; i < parts.size(); ++i) {
        const std::string& part = parts[i];
        if (part.empty() || part == ".") {
            // Skip empty parts and current directory
            continue;
        } else if (part == "..") {
            // Go up one directory
            if (!normalized.empty() && normalized.back() != "..") {
                normalized.pop_back();
            } else {
                normalized.push_back("..");
            }
        } else {
            normalized.push_back(part);
        }
    }
    
    std::string result = join(normalized, "/");
    if (path[0] == '/') {
        result = "/" + result;
    }
    
    return result;
}

std::string resolveRelativePath(const std::string& basePath, const std::string& relativePath) {
    if (relativePath.empty()) return basePath;
    if (relativePath[0] == '/') return relativePath; // Absolute path
    
    std::string baseDir = getDirectoryPath(basePath);
    if (baseDir.empty()) baseDir = ".";
    
    std::string combinedPath = baseDir + "/" + relativePath;
    return normalizePath(combinedPath);
}

std::string getHomeDirectory() {
    const char* homedir = getenv("HOME");
    if (homedir == nullptr) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return homedir ? std::string(homedir) : "";
}

} // namespace cvim
