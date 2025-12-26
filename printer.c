#include "printer.h"

#include <stdio.h>

#include "node.h"

void sc_print_void_typed(void *p, sc_NodeType type) {
  switch (type) {
  case NODE_NODE:
    sc_print_node(p);
    break;
  case NODE_INT:
    printf("%d", *(int *)p);
    break;
  case NODE_FLOAT:
    printf("%f", (double)*(float *)p);
    break;
  case NODE_LITERAL:
    printf("'%s", (char *)p);
    break;
  case NODE_VAR:
    printf("v/%s", (char *)p);
    break;
  case NODE_NONE:
    fputs("None", stdout);
    break;
  default:
    // Probably nothing important anyway
    break;
  }
}

void sc_print_operation(sc_Operation op) {
  switch (op) {
  case OP_PLUS:
    fputs("+", stdout);
    break;
  case OP_MINUS:
    fputs("-", stdout);
    break;
  case OP_MULTIPLICATION:
    fputs("*", stdout);
    break;
  case OP_DIVISION:
    fputs("/", stdout);
    break;
  case OP_LAMBDA:
    fputs("\\", stdout);
    break;
  case OP_APPLY:
    fputs("~", stdout);
    break;
  case OP_APPLY_LAZY:
    fputs(".", stdout);
    break;
  case OP_NONE:
    fputs("@", stdout);
    break;
  case OP_SET_EAGER:
    fputs("=>", stdout);
    break;
  case OP_SET_LAZY:
    fputs("~>", stdout);
    break;
  default:
    break;
  }
}

void sc_print_node(sc_Node *node) {
  fputs("(", stdout);

  // We are printing this shit dynamically LET'S FUCKING GO
  // (I was too lazy to allocate a string properly for this entire thing, so
  // yea...)
  sc_print_operation(node->op);
  fputs(" ", stdout);
  sc_print_void_typed(node->l, node->l_type);
  fputs(" ", stdout);
  sc_print_void_typed(node->r, node->r_type);

  fputs(")", stdout);
}
