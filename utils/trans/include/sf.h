#include <simd.h>
#define v_shuffle_w(vd, va, vb, vc)\
({\
  asm volatile("vshfw %1, %2, %3, %0\n"\
    :"=&f"(*vd)\
    :"f"(va), "f"(vb), "f"(vc)\
  );\
  0;\
})

static __inline doublev8
simd_vshuffled(doublev8 va, doublev8 vb, uint512 vc){
  doublev8 tmp;
  v_shuffle_w(&tmp, va, vb, vc);
  return tmp;
}
static __inline floatv8
simd_vshuffles(floatv8 va, floatv8 vb, uint512 vc){
	floatv8 tmp;
	v_shuffle_w(&tmp, va, vb, vc);
	return tmp;
}

