#pragma once
#include <stdint.h>

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4505)
#elif defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wunused-function"
#endif

#if  defined(__OPTIMIZE_SIZE__) || defined(_MSC_VER)
#define MRBC_INLINE 
#else
#define MRBC_INLINE inline __attribute__((always_inline))
#endif
// BELIEVE OPTIMIZATIONS in the C compiler

// Base instruction types
static MRBC_INLINE uint32_t RISCV_RTYPE(uint32_t opcode, uint32_t rd, uint32_t rs1, uint32_t rs2, uint32_t funct3, uint32_t funct7) {
  return opcode + (funct7 << 25) + (funct3 << 12) + (rs2 << 20) + (rs1 << 15) + (rd << 7);
}
static MRBC_INLINE uint32_t RISCV_ITYPE(uint32_t opcode, uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t funct3) {
  return opcode + (funct3 << 12) + (imm << 20) + (rs1 << 15) + (rd << 7);
}
static MRBC_INLINE uint32_t RISCV_STYPE(uint32_t opcode, uint32_t rs1, uint32_t imm, uint32_t rs2, uint32_t funct3) {
  return opcode + (funct3 << 12) + ((imm >> 5) << 25) + (rs2 << 20) + (rs1 << 15) + ((imm & 0x1F) << 7);
}
static MRBC_INLINE uint32_t RISCV_BTYPE(uint32_t opcode, uint32_t rs1, uint32_t imm, uint32_t rs2, uint32_t funct3) {
  return opcode + (funct3 << 12) + ((imm >> 12) << 31) + (((imm >> 5) & 0x3F) << 25) + (rs2 << 20) + (rs1 << 15) + (((imm & 0x1F) >> 1) << 8) + (((imm >> 11) & 1) << 7);
}
static MRBC_INLINE uint32_t RISCV_UTYPE(uint32_t opcode, uint32_t rd, uint32_t imm) {
  return opcode + (imm & 0xFFFFF000) + (rd << 7);
}
static MRBC_INLINE uint32_t RISCV_JTYPE(uint32_t opcode, uint32_t rd, uint32_t imm) {
  return opcode + (((imm >> 20) & 0x1) << 31) + (((imm >> 1) & 0x3FF) << 21) + (((imm >> 11) & 0x1) << 20) + (imm & 0xFF000) + (rd << 7);
}

// R-type instructions
static MRBC_INLINE uint32_t RISCV_ADD(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x00); }
static MRBC_INLINE uint32_t RISCV_SUB(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x20); }
static MRBC_INLINE uint32_t RISCV_XOR(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x4, 0x00); }
static MRBC_INLINE uint32_t RISCV_OR(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x6, 0x00); }
static MRBC_INLINE uint32_t RISCV_AND(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x7, 0x00); }
static MRBC_INLINE uint32_t RISCV_SHIFT_LEFT(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x1, 0x00); }
static MRBC_INLINE uint32_t RISCV_SHIFT_RIGHT_LOGICAL(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x00); }
static MRBC_INLINE uint32_t RISCV_SHIFT_RIGHT_ARITH(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x20); } // funct7 should be 0x20 for SRA
static MRBC_INLINE uint32_t RISCV_LESS_THAN(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x2, 0x00); }
static MRBC_INLINE uint32_t RISCV_LESS_THAN_UNSIGNED(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x3, 0x00); }

// I-type instructions
static uint32_t RISCV_ADD_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x0); }
static MRBC_INLINE uint32_t RISCV_XOR_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x4); }
static MRBC_INLINE uint32_t RISCV_OR_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x6); }
static MRBC_INLINE uint32_t RISCV_AND_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x7); }
static MRBC_INLINE uint32_t RISCV_SHIFT_LEFT_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x1); }
static MRBC_INLINE uint32_t RISCV_SHIFT_RIGHT_LOGICAL_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x5); }
static MRBC_INLINE uint32_t RISCV_SHIFT_RIGHT_ARITH_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, (imm | (0x20 << 5)), 0x5); }
static MRBC_INLINE uint32_t RISCV_LESS_THAN_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x2); }
static MRBC_INLINE uint32_t RISCV_LESS_THAN_UNSIGNED_IMM(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x13, rd, rs1, imm, 0x3); }

