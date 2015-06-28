#ifndef reg_h
#define reg_h

typedef struct {
    unsigned char code;
    const char *s;
} Reg;

#define NON_REG 0x0f

static Reg regs[] = {
    {0, "\%eax"},
    {1, "\%ecx"},
    {2, "\%edx"},
    {3, "\%ebx"},
    {4, "\%esp"},
    {5, "\%ebp"},
    {6, "\%esi"},
    {7, "\%edi"}
};

#define REG_SIZE sizeof(regs)/sizeof(Reg)

#endif /* reg_h */
