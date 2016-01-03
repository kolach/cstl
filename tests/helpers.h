#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <int_vector.h>

// helper functions to populate and compare content

int IntVector_equals_to(struct IntVector* v, int num, ...);
int IntVector_push_many(struct IntVector* v, int num, ...);

// test structure

typedef struct Packet {
  void* data;
  size_t size;
} Packet;

struct Packet* Packet_create(size_t size);
void Packet_destroy(struct Packet* p);

#endif
