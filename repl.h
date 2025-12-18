#ifndef REPL_H_
#define REPL_H_

#define REPL_OK 0
#define REPL_EMPTY 1
#define REPL_TOO_LONG 2
int sc_getline(const char *, char *, int);

// A designated function for reading string from stdin
// Uses the same return codes as `sc_getline`
int sc_readline(char *, int);

#endif // REPL_H_
