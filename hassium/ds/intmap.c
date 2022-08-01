#include <ds/intmap.h>

static void resize(struct intmap *intmap);

struct intmap *intmap_new() {
  struct intmap *intmap = (struct intmap *)calloc(1, sizeof(struct intmap));
  intmap->values = (int *)calloc(DICT_SIZE, sizeof(void *));
  intmap->size = DICT_SIZE;
  intmap->len = 0;
  return intmap;
}

void intmap_free(struct intmap *intmap) {
  free(intmap->values);
  free(intmap);
}

void intmap_insert(struct intmap *intmap, int key, int value) {
  while (key >= intmap->size) resize(intmap);
  intmap->values[key] = value;
}

int intmap_get(struct intmap *intmap, int key) { return intmap->values[key]; }

static void resize(struct intmap *intmap) {
  intmap->size += DICT_SIZE;
  intmap->values = realloc(intmap->values, intmap->size * sizeof(void *));
}