#include <stdlib.h>
#include "vectorgen.h"
#include "dbg.h"

#ifndef VECTOR_TYPE
#define VECTOR_TYPE Vector
#endif

#ifndef DATA_TYPE
#define DATA_TYPE void*
#endif

#ifndef CSTL_VECTOR_METHOD
#define CSTL_VECTOR_METHOD(method) VECTOR_TYPE ## _ ## method
#endif

#ifndef EXPAND_RATE
#define EXPAND_RATE 300
#endif

/*typedef struct VECTOR_TYPE {*/
  /*size_t size;*/
  /*size_t expand_rate;*/
  /*size_t capacity;*/
  /*DATA_TYPE* data;*/
/*} VECTOR_TYPE;*/


static inline int set_capacity(struct VECTOR_TYPE* v, size_t new_capacity) {
    if (v->capacity == new_capacity) return 0;
    debug("Resizing vector from %zu to %zu", v->capacity, new_capacity);


    if (new_capacity < v->expand_rate + 1) new_capacity = v->expand_rate + 1; 
    size_t old_capacity = v->capacity;

    v->capacity = new_capacity;
    void *data = realloc(v->data, v->capacity * sizeof(DATA_TYPE));
    check_mem(data);
    v->data = data;
    
    if (old_capacity < new_capacity)
      memset(v->data + old_capacity, 0, new_capacity - old_capacity);
    
    return 0;
  error:
    return -1;
}

static inline int expand(struct VECTOR_TYPE* v) {
  size_t new_capacity = v->capacity + v->expand_rate;
  check(set_capacity(v, new_capacity) == 0, "Failed to expand v to new size: %zu", new_capacity);
  return 0;
error:
  return -1;
}

static inline int collapse(struct VECTOR_TYPE* v) {
  size_t new_capacity = v->capacity - v->expand_rate;
  check(set_capacity(v, new_capacity) == 0, "Failed to expand v to new size: %zu", new_capacity);
  return 0;
error:
  return -1;
}

static inline int update_capacity(struct VECTOR_TYPE* v) {
  if (v->size >= v->capacity)
    return expand(v);
  if (v->size + v->expand_rate < v->capacity)
    return collapse(v);
  return 0;
}

static inline int push(struct VECTOR_TYPE* v, DATA_TYPE elem) {
  v->data[v->size] = elem;
  v->size++;
  return update_capacity(v);
}

struct VECTOR_TYPE* CSTL_VECTOR_METHOD(create)(size_t initial_capacity) {
  struct VECTOR_TYPE* v = malloc(sizeof(struct VECTOR_TYPE));
  check_mem(v);
  check(initial_capacity > 0, "initial_size should be greater than zero");
  v->capacity = initial_capacity;
  v->element_size = sizeof(DATA_TYPE);
  v->data = calloc(v->capacity, v->element_size);
  v->size = 0;
  v->expand_rate = EXPAND_RATE;
  check_mem(v->data);
  return v;
error:
  if (v) free(v);
  return 0;
}

void CSTL_VECTOR_METHOD(destroy)(struct VECTOR_TYPE* v) {
  free(v->data);
  free(v);
}

size_t CSTL_VECTOR_METHOD(capacity)(struct VECTOR_TYPE* v) {
  return v->capacity;
}

int CSTL_VECTOR_METHOD(set_capacity)(struct VECTOR_TYPE* v, size_t new_capacity) {
  return set_capacity(v, new_capacity);
}

size_t CSTL_VECTOR_METHOD(size)(struct VECTOR_TYPE* v) {
  return v->size;
}

int CSTL_VECTOR_METHOD(set_size)(struct VECTOR_TYPE* v, size_t new_size) {
  if (v->capacity < new_size) 
    check(set_capacity(v, new_size) == 0, "Failed to set capacity to %zu", new_size);
  v->size = new_size;
  return 0;
error:
  return -1;
}

size_t CSTL_VECTOR_METHOD(expand_rate)(struct VECTOR_TYPE* v) {
  return v->expand_rate;
}

DATA_TYPE* CSTL_VECTOR_METHOD(data)(struct VECTOR_TYPE* v) {
  return v->data;
}

int CSTL_VECTOR_METHOD(set)(struct VECTOR_TYPE* v, size_t index, DATA_TYPE elem) {
  check(index < v->capacity, "attempt to set data beyond allocated area");
  v->data[index] = elem;
  if (index > v->size)
    v->size = index;
  return 0;
error:
  return -1;
}

static inline int get(const struct VECTOR_TYPE* v, size_t index, DATA_TYPE* elem) {
  check(index < v->size, "Attempt to access by index %zu beyound size: %zu", index, v->size);
  *elem = v->data[index];
  return 0;
error:
  return -1;
}

int CSTL_VECTOR_METHOD(get)(const struct VECTOR_TYPE* v, size_t index, DATA_TYPE* elem) {
  return get(v, index, elem);
}

DATA_TYPE CSTL_VECTOR_METHOD(get_unsafe)(const struct VECTOR_TYPE* v, size_t index) {
  DATA_TYPE elem;
  get(v, index, &elem);
  return elem;
}

int CSTL_VECTOR_METHOD(expand)(struct VECTOR_TYPE* v) {
  return expand(v);
}

// shrinks vector capacity at least expand_rate size
int CSTL_VECTOR_METHOD(contract)(struct VECTOR_TYPE* v) {
  size_t new_capacity = v->size < v->expand_rate ? v->expand_rate : v->size;
  return set_capacity(v, new_capacity + 1);
}

int CSTL_VECTOR_METHOD(push)(struct VECTOR_TYPE* v, DATA_TYPE elem) {
  return push(v, elem);
}

static inline int pop(struct VECTOR_TYPE* v, DATA_TYPE* elem) {
  check(v->size > 0, "Attempt to pop from empty array");
  if (elem)
    *elem = v->data[v->size - 1];
  v->size--;
  check(update_capacity(v) == 0, "Failed to update capacity");
  return 0;
error:
  return -1;
}

int CSTL_VECTOR_METHOD(pop)(struct VECTOR_TYPE* v, DATA_TYPE* elem) {
  return pop(v, elem);
}


DATA_TYPE CSTL_VECTOR_METHOD(pop_unsafe)(struct VECTOR_TYPE* v) {
  DATA_TYPE elem;
  pop(v, &elem);
  return elem;
}

int CSTL_VECTOR_METHOD(insert)(struct VECTOR_TYPE* v, size_t pos, DATA_TYPE elem) {
  if (pos == v->size) return push(v, elem);
  check(pos < v->size, "Attempt to insert value beyond size");
  DATA_TYPE* src = v->data + pos;
  DATA_TYPE* dst = src + 1;
  size_t count = (v->size - pos)*sizeof(DATA_TYPE);
  check( memmove(dst, src, count ), "Failed to move data" );
  v->data[pos] = elem;
  v->size++;
  return update_capacity(v);;
error:
  return -1;
}

int CSTL_VECTOR_METHOD(remove)(struct VECTOR_TYPE* v, size_t pos, DATA_TYPE* elem) {
  // small optimization if last element is removed
  if (pos == v->size-1) return pop(v, elem);

  check(pos < v->size, "Attempt to remove beyond size");
  if (elem) *elem = v->data[pos];
  DATA_TYPE* dst = v->data + pos;
  DATA_TYPE* src = dst + 1;
  size_t count = (v->size - pos)*sizeof(DATA_TYPE);
  check( memmove(dst, src, count ), "Failed to move data" );
  v->size--;
  return update_capacity(v);
error:
  return -1;
}

#undef EXPAND_RATE
