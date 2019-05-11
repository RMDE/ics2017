#include "cpu/exec.h"

make_EHelper(real);

make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  printf("1");
  exec_real(eip);
  printf("2");
  decoding.is_operand_size_16 = false;
	printf("this is in operand_size func\n");
}
