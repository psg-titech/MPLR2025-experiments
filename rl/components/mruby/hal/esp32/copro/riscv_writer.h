// BELIEVE OPTIMIZATIONS in the C compiler
#define RISCV_RTYPE(opcode, rd, rs1, rs2, funct3, funct7) (uint32_t)((uint32_t)(funct7) << 25 | (uint32_t)(rs2) << 20 | (uint32_t)(rs1) << 15 | (uint32_t)(funct3) << 12 | (uint32_t)(rd) << 7 | (opcode))
#define RISCV_ITYPE(opcode, rd, rs1, imm, funct3) (uint32_t)((uint32_t)(imm) << 20 | (uint32_t)(rs1) << 15 | (uint32_t)(funct3) << 12 | (uint32_t)(rd) << 7 | (opcode))
#define RISCV_STYPE(opcode, rs1, imm, rs2, funct3) (uint32_t)(((uint32_t)(imm) >> 5) << 25 | (uint32_t)(rs2) << 20 | (uint32_t)(rs1) << 15 | (uint32_t)(funct3) << 12 | ((uint32_t)(imm) & 0x1F) << 7 | (opcode))
#define RISCV_BTYPE(opcode, rs1, imm, rs2, funct3) (uint32_t)((((uint32_t)(imm) >> 12) << 31) | (((uint32_t)(imm) >> 5) & 0x3F) << 25 | (uint32_t)(rs2) << 20 | (uint32_t)(rs1) << 15 | (uint32_t)(funct3) << 12 | (((uint32_t)(imm) & 0x1F) >> 1) << 8 | (((uint32_t)(imm) >> 11) & 1) << 7 | (opcode))
#define RISCV_UTYPE(opcode, rd, imm) (uint32_t)(((uint32_t)(imm) & 0xFFFFF000) | (uint32_t)(rd) << 7 | (opcode))
#define RISCV_JTYPE(opcode, rd, imm) (uint32_t)((((uint32_t)(imm) >> 20) & 0x1) << 31 | (((uint32_t)(imm) >> 1) & 0x3FF) << 21 | (((uint32_t)(imm) >> 11) & 0x1) << 20 | ((uint32_t)(imm) & 0xFF000) | (uint32_t)(rd) << 7 | (opcode))

#define RISCV_ADD(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x00)
#define RISCV_SUB(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x20)
#define RISCV_XOR(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x4, 0x00)
#define RISCV_OR(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x6, 0x00)
#define RISCV_AND(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x7, 0x00)
#define RISCV_SHIFT_LEFT(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x1, 0x00)
#define RISCV_SHIFT_RIGHT_LOGICAL(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x00)
#define RISCV_SHIFT_RIGHT_ARITH(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x00)
#define RISCV_LESS_THAN(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x2, 0x00)
#define RISCV_LESS_THAN_UNSIGNED(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x3, 0x00)

#define RISCV_ADD_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x0)
#define RISCV_XOR_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x4)
#define RISCV_OR_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x6)
#define RISCV_AND_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x7)
#define RISCV_SHIFT_LEFT_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x1)
#define RISCV_SHIFT_RIGHT_LOGICAL_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x5)
#define RISCV_SHIFT_RIGHT_ARITH_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, (imm | (0x20) << 5), 0x5)
#define RISCV_LESS_THAN_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x2)
#define RISCV_LESS_THAN_UNSIGNED_IMM(rd, rs1, imm) RISCV_ITYPE(0x13, rd, rs1, imm, 0x3)

#define RISCV_BRANCH_EQUAL(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x0)
#define RISCV_BRANCH_NOT_EQUAL(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x1)
#define RISCV_BRANCH_LESS(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x4)
#define RISCV_BRANCH_GREATER_OR_EQUAL(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x5)
#define RISCV_BRANCH_LESS_UNSIGNED(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x6)
#define RISCV_BRANCH_GREATER_OR_EQUAL_UNSIGNED(rs1, rs2, imm) RISCV_BTYPE(0x63, rs1, imm, rs2, 0x7)

