static inline void *memcpy(void *dest, const void *src, int n)
{
    asm("cld\n\t"
        "rep\n\t"
        "movsb" ::"c"(n),
        "S"(src), "D"(dest));
    return dest;
}

static inline void *memset(void *s, char c, int count)
{
    asm("cld\n\t"
        "rep\n\t"
        "stosb" ::"a"(c),
        "D"(s), "c"(count));
    return s;
}

void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
