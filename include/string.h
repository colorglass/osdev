extern inline void memcpy(void *dest, const void *src, int n)
{
    asm volatile("cld\n\t"
        "rep\n\t"
        "movsb" ::"c"(n),
        "S"(src), "D"(dest));
}

extern inline void memset(void *s, char c, int count)
{
    asm volatile("cld\n\t"
        "rep\n\t"
        "stosb" ::"a"(c),
        "D"(s), "c"(count));
}