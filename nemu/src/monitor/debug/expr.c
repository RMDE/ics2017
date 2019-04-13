#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,TK_NOT_EQ,TK_LEQ,TK_GEQ,TK_L,TK_G,
  TK_AND,TK_OR,
  TK_NOT,TK_INVERT,
  TK_SHIFT_LEFT,TK_SHIFT_RIGHT,TK_DIV,
  TK_LPAR,TK_RPAR,
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
  {"&&",TK_AND},                          // and
  {"\\|\\|",TK_OR},                           // or
  {"==", TK_EQ},					      // equal
  {"!=",TK_NOT_EQ},                       // not equal
  {"!",TK_NOT},                           // not
  {"~",TK_INVERT},                        // invert
  {"<<",TK_SHIFT_LEFT},                   // left shift
  {">>",TK_SHIFT_RIGHT},                  // right shift
  {"<=",TK_LEQ},                          // less or equal
  {">=",TK_GEQ},                          // greater or equal
  {"<",TK_L},                             // less
  {">",TK_G},                             // greater
  {"\\+", '+'},							  // plus
  {"-", '-'},							  // sub
  {"\\*", '*'},							  // mul
  {"/", TK_DIV},					      // dev
  {"\\(", TK_LPAR},						  // the left parenthese
  {"\\)", TK_RPAR},						  // the right parenthese
  {"0x[0-9a-fA-F]{1,8}", TK_NUMBER_16},    // hexadecimal
  {"[0-9]+", TK_NUMBER_10},				  // decimalism
  {"\\$eax|\\$EAX", TK_EAX},                     // $eax
  {"\\$ebx|\\$EBX", TK_EBX},                     // $ebx
  {"\\$ecx|\\$ECX", TK_ECX},                     // $ecx
  {"\\$edx|\\$EDX", TK_EDX},                     // $edx
  {"\\$esp|\\$EBP", TK_ESP},                     // $esp
  {"\\$ebp|\\$EBP", TK_EBP},                     // $ebp
  {"\\$esi|\\$ESI", TK_ESI},                     // $esi
  {"\\$edi|\\$EDI", TK_EDI},                     // $edi
  {"\\$eip|\\$EIP", TK_EIP}                      // $eip
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
		 int substr_len=pmatch.rm_eo;	
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
	  //  printf("%d:%s %d",nr_token-1,tokens[nr_token-1].str,tokens[nr_token-1].type);
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

bool check_parentheses(int p,int q,bool *success)
{
	int left=0,right=0;
	if(p==0&&q==nr_token-1)
	{
		for(int i=p;i<=q;i++)
		{
			if(tokens[i].type==TK_LPAR)
				left++;
			if(tokens[i].type==TK_RPAR)
				right++;
			if(left<right)
			{
				*success=false;
				return false;
			}
		}
	}
	if(left!=right)
	{
		*success=false;
		return false;
	}
	if(tokens[p].type!=TK_LPAR||tokens[q].type!=TK_RPAR)
		return false;
	else
	{
		int left=0,right=0;
		for(int i=p+1;i<q;i++)
		{
			if(tokens[i].type==TK_LPAR)
				left++;
			if(tokens[i].type==TK_RPAR)
				right++;
			if(left<right)
				return false;
		}
	}
	return true;
}

