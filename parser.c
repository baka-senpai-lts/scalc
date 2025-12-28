#include "parser.h"

#include <assert.h>
#include <stdlib.h>

#include "node.h"
#include "util.h"

char *sc_parse_literal(const char *str, long unsigned int *inc) {
  // Copy until '\0' or ' '
  str++; // skip first

  const char *end = str;
  long unsigned int len = 0;

  for (; *end != '(' && *end != ')' && *end != '\0' && *end != '\n' &&
         *end != ' ';
       end++, len++)
    ;

  char *result = malloc(len + 1);
  char *ptr = result;

  for (; str != end; ptr++, str++) {
    *ptr = *str;
  }

  result[len] = '\0';
  *inc = len + 1;

  return result;
}

char *sc_parse_var(const char *str, long unsigned int *inc) {
  // Copy until '\0' or ' '

  const char *end = str;
  long unsigned int len = 0;

  for (; *end != '(' && *end != ')' && *end != '\0' && *end != '\n' &&
         *end != ' ';
       end++, len++)
    ;

  char *result = malloc(len + 1);
  char *ptr = result;

  for (; str != end; ptr++, str++) {
    *ptr = *str;
  }

  result[len] = '\0';
  *inc = len;

  return result;
}

int sc_parse_int(const char *str, long unsigned int *inc) {
  int result = 0;
  int sign = 1;
  long unsigned int len = 0;

  if (*str == '-') {
    sign = -1;
    str++;
    len++;
  }

  for (; *str >= '0' && *str <= '9'; str++, len++) {
    result = result * 10 + (*str - '0');
  }

  if (inc) {
    *inc = len;
  }

  return sign * result;
}

float sc_parse_float(const char *str, long unsigned int *inc) {
  float result = 0;
  float fraction = 0;
  float factor = 1;
  float sign = 1;
  long unsigned int len = 0;

  if (*str == '-') {
    sign = -1;
    str++;
    len++;
  }

  for (; *str >= '0' && *str <= '9'; str++, len++) {
    result = result * 10 + (*str - '0');
  }

  if (*str != '.' && *str != ',') {
    goto skip_f;
  }

  str++;
  len++;

  for (; *str >= '0' && *str <= '9'; str++, len++) {
    fraction = fraction * 10 + (*str - '0');
    factor *= 10;
  }
  fraction /= factor;

skip_f:
  if (inc) {
    *inc = len;
  }

  return sign * (result + fraction);
}

sc_TokenType sc_parse_token_type(const char *str) {
  // Trust me, it WORKS perfectly

  if (*str == '+' ||
      (*str == '-' && !(*(str + 1) >= '0' && *(str + 1) <= '9')) ||
      *str == '*' || *str == '/' || *str == '\\' || *str == '~' ||
      (*str == '=' && (*(str + 1) == '>')) || *str == '.') {
    return TOK_OPERATION;
  }

  if (*str == '\'') {
    return TOK_LITERAL;
  }

  if (*str >= 'A' && *str <= 'z') {
    return TOK_VAR;
  }

  if (!((*str >= '0' && *str <= '9') || (*str == '.' || *str == ',') || (*str == '-'))) {
    return TOK_VAR;
  }

  if (*str == '-') {
    str++;
  }

  for (; *str >= '0' && *str <= '9'; str++)
    ;

  if (*str == ',' || *str == '.') {
    return TOK_FLOAT;
  }

  return TOK_INT;
}

sc_Operation sc_parse_operation(const char *str, long unsigned int *inc) {
  // Just first character is fine for now
  if (inc != NULL) {
    *inc = 1;
  }

  switch (*str) {
  case '+':
    return OP_PLUS;
  case '-':
    return OP_MINUS;
  case '*':
    return OP_MULTIPLICATION;
  case '/':
    return OP_DIVISION;
  case '\\':
    return OP_LAMBDA;
  case '~':
    if (*(str+1) == '>') {
      if (inc != NULL) {
        *inc = 2;
      }

      return OP_SET_LAZY;
    }
    return OP_APPLY;
  case '.':
    return OP_APPLY_LAZY;
  case '=':
    if (*(str+1) == '>') {
      if (inc != NULL) {
        *inc = 2;
      }

      return OP_SET_EAGER;
    }
    return OP_NONE;
  }
  /* assert(0 && "Incorrect character passed to sc_parse_operation"); */
  return OP_NONE;
}

