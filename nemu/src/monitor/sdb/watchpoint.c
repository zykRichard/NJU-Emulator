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

#include "sdb.h"

#define NR_WP 32
bool global_flag = false;
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char wexpr[1024];
  word_t val;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp() {
  if(free_ == NULL) {
    Log("No availabel watiching point");
    assert(0);
  }
  WP *new = free_;
  free_ = free_ -> next;
  new -> next = head;
  head = new;
  return new;
}

void free_wp(WP *wp) {
  assert(wp);
  if(head == NULL) {
    Log("No using wp");
    return ;
  }
  if(wp == head) {
    head = head -> next;
    wp -> next = free_;
    free_ = wp;
    return ;
  }
  else {
    WP *find_ = head;
    while(find_ -> next != wp){
      find_ = find_ -> next;
      if(find_ -> next == NULL) {
        Log("No such watchpoint!");
        return ;
      }
    }
    find_ -> next = wp -> next;
    wp -> next = free_;
    free_ = wp;
    return ;
  }
}


bool wp_difftest() {
  WP *pr = head;
  if(pr == NULL) return 0;
  while(pr != NULL) {
    int temp = expr(pr -> wexpr, &global_flag);
    if(temp != pr -> val) {
      Log("Value changed at Warching Point %d", pr -> NO);
      Log("Old Value  :  0x%x", pr -> val);
      Log("New Value  :  0x%x", temp);
      pr -> val = temp;
      return true;
    }
    else pr = pr -> next;
  }
  return 0;
}

void wp_display() {
  if(head == NULL) {
    Log("No Watchingpoints");
    return ;
  }
  WP *pr = head;
  while(pr) {
    printf("WP NO      wexpr       val\n");
    printf("%d         %s          0x%x\n", pr -> NO, pr -> wexpr, pr -> val);
    pr = pr -> next;
  }

  return ;
}

void add_wp(char *arg_expr) {
  WP *wp = new_wp();
  word_t val = expr(arg_expr, &global_flag);
  strcpy(wp -> wexpr, arg_expr);
  wp -> val = val;
  Log("New WP %d has been set with expr %s, initial value: 0x%x", wp -> NO, wp -> wexpr, wp -> val);
  return ;
}

void del_wp(int idx) {
  assert(idx >= 0 && idx < NR_WP);
  WP* cur = &wp_pool[idx];
  free_wp(cur);
  Log("The WP %d has been moved", idx);
  return;
}