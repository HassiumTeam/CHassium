#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <ds/hashmap.h>
#include <stdlib.h>
#include <util.h>
#include <vm.h>

struct vm
{
    struct vec *frames;
};

typedef enum
{
    OBJ_ANON,
    OBJ_CLASS,
    OBJ_FUNC,
    OBJ_NONE,
    OBJ_NUM,
    OBJ_STRING,
} obj_ctx_type_t;

struct obj
{
    long id;
    int refs;
    obj_ctx_type_t type;
    void *ctx;
    struct hashmap *attribs;
};

struct func_obj_ctx
{
    struct obj *(*func)(struct obj *, struct vm *, struct vec *);
};

struct num_obj_ctx
{
    float value;
};

struct string_obj_ctx
{
    char *value;
};

struct obj *obj_new(obj_ctx_type_t, void *);
void obj_free(struct obj *);

struct obj *obj_inc_ref(struct obj *);
struct obj *obj_dec_ref(struct obj *);
void obj_setattrib(struct obj *, char *key, struct obj *val);
struct obj *obj_invoke(struct obj *, struct vm *, struct vec *);

struct obj *func_obj_new(struct obj *(*func)(struct obj *, struct vm *, struct vec *));
struct obj *num_obj_new(float);
struct obj *str_obj_new(char *);

struct hashmap *obj_hashmap_new();
struct obj *obj_hashmap_get(struct hashmap *, char *);
void obj_hashmap_set(struct hashmap *, char *, struct obj *);

#endif
