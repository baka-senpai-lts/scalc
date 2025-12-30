#include "util.h"

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

char *sc_alloc_strconcat(const char *str1, const char *str2) {
  unsigned long int len1 = sc_strlen(str1);
  unsigned long int len2 = sc_strlen(str2);

  char *new = malloc(sizeof(char) * (len1 + len2) + 1);

  // Seems reasonable
  sc_strcpy(str1, new);
  sc_strcpy(str2, new+len1);

  return new;
}

char *sc_alloc_strmult(const char *str, unsigned int amount) {
  unsigned long int len = sc_strlen(str);

  char *new = malloc(sizeof(char) * len * amount + 1);

  for (unsigned int i = 0; i < amount; i++) {
    sc_strcpy(str, new + len * i);
  }

  return new;
}


char *sc_alloc_strreverse(const char *str) {
  unsigned int len = (unsigned int)sc_strlen(str);

  char *new = malloc(sizeof(char) * len + 1);

  for (unsigned int i = 0; i < len; i++) {
    new[i] = str[len-i-1];
  }

  new[len] = '\0';

  return new;
}

char *sc_alloc_strslice(const char *str, unsigned int start, unsigned int end) {
  unsigned long int len = sc_strlen(str);

  if (start > len) {
    start = (unsigned int)len;
  }

  if (end > len) {
    end = (unsigned int)len;
  }

  if (end < start) {
    unsigned int swap = end;
    end = start;
    start = swap;
  }

  unsigned long int new_len = end - start;

  char *new = malloc(sizeof(char) * new_len + 1);

  for(unsigned long int i = 0; i < new_len; i++) {
    new[i] = str[start + i];
  }

  new[new_len] = '\0';

  return new;
}

void sc_memmove_n(void *original, void *new, long unsigned int size) {
  // First copy to secondary buffer, then copy from first to second
  char *original_ch = (char *)original;
  char *new_ch = (char *)new;

  char *tmp = malloc(size);

  for(long unsigned int i = 0; i < size; i++, tmp++, original_ch++) {
    *tmp = *original_ch;
    *original_ch = 0;
  }

  tmp -= size; // Going back to original position

  for(long unsigned int i = 0; i < size; i++, tmp++, new_ch++) {
      *new_ch = *tmp;
  }

  free(tmp - size);
}

void sc_fill_seq_with_bytes(void *ptr, char byte, long unsigned int size) {
  char *byte_ptr = (char *)ptr;

  for (; (long unsigned int)(byte_ptr - (char *)ptr) < size; byte_ptr++) {
    *byte_ptr = byte;
  }
}
