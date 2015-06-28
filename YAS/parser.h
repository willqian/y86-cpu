#ifndef parser_h
#define parser_h

#include "token.h"

typedef struct {
    int _lineNumber;
    int _locate;
    Token label;
    Token directive;
    Token op;
    Token param[3];
} LineInfo;

typedef struct {
    LineInfo _info[10000];
} LineInfoArray;

class Parser {

private:
    Lexer *_lexer;
    Token _currentToken;
    Token _bakToken;
    int _lineNumber;
    int _locateBase;
    int _currentLocateOffset;
    LineInfoArray _infoArray;

    Token nextToken();
    int parseLine();
    int directiveHandler(LineInfo *lInfo);
    int opHandler(LineInfo *lInfo);
public:
    Parser(const char *source);
    ~Parser();
    
    LineInfoArray *parse();
};

#endif /* parser_h */
