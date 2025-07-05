.globl read_barrier
.globl read_barrier2
.globl object_new
.text
// arguments:
// x11(a1) address for read
// returns:
// x3 result value
// x11 type
read_barrier:
  addi sp, sp, -32
  sw a0, 4(sp)
  sw a1, 8(sp)
  c.mv a0, a1
  lw a0, 0(a0)    // load the necessary value
  beqz a0, readbarrier_this_is_nil
  andi a1, a0, 1  // a1 <- a0 & 1
  bnez a1, readbarrier_this_is_int  // this is int if a1 != 0
  srai a1, a0, 3  // a1 <- a0 >> 3 (4 and 6 will be zero.)
  beqz a1, readbarrier_this_is_bool // this is bool if a1 == 0

// beginning of read barrier
  sw a0, 28(sp)
  li a1, 0x50000000
  sub a0, a0, a1
  li a1, 16384
  bltu a0, a1, readbarrier_translation_skip // a1<16384(a0) then readbarrier_translation_skip
  sw ra, 0(sp)
  sw a2, 12(sp)
  sw a3, 16(sp)
  sw a4, 20(sp)
  sw a5, 24(sp)
  lw a0, 28(sp)
  jal ra, readfail
  lw a2, 12(sp)
  lw a3, 16(sp)
  lw a4, 20(sp)
  lw a5, 24(sp)
  
  lw a1, 8(sp)
  sw a0, 0(a1)  // update the origin.
  
  lw ra, 0(sp)
  j readbarrier_end_read_barrier
readbarrier_translation_skip:
  lw a0, 28(sp)  // restore the value
// end of read barrier
readbarrier_end_read_barrier:
  lh a1, 0(a0)
  j readbarrier_return
readbarrier_this_is_nil:
  c.li a1, 1 // NIL
  j readbarrier_return
readbarrier_this_is_bool:
  li a1, 3
  srai a0, a0, 2
  j readbarrier_return
readbarrier_this_is_int:
  li a1, 4
  srai a0, a0, 1
readbarrier_return: // return value must be in a0, a1 holds type id.
  c.mv x3, a0
  lw a0, 4(sp)
  addi sp, sp, 32
  ret
readbarrier_debug_out:
//  sw a2, 12(sp)
//  sw a3, 16(sp)
//  sw a4, 20(sp)
//  sw a5, 24(sp)
//  sw ra, 28(sp)
//  call debugout
//  lw a2, 12(sp)
//  lw a3, 16(sp)
//  lw a4, 20(sp)
//  lw a5, 24(sp)
//  lw ra, 28(sp)
//  ret

.extern mrbc_gc_alloc
// Save arguments
// Call alloc
// Assign type id!
// a0 is length of objects << 16 | id
object_new:
  addi sp, sp, -48
  sw ra, 0(sp)
  sw a0, 4(sp)
  sw a1, 8(sp)
  sw a2, 12(sp)
  sw a3, 16(sp)
  sw a4, 20(sp)
  sw a5, 24(sp)
  sw a6, 28(sp)
  sw a7, 32(sp)
  srli a0, a0, 16
  slli a0, a0, 2 // * sizeof(uint32_t)
  addi a0, a0, 4 // add for id and length.
  jal ra, mrbc_gc_alloc
  lw a1, 4(sp) // read a0
  sw a1, 0(a0)
  lw ra, 0(sp)
  lw a1, 8(sp)
  lw a2, 12(sp)
  lw a3, 16(sp)
  lw a4, 20(sp)
  lw a5, 24(sp)
  lw a6, 28(sp)
  lw a7, 32(sp)
  addi sp, sp, 48
  ret
