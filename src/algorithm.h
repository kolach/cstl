#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#define VECTOR_FOR(v, DATA_TYPE, elem) \
  DATA_TYPE* elem = (DATA_TYPE*) v->data; \
  for (size_t i = 0; i < v->size; i++, elem++)

#define VECTOR_FOR_EACH(v, F) \
  for (size_t i = 0; i < v->size; i++) \
    F(v->data[i]);

#define VECTOR_TRANSFORM(v, F) \
  for (size_t i = 0; i < v->size; i++) \
    F(&v->data[i]);

#define VECTOR_MAP(src, dst, F) \
  check(dst->capacity >= src->size, "dst capacity: %zu is less than src size: %zu", dst->capacity, src->size); \
  for (size_t i = 0; i < src->size; i++) \
    dst->data[i] = F(src->data[i]); \
  dst->size = src->size;

#define VECTOR_REDUCE(v, F, A) \
  for (size_t i = 0; i < v->size; i++) \
    F(v->data[i], A);

#endif