// B-type instructions
static MRBC_INLINE uint32_t RISCV_BRANCH_EQUAL(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x0); }
static MRBC_INLINE uint32_t RISCV_BRANCH_NOT_EQUAL(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x1); }
static MRBC_INLINE uint32_t RISCV_BRANCH_LESS(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x4); }
static MRBC_INLINE uint32_t RISCV_BRANCH_GREATER_OR_EQUAL(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x5); }
static MRBC_INLINE uint32_t RISCV_BRANCH_LESS_UNSIGNED(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x6); }
static MRBC_INLINE uint32_t RISCV_BRANCH_GREATER_OR_EQUAL_UNSIGNED(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_BTYPE(0x63, rs1, imm, rs2, 0x7); }

// Load instructions (I-type)
static MRBC_INLINE uint32_t RISCV_LOAD_HALF_UNSIGNED(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(3, rd, rs1, imm, 5); }
static MRBC_INLINE uint32_t RISCV_LOAD_BYTE_UNSIGNED(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(3, rd, rs1, imm, 4); }
static MRBC_INLINE uint32_t RISCV_LOAD_WORD(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(3, rd, rs1, imm, 2); }
static MRBC_INLINE uint32_t RISCV_LOAD_HALF(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(3, rd, rs1, imm, 1); }
static MRBC_INLINE uint32_t RISCV_LOAD_BYTE(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(3, rd, rs1, imm, 0); }

// Store instructions (S-type)
static MRBC_INLINE uint32_t RISCV_STORE_WORD(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_STYPE(35, rs1, imm, rs2, 2); }
static MRBC_INLINE uint32_t RISCV_STORE_HALF(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_STYPE(35, rs1, imm, rs2, 1); }
static MRBC_INLINE uint32_t RISCV_STORE_BYTE(uint32_t rs1, uint32_t rs2, uint32_t imm) { return RISCV_STYPE(35, rs1, imm, rs2, 0); }

// Jump instructions (J-type and I-type)
static MRBC_INLINE uint32_t RISCV_JUMP_AND_LINK(uint32_t rd, uint32_t imm) { return RISCV_JTYPE(0x6F, rd, imm); }
static MRBC_INLINE uint32_t RISCV_JUMP_AND_LINK_REG(uint32_t rd, uint32_t rs1, uint32_t imm) { return RISCV_ITYPE(0x67, rd, rs1, imm, 0x00); }

// U-type instructions
static MRBC_INLINE uint32_t RISCV_LOAD_UPPER_IMM(uint32_t rd, uint32_t imm) { return RISCV_UTYPE(0x37, rd, imm); }
static MRBC_INLINE uint32_t RISCV_ADD_UPPER_IMM_PC(uint32_t rd, uint32_t imm) { return RISCV_UTYPE(0x17, rd, imm); }

// Multiplication and Division instructions (R-type with funct7 = 0x1)
static MRBC_INLINE uint32_t RISCV_MUL(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x0, 0x1); }
static MRBC_INLINE uint32_t RISCV_DIV(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x4, 0x1); }
static MRBC_INLINE uint32_t RISCV_DIVU(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x5, 0x1); }
static MRBC_INLINE uint32_t RISCV_REM(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x6, 0x1); }
static MRBC_INLINE uint32_t RISCV_REMU(uint32_t rd, uint32_t rs1, uint32_t rs2) { return RISCV_RTYPE(0x33, rd, rs1, rs2, 0x7, 0x1); } // Corrected REMU funct3 to 0x7