#define RISCV_LOAD_HALF_UNSIGNED(rd, rs1, imm) RISCV_ITYPE(3, rd, rs1, imm, 5)
#define RISCV_LOAD_BYTE_UNSIGNED(rd, rs1, imm) RISCV_ITYPE(3, rd, rs1, imm, 4)
#define RISCV_LOAD_WORD(rd, rs1, imm) RISCV_ITYPE(3, rd, rs1, imm, 2)
#define RISCV_LOAD_HALF(rd, rs1, imm) RISCV_ITYPE(3, rd, rs1, imm, 1)
#define RISCV_LOAD_BYTE(rd, rs1, imm) RISCV_ITYPE(3, rd, rs1, imm, 0)

#define RISCV_STORE_WORD(rs1, rs2, imm) RISCV_STYPE(35, rs1, imm, rs2, 2)
#define RISCV_STORE_HALF(rs1, rs2, imm) RISCV_STYPE(35, rs1, imm, rs2, 1)
#define RISCV_STORE_BYTE(rs1, rs2, imm) RISCV_STYPE(35, rs1, imm, rs2, 0)

#define RISCV_JUMP_AND_LINK(rd, imm) RISCV_JTYPE(0x6F, rd, imm)
#define RISCV_JUMP_AND_LINK_REG(rd, rs1, imm) RISCV_ITYPE(0x67, rd, rs1, imm, 0x00)

#define RISCV_LOAD_UPPER_IMM(rd, imm) RISCV_UTYPE(0x37, rd, imm)
#define RISCV_ADD_UPPER_IMM_PC(rd, imm) RISCV_UTYPE(0x17, rd, imm)

#define RISCV_MUL(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x1)
#define RISCV_DIV(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x4, 0x1)
#define RISCV_DIVU(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x1)
#define RISCV_REM(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x6, 0x1)
#define RISCV_REU(rd, rs1, rs2) RISCV_RTYPE(0x33, rd, rs1, rs2, 0x7, 0x1)

#define RISCV_CRTYPE(opcode, rs1, rs2, funct4) (uint16_t)(((uint32_t)(funct4) << 12) | ((uint32_t)(rs1) << 7) | ((uint32_t)(rs2) << 2) | (opcode))
#define RISCV_CITYPE(opcode, rd, imm, funct3) (uint16_t)(((uint32_t)(funct3) << 13) | (((uint32_t)(imm) & 0x20) << (12 - 5)) | ((uint32_t)(rd) << 7) | (((uint32_t)(imm) & 0x1F) << 2) | (opcode))
// #define RISCV_CSSTYPE(opcode, rs2, imm, funct3) !Different
#define RISCV_CIWTYPE(opcode, rd, imm, funct3) (uint16_t)(((uint32_t)(funct3) << 13) | ((((uint32_t)(imm) >> 4) & 0x3) << 11) | ((((uint32_t)(imm) >> 6) & 0xF) << 7) | ((((uint32_t)(imm) >> 2) & 1) << 6) | ((((uint32_t)(imm) >> 3) & 1) << 5) | ((uint32_t)(rd) << 2) | (opcode))
//#define RISCV_CLTYPE(opcode, rs1, rd, imm, funct3) !Different
//#define RISCV_CSTYPE(opcode, rs1, rd, imm, funct3) !Different
#define RISCV_CATYPE(opcode, rs1, rs2, funct2, funct6) (((uint32_t)(funct6) << 10) | (((uint32_t)(rs1) & 0x7) << 7) | ((uint32_t)(funct2) << 5) | (((uint32_t)(rs2) & 0x7) << 2) | ((uint32_t)(opcode)))
#define RISCV_CBTYPE(opcode, rs1, imm, funct3) (((uint32_t)(funct3) << 13) | ((((uint32_t)(imm) >> 8) & 1) << 12) | (((uint32_t)(imm) & 0x18) << 7) | (((uint32_t)(rs1) & 0x7) << 7) | (((uint32_t)(imm) & 0xC0) >> 1) | (((uint32_t)(imm) & 0x6) << 2) | (((uint32_t)(imm) & 0x20) >> 3) | (opcode))
#define RISCV_CBSTARTYPE(opcode, rs1, imm, funct3, funct2) (((uint32_t)(funct3) << 13) | ((((uint32_t)(imm) >> 5) & 1) << 12) | ((uint32_t)(funct2) << 10) | (((uint32_t)(rs1) & 0x7) << 7) | (((uint32_t)(imm) & 0x1F) << 1) | (opcode))
#define RISCV_CJTYPE(opcode, imm, funct3) (((uint32_t)(funct3) << 13) | ((((uint32_t)(imm) >> 11) & 1) << 12) | ((((uint32_t)(imm) >> 4) & 1) << 11) | ((((uint32_t)(imm) >> 8) & 0x3) << 9) | ((((uint32_t)(imm) >> 10) & 1) << 8) | ((((uint32_t)(imm) >> 6) & 1) << 7) | ((((uint32_t)(imm) >> 7) & 1) << 6) | ((((uint32_t)(imm) >> 1) & 0x7) << 3) | ((((uint32_t)(imm) >> 5) & 1) << 2) | (opcode))

