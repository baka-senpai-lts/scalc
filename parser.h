#ifndef PARSER_H_
#define PARSER_H_

#include "node.h"

typedef enum {
  TOK_OPERATION,
  TOK_INT,
  TOK_FLOAT,
  TOK_LITERAL,
  TOK_VAR,
  TOK_STRING
} sc_TokenType;

// All these parse first symbol in string
char *sc_parse_literal(const char *, long unsigned int *);
char *sc_parse_var(const char *, long unsigned int *);
char *sc_parse_string(const char *, long unsigned int *);

int sc_parse_int(const char *, long unsigned int *);
float sc_parse_float(const char *, long unsigned int *);
sc_Operation sc_parse_operation(const char *, long unsigned int *);
sc_TokenType sc_parse_token_type(const char *);

sc_Node *sc_str_to_node(const char *, long unsigned int *);

#endif // PARSER_H_
