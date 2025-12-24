#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "result.h"

typedef struct {
  void *next; // Trust me, I must be sc_Context *
  sc_Result value;
  char *key;
} sc_Context;

// I will allocate new result, don't copy it again
sc_Result sc_context_get(sc_Context *, const char *);

// Pass me pointer to where you want pointer to your struct, I will
// create it on demand
void sc_context_set(sc_Context **, const char *, sc_Result);

void sc_context_free(sc_Context *);

#endif // CONTEXT_H_