// Compressed Instruction Set (C-extension)
static MRBC_INLINE uint16_t RISCV_CRTYPE(uint32_t opcode, uint32_t rs1, uint32_t rs2, uint32_t funct4) {
    return (uint16_t)(opcode + (funct4 << 12) + (rs1 << 7) + (rs2 << 2));
}
static MRBC_INLINE uint16_t RISCV_CITYPE(uint32_t opcode, uint32_t rd, uint32_t imm, uint32_t funct3) {
    return (uint16_t)(opcode + (funct3 << 13) + ((imm & 0x20) << (12 - 5)) + (rd << 7) + ((imm & 0x1F) << 2));
}
// #define RISCV_CSSTYPE(opcode, rs2, imm, funct3) !Different
static MRBC_INLINE uint16_t RISCV_CIWTYPE(uint32_t opcode, uint32_t rd, uint32_t imm, uint32_t funct3) {
    return (uint16_t)(opcode + (funct3 << 13) + (((imm >> 4) & 0x3) << 11) + (((imm >> 6) & 0xF) << 7) + (((imm >> 2) & 1) << 6) + (((imm >> 3) & 1) << 5) + (rd << 2));
}
//#define RISCV_CLTYPE(opcode, rs1, rd, imm, funct3) !Different
//#define RISCV_CSTYPE(opcode, rs1, rd, imm, funct3) !Different
static MRBC_INLINE uint16_t RISCV_CATYPE(uint32_t opcode, uint32_t rs1, uint32_t rs2, uint32_t funct2, uint32_t funct6) {
    return (uint16_t)(opcode + (funct6 << 10) + (funct2 << 5) + ((rs1 & 0x7) << 7) + ((rs2 & 0x7) << 2));
}
static MRBC_INLINE uint16_t RISCV_CBTYPE(uint32_t opcode, uint32_t rs1, uint32_t imm, uint32_t funct3) {
    return (uint16_t)(opcode + (funct3 << 13) + (((imm >> 8) & 1) << 12) + ((imm & 0x18) << 7) + ((rs1 & 0x7) << 7) + ((imm & 0xC0) >> 1) + ((imm & 0x6) << 2) + ((imm & 0x20) >> 3));
}
static MRBC_INLINE uint16_t RISCV_CBSTARTYPE(uint32_t opcode, uint32_t rs1, uint32_t imm, uint32_t funct3, uint32_t funct2) {
    return (uint16_t)(opcode + (funct3 << 13) + (((imm >> 5) & 1) << 12) + (funct2 << 10) + ((rs1 & 0x7) << 7) + ((imm & 0x1F) << 2));
}
static MRBC_INLINE uint16_t RISCV_CJTYPE(uint32_t opcode, uint32_t imm, uint32_t funct3) {
    return (uint16_t)(opcode + (funct3 << 13) + (((imm >> 11) & 1) << 12) + (((imm >> 4) & 1) << 11) + (((imm >> 8) & 0x3) << 9) + (((imm >> 10) & 1) << 8) + (((imm >> 6) & 1) << 7) + (((imm >> 7) & 1) << 6) + (((imm >> 1) & 0x7) << 3) + (((imm >> 5) & 1) << 2));
}
static uint16_t RISCV_C_SWSP(uint32_t rs2, uint32_t imm) {
    return (uint16_t)(0x2 + (0x6 << 13) + (((imm >> 2) & 0xF) << 9) + (((imm >> 6) & 0x3) << 7) + (rs2 << 2));
}
static uint16_t RISCV_C_LWSP(uint32_t rd, uint32_t imm) {
    return (uint16_t)(0x2 + (0x2 << 13) + (((imm >> 5) & 1) << 12) + (rd << 7) + (((imm >> 2) & 0x7) << 4) + (((imm >> 6) & 0x3) << 2));
}

