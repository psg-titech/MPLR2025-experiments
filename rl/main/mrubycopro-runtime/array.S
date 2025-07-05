.extern stopreason_info
.extern read_barrier
.globl array_get
.globl array_set
.globl string_getbyte

// a0 <- a0[a1]
array_get:
  lh a2, 2(a0)
  bltu a1, a2, array_get_ok
  li x10, 0
  li x11, 1 // NIL
  ret
array_get_ok:
  slli a1, a1, 2
  add a1, a1, a0
  addi a1, a1, 4
  addi sp, sp, -16
  sw ra, 0(sp)
  call read_barrier
  lw ra, 0(sp)
  addi sp, sp, 16
  c.mv x10, x3
  ret

string_getbyte:
  lh a2, 2(a0)
  bltu a1, a2, string_getbyte_ok
  li x10, 0
  li x11, 1 // NIL
  ret
string_getbyte_ok:
  add a1, a1, a0
  lbu a0, 4(a1)
  li x11, 4 // INTEGER
  ret

// a0[a1] <- a2
// a0 <- a2
array_set:
  lh a3, 2(a0)
  bltu a1, a3, array_set_ok
  ret // TODO: enlarge the array.
array_set_ok:
  slli a1, a1, 2
  add a1, a1, a0
  addi a1, a1, 4
  sw a2, 0(a1)
  c.mv a0, a2
  ret

  // addi sp, sp, -32
  // sw a0, 4(sp)
  // sw a1, 8(sp)
  // sw a2, 12(sp)
  // sw a3, 16(sp)
  // sw a4, 20(sp)
  // sw a5, 24(sp)
  // sw ra, 28(sp)
  // c.mv a0, a1
  // call debugout
  // lw a0, 4(sp)
  // lw a1, 8(sp)
  // lw a2, 12(sp)
  // lw a3, 16(sp)
  // lw a4, 20(sp)
  // lw a5, 24(sp)
  // lw ra, 28(sp)
  // addi sp, sp, 32