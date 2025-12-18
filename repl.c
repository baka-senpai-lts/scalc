#include "repl.h"

#include <stdio.h>

#include "util.h"

int sc_getline(const char *prompt, char *buffer, int size) {
  if (prompt != NULL) {
	fputs(prompt, stdout);
  }

  if (fgets(buffer, size, stdin) == NULL) {
	return REPL_EMPTY;
  }

  if(buffer[0] == '\0') {
	return REPL_EMPTY;
  }

  if(buffer[sc_strlen(buffer)-1] != '\n') {
	return REPL_TOO_LONG;
  }

  return REPL_OK;
}
