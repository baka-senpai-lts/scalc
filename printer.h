#ifndef PRINTER_H_
#define PRINTER_H_

#include "node.h"

void sc_print_void_typed(void *p, sc_NodeType type);
void sc_print_operation(sc_Operation op);
void sc_print_node(sc_Node *);

#endif // PRINTER_H_
