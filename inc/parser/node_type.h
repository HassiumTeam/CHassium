#ifndef _NODE_TYPE_H_
#define _NODE_TYPE_H_

typedef enum {
    assign_node, attrib_access_node, bin_op_node, block_node, break_node,
    class_node, closure_node, continue_node, expr_stmt_node, for_node,
    foreach_node, func_call_node, func_decl_node, id_node, if_node,
    import_node, list_decl_node, number_node, obj_decl_node, raise_node,
    return_node, string_node, subscript_node, try_catch_node, typeof_node,
    unary_op_node, use_node, while_node,
} node_type_t;

#endif
