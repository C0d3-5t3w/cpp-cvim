#ifndef CVIM_BUFFER_UTILS_H
#define CVIM_BUFFER_UTILS_H

#include "editor.h"

namespace cvim {

// Extend the Buffer class with additional methods
class BufferUtils {
public:
    static void insertCharAtPosition(Buffer& buffer, int row, int col, char c);
    static void deleteCharAtPosition(Buffer& buffer, int row, int col);
    static void insertLineBreak(Buffer& buffer, int row, int col);
    static void joinLines(Buffer& buffer, int line);
    static int getLineLength(const Buffer& buffer, int line);
    static bool isWordChar(char c);
    static int findNextWordStart(const Buffer& buffer, int row, int col);
    static int findPrevWordStart(const Buffer& buffer, int row, int col);
    static int findWordEnd(const Buffer& buffer, int row, int col);
};

} // namespace cvim

#endif // CVIM_BUFFER_UTILS_H
