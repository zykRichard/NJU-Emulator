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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,            // =
  DEC_NUM,          // decimal number
  HEX_NUM,          // hexadecimal number
  NEQ,              // !=
  LOGIC_AND,        // &&
  BIT_AND,          // &
  NEG,              // -
  REG,              // register
  DEREF             // dereference
  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},                      // spaces
    {"[0-9]+", DEC_NUM},                    // decimal number
    {"0x[0-9, a-f]+", HEX_NUM},             // hexadecimal number
    {"\\+", '+'},                           // plus
    {"\\-", '-'},                           // minus
    {"\\*", '*'},                           // multiply
    {"\\/", '/'},                           // divide
    {"\\(", '('},                           // left parenthese
    {"\\)", ')'},                           // right parenthese
    {"==", TK_EQ},                          // equal
    {"!=", NEQ},                            // not equal
    {"&&", LOGIC_AND},                      // logical and
    {"\\&", BIT_AND},                       // bit and
    {"\\$[\\$,a-z][a-z,0-9]+", REG},        // register

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;
        
        // record tokens:

        if(rules[i].token_type != TK_NOTYPE) {
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          nr_token++;          
        }

        // switch (rules[i].token_type)
        // {
        // default:
        //   TODO();
        // }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  // operations on unary operator:
  
  for (int i = 0; i < nr_token; i++) {
    // NEG
    if(tokens[i].type == '-') {
      if(i == 0) tokens[i].type = NEG;

      else if(tokens[i-1].type != DEC_NUM &&
              tokens[i-1].type != HEX_NUM &&
              tokens[i-1].type != ')'     &&
              tokens[i-1].type != REG)

              tokens[i].type = NEG;
    }
    // DEREF 
    if(tokens[i].type == '*') {
      if(i == 0) tokens[i].type = DEREF;

      else if(tokens[i-1].type != DEC_NUM &&
              tokens[i-1].type != HEX_NUM &&
              tokens[i-1].type != ')'     &&
              tokens[i-1].type != REG)

              tokens[i].type = DEREF;
    }
  }



  return true;
}

/************************************************************************************/
/* 
check if expr is surrounded by parenthese
as well as judge the validation by bracket matching 
*/
static bool check_parenthese(int p, int q) {

  if(tokens[p].type != '(' || tokens[q].type != ')') {
    Log("dismatch parenthese at pos %d - %d", p, q);
    return false;    
  }

  if(p + 1 >= q) {
    Log("Invalid range of parenthese");
    return false;
  }

  // bracket matching judging:
  int cnt = 0;
  for(int pr = p; pr <= q; pr ++){
    if(cnt < 0) {
      Log("Invalid parenthese match at %d", pr)
      return false;
    }

    if(tokens[pr].type == "(") cnt ++;
    else if(tokens[pr].type == ")") cnt --;

  }
  if(cnt == 0) return true;
  else return false;
}

/************************************************************************************/

int find_op(int p, int q) {
  int idx = p;
  int flag = 0;
  while(idx <= q) {
    if(tokens[idx].type == '(') { flag ++; }
    else if(tokens[idx].type == ')') flag --;
  }
}


word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
