#ifndef token_h
#define token_h

#include "op.h"
#include "reg.h"
#include "directive.h"

#define OP_FLAG     0x100
#define REG_FLAG    0x1000
#define EOS         0x10000
#define COMMENT     0x10001
#define NAME        0x10002
#define NUMBER      0x10003
#define DIRECTIVE   0X10004
#define LABEL       0x10005
#define INUMBER     0x10006
#define MNUMBER     0x10007

typedef struct {
    int r;
    char *s;
} TokenInfo;

typedef struct {
    /* token type */
    int token;
    /* token info */
    TokenInfo info;
} Token;

#endif /* token_h */
