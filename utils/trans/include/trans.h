
typedef struct float_trans_half {
    const float *A;
    short *HA;
    unsigned long num;
} float_trans_half;

typedef struct half_trans_float {
    const short *HA;
    float *A;
    unsigned long num;
} half_trans_float;

void sw_float_trans_half(const float *A,short *HA,unsigned long num);

void sw_half_trans_float(const short *HB,float *B,unsigned long num);
