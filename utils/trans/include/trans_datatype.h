#include <stdio.h>
#include <math.h>
#include <string.h>
#include <simd.h>
#include "sf.h"

void simd_trans_a(int num, float *a_slave, _Float16 *a_slaveH);
void simd_trans_hf_sf(int num, _Float16 *c_slaveH, float *c_slave);
float simd_trans_hf_sf_add(int num, _Float16 *c_slaveH);
void simd_trans_hf_sf_v2v(float16v32 *vc_slaveH, floatv8 *vc_slave);