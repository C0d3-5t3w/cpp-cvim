#ifndef CVIM_UTILS_H
#define CVIM_UTILS_H

#include <string>
#include <vector>
#include <functional>

namespace cvim {

struct Size {
    int height;
    int width;
};

struct Position {
    int row;
    int col;
};

struct Range {
    Position start;
    Position end;
    
    bool contains(const Position& pos) const;
};

// String utilities
std::vector<std::string> split(const std::string& str, char delimiter);
std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
std::string trim(const std::string& str);

// File utilities
bool fileExists(const std::string& path);
std::string getFileExtension(const std::string& path);
std::string getFileName(const std::string& path);
std::string getDirectoryPath(const std::string& path);

// Path utilities
std::string normalizePath(const std::string& path);
std::string resolveRelativePath(const std::string& basePath, const std::string& relativePath);
std::string getHomeDirectory();

// Error handling
class Result {
public:
    Result() : success_(true) {}
    Result(const std::string& error) : success_(false), error_(error) {}
    
    bool isSuccess() const { return success_; }
    bool isError() const { return !success_; }
    const std::string& getError() const { return error_; }
    
    static Result success() { return Result(); }
    static Result error(const std::string& message) { return Result(message); }

private:
    bool success_;
    std::string error_;
};

} // namespace cvim

#endif // CVIM_UTILS_H
