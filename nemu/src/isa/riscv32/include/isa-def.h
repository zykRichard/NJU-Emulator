/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  word_t gpr[32];
  /* CSR */


  //word_t csregs[4];
  /* wrapper */

  // word_t mstatus;// 0x300
  union {
    struct {
      uint32_t UIE : 1;
      uint32_t SIE : 1;
      uint32_t rev0 : 1;
      uint32_t MIE : 1;
      uint32_t UPIE : 1;
      uint32_t SPIE : 1;
      uint32_t rev1 : 1;
      uint32_t MPIE : 1;
      uint32_t SPP : 1;
      uint32_t rev2 : 2;
      uint32_t MPP_LOW : 1;
      uint32_t MPP_HIGH : 1;
      uint32_t FS_LOW : 1;
      uint32_t FS_HIGH : 1;
      uint32_t XS_LOW : 1;
      uint32_t XS_HIGH : 1;
      uint32_t MPRV : 1;
      uint32_t SUM : 1;
      uint32_t MXR : 1;
      uint32_t TVM : 1;
      uint32_t TW : 1;
      uint32_t TSR : 1;
      uint32_t rev3 : 8;
      uint32_t SD : 1;
    } sig;
    int32_t val;
  } mstatus;

  // word_t mtvec;  // 0x305
  union {
    struct {
      uint32_t MODE : 2;
      uint32_t rev : 30;
    } sig;
    int32_t val;
  } mtvec;
  
  // word_t mepc;   // 0x341
  int32_t mepc;
  
  // word_t mcause; // 0x342
  union {
    struct {
      uint32_t EXCODE : 31;
      uint32_t INT_SIG : 1;
    } sig;
    int32_t val;
  } mcause;

  // word_t satp; // 0x180
  union {
    struct {
      uint32_t PPN : 22;
      uint32_t ASID : 9;
      uint32_t MODE : 1;
    } sig;
    int32_t val;
  } satp;

  vaddr_t pc;

  // time interrupt
  bool INTR;

} riscv32_CPU_state;

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv32_ISADecodeInfo;

#define Bare 0
#define Sv32 1
int isa_mmu_check(vaddr_t vaddr, int len, int type);

#endif
