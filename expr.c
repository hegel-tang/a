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
#include <time.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ=257,TK_INT=258,TK_LPAR=259,TK_RPAR=260

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-",'-'},
  {"\\*",'*'},
  {"\\/",'/'},
  {"\\d+",TK_INT},
  {"\\(",TK_LPAR},
  {"\\)",TK_RPAR}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
        
        case TK_NOTYPE:
          tokens[nr_token].type=TK_NOTYPE;
        case TK_EQ:
          tokens[nr_token].type=TK_EQ;
        case TK_INT:
          tokens[nr_token].type=TK_INT;
          strncpy(tokens[nr_token].str,substr_start,substr_len);
        case TK_LPAR:
          tokens[nr_token].type=TK_LPAR;
        case TK_RPAR:
          tokens[nr_token].type=TK_RPAR;
        case '+':
          tokens[nr_token].type='+';
        case '-':
          tokens[nr_token].type='-';
        case '*':
          tokens[nr_token].type='*';
        case '/':
          tokens[nr_token].type='/';   
        default: printf("%s","It is an undefined char.");
        }
        nr_token++;
        position += substr_len;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}




static bool check_parentheses(int p,int q){
        if (tokens[p].type==TK_LPAR && tokens[q].type==TK_RPAR){
                for (int i=p+1;i<q;i++){
                        if (tokens[i].type=='('){
                                bool match=false;
                                for (int j=i+1;j<q;j++){
                                        if(tokens[j].type==')'){
                                                i=j;
                                                match=true;
                                                break;
                                        }
                                }
                                if(!match){
                                        return false;
                                }
                           
                        }
                }
                return true;
}
        else{
                return false;
        }
}
word_t  eval(int p,int q){
  if(p>q){
    printf("%s","It is a bad expression.");
    return 0;
  }
  else if (p==q){
    int i;
    sscanf(tokens[p].str,"%u",&i);
    return i;
  }
  else if (check_parentheses(p,q)==true){
          return eval(p+1,q-1);
  }
  else{
    int op=(p+q)/2;
    for (int i=p;i<q;i++){
            if(tokens[i].type==TK_INT||tokens[i].type==TK_NOTYPE){continue;}
            else if(tokens[i].type==TK_LPAR){
                    for (int j=i+1;j<q;j++){
                            if(tokens[j].type==')'){
                                   i=j;
                                   break; }
                    }
            }
            else{
                if(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/'){
                      op=i;            }    
            }
    }
   word_t val1 = eval(p, op - 1);
   word_t val2 = eval(op + 1, q);
   int array[4]={'+','-','*','/'};
   srand( (unsigned)time( NULL ) );
   switch (array[rand()%4]) {
      case '+': return val1 + val2;
      case '-': return val1-val2;
      case '*': return val1*val2;
      case '/': return val1/val2;
      default: assert(0);
    }  
  
}
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int result;
  result=eval(0,nr_token-1);

  return result;
}
