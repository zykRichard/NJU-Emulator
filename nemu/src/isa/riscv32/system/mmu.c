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

#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

#define PAGESIZE (1 << 12)
#define PTESIZE 4

int isa_mmu_check(vaddr_t vaddr, int len, int type) {
  int mode = cpu.satp.sig.MODE;
  switch(mode) {
    case Bare : Log("mmu direct");return MMU_DIRECT;
    case Sv32 : Log("mmu translate");return MMU_TRANSLATE;
    default : panic("mode error");return MMU_FAIL;
  }
  return -1;
}



paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  word_t vpn1 = vaddr >> 22;
  word_t vpn0 = (vaddr >> 12) & 0x3ff;
  word_t vpn_off = vaddr & 0xfff;

  // PDE : satp.PNN * PGSIZE + vpn1 * PTESZ
  paddr_t PDE_addr = cpu.satp.sig.PPN * PAGESIZE + vpn1 * PTESIZE;
  word_t PDE = paddr_read(PDE_addr, 4);
  if(!(PDE & 0x1))
    panic("PDE invalid");

  // PTE : PDE.PPN * PGSIZE + vpn0 * PTESZ
  paddr_t PTE_addr = (PDE >> 12) * PAGESIZE + vpn0 * PTESIZE;
  word_t PTE = paddr_read(PTE_addr, 4);
  if(!(PTE & 0x1))
    panic("PTE invalid");
  
  // paddr : PTE.PPN * PGSIZE + vpn_off
  paddr_t ret = (PTE >> 12) * PAGESIZE + vpn_off;
  return ret;
}
