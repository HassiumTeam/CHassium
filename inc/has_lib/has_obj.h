#ifndef _HAS_OBJ_H_
#define _HAS_OBJ_H_

#include <util/dict.h>
#include <util/vector.h>

struct has_obj {
    struct dict_state * attribs;
    struct has_obj * type;

    void * state;
};

struct has_obj * has_obj_init (void * state);
void             has_obj_free (struct has_obj * obj);

struct has_obj * has_obj_get_attrib (struct has_obj * obj, char * name);
void             has_obj_set_attrib (struct has_obj * obj, char * name, struct has_obj * val);

#endif
