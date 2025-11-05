/* simple reference implementations */

void *memset(void *dst, int ch, unsigned long sz)
{
    char *d = (char *)dst;
    while(sz--)
        *d++ = ch;
    return dst;
}

void *memcpy(void *dst, const void *src, unsigned long sz)
{
    char *d = (char *)dst;
    const char *s = (const char *)src;

    while(sz--)
        *d++ = *s++;

    return dst;
}

void *memmove(void *dst, const void *src, unsigned long sz)
{
    char *d = (char *)dst;
    const char* s = (const char*)src;

    if (d > s) {
        d += sz;
        s += sz;
        while (sz--) {
            *--d = *--s;
        }
    } else {
        while (sz--) {
            *d++ = *s++;
        }
    }
    return dst;
}
