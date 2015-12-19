#include "minunit.h"
#include <int_vector.h>


struct IntVector* vec;
size_t initial_capacity;


void before_all() {}

void after_all() {}

void before_each() {
  initial_capacity = 10;
  vec = IntVector_create(initial_capacity);
}

void after_each() {
  IntVector_destroy(vec);
}

char* test_create() {
  mu_assert(IntVector_capacity(vec) == initial_capacity, "Initial capacity is incorrect");
  mu_assert(IntVector_size(vec) == 0, "Initial size should be 0");
  check_mem(vec);
  return NULL;
error:
  return "Failed to create vector";
}

char* test_expand_and_contract() {
  size_t current_capacity = IntVector_capacity(vec);
  size_t expand_rate = IntVector_expand_rate(vec);
  IntVector_expand(vec);
  mu_assert(IntVector_capacity(vec) == current_capacity + expand_rate,
      "Exapnd should increase by with expand_rate");
  IntVector_contract(vec);
  mu_assert(IntVector_capacity(vec) == expand_rate + 1,
      "contract should set capacity to at expand_rate+1");
  IntVector_contract(vec);
  // should preserve the same capacity
  mu_assert(IntVector_capacity(vec) == expand_rate + 1,
      "contract should set capacity to at expand_rate+1");
  return NULL;
}

char* test_push_expands_vector() {
  size_t initial_size = IntVector_size(vec);
  size_t expand_rate = IntVector_expand_rate(vec);

  size_t push_qty = initial_capacity - initial_size;
  for (int i = 1; i < (int) push_qty; i++)
    IntVector_push(vec, i);

  mu_assert(IntVector_capacity(vec) == initial_capacity,
      "Capacity should stay the same");

  IntVector_push(vec, push_qty);
  mu_assert(IntVector_capacity(vec) == initial_capacity + expand_rate,
      "Capacity should be encreased by expand_rate when size is equal current capacity");
  mu_assert(IntVector_size(vec) == initial_size + push_qty,
      "Vector size should correspond to number of pushed elements");


  // let's reach the capacity again and check that vector is expanded
  size_t current_capacity = IntVector_capacity(vec);
  size_t current_size = IntVector_size(vec);
  push_qty = current_capacity - current_size;
  for (int i = 0; i < (int) push_qty; i++)
    IntVector_push(vec, i);

  mu_assert(IntVector_capacity(vec) == current_capacity + expand_rate,
      "Capacity should be encreased by expand_rate when size is equal current capacity");
  mu_assert(IntVector_size(vec) == current_size + push_qty,
      "Vector size should correspond to number of pushed elements");

  return NULL;
}

char* test_pop_collapses_vector() {

  size_t initial_size = IntVector_size(vec);

  mu_assert(initial_size == 0, "it's necessary that size is 0 at the beginning of this test");

  size_t expand_rate = IntVector_expand_rate(vec);

  size_t push_qty = initial_capacity - initial_size;
  for (int i = 0; i < (int) push_qty; i++)
    IntVector_push(vec, i);

  size_t current_capacity = IntVector_capacity(vec);
  size_t current_size = IntVector_size(vec);

  mu_assert(current_capacity > initial_capacity, "vector should grow in capacity");
  mu_assert(current_size > initial_size, "vector should grow in size");

  for (int i = 0; i < (int) push_qty; i++)
    IntVector_pop(vec, NULL);

  current_capacity = IntVector_capacity(vec);
  current_size = IntVector_size(vec);

  debug("Current current capacity is: %zu", current_capacity);
  debug("Current size is: %zu", current_size);

  mu_assert(current_size == initial_size,
      "size should return to original value");
  mu_assert(current_capacity == expand_rate+1,
      "capacity should collapse to it's minimum which is equal to expand_rate+1")

  // pusing 2*epand_rate + initial_capacity, then popping epandrate
  // checking how much vector was collapsed
  push_qty = 2*expand_rate + initial_capacity;
  for (int i = 0; i < (int) push_qty; i++)
    IntVector_push(vec, i);

  mu_assert(IntVector_capacity(vec) == current_capacity + 2*expand_rate,
      "capacity should be encreasesd by 2*expand_rate");

  // now let's pop expand_rate of elements
  for (int i = 0; i < (int) expand_rate; i++)
    IntVector_pop(vec, NULL);

  mu_assert(IntVector_capacity(vec) == current_capacity + expand_rate,
      "capacity should be reduced by expand_rate");

  return NULL;
}

char* test_push_pop() {
  int push_qty = 1000;
  for (int i = 1; i <= push_qty; i++)
    mu_assert( IntVector_push(vec, i) == 0, "Unsuccessful push");

  for (int i = push_qty; i > 0; i--) {
    int val;
    mu_assert( IntVector_pop(vec, &val) == 0, "Unsuccessful pop");
    mu_assert( val == i, "Poped value does not correspond to pushed");
  }
  return NULL;
}

char* test_pop() {
  int err = IntVector_pop(vec, NULL);
  mu_assert(err == -1, "pop from empty array should return -1");
  return NULL;
}

char* test_get() {
  int pushed_val = 1;
  IntVector_push(vec, pushed_val);
  int val;
  int err = IntVector_get(vec, 0, &val);
  mu_assert(err == 0, "getting value if index < size should return 0 error code");
  mu_assert(val == pushed_val, "get value should be equal to pushed value");

  err = IntVector_get(vec, 1, &val);
  mu_assert(err == -1, "get by index that >= size should return -1");
  return NULL;
}

char* test_set() {
  size_t index_beyond_capacity = initial_capacity;
  int err = IntVector_set(vec, index_beyond_capacity, 1);
  mu_assert(err == -1, "setting value beyon allocated area should fail with error code");

  size_t index_inside_capacity = initial_capacity - 1;
  mu_assert(index_inside_capacity > IntVector_size(vec), 
      "for this test it's necessary to have index greater than vector size");
  err = IntVector_set(vec, index_inside_capacity, 1);
  mu_assert(err == 0, "setting value inside allocated area is OK");
  mu_assert(IntVector_size(vec) == index_inside_capacity, 
      "size should be updated if index is greater than size");
  return NULL;
}

char *all_tests() {
  mu_suite_start(before_each, after_each);
  before_all(); 
  mu_run_test(test_create);
  mu_run_test(test_expand_and_contract);
  mu_run_test(test_push_expands_vector);
  mu_run_test(test_pop_collapses_vector);
  mu_run_test(test_push_pop);
  mu_run_test(test_pop);
  mu_run_test(test_get);
  mu_run_test(test_set);
  after_all();
  return NULL;
}

RUN_TESTS(all_tests);
