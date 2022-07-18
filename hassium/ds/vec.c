#include <ds/vec.h>

static void expand(struct vec *);

struct vec *vec_new()
{
    struct vec *vec = (struct vec *)calloc(1, sizeof(struct vec));
    vec->data = calloc(VEC_EXPAND_AT, sizeof(void *));
    vec->len = 0;
    vec->size = VEC_EXPAND_AT;
    return vec;
}

void vec_free(struct vec *vec)
{
    free(vec->data);
    free(vec);
}

void vec_free_deep(struct vec *vec)
{
    for (int i = 0; i < vec->len; i++)
        free(vec_get(vec, i));
    vec_free(vec);
}

void *vec_peek(struct vec *vec)
{
    if (vec->len == 0)
        return NULL;
    return vec->data[vec->len - 1];
}

void *vec_pop(struct vec *vec)
{
    if (vec->len == 0)
        return NULL;
    return vec->data[--vec->len];
}

void vec_push(struct vec *vec, void *val)
{
    if (vec->len >= vec->size)
        expand(vec);
    vec->data[vec->len++] = val;
}

void *vec_get(struct vec *vec, int idx)
{
    return vec->data[idx];
}

void vec_set(struct vec *vec, int idx, void *val)
{
    while (idx >= vec->size)
        expand(vec);
    vec->data[idx] = val;
    if (idx >= vec->len)
        vec->len = idx + 1;
}

static void expand(struct vec *vec)
{
    vec->size += VEC_EXPAND_AT;
    vec->data = realloc(vec->data, sizeof(void *) * vec->size);
}