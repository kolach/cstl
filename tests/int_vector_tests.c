#include "minunit.h"
#include <int_vector.h>
#include <stdarg.h>

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


char* test_remove_beyond_size() {
  size_t index_beyond_size = IntVector_size(vec);
  int val;
  int err = IntVector_remove(vec, index_beyond_size, &val);
  mu_assert(err == -1, "Removing beyond size should fail with error code -1");
  return NULL;
}

char* test_remove_middle() {

  size_t N = 5; // total values to insert
  IntVector_push_many(vec, N, 1, 2, 3, 4, 5);

  // vector contains: 1, 2, 3, 4, 5 ... N
  // remove '3' in the middle (index: 2)
  size_t M = 2; // position to remove
  int val;
  int err = IntVector_remove(vec, M, &val);
  mu_assert(err == 0, "failed to remove element");
  mu_assert(IntVector_size(vec) == N-1, "vector should decrease in size");
  mu_assert(val == 3, "vector should return removed value");

  // vector contains: 1, 2, 4, 5
  int j = 1;
  for (size_t i = 0; i < N-1; i++) {
    if (i == M)
      j = j+1;
    mu_assert(IntVector_get_unsafe(vec, i) == j, "vector contains wrong value");
    j++;
  }

  return NULL;
}

char* test_remove_last() {
  size_t N = 5; // total values to insert
  IntVector_push_many(vec, N, 1, 2, 3, 4, 5);

  // vector contains: 1, 2, 3, 4, 5 ... N
  for (int i = N-1; i >= 0; i--) {
    int j;
    int err = IntVector_remove(vec, i, &j);
    mu_assert(err == 0, "failed to remove element");
    mu_assert(j == i+1, "should return removed element");
    mu_assert(IntVector_size(vec) == (size_t)i, "size should be decreased");
    // check values that are left
    for (int k = 0; k < i; k++)
      mu_assert(IntVector_get_unsafe(vec, k) == k+1, "vector contains wrong value");
  }
  mu_assert(IntVector_size(vec) == 0, "all values should be revode");
  return NULL;
}

char* test_remove_first() {
  size_t N = 5; // total values to insert
  IntVector_push_many(vec, N, 1, 2, 3, 4, 5);

  for (int i = 0; i < (int) N; i++) {
    int j;
    int err = IntVector_remove(vec, 0, &j);
    mu_assert(err == 0, "failed to remove element");
    mu_assert(j == i+1, "should return removed element");
    mu_assert(IntVector_size(vec) == N-(i+1), "size should be decreased");
    // check values that are left
    for (int k = 0; k < (int) IntVector_size(vec); k++) {
      mu_assert(IntVector_get_unsafe(vec, k) == j+1+k, "vector contains wrong value");
    }
  }

  mu_assert(IntVector_size(vec) == 0, "all values should be revode");
  return NULL;
}

char* test_insert_beyond_size() {
  size_t index_beyond_size = IntVector_size(vec) + 1;
  int err = IntVector_insert(vec, index_beyond_size, 1);
  mu_assert(err == -1, "insert beyond vector size should fail with error code = -1");
  return NULL;
}

char* test_insert_middle() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  IntVector_insert(vec, 2, 6);
  mu_assert(IntVector_equals_to(vec, 6, 1,2,6,3,4,5), "vector is not equal to expected values");
  return NULL;
}


char* test_insert_front() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  IntVector_insert(vec, 0, 6);
  mu_assert(IntVector_equals_to(vec, 6, 6,1,2,3,4,5), "vector is not equal to expected values");
  return NULL;
}

char* test_insert_back() {
  IntVector_push_many(vec, 5, 1,2,3,4,5);
  IntVector_insert(vec, 5, 6);
  mu_assert(IntVector_equals_to(vec, 6, 1,2,3,4,5,6), "vector is not equal to expected values");
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
  mu_run_test(test_remove_beyond_size);
  mu_run_test(test_remove_middle);
  mu_run_test(test_remove_last);
  mu_run_test(test_remove_first);
  mu_run_test(test_insert_beyond_size);
  mu_run_test(test_insert_middle);
  mu_run_test(test_insert_front);
  mu_run_test(test_insert_back);
  after_all();
  return NULL;
}

RUN_TESTS(all_tests);