#define RISCV_C_SWSP(rs2, imm) ((0x6 << 13) | ((((uint32_t)(imm) >> 2) & 0xF) << 9) | ((((uint32_t)(imm) >> 6) & 0x3) << 7) | ((uint32_t)(rs2) << 2) | 0x2)
#define RISCV_C_LWSP(rd, imm) ((0x2 << 13) | ((((uint32_t)(imm) >> 5) & 1) << 12) | ((uint32_t)(rd) << 7) | ((((uint32_t)(imm) >> 2) & 0x7) << 4) | ((((uint32_t)(imm) >> 6) & 0x3) << 2) | 0x2)
#define RISCV_C_JMP(imm) RISCV_CJTYPE(1, imm, 5)
#define RISCV_C_MOVE(rd, rs1) RISCV_CRTYPE(2, rd, rs1, 8)
#define RISCV_C_ADD(rd, rs1) RISCV_CRTYPE(2, rd, rs1, 9)
#define RISCV_C_AND(rd, rs2) RISCV_CATYPE(1, rd, rs2, 3, 0x23)
#define RISCV_C_AND_IMM(rd, imm) RISCV_CBSTARTYPE(1, rd, imm, 4, 2)
#define RISCV_C_OR(rd, rs2) RISCV_CATYPE(1, rd, rs2, 2, 0x23)
#define RISCV_C_XOR(rd, rs2) RISCV_CATYPE(1, rd, rs2, 1, 0x23)
#define RISCV_C_SUB(rd, rs2) RISCV_CATYPE(1, rd, rs2, 0, 0x23)
#define RISCV_C_JR(rs1) RISCV_CRTYPE(2, rs1, 0, 8)
#define RISCV_C_ADDI(rd, imm) RISCV_CITYPE(1, rd, imm, 0)
#define RISCV_C_SHIFT_LEFT_IMM(rd, imm) RISCV_CITYPE(2, rd, imm, 0)
#define RISCV_C_LI(rd, imm) RISCV_CITYPE(1, rd, imm, 2)
// Only imm[17:12] is acceptable.
#define RISCV_C_LUI(rd, imm) RISCV_CITYPE(1, rd, imm, 3)
#define RISCV_C_BEQZ(rs1, offset) RISCV_CBTYPE(1, rs1, offset, 6)
#define RISCV_C_BNEZ(rs1, offset) RISCV_CBTYPE(1, rs1, offset, 7)


#define RISCV_C_ADDI16SP(imm) ((0x3 << 13) | ((((uint32_t)(imm) >> 9) & 1) << 12) | (0x2 << 7) | ((((uint32_t)(imm) >> 4) & 1) << 6) | ((((uint32_t)(imm) >> 6) & 1) << 5) | ((((uint32_t)(imm) >> 7) & 0x3) << 3) | ((((uint32_t)(imm) >> 5) & 0x1) << 2) | 0x1)

#define RISCV_ARGS_REGISTER(r) ((r) + 10)
#define RISCV_CALLEE_SAVED_REGISTER(r) ((r) + 18)

#define RISCV_CALLEE_SAVE_REGISTERS 0x0FFC0304
#define RISCV_CALLER_SAVE_REGISTERS 0xF003FCE2
#define RISCV_NORMAL_CALLER_SAVE_REGISTERS 0xF003FCE0 // EXCLUDES RA (Return Address) Register.

#define RISCV_THESE_REGISTERS_ARE_COMPRESSED_AVAILABLE(r1, r2) (((r1) & (r2) & 0x8) == 0x8)