int find_dominated_op(int p,int q,bool *success)
{
	printf("now in find");
    int level =0;
    int op=-1;
    int type=15;//初始化
    for(int i=p;i<=q;i++)
    {
        if(tokens[i].type==TK_LPAR)
            level++;
        else if(tokens[i].type==TK_RPAR)
            level--;
        if(level==0&&i!=p)
        {
            //判断是否是运算符且是否比之前的运算符优先级低，优先级一致则看顺序
            switch(tokens[i].type)
            {
              case TK_AND:op=type>0? i:op;type=type>0? 0:type;break;
              case TK_OR:op=type>0? i:op;type=type>0? 0:type;break;
              case TK_EQ:op=type>1? i:op;type=type>1? 1:type;break;
              case TK_NOT_EQ:op=type>1? i:op;type=type>1? 1:type;break;
              case TK_LEQ:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_GEQ:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_L:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_G:op=type>2? i:op;type=type>2? 2:type;break;
              case '+':op=type>3? i:op;type=type>3? 3:type;break;
              case '-':op=type>3? i:op;type=type>3? 3:type;break;
              case '*':op=type>4? i:op;type=type>4? 4:type;break;
              case TK_DIV:op=type>4? i:op;type=type>4? 4:type;break;
              case TK_SHIFT_LEFT:op=type>5? i:op;type=type>5? 5:type;break;
              case TK_SHIFT_RIGHT:op=type>5? i:op;type=type>5? 5:type;break;
              case TK_NOT:op=type>6? i:op;type=type>6? 6:type;break;
              case TK_INVERT:op=type>6? i:op;type=type>6? 6:type;break;
            }
        }
		else if(i==p)
		{
			switch(tokens[i].type)
			{
              case TK_AND:op=type>0? i:op;type=type>0? 0:type;break;
              case TK_OR:op=type>0? i:op;type=type>0? 0:type;break;
              case TK_EQ:op=type>1? i:op;type=type>1? 1:type;break;
              case TK_NOT_EQ:op=type>1? i:op;type=type>1? 1:type;break;
              case TK_LEQ:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_GEQ:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_L:op=type>2? i:op;type=type>2? 2:type;break;
              case TK_G:op=type>2? i:op;type=type>2? 2:type;break;
              case '+':op=type>3? i:op;type=type>3? 3:type;break;
              case '-':op=type>6? i:op;type=type>6? 6:type;break;
              case '*':op=type>4? i:op;type=type>4? 4:type;break;
              case TK_DIV:op=type>4? i:op;type=type>4? 4:type;break;
              case TK_SHIFT_LEFT:op=type>5? i:op;type=type>5? 5:type;break;
              case TK_SHIFT_RIGHT:op=type>5? i:op;type=type>5? 5:type;break;
			}
		}
    }
	printf("%d\n",op);
    if(op==-1)
        *success=false;
    return op;
}

uint32_t eval(int p,int q,bool *success)
{
	//printf("now in eval,%d,%d\n",p,q);
	if(!*success)
		return 0;
	if(p>q)
	{
		//Bad expression
		return 0;
	}
	else if(p==q)
	{
		//Single token, must be a number, return the value of the number
		uint32_t val;
		switch(tokens[p].type)
		{
			case TK_NUMBER_16:sscanf(tokens[p].str,"%x",&val);break;
			case TK_NUMBER_10:sscanf(tokens[p].str,"%d",&val);break;
			case TK_EAX:val=cpu.gpr[0]._32;break;
			case TK_ECX:val=cpu.gpr[1]._32;break;
			case TK_EDX:val=cpu.gpr[2]._32;break;
			case TK_EBX:val=cpu.gpr[3]._32;break;
			case TK_ESP:val=cpu.gpr[4]._32;break;
			case TK_EBP:val=cpu.gpr[5]._32;break;
			case TK_ESI:val=cpu.gpr[6]._32;break;
			case TK_EDI:val=cpu.gpr[7]._32;break;
			case TK_EIP:val=cpu.eip;break;
			default:*success=0;return 0;
		}
		return val;
	}
	bool res=check_parentheses(p,q,success);
	if(!*success)
		return 0;
	else if(res)
	{
		//The expression is surrounded by a matched pair of parentheses
		//Throw away the parentheses
		return eval(p+1,q-1,success);
	}
	else
	{
		//将表达式再往下分成两个子表达式
		int mid=find_dominated_op(p,q,success);
		if(!*success)
			return 0;
		uint32_t left=eval(p,mid-1,success);
		if(!*success)
			return 0;
		uint32_t right=eval(mid+1,q,success);
		if(!*success)
			return 0;
		//evaluate the value
		if(p==q-1) //单目运算符
		{
			switch(tokens[mid].type)
			{
				case TK_NOT:return !right;
				case TK_INVERT:return ~right;
				case '-':return -right;
			}
		}						
		else //双目运算符
		{
			switch(tokens[mid].type)
			{
				case TK_AND:return left&&right;
				case TK_OR:return left||right;
				case TK_EQ:return left==right;
				case TK_NOT_EQ:return left!=right;
				case TK_SHIFT_LEFT:return left<<right;
				case TK_SHIFT_RIGHT:return left>>right;
				case TK_LEQ:return left<=right;
				case TK_GEQ:return left>=right;
				case TK_L:return left<right;
				case TK_G:return left>right;
				case '+':return left+right;
				case '-':return left-right;
				case '*':return left*right;
				case TK_DIV:return left/right;
			}
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return eval(0,nr_token-1,success);
}
