#ifndef CVIM_CURSOR_H
#define CVIM_CURSOR_H

#include "../utils/utils.h"
#include <memory>

namespace cvim {

class Buffer;

class Cursor {
public:
    Cursor();
    ~Cursor();
    
    void setPosition(int row, int col);
    void setRow(int row);
    void setCol(int col);
    
    int getRow() const;
    int getCol() const;
    Position getPosition() const;
    
    void moveUp(int count = 1);
    void moveDown(int count = 1);
    void moveLeft(int count = 1);
    void moveRight(int count = 1);
    
    void moveToLineStart();
    void moveToLineEnd();
    void moveToFileStart();
    void moveToFileEnd();
    
    void moveToWordStart();
    void moveToWordEnd();
    void moveToNextWord();
    void moveToPrevWord();
    
    void limitToValidPosition(const std::shared_ptr<Buffer>& buffer);
    
private:
    Position position_;
    Position savedPosition_; // For operations like 'j' and 'k' that remember the column
};

} // namespace cvim

#endif // CVIM_CURSOR_H
