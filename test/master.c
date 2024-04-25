#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crts.h>
#include <sys/time.h>
#include <athread.h>
#include <simd.h>
#include <math.h>
#include "param.h"

extern void SLAVE_FUN(test_float());


int main(int argc, char **argv)
{
    Param * param = (Param*)malloc(sizeof(Param));
    int i,j,k;
    param->is_half = atoi(argv[1]);
    param->x = (float*)malloc(600*sizeof(float));
    param->y = (float*)malloc(600*sizeof(float));

    srand((unsigned)time(NULL));
    // rand x rand y
    // rand()/(float)(RAND_MAX)
    for(i=0;i<600;++i)
    {
        (param->x)[i] = rand()/(float)(RAND_MAX);
        (param->y)[i] = rand()/(float)(RAND_MAX);
    }

    athread_init();
    for(i=1;i<=1;++i)
    {
        param->n_neighbor = i;
        if(param->is_half==0)
        {
            if(param->n_neighbor==1) param->vector_len = 112;
            else if(param->n_neighbor==2) param->vector_len = 304;
            else param->vector_len = 592;
            athread_spawn(test_float, param);
            athread_join();
        }
    }
    athread_halt();
    return 0;
}



