/*
 * This sample program executes a static mruby/c program.
 * The compiled mruby bytecode file should be prepared by "mrbc" command
 * as followings:
 *
 *   mrbc --remove-lv -Bmrbbuf sample_include_bytecode.rb
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"
#include "hal_esp32/copro/copro.h"

#include "test_copro_bytecode.c"

#if !defined(MRBC_MEMORY_SIZE)
#define MRBC_MEMORY_SIZE (1024*40)
#endif
static uint8_t memory_pool[MRBC_MEMORY_SIZE];

int main(void)
{
  mrbc_init(memory_pool, MRBC_MEMORY_SIZE);
  mrbc_add_copro_class(0);

  if( mrbc_create_task(mrbbuf, 0) != NULL ){
    mrbc_run();
  }

  return 0;
}
