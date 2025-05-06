#include "cursor.h"
#include "editor.h"
#include <climits>
#include <algorithm>

namespace cvim {

Cursor::Cursor() {
    position_.row = 0;
    position_.col = 0;
    savedPosition_.row = 0;
    savedPosition_.col = 0;
}

Cursor::~Cursor() {}

void Cursor::setPosition(int row, int col) {
    position_.row = row;
    position_.col = col;
    savedPosition_ = position_;
}

void Cursor::setRow(int row) {
    position_.row = row;
}

void Cursor::setCol(int col) {
    position_.col = col;
    savedPosition_.col = col;
}

int Cursor::getRow() const {
    return position_.row;
}

int Cursor::getCol() const {
    return position_.col;
}

Position Cursor::getPosition() const {
    return position_;
}

void Cursor::moveUp(int count) {
    position_.row = std::max(0, position_.row - count);
    // Preserve the remembered column position
    position_.col = savedPosition_.col;
}

void Cursor::moveDown(int count) {
    position_.row += count;
    // Preserve the remembered column position
    position_.col = savedPosition_.col;
}

void Cursor::moveLeft(int count) {
    position_.col = std::max(0, position_.col - count);
    savedPosition_ = position_;
}

void Cursor::moveRight(int count) {
    position_.col += count;
    savedPosition_ = position_;
}

void Cursor::moveToLineStart() {
    position_.col = 0;
    savedPosition_ = position_;
}

void Cursor::moveToLineEnd() {
    // The actual end position depends on the buffer content
    // This will be adjusted by the limitToValidPosition function
    position_.col = INT_MAX;
    savedPosition_ = position_;
}

void Cursor::moveToFileStart() {
    position_.row = 0;
    position_.col = 0;
    savedPosition_ = position_;
}

void Cursor::moveToFileEnd() {
    // The actual end position depends on the buffer content
    // This will be adjusted by the limitToValidPosition function
    position_.row = INT_MAX;
    position_.col = 0;
    savedPosition_ = position_;
}

void Cursor::moveToWordStart() {
    // This requires the buffer content to find word boundaries
    // Will be implemented in the Editor class
}

void Cursor::moveToWordEnd() {
    // This requires the buffer content to find word boundaries
    // Will be implemented in the Editor class
}

void Cursor::moveToNextWord() {
    // This requires the buffer content to find word boundaries
    // Will be implemented in the Editor class
}

void Cursor::moveToPrevWord() {
    // This requires the buffer content to find word boundaries
    // Will be implemented in the Editor class
}

void Cursor::limitToValidPosition(const std::shared_ptr<Buffer>& buffer) {
    if (!buffer) return;
    
    // Limit row
    int maxRow = buffer->getLines().size() - 1;
    position_.row = std::max(0, std::min(position_.row, maxRow));
    
    // Limit column
    if (position_.row <= maxRow) {
        int maxCol = buffer->getLines()[position_.row].length();
        position_.col = std::max(0, std::min(position_.col, maxCol));
    } else {
        position_.col = 0;
    }
    
    // Update saved column only if we moved horizontally
    if (savedPosition_.col != position_.col) {
        savedPosition_.col = position_.col;
    }
}

} // namespace cvim
