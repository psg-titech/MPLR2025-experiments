#include "vector_t.h"
#include "../../src/alloc.h"
#include <string.h>
#include "mymem.h"

int mrbcopro_vector_new(struct VM * vm, mrbcopro_vector_t * self, int size) {
  char * buf = (char *)mrbcopro_alloc(vm, size);
  if(buf == 0) return 1;
  self->head = buf;
  self->last = &(buf[size]);
  self->cur = buf;
  return 0;
}

int mrbcopro_vector_extend(struct VM * vm, mrbcopro_vector_t * self, int write_bytes) {
  size_t bufCur = (size_t)self->cur, bufLast = (size_t)self->last;
  if((bufCur + write_bytes) > bufLast) {
    size_t buf = (size_t)self->head;
    size_t extend_size = ((write_bytes >> 5) + 1) << 5;
    size_t after_size = bufCur + extend_size - buf;
    void * newp = mrbcopro_realloc(vm, (void *)buf, after_size);
    if(newp == NULL) {
      mrbc_raise(vm, NULL, "OUT OF MEMORY");
      return 1;
    }
    self->head = newp;
    self->last = (char *)((size_t)newp + after_size);
    self->cur = (char *)((size_t)newp + bufCur - buf);
  }
  return 0;
}

int mrbcopro_vector_append_without_fill(struct VM * vm, mrbcopro_vector_t * self, int len) {
  int ret = mrbcopro_vector_extend(vm, self, len);
  if(ret) return ret;
  self->cur = &(self->cur[len]);
  return 0;
}
int mrbcopro_vector_append(struct VM * vm, mrbcopro_vector_t * self, int len, char * buf) {
  int ret = mrbcopro_vector_extend(vm, self, len);
  if(ret) return ret;
  memcpy(self->cur, buf, len);
  self->cur = &(self->cur[len]);
  return 0;
}
int mrbcopro_vector_append8(struct VM * vm, mrbcopro_vector_t * self, char val) {
  int ret = mrbcopro_vector_extend(vm, self, 1);
  if(ret) return ret;
  *(self->cur) = val;
  self->cur = &(self->cur[1]);
  return 0;
}
int mrbcopro_vector_append16(struct VM * vm, mrbcopro_vector_t * self, uint16_t val) {
  int ret = mrbcopro_vector_extend(vm, self, 2);
  if(ret) return ret;
  uint16_t * c = (uint16_t *)self->cur;
  *c = val;
  self->cur = (char *)(&(c[1]));
  return 0;
}
int mrbcopro_vector_append32(struct VM * vm, mrbcopro_vector_t * self, uint32_t val) {
  int ret = mrbcopro_vector_extend(vm, self, 4);
  if(ret) return ret;
  uint32_t * c = (uint32_t *)self->cur;
  *c = val;
  self->cur = (char *)(&(c[1]));
  return 0;
}
void mrbcopro_vector_free(struct VM * vm, mrbcopro_vector_t * self){
  mrbcopro_free(vm, self->head);
  self->head = 0;
  self->cur = 0;
  self->last = 0;
}
