#include "util.h"

#include <stdio.h>
#include <stdlib.h>

int sc_max(int a, int b) { return a > b ? a : b; }

int sc_pow(int n, int p) {
  if (p <= 0) {
    return 1;
  }

  return n * sc_pow(n, p - 1);
}

long unsigned int sc_strlen(const char *str) {
  long unsigned int len = 0;
  for (; *str != '\0'; str++, len++)
    ;
  return len;
}

void sc_strcpy(const char *original, char *copy) {
  for (; *original != '\0'; original++, copy++) {
    *copy = *original;
  }

  *copy = '\0';
}

bool sc_strcmp(const char *l, const char *r) {
  for (; *l != '\0' && *r != '\0'; l++, r++) {
    if (*l != *r) {
      return false;
    }
  }

  // After \0
  if (*l != *r) {
    return false;
  }

  return true;
}

char *sc_alloc_strcpy(const char *original) {
  char *copy = malloc(sizeof(char) * sc_strlen(original) + 1);
  // If you are trying to be thread-safe, this can get fucked up really quickly
  // in this EXACT place
  sc_strcpy(original, copy);
  return copy;
}

void sc_fill_seq_with_bytes(void *ptr, char byte, int size) {
  char *byte_ptr = (char *)ptr;

  for (; byte_ptr - (char *)ptr < size; byte_ptr++) {
    *byte_ptr = byte;
  }
}
