#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "trans.h"
#include <crts.h>
int main(){
    CRTS_init();
    float *Input, *Output;
    short *Outputf;
    int size = 1024;
    Input = (float *)malloc(size * sizeof(float));
    Output = (float *)malloc(size * sizeof(float));
    //Outputf = (short *)malloc(size * sizeof(short));
    Outputf = (short *)aligned_alloc(256, size * sizeof(short));
    for (size_t i=0; i<size;++i){
        Input[i] = (rand()%1000)/1000.0;
    }
    sw_float_trans_half(Input, Outputf, size);
    sw_half_trans_float(Outputf, Output, size);
    printf("%lf vs %lf\n", Input[1], Output[1]);

    free(Input);
    free(Output);
    free(Outputf);
    return 0;
}    
