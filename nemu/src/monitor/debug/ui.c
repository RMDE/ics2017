#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

uint32_t expr(char *e,bool *sucess,bool *flag);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","Single step execution",cmd_si },
  {"info","Print Register",cmd_info},
  {"x","Scan Memory",cmd_x},
  {"p","Evaluate the value",cmd_p}

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_si(char *args){
	//using function "strtok" to read N
	char *arg = strtok(NULL," ");
	int step=0;
	if(arg == NULL)
		step = 1;
	else if(strcmp(arg,"-1")==0)
		step = -1;
	else if(arg)
	{
		for(int i=0;i<strlen(arg);i++)
		{
			if(arg[i]<'0'||arg[i]>'9')
			{
				printf("Unknown command '%s'\n", arg);
				return 0;
			}
			step=step*10+(arg[i]-'0');
		}
	}
	//printf("%d\n",step);
	cpu_exec(step);
	return 0;
}

static int cmd_info(char *args){
	char *arg=strtok(NULL," ");
	if(arg==NULL)
	{
		printf("Unknown command\n");
		return 0;
	}
	else if(strcmp(arg,"r")==0)
	{
		for(int i=0;i<8;i++)
			printf("%s:\t0x%08x\t%d\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
		printf("eip:\t0x%08x\t%d\n",cpu.eip,cpu.eip);
	}
	else
	{
		printf("Unknown command '%s'\n",arg);
		return 0;
	}
	return 0;
}

static int cmd_x(char *args){
	char *number=strtok(NULL," ");
	char n=number[0];
	int len=n-'0';
	//printf("%d\n",len);
	char *arg=strtok(NULL," ");
	printf("%s\n",arg);
	//strtok(arg,"x");
	//arg=strtok(NULL," ");
	unsigned int addr=0;
	bool* success=(bool*)malloc(sizeof(bool));
	*success=true;
	printf("start ot expr");
	addr=expr(arg,success,success);
	//for(int i=0;i<strlen(arg);i++)
	//	addr=addr*16+(arg[i]-'0');
	//printf("%u\n",addr);
	printf("Address         Dword block    Byte sequence\n");
	for(int i=0;i<len;i++)
	{
		unsigned int val=vaddr_read(addr,4);
		printf("0x%08x\t0x%08x\t",addr,val);
		for(int j=0;j<4;j++)
		{
			val=vaddr_read(addr,1);
			printf("%02x ",val);
			addr+=1;
		}
		printf("\n");
	}
	return 0;
}

static int cmd_p(char *args){
	bool *success,*flag;
	int res;	
	char *expre=args;
	success=(bool*)malloc(sizeof(bool));
	flag=(bool*)malloc(sizeof(bool));
	*success=true;
	if(expre==NULL)
	{
		printf("Commond not find\n");
		return 0;
	}
	res=expr(expre,success,flag);
	if(!*success)
	{
		printf("The expression input is not legal\n");
	}
	else
	{
		if(*flag)
			printf("%d\n",res);
		else
			printf("0x%08x\n",res);
	}
	return 0;
}


static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
