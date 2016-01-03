#include "minunit.h"
#include "helpers.h"
#include <vector.h>
#include <algorithm.h>

struct Vector* v = NULL;
size_t initial_capacity = 10;
size_t call_count = 0;

void before_all()   {}
void after_all()    {}

void before_each() {
  v = Vector_create(initial_capacity);
  call_count = 0;
}

void after_each() {
  VECTOR_FOR_EACH(v, Packet_destroy);
  Vector_destroy(v);
}

char* test_VECTOR_FOR() {
  Vector_push( v, Packet_create(1) );
  Vector_push( v, Packet_create(2) );
  VECTOR_FOR(v, struct Packet*, p) {
    mu_assert( (*p)->size == i+1, "Incorrect packet size" );
  }
  return NULL;
}

char *all_tests() {
  mu_suite_start(before_each, after_each);
  before_all();
  mu_run_test(test_VECTOR_FOR);
  after_all();
  return NULL;
}

RUN_TESTS(all_tests);