static MRBC_INLINE uint16_t RISCV_C_JMP(uint32_t imm) { return RISCV_CJTYPE(1, imm, 5); }
static MRBC_INLINE uint16_t RISCV_C_MOVE(uint32_t rd, uint32_t rs1) { return RISCV_CRTYPE(2, rd, rs1, 8); }
static MRBC_INLINE uint16_t RISCV_C_ADD(uint32_t rd, uint32_t rs1) { return RISCV_CRTYPE(2, rd, rs1, 9); }
static MRBC_INLINE uint16_t RISCV_C_AND(uint32_t rd, uint32_t rs2) { return RISCV_CATYPE(1, rd, rs2, 3, 0x23); }
static MRBC_INLINE uint16_t RISCV_C_AND_IMM(uint32_t rd, uint32_t imm) { return RISCV_CBSTARTYPE(1, rd, imm, 4, 2); }
static MRBC_INLINE uint16_t RISCV_C_OR(uint32_t rd, uint32_t rs2) { return RISCV_CATYPE(1, rd, rs2, 2, 0x23); }
static MRBC_INLINE uint16_t RISCV_C_XOR(uint32_t rd, uint32_t rs2) { return RISCV_CATYPE(1, rd, rs2, 1, 0x23); }
static MRBC_INLINE uint16_t RISCV_C_SUB(uint32_t rd, uint32_t rs2) { return RISCV_CATYPE(1, rd, rs2, 0, 0x23); }
static MRBC_INLINE uint16_t RISCV_C_JR(uint32_t rs1) { return RISCV_CRTYPE(2, rs1, 0, 8); }
static MRBC_INLINE uint16_t RISCV_C_ADDI(uint32_t rd, uint32_t imm) { return RISCV_CITYPE(1, rd, imm, 0); }
static MRBC_INLINE uint16_t RISCV_C_SHIFT_LEFT_IMM(uint32_t rd, uint32_t imm) { return RISCV_CITYPE(2, rd, imm, 0); }
static MRBC_INLINE uint16_t RISCV_C_SHIFT_RIGHT_LOGICAL_IMM(uint32_t rd, uint32_t imm) { return RISCV_CBSTARTYPE(1, rd, imm, 4, 0); }
static MRBC_INLINE uint16_t RISCV_C_SHIFT_RIGHT_ARITHMETIC_IMM(uint32_t rd, uint32_t imm) { return RISCV_CBSTARTYPE(1, rd, imm, 4, 1); }

static MRBC_INLINE uint16_t RISCV_C_LI(uint32_t rd, uint32_t imm) { return RISCV_CITYPE(1, rd, imm, 2); }
// Only imm[17:12] is acceptable.
static MRBC_INLINE uint16_t RISCV_C_LUI(uint32_t rd, uint32_t imm) { return RISCV_CITYPE(1, rd, imm, 3); }
static MRBC_INLINE uint16_t RISCV_C_BEQZ(uint32_t rs1, uint32_t offset) { return RISCV_CBTYPE(1, rs1, offset, 6); }
static MRBC_INLINE uint16_t RISCV_C_BNEZ(uint32_t rs1, uint32_t offset) { return RISCV_CBTYPE(1, rs1, offset, 7); }

static uint16_t RISCV_C_ADDI16SP(uint32_t imm) {
    return (0x1 + (0x3 << 13) + (((imm >> 9) & 1) << 12) + (0x2 << 7) + (((imm >> 4) & 1) << 6) + (((imm >> 6) & 1) << 5) + (((imm >> 7) & 0x3) << 3) + (((imm >> 5) & 0x1) << 2));
}

#define RISCV_ARGS_REGISTER(r) ((r) + 10)
#define RISCV_CALLEE_SAVED_REGISTER(r) ((r) + 18)

#define RISCV_CALLEE_SAVE_REGISTERS 0x0FFC0304
#define RISCV_CALLER_SAVE_REGISTERS 0xF003FCE2
#define RISCV_NORMAL_CALLER_SAVE_REGISTERS 0xF003FCE0 // EXCLUDES RA (Return Address) Register.

#define RISCV_THESE_REGISTERS_ARE_COMPRESSED_AVAILABLE(r1, r2) (((r1) & (r2) & 0x8) == 0x8)

#undef MRBC_INLINE

#ifdef _MSC_VER
  #pragma warning(pop)
#elif defined(__clang__)
  #pragma clang diagnostic pop
#elif defined(__GNUC__)
  #pragma GCC diagnostic pop
#endif