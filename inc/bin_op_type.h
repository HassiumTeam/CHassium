#ifndef _BIN_OP_TYPE_H_
#define _BIN_OP_TYPE_H_

typedef enum {
    add_bin_op, bitwise_and_bin_op, bitwise_or_bin_op, div_bin_op,
    equal_bin_op, greater_bin_op, greater_or_equal_bin_op, instanceof_bin_op,
    lesser_bin_op, lesser_or_equal_bin_op, logical_and_bin_op, logical_or_bin_op,
    mod_bin_op, mul_bin_op, sub_bin_op, xor_bin_op,
} bin_op_type_t;

#endif
