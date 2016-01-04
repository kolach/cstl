#include "test_helpers.h"
#include <stdarg.h>
#include <stdlib.h>
#include <dbg.h>

int IntVector_equals_to(struct IntVector* v, int num, ...) {
  size_t size = IntVector_size(v);
  va_list args;
  va_start(args, num);
  check(size == (size_t) num, "vector size: %zu is not equal to supplied arg number: %d", size, num);
  for (size_t i = 0; i < size; i++) {
    int actual = IntVector_get_unsafe(v, i);
    int expected = va_arg(args, int);
    check( expected == actual, "Expected vector contains %d at postion %zu, but found %d", expected, i, actual);
  }
  va_end(args);
  return 1;
error:
  va_end(args);
  return 0;
}

int IntVector_push_many(struct IntVector* v, int num, ...) {
  va_list values;
  va_start(values, num);
  for (int i = 0; i < num; i++) {
    int val = va_arg(values, int);
    int err = IntVector_push(v, val);
    check(err == 0, "Failed to push value");
  }
  va_end(values);
  return 0;
error:
  va_end(values);
  return -1;
}

struct Packet* Packet_create(size_t size) {
  struct Packet* p = malloc(sizeof(struct Packet));
  check_mem(p);
  p->data = calloc(size, sizeof(void*));
  p->size = size;
  check_mem(p->data);
  return p;
error:
  if (p) free(p);
  return NULL;
}

void Packet_destroy(struct Packet* p) {
  free(p->data);
  free(p);
}
