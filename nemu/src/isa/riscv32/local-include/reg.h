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

#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include <common.h>

static inline int check_reg_idx(int idx) {
  IFDEF(CONFIG_RT_CHECK, assert(idx >= 0 && idx < 32));
  return idx;
}



// static inline int* csr_reg_chosen(int id) {
//   extern CPU_state cpu;
//   switch (id)
//   {
//   case 0x300:
//     return &cpu.mstatus.val;
//     break;
  
//   case 0x305:
//     return &cpu.mtvec.val;
//     break;

//   case 0x341:
//     return &cpu.mepc;
//     break;
  
//   case 0x342:
//     return &cpu.mcause.val;
//     break;

//   case 0x180:
//     return &cpu.satp.val;
//   default:
//     panic("NO SUCH CSR !");
//     break;
//   }
// } 

#define gpr(idx) cpu.gpr[check_reg_idx(idx)]
 


static inline const char* reg_name(int idx, int width) {
  extern const char* regs[];
  return regs[check_reg_idx(idx)];
}

#endif
