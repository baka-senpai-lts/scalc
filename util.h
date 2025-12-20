#ifndef UTIL_H_
#define UTIL_H_

// Fuck yea, we have our own bool now
typedef int bool;

#define true 1
#define false 0

// And our own NULL now too
#ifndef NULL
#define NULL 0
#endif

#ifndef BUFSIZ
#define BUFSIZ 8192
#endif

int sc_max(int, int);

// Only supports positive powers
int sc_pow(int, int);

long unsigned int sc_strlen(const char *);

// Yes, we are unsafe
// Yes, I don't care
bool sc_strcmp(const char *, const char *);

// Come on, do an unsafe copy, I dare you
void sc_strcpy(const char *, char *);

// Might've had fun instead
char *sc_alloc_strcpy(const char *);

void sc_memmove_n(void *, void *, long unsigned int);

void sc_fill_seq_with_bytes(void *, char, long unsigned int);

#endif // UTIL_H_