static sc_Node *sc_append_operator_to_tree(sc_Node **root, sc_Operation op) {
  if ((*root)->op == OP_NONE) {
    (*root)->op = op;
  } else {
    sc_Node *new_node;

    // I guess we support everything now?
    switch (op) {
    case OP_APPLY:
    case OP_APPLY_LAZY:
    case OP_PLUS:
    case OP_MINUS:
      if (!(*root)->r) {
        break;
      }
      new_node = malloc(sizeof(sc_Node));

    append:
      new_node->l = (*root);
      new_node->l_type = NODE_NODE;
      new_node->r = NULL;
      new_node->r_type = NODE_NONE;
      new_node->op = op;

      (*root) = new_node;
      break;
    case OP_MULTIPLICATION:
    case OP_DIVISION:
      if (!(*root)->r) {
        break;
      }
      new_node = malloc(sizeof(sc_Node));

      if ((*root)->op == OP_DIVISION || (*root)->op == OP_MULTIPLICATION) {
        goto append;
      }

    on_right:
      // We have to rotate the tree a little for this shit
      new_node->l = (*root)->r;
      new_node->l_type = (*root)->r_type;
      new_node->r = NULL;
      new_node->r_type = NODE_NONE;
      new_node->op = op;

      (*root)->r = new_node;
      (*root)->r_type = NODE_NODE;

      return new_node;
      break;
    case OP_SET_EAGER:
    case OP_SET_LAZY:
    case OP_LAMBDA:
      if (!(*root)->r) {
        break;
      }

      new_node = malloc(sizeof(sc_Node));

      goto on_right; // Yes I know
    default:
      break;
    }
  }

  return NULL;
}

