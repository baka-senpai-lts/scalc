#ifndef NODE_H_
#define NODE_H_

typedef enum {
  OP_NONE,
  OP_PLUS,
  OP_MINUS,
  OP_MULTIPLICATION,
  OP_DIVISION,

  // We are here for your convenience
  OP_SET_EAGER,
  OP_SET_LAZY,

  // We two make this shit Turing-complete
  OP_LAMBDA,
  OP_APPLY,
  OP_APPLY_LAZY,

  // Here go comparisons
  OP_EQUALS,
  OP_GREATER,
  OP_LESSER,
  OP_GREATER_EQUALS,
  OP_LESSER_EQUALS,
  OP_NOT_EQUALS
} sc_Operation;

typedef enum {
  NODE_NODE = 255,
  NODE_NONE = 0,
  NODE_INT = 10,
  NODE_FLOAT = 12,
  NODE_LITERAL = 13,
  NODE_VAR = 15,
} sc_NodeType;

typedef struct {
  void *l;
  sc_NodeType l_type;

  void *r;
  sc_NodeType r_type;

  sc_Operation op;
} sc_Node;

sc_Node *sc_construct_node(void *, sc_NodeType, sc_Operation, void *,
                           sc_NodeType);

// Free sc_Node instance too after using me
sc_Node *sc_copy_node_tree(const sc_Node *);

void sc_free_node_tree_children(const sc_Node *);

#endif // NODE_H_
