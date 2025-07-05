#pragma once
#include <stdint.h>
#include "vm.h"

typedef struct mrbcopro_vector_t  {
  char * head;
  char * last;
  char * cur;
} mrbcopro_vector_t;
int mrbcopro_vector_new(struct VM * vm, mrbcopro_vector_t * self, int size);
int mrbcopro_vector_extend(struct VM * vm, mrbcopro_vector_t * self, int write_bytes);
int mrbcopro_vector_append(struct VM * vm, mrbcopro_vector_t * self, int len, char * buf);
int mrbcopro_vector_append_without_fill(struct VM * vm, mrbcopro_vector_t * self, int len);
int mrbcopro_vector_append8(struct VM * vm, mrbcopro_vector_t * self, char val);
int mrbcopro_vector_append16(struct VM * vm, mrbcopro_vector_t * self, uint16_t val);
int mrbcopro_vector_append32(struct VM * vm, mrbcopro_vector_t * self, uint32_t val);
void mrbcopro_vector_free(struct VM * vm, mrbcopro_vector_t * self);

#define mrbcopro_vector_bytes(vec) ((int)(vec)->cur - (int)(vec)->head)
#define mrbcopro_vector_bytes2(vec) ((int)(vec).cur - (int)(vec).head)