sc_Node *sc_str_to_node(const char *str, long unsigned int *inc) {
  // Look man, I know this looks scary, but it somehow works incredibly well
  // because it doesn't give a fuck about anything

  long unsigned int len_max = sc_strlen(str);

  long unsigned int len = 0;
  long unsigned int inner_inc = 0;
  sc_Node *root = malloc(sizeof(sc_Node));

  sc_Node *appending = NULL;

  root->op = OP_NONE;
  root->l = NULL;
  root->l_type = NODE_NONE;
  root->r = NULL;
  root->r_type = NODE_NONE;

  for (; len_max >= len && *str != '\0'; str++, len++) {
    if (*str == ' ' || *str == '\t' || *str == '\n') {
      continue;
    }

    if (*str == '(') {
      if (!root->l) {
        root->l = sc_str_to_node(str + 1, &inner_inc);
        root->l_type = NODE_NODE;
      } else if (!root->r) {
        root->r = sc_str_to_node(str + 1, &inner_inc);
        root->r_type = NODE_NODE;
      } else if (appending && !appending->r) {
        appending->r = sc_str_to_node(str + 1, &inner_inc);
        appending->r_type = NODE_NODE;
      } else if (!appending) {
        appending = sc_append_operator_to_tree(&root, root->op);
        if (appending) {
          appending->r = sc_str_to_node(str + 1, &inner_inc);
          appending->r_type = NODE_NODE;
        } else if (!root->r) {
          root->r = sc_str_to_node(str + 1, &inner_inc);
          root->r_type = NODE_NODE;
        }
      } else if (appending && appending->r &&
                 (appending->op == OP_LAMBDA || appending->op == OP_SET_EAGER ||
                  appending->op == OP_SET_LAZY)) {
        // Insert into right on appending, steal right and put to left,
        // enables implicit currying like (\ 'a 'b 'c (+ a b c))
        appending = sc_append_operator_to_tree(&appending, appending->op);
        // If it's not guaranteed to be free, fuck me
        appending->r = sc_str_to_node(str + 1, &inner_inc);
        appending->r_type = NODE_NODE;
      }

      len += inner_inc;
      str += inner_inc;
      continue;
    }

    if (*str == ')') {
      break;
    }

    sc_TokenType token = sc_parse_token_type(str);

    switch (token) {
    case TOK_VAR: {
      char *n = sc_parse_var(str, &inner_inc);

      if (!root->l && root->l_type == NODE_NONE) {
        root->l = n;
        root->l_type = NODE_VAR;
      } else if (!root->r && root->r_type == NODE_NONE) {
        root->r = n;
        root->r_type = NODE_VAR;
      } else if (appending && !appending->r && appending->r_type == NODE_NONE) {
        appending->r = n;
        appending->r_type = NODE_VAR;
      } else if (!appending) {
        appending = sc_append_operator_to_tree(&root, root->op);

        if (appending) {
          if (appending->r) {
            if (appending->r_type == NODE_NODE) {
              sc_free_node_tree_children(appending->r);
            }
            free(appending->r);
          }

          appending->r = n;
          appending->r_type = NODE_VAR;
        } else {
          if (root->r) {
            if (root->r_type == NODE_NODE) {
              sc_free_node_tree_children(root->r);
            }
            free(root->r);
          }

          root->r = n;
          root->r_type = NODE_VAR;
        }
      } else if (appending && appending->r &&
                 (appending->op == OP_LAMBDA || appending->op == OP_SET_EAGER ||
                  appending->op == OP_SET_LAZY)) {
        // Insert into right on appending, steal right and put to left,
        // enables implicit currying like (\ 'a 'b 'c (+ a b c))
        appending = sc_append_operator_to_tree(&appending, appending->op);
        // If it's not guaranteed to be free, fuck me
        appending->r = n;
        appending->r_type = NODE_VAR;
      } else {
        free(n);
      }
      break;
    }

    case TOK_LITERAL: {
      char *n = sc_parse_literal(str, &inner_inc);

      if (!root->l && root->l_type == NODE_NONE) {
        root->l = n;
        root->l_type = NODE_LITERAL;
      } else if (!root->r && root->r_type == NODE_NONE) {
        root->r = n;
        root->r_type = NODE_LITERAL;
      } else if (appending && !appending->r && appending->r_type == NODE_NONE) {
        appending->r = n;
        appending->r_type = NODE_LITERAL;
      } else if (!appending) {
        appending = sc_append_operator_to_tree(&root, root->op);

        if (appending) {
          if (appending->r) {
            if (appending->r_type == NODE_NODE) {
              sc_free_node_tree_children(appending->r);
            }
            free(appending->r);
          }

          /* puts("Appending right"); */
          appending->r = n;
          appending->r_type = NODE_LITERAL;
        } else {
          if (root->r) {
            if (root->r_type == NODE_NODE) {
              sc_free_node_tree_children(root->r);
            }
            free(root->r);
          }

          root->r = n;
          root->r_type = NODE_LITERAL;
        }
      } else if (appending && appending->r &&
                 (appending->op == OP_LAMBDA || appending->op == OP_SET_EAGER ||
                  appending->op == OP_SET_LAZY)) {
        // Insert into right on appending, steal right and put to left,
        // enables implicit currying like (\ 'a 'b 'c (+ a b c))
        appending = sc_append_operator_to_tree(&appending, appending->op);
        // If it's not guaranteed to be free, fuck me
        appending->r = n;
        appending->r_type = NODE_LITERAL;
      } else {
        free(n);
      }
      break;
    }

    case TOK_INT: {
      int n = sc_parse_int(str, &inner_inc);

      if (!root->l && root->l_type == NODE_NONE) {
        root->l = malloc(sizeof(int));
        *(int *)root->l = n;
        root->l_type = NODE_INT;
      } else if (!root->r && root->r_type == NODE_NONE) {
        root->r = malloc(sizeof(int));
        *(int *)root->r = n;
        root->r_type = NODE_INT;
      } else if (appending && !appending->r && appending->r_type == NODE_NONE) {
        appending->r = malloc(sizeof(int));
        *(int *)appending->r = n;
        appending->r_type = NODE_INT;
      } else if (appending && appending->r &&
                 (appending->op == OP_LAMBDA || appending->op == OP_SET_EAGER ||
                  appending->op == OP_SET_LAZY)) {
        appending = sc_append_operator_to_tree(&appending, appending->op);
        appending->r = malloc(sizeof(int));
        *(int *)appending->r = n;
        appending->r_type = NODE_INT;
      } else if (!appending) {
        appending = sc_append_operator_to_tree(&root, root->op);

        if (appending) {
          if (appending->r) {
            if (appending->r_type == NODE_NODE) {
              sc_free_node_tree_children(appending->r);
            }
            free(appending->r);
          }

          appending->r = malloc(sizeof(int));
          *(int *)appending->r = n;
          appending->r_type = NODE_INT;
        } else {
          if (root->r) {
            if (root->r_type == NODE_NODE) {
              sc_free_node_tree_children(root->r);
            }
            free(root->r);
          }

          root->r = malloc(sizeof(int));
          *(int *)root->r = n;
          root->r_type = NODE_INT;
        }
      }
      break;
    }

    case TOK_FLOAT: {
      float n = sc_parse_float(str, &inner_inc);
      if (!root->l && root->l_type == NODE_NONE) {
        root->l = malloc(sizeof(float));
        *(float *)root->l = n;
        root->l_type = NODE_FLOAT;
      } else if (!root->r && root->r_type == NODE_NONE) {
        root->r = malloc(sizeof(float));
        *(float *)root->r = n;
        root->r_type = NODE_FLOAT;
      } else if (appending && !appending->r && appending->r_type == NODE_NONE) {
        appending->r = malloc(sizeof(float));
        *(float *)appending->r = n;
        appending->r_type = NODE_FLOAT;
      } else if (appending && appending->r &&
                 (appending->op == OP_LAMBDA || appending->op == OP_SET_EAGER ||
                  appending->op == OP_SET_LAZY)) {
        appending = sc_append_operator_to_tree(&appending, appending->op);
        appending->r = malloc(sizeof(float));
        *(float *)appending->r = n;
        appending->r_type = NODE_FLOAT;
      } else if (!appending) {
        appending = sc_append_operator_to_tree(&root, root->op);

        if (appending) {
          if (appending->r) {
            if (appending->r_type == NODE_NODE) {
              sc_free_node_tree_children(appending->r);
            }
            free(appending->r);
          }

          appending->r = malloc(sizeof(float));
          *(float *)appending->r = n;
          appending->r_type = NODE_FLOAT;
        } else {
          if (root->r) {
            if (root->r_type == NODE_NODE) {
              sc_free_node_tree_children(root->r);
            }
            free(root->r);
          }

          root->r = malloc(sizeof(float));
          *(float *)root->r = n;
          root->r_type = NODE_FLOAT;
        }
      }
      break;
    }

    case TOK_OPERATION: {
      appending = sc_append_operator_to_tree(
          &root, sc_parse_operation(str, &inner_inc));
      break;
    }
    }

    len += inner_inc - 1;
    str += inner_inc - 1;
  }

  if (inc != NULL) {
    *inc = len + 1;
  }

  return root;
}
