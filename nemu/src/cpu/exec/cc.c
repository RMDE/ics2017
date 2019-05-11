#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:rtl_get_OF(dest);break;
    case CC_B:rtl_get_CF(dest);break;
    case CC_E:rtl_get_ZF(dest);break;
    case CC_BE:rtl_get_CF(&t1);rtl_get_ZF(&t2);rtl_or(dest,&t1,&t2);break;
    case CC_S:rtl_get_SF(dest);break;
	case CC_L:rtl_get_SF(&t1);rtl_get_OF(&t2);*dest=(t1==t2)? 0:1;break;
	case CC_LE:rtl_get_SF(&t1);rtl_get_OF(&t2);*dest=(t1==t2)? 0:1;\
			   rtl_get_ZF(&t0);rtl_and(dest,&t0,dest);break;
	case CC_NO:rtl_get_OF(dest);rtl_not(dest);break;
	case CC_NB:rtl_get_CF(dest);rtl_not(dest);break;
	case CC_NE:rtl_get_ZF(dest);rtl_not(dest);break;
	case CC_NBE:rtl_get_CF(&t1);rtl_get_ZF(&t2);rtl_or(dest,&t1,&t2);rtl_not(dest);break;
	case CC_NS:rtl_get_SF(dest);rtl_not(dest);break;
	case CC_NL:rtl_get_SF(&t1);rtl_get_OF(&t2);*dest=(t1==t2)? 0:1;rtl_not(dest);break;
	case CC_NLE:rtl_get_SF(&t1);rtl_get_OF(&t2);*dest=(t1==t2)? 0:1;\
			    rtl_get_ZF(&t0);rtl_and(dest,&t0,dest);rtl_not(dest);break;	
    default: panic("should not reach here");
    case CC_P: 
	case CC_NP:panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
