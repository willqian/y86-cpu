#ifndef directive_h
#define directive_h

typedef struct {
    const char *s;
} Directive;

static Directive directives[] = {
    {".align"},
    {".long"},
    {".pos"}
};

#define DIRECTIVE_SIZE sizeof(directives)/sizeof(Directive)

#endif /* directive_h */
