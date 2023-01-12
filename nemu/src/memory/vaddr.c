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

// int isa_mmu_check(vaddr_t vaddr, int len, int type);
// paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type);

word_t vaddr_ifetch(vaddr_t addr, int len) {
  int mode = isa_mmu_check(addr, len, 0);
  switch(mode) {
    case MMU_DIRECT : return paddr_read(addr, len);
    case MMU_TRANSLATE : return paddr_read(isa_mmu_translate(addr, len, 0), len);
    default : panic("invalid mode");
  }
  return -1;
}

word_t vaddr_read(vaddr_t addr, int len) {
  int mode = isa_mmu_check(addr, len, 0);
  switch(mode) {
    case MMU_DIRECT : return paddr_read(addr, len);
    case MMU_TRANSLATE : return paddr_read(isa_mmu_translate(addr, len, 0), len);
    default : panic("invalid mode");
  }
  return -1;
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  int mode = isa_mmu_check(addr, len, 0);
  switch(mode) {
    case MMU_DIRECT : paddr_write(addr, len, data); break;
    case MMU_TRANSLATE : paddr_write(isa_mmu_translate(addr, len, 0), len, data); break;
    default : panic("invalid mode");
  }
  
}
