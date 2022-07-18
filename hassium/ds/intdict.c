#include <ds/intdict.h>

static void resize(struct intdict *intdict);

struct intdict *intdict_new()
{
    struct intdict *intdict = (struct intdict *)calloc(1, sizeof(struct intdict));
    intdict->values = (int *)calloc(DICT_SIZE, sizeof(void *));
    intdict->size = DICT_SIZE;
    intdict->len = 0;
    return intdict;
}

void intdict_free(struct intdict *intdict)
{
    free(intdict->values);
    free(intdict);
}

void intdict_insert(struct intdict *intdict, int key, int value)
{
    while (key >= intdict->size)
        resize(intdict);
    intdict->values[key] = value;
}

int intdict_get(struct intdict *intdict, int key)
{
    return intdict->values[key];
}

static void resize(struct intdict *intdict)
{
    intdict->size += DICT_SIZE;
    intdict->values = (void *)realloc(intdict->values, intdict->size);
}