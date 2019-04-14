#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp()
{
	WP *p;
	p=free_;
	if(!p)//无空闲链表节点
	{
		printf("No more space!\n");
		return NULL;
	}
	free_=free_->next; //新增监视点信息位于开头
	p->next=head;
	if(head)
		p->NO=head->NO+1; //编号
	else
		p->NO=0;
	head=p;
	for(int i=0;i<MAX_LEN;i++) //清空原存储表达式
		head->expr[i]='\0';
	return head;
}

void free_wp(int number)
{
	printf("in free\n");
	if(!head)  //当已设监视点数为0时
	{
		printf("No settled watchpoint\n");
		return;
	}
	printf("head exit\n");
	WP *wp=head;
	if(number==head->NO) //特定监视点恰好位于第一个
	{
		head=head->next;
		wp->next=free_;
		free_=wp;
		printf("Watchpoint %d deleted",head->NO);
		return;
	}
	WP *p=head;
	printf("scan");
	while(p->next)//遍历寻找特定监视点所在的节点
	{
		if(p->next->NO==number)
		{
			wp=p->next;
			p->next=wp->next;
			wp->next=free_;
			free_=wp;
			printf("Watchpoint %d deleted",p->next->NO);
			return;
		}
		p=p->next;
	}
	printf("There isn't the watchpoint\n");
	return;
}

void print_wp(WP* head)
{
	if(head->next)
		print_wp(head->next);
	printf("%d\t%s\t%08x\n",head->NO,head->expr,head->old_val);
}
bool list_watchpoint()
{
	if(!head) //当没有设监视点时
		return false;
	printf("NO\tExpr\tOld Value\n");
	print_wp(head);
	return true;
}
