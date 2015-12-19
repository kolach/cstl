/*************************************************************
 *
 * Zed’s Awesome Debug Macros”
 *
 ************************************************************/

#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
// If NDEBUG is defined, then “no debug” messages will remain
#define debug(M,...)
#else
// debug macros translates any use of debug("format", arg1, arg2) into an fprintf call to stderr.
// The macros actually work like printf and take variable arguments.
// Some C compilers (actually CPP) don’t support this, but the ones that matter do.
// The magic here is the use of ##__VA_ARGS__ that says “put whatever they had for extra arguments (...) here.”
// __FILE__ and __LINE__ are used to get the current file:line for the debug message.
//
// Example of usage:
//
// debug("I have Brown Hair.");     // notice you don't need the '\n'
// debug("I am %d years old.", 37); // passing in arguments like printf
//
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

// used in the others to get a safe, readable version of errno
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

// log_err, log_warn, log_info
//
// log_err, log_warn, and log_info, macros for logging messages that are meant for the end user.
// They work like debug but can’t be compiled out
//
// Example of usage:
//
// log_err("I believe everything is broken.");
// log_err("There are %d problems in %s.", 0, "space");
// log_warn("You can safely ignore this.");
// log_warn("Maybe consider looking at: %s.", "/etc/passwd");
// log_info("Well I did something mundane.");
// log_info("It happened %f times today.", 1.3f);
//
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

// check
//
// The best macro ever!
// check, will make sure the condition A is true, and if not,
// it logs the error M (with variable arguments for log_err), and then jumps to the function’s error: for cleanup
//
// Example of usage:
//
// int test_check(char *file_name)
// {
//     FILE *input = NULL;
//     char *block = NULL;
//
//     block = malloc(100);
//     check_mem(block);                // should work
//
//     input = fopen(file_name, "r");
//     check(input, "Failed to open %s.", file_name);
//
//     free(block);
//     fclose(input);
//     return 0;
//
// error:
//     if (block) free(block);
//     if (input) fclose(input);
//     return -1;
// }
//
#define check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

// sentinel
//
// The second best macro ever!
// sentinel, is placed in any part of a function that shouldn’t run,
// and if it does, it prints an error message and then jumps to the error: label.
// You put this in if-statements and switch-statements to catch conditions that shouldn’t happen, like the default:.
//
// Example of usage:
//
// int test_sentinel(int code)
// {
//     char *temp = malloc(100);
//     check_mem(temp);
//
//     switch (code) {
//         case 1:
//             log_info("It worked.");
//             break;
//         default:
//             sentinel("I shouldn't run.");
//     }
//
//     free(temp);
//     return 0;
//
// error:
//     if (temp)
//         free(temp);
//     return -1;
// }
//
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

// A shorthand macro called check_mem that makes sure a pointer is valid,
// and if it isn’t, it reports it as an error with “Out of memory.”
//
// Example of usage:
//
// int test_check_mem()
// {
//     char *test = NULL;
//     check_mem(test);
//
//     free(test);
//     return 1;
// error:
//     return -1;
// }
//
#define check_mem(A) check((A), "Out of memory.")

// An alternative macro, check_debug, which still checks and handles an error,
// but if the error is common, then it doesn’t bother reporting it.
// In this one, it will use debug instead of log_err to report the message.
// So when you define NDEBUG, the check still happens, and the error jump goes off, but the message isn’t printed
//
// Example of usage:
//
// int test_check_debug()
// {
//     int i = 0;
//     check_debug(i != 0, "Oops, I was 0.");
//     return 0;
// error:
//     return -1;
// }
//
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
