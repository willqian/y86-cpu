#ifndef lexer_h
#define lexer_h

#include "token.h"

#define MAX_LINE 10000

class Lexer {

private:
    int _current;
    int _lineNumber;
    int _lastLine;
    Token _currentToken;
    Token _lookAheadToken;

    char *_buf;
    int _bufSize;
    int _bufIndex;

    char *_sourceBuf;
    int _sourceBufSize;
    int _sourceIndex;

    void next();
    int currIsNewLine();
    void save(int c);
    void saveAndNext();
    void readNumber(TokenInfo *info);
    int lex(TokenInfo *info);
    void incLineNumber();
    void resetBuf();
    int isOp(char *s);
    int isReg(char *s);
    int isDirective(char *s);

public:
    Lexer(const char *filename);
    ~Lexer();

    Token getCurrentToken();
    int getCurrentLineNumber();

    void nextToken();
    int lookAhead();
};

#endif /* lexer_h */
