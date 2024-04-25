#include <stdio.h>
#include <stdlib.h>
#include <crts.h>
#include "trans.h"
extern SLAVE_FUN(sw_slave_float_trans_half)();
extern SLAVE_FUN(sw_slave_half_trans_float)();
void sw_float_trans_half(const float *A,short *HA,unsigned long num)
{

        float_trans_half para;
        para.A=A;
        para.HA=HA;
        para.num=num;

        athread_spawn(sw_slave_float_trans_half,&para);

        athread_join();

}
void sw_half_trans_float(const short *HB,float *B,unsigned long num)
{
        half_trans_float para;
        para.HA=HB;
        para.A=B;
        para.num=num;

        athread_spawn(sw_slave_half_trans_float,&para);
        athread_join();

}

