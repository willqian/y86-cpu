#include "op.h"
#include "reg.h"
#include "directive.h"
#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 *  token:
 *  NUMBER      1, -1, 0xd
 *  STRING      
 *  OP          halt
 *  REG         %eax
 *  INUMBER     $4
 *  MNUMBER     8(%ebp)
 *  DIRECTIVE   .align
 *  LABEL       Main:
 *  NAME        Main
 *  COMMENT     # this is comment
 */

#if 1
#define LEX_DEBUG(fmt, args...) \
    do { \
        printf("[%s %d]\t" fmt, __func__, __LINE__, ##args); \
    } while(0)
#else
#define LEX_DEBUG(fmt, args...) \
    do {} while(0)
#endif

#if 1
#define LEX_ASSERT(fmt, args...) \
    do { \
        printf("ASSERT [%s %d]\t" fmt, __func__, __LINE__, ##args); \
        exit(1); \
    } while(0)
#else
#define LEX_ASSERT(fmt, args...) \
    do {} while(0)
#endif 
    
int Lexer::currIsNewLine()
{
    return _current == '\n' || _current == '\r';
}

void Lexer::incLineNumber()
{
    int old = _current;
    if (!this->currIsNewLine()) {
        LEX_ASSERT("currIsNewLine error\n");
    }
    this->next();
    if (this->currIsNewLine() && _current != old) {
        this->next(); /* skip \r\n or \n\r */
    }
    if (++_lineNumber >= MAX_LINE) {
        LEX_ASSERT("too large line number\n");
    }
}

void Lexer::next()
{
    _current = _sourceBuf[_sourceIndex++]; 
}

void Lexer::save(int c)
{
    if (_bufIndex >= _bufSize) {
        _bufSize = _bufSize * 2;
        _buf = (char *)realloc((void *)_buf, _bufSize);
    }
     _buf[_bufIndex++] = (char)c;
}

void Lexer::saveAndNext()
{
    this->save(_current);
    this->next();
}

/* handle negative and hex number */
void Lexer::readNumber(TokenInfo *info)
{
    int hexFlag = 0;
    this->saveAndNext();
    if ('x' == _current || 'X' == _current) {
        if (_buf[0] != '0') {
            LEX_ASSERT("[lex] invalid hex number\n");
        }
        /* hex number */
        hexFlag = 1;
        this->saveAndNext();
    }
    for (;;) {
        if (isdigit(_current)) {
            this->saveAndNext();
        } else if (1 == hexFlag) {
            if ('a' == _current || 'A' == _current
                || 'b' == _current || 'B' == _current
                || 'c' == _current || 'C' == _current
                || 'd' == _current || 'D' == _current
                || 'e' == _current || 'E' == _current
                || 'f' == _current || 'F' == _current) {
                this->saveAndNext();
            } else {
                break;
            }
        } else {
            break;
        }
    }
    this->save('\0');
    LEX_DEBUG("[lex] read number [%s]\n", _buf);
    info->r = atoi(_buf);
}

void Lexer::resetBuf()
{
    _bufIndex = 0;
    memset(_buf, 0, _bufSize);
}

int Lexer::isOp(char *s)
{
    for (int i = 0; i < (int)OP_SIZE; i ++) {
        if (0 == strcmp(ops[i].s, s)) {
            return ops[i].code | OP_FLAG;
        }
    }
    return -1;
}

int Lexer::isReg(char *s)
{
    for (int i = 0; i < (int)REG_SIZE; i ++) {
        if (0 == strcmp(regs[i].s, s)) {
            return regs[i].code | REG_FLAG;
        }
    }
    return -1;
}

int Lexer::isDirective(char *s)
{
    for (int i = 0; i < (int)DIRECTIVE_SIZE; i ++) {
        if (0 == strcmp(directives[i].s, s)) {
            return DIRECTIVE;
        }
    }
    return -1;
}

int Lexer::lex(TokenInfo *info)
{
    this->resetBuf();
    for (;;) {
        switch (_current) {
            case '\n': case '\r':
            {
                //LEX_DEBUG("[lex] [line-%d] new line\n", _lineNumber);
                this->incLineNumber();
                break;
            }
            case ' ': case '\f': case '\t': case '\v':
            {
                this->next();
                break;
            }
            case '#': /* comment */
            {
                do {
                    this->saveAndNext();
                } while ('\n' != _current && '\r' != _current);
                LEX_DEBUG("[lex] [line-%d] comment to line end [%s]\n", _lineNumber, _buf);
                return COMMENT;
            }
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '-':
            {
                this->readNumber(info);
                //LEX_DEBUG("[lex] [line-%d] number[%d]\n", _lineNumber, info->r);
                return NUMBER;
            }
            case '$':
            {
                this->next();
                this->readNumber(info);
                return INUMBER;
            }
            case '%':
            {
                int token = -1;
                do {
                    this->saveAndNext();
                } while (isalnum(_current));
                this->save('\0');
                info->s = (char *)malloc(strlen(_buf) + 1);
                strncpy(info->s, _buf, strlen(_buf));
                token = this->isReg(info->s);
                if (token > 0) {
                    LEX_DEBUG("[lex] [line-%d] regsiter [%s - %08x]\n", _lineNumber, info->s, token);
                    return token;
                } else {
                    LEX_ASSERT("[lex] invalid register\n");
                }
            }
            case '.':
            {
                int token = -1;
                do {
                    this->saveAndNext();
                } while (isalnum(_current));
                this->save('\0');
                info->s = (char *)malloc(strlen(_buf) + 1);
                strncpy(info->s, _buf, strlen(_buf));
                token = this->isDirective(info->s);
                if (token > 0) {
                    LEX_DEBUG("[lex] [line-%d] directive [%s - %08x]\n", _lineNumber, info->s, token);
                    return token;
                } else {
                    LEX_ASSERT("[lex] invalid directive [%s]\n", info->s);
                }
            }
            case -1:
            {
                LEX_DEBUG("[lex] EOS\n");
                return EOS;
            }
            default:
            {
                if (isalpha(_current)) {
                    int token = -1;
                    do {
                        this->saveAndNext();
                    } while (isalnum(_current));
                    if (':' == _current) {
                        this->saveAndNext();
                        this->save('\0');
                        info->s = (char *)malloc(strlen(_buf) + 1);
                        strncpy(info->s, _buf, strlen(_buf));
                        LEX_DEBUG("[lex] [line-%d] label [%s]\n", _lineNumber, info->s);
                        return LABEL;
                    } else {
                        this->save('\0');
                        info->s = (char *)malloc(strlen(_buf) + 1);
                        strncpy(info->s, _buf, strlen(_buf));
                        token = this->isOp(info->s);
                        if (token > 0){
                            LEX_DEBUG("[lex] [line-%d] op [%s - %08x]\n", _lineNumber, info->s, token);
                            return token;
                        } else {
                            LEX_DEBUG("[lex] [line-%d] name [%s]\n", _lineNumber, info->s);
                            return NAME;
                        }
                    }
                } else {
                    int c = _current;
                    this->next();
                    LEX_DEBUG("[lex] [line-%d] normal char[%c]\n", _lineNumber, c);
                    return c;
                }
            }
        }
    }
}

Lexer::Lexer(const char *filename)
{
    int baseSize = 1024;
    int fd = -1;
    int readBytes = -1;
    int totalBytes = 0;

    _current = -1;
    _lineNumber = 1;
    _lastLine = 1;
    _currentToken = {EOS, {-1, NULL}};
    _lookAheadToken = {EOS, {-1, NULL}};

    _buf = (char *)malloc(baseSize);
    memset(_buf, 0, baseSize);
    _bufSize = baseSize;
    _bufIndex = 0;

    _sourceBuf = (char *)malloc(baseSize);
    memset(_sourceBuf, 0, baseSize);
    _sourceBufSize = baseSize;
    _sourceIndex = 0;

    fd = open(filename, O_RDONLY);
    while (1) {
        readBytes = read(fd, _sourceBuf + totalBytes, baseSize);
        if (-1 == readBytes) {
            printf("read error\n");
            exit(1);
        } else {
            totalBytes += readBytes;
            baseSize = baseSize * 2;
            _sourceBufSize = baseSize;
            _sourceBuf = (char *)realloc((void *)_sourceBuf, baseSize);
            _sourceBuf[totalBytes] = -1;
            if (0 == readBytes) {
                break;
            }
        }
    }
    _current = _sourceBuf[0];
    _sourceIndex = 1;
    close(fd);
    for (int i = 0; i < _sourceBufSize; i ++) {
        if (-1 == _sourceBuf[i]) {
            break;
        }
        printf("%c",_sourceBuf[i]);
    }
}

Lexer::~Lexer()
{
    free(_buf);
    free(_sourceBuf);
    _buf = NULL;
    _sourceBuf = NULL;

    if (NULL != _currentToken.info.s) {
        free(_currentToken.info.s);
    }
    if (NULL != _lookAheadToken.info.s) {
        free(_lookAheadToken.info.s);
    }
}

Token Lexer::getCurrentToken()
{
    return _currentToken;
}

int Lexer::getCurrentLineNumber()
{
    return _lineNumber;
}

void Lexer::nextToken()
{
    _lastLine = _lineNumber;
    if (_lookAheadToken.token != EOS) {
        _currentToken = _lookAheadToken;
        _lookAheadToken.token = EOS;
    } else {
        _currentToken.token = this->lex(&_currentToken.info);
    }
}

int Lexer::lookAhead()
{
    if (EOS == _lookAheadToken.token) {
        LEX_ASSERT("lookAheadToken error\n");
    }
    _lookAheadToken.token = this->lex(&_lookAheadToken.info);
    return _lookAheadToken.token;
}

//#define TEST_LEX
#ifdef TEST_LEX
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: ./lexer filename\n");
        return -1;
    }
    Lexer *lexer = new Lexer(argv[1]);
    Token token;
    while (1) {
        lexer->nextToken();
        token = lexer->getCurrentToken();
        if (EOS == token.token) {
            printf("file end\n");
            break;
        }
    }
    return 0;
}
#endif
