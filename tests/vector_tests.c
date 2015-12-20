#include "minunit.h"

void before_all()   {}
void after_all()    {}
void before_each()  {}
void after_each()   {}

char* test_create() {
  return "Pending implementation";
}

char *all_tests() {
  mu_suite_start(before_each, after_each);
  before_all();
  mu_run_test(test_create);
  after_all();
  return NULL;
}

RUN_TESTS(all_tests);
