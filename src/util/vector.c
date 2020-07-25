#include <util/vector.h>

static void vector_realloc (vector_t * v);

vector_t * vector_init () {
  vector_t * v;

  v       = (vector_t *)calloc (1, sizeof (vector_t));
  v->data = calloc (VECTOR_BUF_INC_SIZE, sizeof (void *));
  v->len  = 0;
  v->size = VECTOR_BUF_INC_SIZE;

  return v;
}

void vector_free (vector_t * v) {
  if (v->data) {
    free (v->data);
  }

  free (v);
}

void vector_deep_free (vector_t * v, void (*free_fn)(void *)) {
  if (v->data) {
    for (int i = 0; i < v->len; i ++) {
      if (v->data [i]) free_fn (v->data [i]);
    }

    free (v->data);
  }

  free (v);
}

void * vector_pop (vector_t * v) {
  if (v->len <= 0) return NULL;
  else return v->data [--v->len];
}

void vector_push (vector_t * v, void * ptr) {
  if (v->len >= v->size) {
    vector_realloc (v);
  }

  v->data [v->len++] = ptr;
}

void * vector_get (vector_t * v, int i) {
  if (i < 0 || i >= v->size) {
    return NULL;
  }

  return v->data [i];
}

void vector_set (vector_t * v, int i, void * ptr) {
  if (i <= 0) return;
  while (i >= v->size) {
    vector_realloc (v);
    v->len = i;
  }

  v->data [i] = ptr;
}

static void vector_realloc (vector_t * v) {
  v->size += VECTOR_BUF_INC_SIZE;
  v->data = (void **)realloc (v->data, v->size);
}
