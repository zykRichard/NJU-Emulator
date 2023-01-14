#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)
#define USER 0
#define KERNEL 1

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE   ((32 + 5 + 1) * XLEN)
#define OFFSET_SP      ( 2 * XLEN)
#define OFFSET_CAUSE   (32 * XLEN)
#define OFFSET_STATUS  (33 * XLEN)
#define OFFSET_EPC     (34 * XLEN)
#define OFFSET_SCRATCH (35 * XLEN)
#define OFFSET_NP      (36 * XLEN)


.align 3
.globl __am_asm_trap
__am_asm_trap:
  # stack switch:
  csrrw sp, mscratch, sp   // atomocally exchange sp && mscratch
  bnez sp, save_context    // if ksp != 0, save context
  csrr sp, mscratch        // else restore
  

save_context: 
  addi sp, sp, -CONTEXT_SIZE

  MAP(REGS, PUSH)

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc
  csrr t3, mscratch # old sp

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
  STORE t3, OFFSET_SP(sp)

  # set mstatus.MPRV to pass difftest
  xor t1, a0, a0
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1

  mv a0, sp

  # c -> np:
  addi t0, sp, CONTEXT_SIZE
  csrr t1, mscratch
  xor t0, t0, t1
  bnez t0, NP_U
  xor t2, t0, t0
  addi t2, t2, KERNEL
  STORE t2, OFFSET_NP(sp)
  j HANDLER

NP_U:
  xor t2, t0, t0
  addi t2, t2, USER
  STORE t2, OFFSET_NP(sp)

HANDLER:
  xor t2, t0, t0
  csrw mscratch, t2
  jal __am_irq_handle
  
  # set sp as new context
  mv sp, a0
  
  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  LOAD t3, OFFSET_SCRATCH(sp)
  csrw mstatus, t1
  csrw mepc, t2
  csrw mscratch, t3

  MAP(REGS, POP)

  LOAD sp, OFFSET_SP(sp)
  mret
