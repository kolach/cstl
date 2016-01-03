#ifndef DATA_TYPE
#error "Symbol DATA_TYPE must be defined"
#else
#define CONCAT(x,y) x ## y
#define CONCAT3_(x,y,z) x##y##z
#define CONCAT3(x,y,z) CONCAT3_(x,y,z)
#define CSTL_VECTOR_METHOD(m) CONCAT3(VECTOR_TYPE,_,m)

#include <stddef.h>

typedef struct VECTOR_TYPE {
  size_t size;
  size_t element_size;
  size_t expand_rate;
  size_t capacity;
  DATA_TYPE* data;
} VECTOR_TYPE;

struct VECTOR_TYPE* CSTL_VECTOR_METHOD(create)(size_t initial_capacity);

size_t CSTL_VECTOR_METHOD(capacity)(struct VECTOR_TYPE* v);

size_t CSTL_VECTOR_METHOD(size)(struct VECTOR_TYPE* v);

size_t CSTL_VECTOR_METHOD(expand_rate)(struct VECTOR_TYPE* v);

DATA_TYPE* CSTL_VECTOR_METHOD(data)(struct VECTOR_TYPE* v);

int CSTL_VECTOR_METHOD(set)(struct VECTOR_TYPE* v, size_t index, DATA_TYPE elem);

int CSTL_VECTOR_METHOD(get)(const struct VECTOR_TYPE* v, size_t index, DATA_TYPE* elem);

DATA_TYPE CSTL_VECTOR_METHOD(get_unsafe)(const struct VECTOR_TYPE* v, size_t index);

int CSTL_VECTOR_METHOD(set_capacity)(struct VECTOR_TYPE* v, size_t new_capacity);

int CSTL_VECTOR_METHOD(set_size)(struct VECTOR_TYPE* v, size_t new_size);

int CSTL_VECTOR_METHOD(expand)(struct VECTOR_TYPE* v);

int CSTL_VECTOR_METHOD(contract)(struct VECTOR_TYPE* v);

void CSTL_VECTOR_METHOD(destroy)(struct VECTOR_TYPE* v);

int CSTL_VECTOR_METHOD(push)(struct VECTOR_TYPE* v, DATA_TYPE elem);

int CSTL_VECTOR_METHOD(pop)(struct VECTOR_TYPE* v, DATA_TYPE* elem);

DATA_TYPE CSTL_VECTOR_METHOD(pop_unsafe)(struct VECTOR_TYPE* v);

int CSTL_VECTOR_METHOD(insert)(struct VECTOR_TYPE* v, size_t pos, DATA_TYPE elem);

int CSTL_VECTOR_METHOD(remove)(struct VECTOR_TYPE* v, size_t pos, DATA_TYPE* elem);

#endif
