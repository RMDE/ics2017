#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#define MAX_LEN 32
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[MAX_LEN];
  uint32_t new_val;
  uint32_t old_val;
} WP;
#endif
