#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_DIV,TK_LPAR,TK_RPAR,
  TK_NUMBER_16,TK_NUMBER_10,
  TK_EAX,TK_EBX,TK_ECX,TK_EDX,TK_ESP,TK_EBP,TK_ESI,TK_EDI,TK_EIP

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},					  // spaces
  {"\\+", '+'},							  // plus
  {"==", TK_EQ},					      // equal
  {"-", '-'},							  // sub
  {"\\*", '*'},							  // mul
  {"/", TK_DIV},					      // dev
  {"\\(", TK_LPAR},						  // the left parenthese
  {"\\)", TK_RPAR},						  // the right parenthese
  {"0x[0-9a-fA-F]{1,8}", TK_NUMBER_16},    // hexadecimal
  {"[0-9]+", TK_NUMBER_10},				  // decimalism
  {"\\$eax", TK_EAX},                     // $eax
  {"\\$ebx\\|\\$EBX", TK_EBX},                     // $ebx
  {"\\$ecx\\|\\$ECX", TK_ECX},                     // $ecx
  {"\\$edx\\|\\$EDX", TK_EDX},                     // $edx
  {"\\$esp\\|\\$EBP", TK_ESP},                     // $esp
  {"\\$ebp\\|\\$EBP", TK_EBP},                     // $ebp
  {"\\$esi\\|\\$ESI", TK_ESI},                     // $esi
  {"\\$edi\\|\\$EDI", TK_EDI},                     // $edi
  {"\\$eip\\|\\$EIP", TK_EIP}                      // $eip
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') 
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) 
	{
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) 
	  {
		 char *substr_start = e + position;
         int substr_len = pmatch.rm_eo;

         Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
		 
		 for(int j=0;j<32;++j) //
		 { 
		 	tokens[nr_token].str[j]='\0';//
		 }
		 strncpy(tokens[nr_token].str,e+position,substr_len);  //
         position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) 
		{
			case TK_NOTYPE:break;
			default: tokens[nr_token++].type=rules[i].token_type;
        }
	    //printf("%d:%s %d",nr_token-1,tokens[nr_token-1].str,tokens[nr_token-1].type);
         break;
      }
    }

    if (i == NR_REGEX)
   	{
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
}
