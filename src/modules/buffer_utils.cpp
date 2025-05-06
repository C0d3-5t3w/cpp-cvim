#include "buffer_utils.h"

namespace cvim {

void BufferUtils::insertCharAtPosition(Buffer& buffer, int row, int col, char c) {
    auto& lines = const_cast<std::vector<std::string>&>(buffer.getLines());
    
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return;
    }
    
    std::string& line = lines[row];
    if (col < 0 || col > static_cast<int>(line.size())) {
        return;
    }
    
    line.insert(col, 1, c);
    buffer.setModified(true);
}

void BufferUtils::deleteCharAtPosition(Buffer& buffer, int row, int col) {
    auto& lines = const_cast<std::vector<std::string>&>(buffer.getLines());
    
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return;
    }
    
    std::string& line = lines[row];
    if (col < 0 || col >= static_cast<int>(line.size())) {
        return;
    }
    
    line.erase(col, 1);
    buffer.setModified(true);
}

void BufferUtils::insertLineBreak(Buffer& buffer, int row, int col) {
    auto& lines = const_cast<std::vector<std::string>&>(buffer.getLines());
    
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return;
    }
    
    std::string& line = lines[row];
    if (col < 0 || col > static_cast<int>(line.size())) {
        return;
    }
    
    // Extract the part of the line after the cursor
    std::string newLine = line.substr(col);
    // Truncate the current line at the cursor position
    line.resize(col);
    
    // Insert the new line after the current one
    lines.insert(lines.begin() + row + 1, newLine);
    buffer.setModified(true);
}

void BufferUtils::joinLines(Buffer& buffer, int line) {
    auto& lines = const_cast<std::vector<std::string>&>(buffer.getLines());
    
    if (line < 0 || line >= static_cast<int>(lines.size()) - 1) {
        return;
    }
    
    // Append the next line to the current line
    lines[line] += lines[line + 1];
    
    // Remove the next line
    lines.erase(lines.begin() + line + 1);
    buffer.setModified(true);
}

int BufferUtils::getLineLength(const Buffer& buffer, int line) {
    const auto& lines = buffer.getLines();
    if (line < 0 || line >= static_cast<int>(lines.size())) {
        return 0;
    }
    
    return lines[line].length();
}

bool BufferUtils::isWordChar(char c) {
    // Word characters are alphanumeric or underscore
    return isalnum(c) || c == '_';
}

int BufferUtils::findNextWordStart(const Buffer& buffer, int row, int col) {
    const auto& lines = buffer.getLines();
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return col;
    }
    
    const std::string& line = lines[row];
    int length = line.length();
    
    if (col >= length) {
        return col;
    }
    
    // Current state (in a word or not)
    bool inWord = isWordChar(line[col]);
    
    // Move forward until we find a word boundary
    int pos = col;
    while (pos < length) {
        if (inWord && !isWordChar(line[pos])) {
            // We were in a word, and now we're not
            inWord = false;
        } else if (!inWord && isWordChar(line[pos])) {
            // We were not in a word, and now we are - this is a word start
            return pos;
        }
        pos++;
    }
    
    // Didn't find a word start
    return col;
}

int BufferUtils::findPrevWordStart(const Buffer& buffer, int row, int col) {
    const auto& lines = buffer.getLines();
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return col;
    }
    
    const std::string& line = lines[row];
    
    if (col <= 0) {
        return col;
    }
    
    // Move backward until we find a word boundary
    int pos = col - 1;
    bool inWord = false;
    
    while (pos >= 0) {
        if (!inWord && isWordChar(line[pos])) {
            // We were not in a word, and now we are - check if this is the word start
            if (pos == 0 || !isWordChar(line[pos - 1])) {
                return pos;
            }
            inWord = true;
        } else if (inWord && !isWordChar(line[pos])) {
            // We were in a word, and now we're not
            inWord = false;
        }
        pos--;
    }
    
    // Didn't find a word start
    return col;
}

int BufferUtils::findWordEnd(const Buffer& buffer, int row, int col) {
    const auto& lines = buffer.getLines();
    if (row < 0 || row >= static_cast<int>(lines.size())) {
        return col;
    }
    
    const std::string& line = lines[row];
    int length = line.length();
    
    if (col >= length) {
        return col;
    }
    
    // Current state (in a word or not)
    bool inWord = isWordChar(line[col]);
    
    // Move forward until we find a word boundary
    int pos = col;
    while (pos < length) {
        if (inWord && !isWordChar(line[pos])) {
            // We were in a word, and now we're not - previous position was the word end
            return pos - 1;
        } else if (!inWord && isWordChar(line[pos])) {
            // We were not in a word, and now we are
            inWord = true;
        }
        pos++;
    }
    
    // If we reached the end of the line and were in a word, pos-1 is the word end
    if (inWord && pos > col) {
        return pos - 1;
    }
    
    // Didn't find a word end
    return col;
}

} // namespace cvim
