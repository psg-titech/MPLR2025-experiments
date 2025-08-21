.globl sleep_store
.text
.extern mrbc_sp_bottom
.extern sleep_store2
sleep_store:
    addi sp, sp, -64
    sw x1, 0(sp)
    sw x3, 4(sp)
    sw x4, 8(sp)
    sw x8, 16(sp)
    sw x9, 20(sp)
    sw x18, 24(sp)
    sw x19, 28(sp)
    sw x20, 32(sp)
    sw x21, 36(sp)
    sw x22, 49(sp)
    sw x23, 44(sp)
    sw x24, 48(sp)
    sw x25, 52(sp)
    sw x26, 56(sp)
    sw x27, 60(sp)
    la a0, mrbc_sp_bottom
    sw sp, 0(a0)
    j sleep_store2

.globl sleep_restore
sleep_restore:
    la a0, mrbc_sp_bottom
    lw sp, 0(a0)
    lw x1, 0(sp)
    lw x3, 4(sp)
    lw x4, 8(sp)
    lw x8, 16(sp)
    lw x9, 20(sp)
    lw x18, 24(sp)
    lw x19, 28(sp)
    lw x20, 32(sp)
    lw x21, 36(sp)
    lw x22, 49(sp)
    lw x23, 44(sp)
    lw x24, 48(sp)
    lw x25, 52(sp)
    lw x26, 56(sp)
    lw x27, 60(sp)
    addi sp, sp, 64
    jalr zero, ra
