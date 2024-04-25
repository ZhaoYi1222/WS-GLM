#define SIMD_SIZE_I 16
#define SIMD_SIZE_F 8
#define SIMD_SIZE_H 32
#define LDM_CAPACITY 64000 // 250KB 64000 float
#define HALF_CAPACITY 128000
#define SLAVE_NUM 64

typedef struct Param_{
    float * x;
    float * y;

    int n_neighbor;
    int vector_len;
    int is_half;

} Param;








