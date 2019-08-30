#ifndef _FUNC_PARAM_H_
#define _FUNC_PARAM_H_

#include <util/vector.h>

typedef enum {
    enforced_param, object_param, regular_param,
} func_param_type_t;

struct func_param {
    func_param_type_t         type;
    char                    * id;
    int                       symbol;
    struct vector_state     * ids;
    struct int_vector_state * symbols;
    struct vector_state     * enforced_type;
};

#endif
