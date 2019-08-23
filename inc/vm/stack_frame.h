#ifndef _STACK_FRAME_H_
#define _STACK_FRAME_H_

#include <has_lib/has_obj.h>
#include <util/int_vector.h>
#include <util/vector.h>

struct frame {
    struct int_vector_state * ids;
    struct vector_state     * vals;
};

struct stack_frame {
    struct vector_state * frames;
    struct frame        * global_frame;
};

struct stack_frame * stack_frame_init ();
void                 stack_frame_free (struct stack_frame * state);

struct has_obj * get_global (struct stack_frame * state, int id);
struct has_obj * get_var    (struct stack_frame * state, int id);

struct frame   * peek_frame (struct stack_frame * state);
struct frame   * pop_frame  (struct stack_frame * state);
void             push_frame (struct stack_frame * state, struct frame * frame);

void             set_global (struct stack_frame * state, int id, struct has_obj * val);
void             set_var    (struct stack_frame * state, int id, struct has_obj * val);

#endif
