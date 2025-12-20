#ifndef REPL_H_
#define REPL_H_

#define REPL_OK 0
#define REPL_EMPTY 1
#define REPL_TOO_LONG 2

void sc_terminal_set_raw(void);
void sc_terminal_set_cooked(void);

int sc_getline(const char *, char *, int);

#endif // REPL_H_
