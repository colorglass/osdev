<<<<<<< HEAD
static inline void *memcpy(void *dest, const void *src, int n)
=======
extern inline void memcpy(void *dest, const void *src, int n)
>>>>>>> f8b6ea7c90fefdbddc97b958f986161cae5dd0b6
{
    asm volatile("cld\n\t"
        "rep\n\t"
        "movsb" ::"c"(n),
        "S"(src), "D"(dest));
}

<<<<<<< HEAD
static inline void *memset(void *s, char c, int count)
=======
extern inline void memset(void *s, char c, int count)
>>>>>>> f8b6ea7c90fefdbddc97b958f986161cae5dd0b6
{
    asm volatile("cld\n\t"
        "rep\n\t"
        "stosb" ::"a"(c),
        "D"(s), "c"(count));
<<<<<<< HEAD
    return s;
}

void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(const char s[]);
=======
}
>>>>>>> f8b6ea7c90fefdbddc97b958f986161cae5dd0b6
