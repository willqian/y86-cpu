#ifndef op_h
#define op_h

typedef struct {
    unsigned char code;
    const char *s;
} Op;

static Op ops[] = {
    {0x00, "halt"},
    
    {0x10, "nop"},
    
    {0x20, "rrmovl"},
    {0x21, "cmovle"},
    {0x22, "cmovl"},
    {0x23, "cmove"},
    {0x24, "cmovne"},
    {0x25, "cmovge"},
    {0x26, "cmovg"},
    
    {0x30, "irmovl"},
    
    {0x40, "rmmovl"},
    
    {0x50, "mrmovl"},
    
    {0x60, "addl"},
    {0x61, "subl"},
    {0x62, "andl"},
    {0x63, "xorl"},
    
    {0x70, "jmp"},
    {0x71, "jle"},
    {0x72, "jl"},
    {0x73, "je"},
    {0x74, "jne"},
    {0x75, "jge"},
    {0x76, "jg"},

    {0x80, "call"},

    {0x90, "ret"},

    {0xA0, "pushl"},

    {0xB0, "popl"},
};

#define OP_SIZE sizeof(ops)/sizeof(Op)

#endif /* op_h */
