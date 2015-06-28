#include "op.h"
#include "reg.h"
#include "directive.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser::Parser(const char *source)
{
    _lexer = new Lexer(source);
    _currentToken = {EOS, {-1, NULL}};
    _bakToken = {EOS, {-1, NULL}};
    _lineNumber = -1;
    _locateBase = -1;
    _currentLocateOffset = -1;
    memset(&_infoArray, 0, sizeof(_infoArray));
}

Parser::~Parser()
{
    delete _lexer;
}

Token Parser::nextToken()
{
    _lexer->nextToken();
    _currentToken = _lexer->getCurrentToken();
    _lineNumber = _lexer->getCurrentLineNumber();
    return _currentToken;
}

int Parser::directiveHandler(LineInfo *lInfo)
{
    char *str = lInfo->directive.info.s;

    if (NULL == str) {
        return -1;
    }
    if (0 == strcmp(".align", str)) {
    } else if (0 == strcmp(".long", str)) {
    } else if (0 == strcmp(".pos", str)) {
    }
}

int Parser::opHandler(LineInfo *lInfo)
{
}

int Parser::parseLine()
{
    Token token;
    int parseLineNumber = -1;
    int paramIndex = 0;
    LineInfo *lInfo = NULL;
    if (EOS == _bakToken.token) {
        token = this->nextToken();
    } else {
        token = _bakToken;
    }

    parseLineNumber = _lineNumber;
    lInfo = &(_infoArray._info[parseLineNumber]);
    lInfo->_lineNumber = _lineNumber;
    lInfo->_locate = _currentLocateOffset;

    while (parseLineNumber == _lineNumber) {
        printf("[PARSER] line number [%d]\n", _lineNumber);
        switch(token.token) {
        case LABEL:
            lInfo->label = token;
            printf("[PARSER] LABEL token [%08x] info [%s]\n", 
                    lInfo->label.token, lInfo->label.info.s);
            break;
        case DIRECTIVE:
            lInfo->directive = token;
            printf("[PARSER] DIRECTIVE token [%08x] info [%s]\n", 
                    lInfo->directive.token, lInfo->directive.info.s);
            break;
        case EOS:
            printf("file end\n");
            return -1;
        default:
            printf("unknown token\n");
            break;
        }
        token = this->nextToken();
    }
    _bakToken = token;
    this->directiveHandler(lInfo);
    this->opHandler(lInfo);

    if (EOS == _bakToken.token) {
        printf("file end\n");
        return -1;
    }
    return 0;
}

LineInfoArray *Parser::parse()
{
    int ret;
    while (1) {
        ret = this->parseLine();
        if (-1 == ret) {
            return &_infoArray;
        }
    }
}

#define TEST_PARSER
#ifdef TEST_PARSER
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: ./parser filename\n");
        return -1;
    }
    Parser *parser = new Parser(argv[1]);
    parser->parse();
    return 0;
}
#endif
