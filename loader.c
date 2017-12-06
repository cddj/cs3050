/*! \file loader.c
 *  \brief Loads a world from a file
 */

#include <errno.h>
#include "loader.h"
#include "world.h"

struct world* load_world(FILE* file) {
    char c;
    int lineCount = 0;
    int maxCharCount = 0;
    int charCount = 0;
    int x = 0;
    int y = 0;
    for (c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {
           lineCount++;
           if(charCount > maxCharCount)
           {
               maxCharCount = charCount;
           }
           charCount = 0;
        }
        else {
            charCount++;
        }
    }
    char rawChars[maxCharCount][lineCount];
    rewind(file);
    for (c = getc(file); c != EOF; c = getc(file)) {
        if (c == '\n') {
           y++;
           x = 0;
        }
        else {
            rawChars[x][y] = c;
            x++;
        }
    }
    struct world* w = world_let_there_be_light(lineCount * maxCharCount);
    // Load everything into w here.
    return w;
}
