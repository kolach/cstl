#undef NDEBUG
#ifndef _miniunit_h
#define _miniunit_h

#include <stdio.h>
#include <dbg.h>
#include <stdlib.h>


typedef void (*mu_hook)();

#define mu_suite_start(before_each, after_each) \
  char* message = NULL; \
  mu_hook _before_each = before_each; \
  mu_hook _after_each = after_each;


#define mu_assert(test, message) if (!(test)) { log_err(message); return message; }

#define mu_run_test(test) \
  debug("\n-----%s", " " #test); \
  if (_before_each) _before_each(); \
  message = test(); \
  tests_run++; \
  if (_after_each) _after_each(); \
  if (message) \
    return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
	argc = 1; \
	debug("----- RUNNING: %s", argv[0]);\
	printf("----\nRUNNING: %s\n", argv[0]);\
	char *result = name();\
	if (result != 0) {\
		printf("FAILED: %s\n", result);\
	}\
	else {\
		printf("ALL TESTS PASSED\n");\
	}\
	printf("Tests run: %d\n", tests_run);\
	exit(result != 0);\
}

int tests_run;

#endif
