#include "minunit.h"
#include "test_helpers.h"

struct IntVector* vec;
size_t initial_capacity;
size_t initial_size;
struct IntVector* call_args;
size_t call_count;

void before_all() {}

void after_all() {}

void before_each() {
  initial_capacity = 10;
  vec = IntVector_create(initial_capacity);
  call_count = 0;
  call_args = IntVector_create(initial_capacity);
}

void after_each() {
  IntVector_destroy(vec);
  IntVector_destroy(call_args);
}

void sample_call(int v) {
  call_count++;
  IntVector_push(call_args, v);
}

char* test_VECTOR_FOREACH() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  VECTOR_FOR_EACH(vec, sample_call);
  mu_assert(call_count == 5, "Incorrect call count");
  mu_assert(IntVector_equals_to(vec, 5, 1,2,3,4,5), "Incorrect arguments passed to function");
  return NULL;
}

void add(int val, int* a) { *a += val; }

char* test_VECTOR_REDUCE() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  int sum = 0;
  VECTOR_REDUCE(vec, add, &sum);
  mu_assert(sum == 15, "sum of vector elements is not correct");
  return NULL;
}

void square(int* val) {
  *val *= *val; 
}

char* test_VECTOR_FOR() {
  IntVector_push_many( vec, 5, 0,1,2,3,4 );
  VECTOR_FOR(vec, int, v) {
    mu_assert( *v == (int) i, "Incorrect value" );
  }
  return NULL;
}

char* test_VECTOR_TRANSFORM() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  VECTOR_TRANSFORM(vec, square);
  mu_assert(IntVector_equals_to(vec, 5, 1,4,9,16,25), "vector is not equal to expected values");
  return NULL;
}

int map_decremented(int val) { return val - 1; }

char* test_VECTOR_MAP() {
  IntVector* m = IntVector_create(5);
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  VECTOR_MAP(vec, m, map_decremented);
  mu_assert(IntVector_equals_to(m, 5, 0,1,2,3,4), "vector is not equal to expected values");
  IntVector_destroy(m);
  return NULL;
error:
  IntVector_destroy(m); 
  return "Failed to MAP vector";
}

char* test_VECTOR_MAP_failure() {
  IntVector* m = IntVector_create(1);
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  VECTOR_MAP(vec, m, map_decremented);
  IntVector_destroy(m);
  return "Should FAIL but PASSED";
error:
  IntVector_destroy(m); 
  return NULL;
}

char *all_tests() {
  mu_suite_start(before_each, after_each);
  before_all();
  mu_run_test(test_VECTOR_FOR);
  mu_run_test(test_VECTOR_FOREACH);
  mu_run_test(test_VECTOR_REDUCE);
  mu_run_test(test_VECTOR_TRANSFORM);
  mu_run_test(test_VECTOR_MAP);
  mu_run_test(test_VECTOR_MAP_failure);
  after_all();
  return NULL;
}

RUN_TESTS(all_tests);
