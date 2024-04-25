#include <stdio.h>
#include <simd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <crts.h>
#include <slave.h>
#include "param.h"

void *aligned_malloc(size_t required_bytes, size_t alignment){
	void *p1;
	void **p2;
	int offset = alignment-1+sizeof(void*);
	if((p1 = (void*)ldm_malloc(required_bytes+offset))==NULL){
		return NULL;
	}
	p2 = (void**)(((size_t)(p1)+offset) & ~(alignment-1));
	p2[-1]=p1;
	return p2;
}

void aligned_free(void *p2, size_t required_bytes, size_t alignment){
	void *p1 = ((void**)p2)[-1];
	int offset = alignment - 1 + sizeof(void*);
	ldm_free(p1, required_bytes+offset);
}

inline unsigned long rpcc_slave() {return  athread_stime_cycle();}


/*
inline float euler_dist_304(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	int base;
	int simd_len = len / SIMD_SIZE_F;
	floatv8 s_sum=0.0;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_F*8;
		simd_load(s11, &(x[base]));
		simd_load(s12, &(y[base]));
		d1 = s11 - s12;
		d1 = d1 * d1;
		s_sum_1 += d1;
		simd_load(s21, &(x[base+SIMD_SIZE_F*1]));
		simd_load(s22, &(y[base+SIMD_SIZE_F*1]));
		d2 = s21 - s22;
		d2 = d2 * d2;
		s_sum_2 += d2;
		simd_load(s31, &(x[base+SIMD_SIZE_F*2]));
		simd_load(s32, &(y[base+SIMD_SIZE_F*2]));
		d3 = s31 - s32;
		d3 = d3 * d3;
		s_sum_3 += d3;
		simd_load(s41, &(x[base+SIMD_SIZE_F*3]));
		simd_load(s42, &(y[base+SIMD_SIZE_F*3]));
		d4 = s41 - s42;
		d4 = d4 * d4;
		s_sum_4 += d4;
		simd_load(s51, &(x[base+SIMD_SIZE_F*4]));
		simd_load(s52, &(y[base+SIMD_SIZE_F*4]));
		d5 = s51 - s52;
		d5 = d5 * d5;
		s_sum_5 += d5;
		simd_load(s61, &(x[base+SIMD_SIZE_F*5]));
		simd_load(s62, &(y[base+SIMD_SIZE_F*5]));
		d6 = s61 - s62;
		d6 = d6 * d6;
		s_sum_6 += d6;
		simd_load(s71, &(x[base+SIMD_SIZE_F*6]));
		simd_load(s72, &(y[base+SIMD_SIZE_F*6]));
		d7 = s71 - s72;
		d7 = d7 * d7;
		s_sum_7 += d7;
		simd_load(s81, &(x[base+SIMD_SIZE_F*7]));
		simd_load(s82, &(y[base+SIMD_SIZE_F*7]));
		d8 = s81 - s82;
		d8 = d8 * d8;
		s_sum_8 += d8;
	}
	base = i*SIMD_SIZE_F*8;
	simd_load(s11, &(x[base]));
	simd_load(s12, &(y[base]));
	d1 = s11 - s12;
	d1 = d1 * d1;
	s_sum_1 += d1;
	simd_load(s21, &(x[base+SIMD_SIZE_F*1]));
	simd_load(s22, &(y[base+SIMD_SIZE_F*1]));
	d2 = s21 - s22;
	d2 = d2 * d2;
	s_sum_2 += d2;
	simd_load(s31, &(x[base+SIMD_SIZE_F*2]));
	simd_load(s32, &(y[base+SIMD_SIZE_F*2]));
	d3 = s31 - s32;
	d3 = d3 * d3;
	s_sum_3 += d3;
	simd_load(s41, &(x[base+SIMD_SIZE_F*3]));
	simd_load(s42, &(y[base+SIMD_SIZE_F*3]));
	d4 = s41 - s42;
	d4 = d4 * d4;
	s_sum_4 += d4;
	simd_load(s51, &(x[base+SIMD_SIZE_F*4]));
	simd_load(s52, &(y[base+SIMD_SIZE_F*4]));
	d5 = s51 - s52;
	d5 = d5 * d5;
	s_sum_5 += d5;
	simd_load(s61, &(x[base+SIMD_SIZE_F*5]));
	simd_load(s62, &(y[base+SIMD_SIZE_F*5]));
	d6 = s61 - s62;
	d6 = d6 * d6;
	s_sum_6 += d6;
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

float euler_dist_592(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	int base;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_F;
	floatv8 s_sum=0.0;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	// for(i=0;i<simd_len/8;++i)
	for(i=0;i<200;++i)
	{
		// for debug
		// x_offset_addr = x;
		// y_offset_addr = y;
		//
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);
	
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);
	}
	simd_load(s11, x_offset_addr);
	simd_load(s12, y_offset_addr);
	x_offset_addr += SIMD_SIZE_F;
	y_offset_addr += SIMD_SIZE_F;
	d1 = s11 - s12;
	s_sum_1 = simd_vmas(d1, d1, s_sum_1);

	simd_load(s21, x_offset_addr);
	simd_load(s22, y_offset_addr);
	x_offset_addr += SIMD_SIZE_F;
	y_offset_addr += SIMD_SIZE_F;
	d2 = s21 - s22;
	s_sum_2 = simd_vmas(d2, d2, s_sum_2);

	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

float euler_dist_592_new(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	int base;
	int simd_len = len / SIMD_SIZE_F;
	floatv8 s_sum=0.0;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82;
	floatv8 d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_F*8;
		simd_load(s11, &(x[base]));
		simd_load(s12, &(y[base]));

		d1 = s11 - s12;
		d1 = d1 * d1;
		s_sum_1 += d1;
		// s_sum_1 = simd_vmas(d1, d1, s_sum_1);
		
		simd_load(s21, &(x[base+SIMD_SIZE_F*1]));
		simd_load(s22, &(y[base+SIMD_SIZE_F*1]));

		d2 = s21 - s22;
		d2 = d2 * d2;
		s_sum_2 += d2;
		// s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, &(x[base+SIMD_SIZE_F*2]));
		simd_load(s32, &(y[base+SIMD_SIZE_F*2]));

		d3 = s31 - s32;
		d3 = d3 * d3;
		s_sum_3 += d3;
		// s_sum_3 = simd_vmas(d3, d3, s_sum_3);


		simd_load(s41, &(x[base+SIMD_SIZE_F*3]));
		simd_load(s42, &(y[base+SIMD_SIZE_F*3]));

		d4 = s41 - s42;
		d4 = d4 * d4;
		s_sum_4 += d4;
		// s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, &(x[base+SIMD_SIZE_F*4]));
		simd_load(s52, &(y[base+SIMD_SIZE_F*4]));

		d5 = s51 - s52;
		d5 = d5 * d5;
		s_sum_5 += d5;
		// s_sum_5 = simd_vmas(d5, d5, s_sum_5);

		simd_load(s61, &(x[base+SIMD_SIZE_F*5]));
		simd_load(s62, &(y[base+SIMD_SIZE_F*5]));

		d6 = s61 - s62;
		d6 = d6 * d6;
		s_sum_6 += d6;
		// s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, &(x[base+SIMD_SIZE_F*6]));
		simd_load(s72, &(y[base+SIMD_SIZE_F*6]));

		d7 = s71 - s72;
		d7 = d7 * d7;
		s_sum_7 += d7;
		// s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, &(x[base+SIMD_SIZE_F*7]));
		simd_load(s82, &(y[base+SIMD_SIZE_F*7]));
	

		d8 = s81 - s82;
		d8 = d8 * d8;
		s_sum_8 += d8;
		// s_sum_8 = simd_vmas(d8, d8, s_sum_8);

	}
	base = i*SIMD_SIZE_F*8;

	simd_load(s11, &(x[base]));
	simd_load(s12, &(y[base]));

	d1 = s11 - s12;
	d1 = d1 * d1;
	s_sum_1 += d1;
	// s_sum_1 = simd_vmas(d1, d1, s_sum_1);

	simd_load(s21, &(x[base+SIMD_SIZE_F*1]));
	simd_load(s22, &(y[base+SIMD_SIZE_F*1]));

	d2 = s21 - s22;
	d2 = d2 * d2;
	s_sum_2 += d2;
	// s_sum_2 = simd_vmas(d2, d2, s_sum_2);


	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

float euler_dist_592_asm(float* x, float* y, int len)
{
	int i;
	for(i=0; i<len; i+=64, x+=64, y+=64)
	{
		asm volatile(
			"vlds $50, 0(%0)\n"
			"vlds $51, 32(%0)\n"
			"vlds $52, 64(%0)\n"
			"vlds $53, 96(%0)\n"
			"vlds $54, 128(%0)\n"
			"vlds $55, 160(%0)\n"
			"vlds $56, 192(%0)\n"
			"vlds $57, 224(%0)\n"
			"vlds $40, 0(%1)\n"
			"vlds $41, 32(%1)\n"
			"vlds $42, 64(%1)\n"
			"vlds $43, 96(%1)\n"
			"vlds $44, 128(%1)\n"
			"vlds $45, 160(%1)\n"
			"vlds $46, 192(%1)\n"
			"vlds $47, 224(%1)\n"
			"vsubs $40, $50, $50\n"
			"vsubs $41, $51, $51\n"
			"vsubs $42, $52, $52\n"
			"vsubs $43, $53, $53\n"
			"vsubs $44, $54, $54\n"
			"vsubs $45, $55, $55\n"
			"vsubs $46, $56, $56\n"
			"vsubs $47, $57, $57\n"
			"vmas $50, $50, $40, $40\n"
			"vmas $51, $51, $41, $41\n"
			"vmas $52, $52, $42, $42\n"
			"vmas $53, $53, $43, $43\n"
			"vmas $54, $54, $44, $44\n"
			"vmas $55, $55, $45, $45\n"
			"vmas $56, $56, $46, $46\n"
			"vmas $57, $57, $47, $47\n"
			// "vlds $54, 0(%1)\n"
			// "vlds $55, 32(%1)\n"
			// "vlds $56, 64(%1)\n"
			// "vlds $57, 96(%1)\n"
			// "vlds $40, 0(%1)\n"
			// "vlds $41, 32(%1)\n"
			// "vlds $42, 64(%1)\n"
			// "vlds $43, 96(%1)\n"
			// "vsubs $54, $50, $50\n"
			// "vsubs $55, $51, $51\n"
			// "vsubs $56, $52, $52\n"
			// "vsubs $57, $53, $53\n"
			// "vmas $50, $50, $40, $40\n"
			// "vmas $51, $51, $41, $41\n"
			// "vmas $52, $52, $42, $42\n"
			// "vmas $53, $53, $43, $43\n"
			:
			:"r"(x),"r"(y)
			:"$50","$51","$52","$53","$54","$55","$56","$57",\
			"$40","$41","$42","$43","$44","$45","$46","$47","memory"
		);
	}


}
*/


inline float euler_dist_16_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s91,s92,s101,s102,s111,s112,s121,s122,s131,s132,s141,s142,s151,s152,s161,s162,d9,d10,d11,d12,d13,d14,d15,d16;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	floatv8 s_sum_9 = 0.0, s_sum_10 = 0.0, s_sum_11 = 0.0,s_sum_12 = 0.0,s_sum_13 = 0.0,s_sum_14 = 0.0,s_sum_15 = 0.0,s_sum_16 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);
	
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);

		simd_load(s91, x_offset_addr);
		simd_load(s92, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d9 = s91 - s92;
		s_sum_9 = simd_vmas(d9, d9, s_sum_9);

		simd_load(s101, x_offset_addr);
		simd_load(s102, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d10 = s101 - s102;
		s_sum_10 = simd_vmas(d10, d10, s_sum_10);

		simd_load(s111, x_offset_addr);
		simd_load(s112, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d11 = s111 - s112;
		s_sum_11 = simd_vmas(d11, d11, s_sum_11);

		simd_load(s121, x_offset_addr);
		simd_load(s122, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d12 = s121 - s122;
		s_sum_12 = simd_vmas(d12, d12, s_sum_12);

		simd_load(s131, x_offset_addr);
		simd_load(s132, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d13 = s131 - s132;
		s_sum_13 = simd_vmas(d13, d13, s_sum_13);
	
		simd_load(s141, x_offset_addr);
		simd_load(s142, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d14 = s141 - s142;
		s_sum_14 = simd_vmas(d14, d14, s_sum_14);

		simd_load(s151, x_offset_addr);
		simd_load(s152, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d15 = s151 - s152;
		s_sum_15 = simd_vmas(d15, d15, s_sum_15);

		simd_load(s161, x_offset_addr);
		simd_load(s162, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d16 = s161 - s162;
		s_sum_16 = simd_vmas(d16, d16, s_sum_16);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_9 += s_sum_10;
	s_sum_11 += s_sum_12;
	s_sum_13 += s_sum_14;
	s_sum_15 += s_sum_16;

	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_9 += s_sum_11;
	s_sum_13 += s_sum_15;

	s_sum_1 += s_sum_5;
	s_sum_9 += s_sum_13;
	s_sum_1 += s_sum_9;

	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_16_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s91,s92,s101,s102,s111,s112,s121,s122,s131,s132,s141,s142,s151,s152,s161,s162,d9,d10,d11,d12,d13,d14,d15,d16;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	floatv8 s_sum_9 = 0.0, s_sum_10 = 0.0, s_sum_11 = 0.0,s_sum_12 = 0.0,s_sum_13 = 0.0,s_sum_14 = 0.0,s_sum_15 = 0.0,s_sum_16 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*16;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F);
		simd_load(s22, y+base+SIMD_SIZE_F);
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x+base+SIMD_SIZE_F*2);
		simd_load(s32, y+base+SIMD_SIZE_F*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x+base+SIMD_SIZE_F*3);
		simd_load(s42, y+base+SIMD_SIZE_F*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x+base+SIMD_SIZE_F*4);
		simd_load(s52, y+base+SIMD_SIZE_F*4);
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);

		simd_load(s61, x+base+SIMD_SIZE_F*5);
		simd_load(s62, y+base+SIMD_SIZE_F*5);
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x+base+SIMD_SIZE_F*6);
		simd_load(s72, y+base+SIMD_SIZE_F*6);
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x+base+SIMD_SIZE_F*7);
		simd_load(s82, y+base+SIMD_SIZE_F*7);
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);

		simd_load(s91, x+base+SIMD_SIZE_F*8);
		simd_load(s92, y+base+SIMD_SIZE_F*8);
		d9 = s91 - s92;
		s_sum_9 = simd_vmas(d9, d9, s_sum_9);

		simd_load(s101, x+base+SIMD_SIZE_F*9);
		simd_load(s102, y+base+SIMD_SIZE_F*9);
		d10 = s101 - s102;
		s_sum_10 = simd_vmas(d10, d10, s_sum_10);

		simd_load(s111, x+base+SIMD_SIZE_F*10);
		simd_load(s112, y+base+SIMD_SIZE_F*10);
		d11 = s111 - s112;
		s_sum_11 = simd_vmas(d11, d11, s_sum_11);

		simd_load(s121, x+base+SIMD_SIZE_F*11);
		simd_load(s122, y+base+SIMD_SIZE_F*11);
		d12 = s121 - s122;
		s_sum_12 = simd_vmas(d12, d12, s_sum_12);

		simd_load(s131, x+base+SIMD_SIZE_F*12);
		simd_load(s132, y+base+SIMD_SIZE_F*12);
		d13 = s131 - s132;
		s_sum_13 = simd_vmas(d13, d13, s_sum_13);
	
		simd_load(s141, x+base+SIMD_SIZE_F*13);
		simd_load(s142, y+base+SIMD_SIZE_F*13);
		d14 = s141 - s142;
		s_sum_14 = simd_vmas(d14, d14, s_sum_14);

		simd_load(s151, x+base+SIMD_SIZE_F*14);
		simd_load(s152, y+base+SIMD_SIZE_F*14);
		d15 = s151 - s152;
		s_sum_15 = simd_vmas(d15, d15, s_sum_15);

		simd_load(s161, x+base+SIMD_SIZE_F*15);
		simd_load(s162, y+base+SIMD_SIZE_F*15);
		d16 = s161 - s162;
		s_sum_16 = simd_vmas(d16, d16, s_sum_16);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_9 += s_sum_10;
	s_sum_11 += s_sum_12;
	s_sum_13 += s_sum_14;
	s_sum_15 += s_sum_16;

	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_9 += s_sum_11;
	s_sum_13 += s_sum_15;

	s_sum_1 += s_sum_5;
	s_sum_9 += s_sum_13;
	s_sum_1 += s_sum_9;

	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float euler_dist_12_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s91,s92,s101,s102,s111,s112,s121,s122,d9,d10,d11,d12;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	floatv8 s_sum_9 = 0.0, s_sum_10 = 0.0, s_sum_11 = 0.0,s_sum_12 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);
	
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);

		simd_load(s91, x_offset_addr);
		simd_load(s92, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d9 = s91 - s92;
		s_sum_9 = simd_vmas(d9, d9, s_sum_9);

		simd_load(s101, x_offset_addr);
		simd_load(s102, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d10 = s101 - s102;
		s_sum_10 = simd_vmas(d10, d10, s_sum_10);

		simd_load(s111, x_offset_addr);
		simd_load(s112, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d11 = s111 - s112;
		s_sum_11 = simd_vmas(d11, d11, s_sum_11);

		simd_load(s121, x_offset_addr);
		simd_load(s122, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d12 = s121 - s122;
		s_sum_12 = simd_vmas(d12, d12, s_sum_12);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_9 += s_sum_10;
	s_sum_11 += s_sum_12;

	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_9 += s_sum_11;

	s_sum_1 += s_sum_5;
	s_sum_1 += s_sum_9;

	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_12_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s91,s92,s101,s102,s111,s112,s121,s122,d9,d10,d11,d12;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	floatv8 s_sum_9 = 0.0, s_sum_10 = 0.0, s_sum_11 = 0.0,s_sum_12 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*12;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F);
		simd_load(s22, y+base+SIMD_SIZE_F);
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x+base+SIMD_SIZE_F*2);
		simd_load(s32, y+base+SIMD_SIZE_F*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x+base+SIMD_SIZE_F*3);
		simd_load(s42, y+base+SIMD_SIZE_F*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x+base+SIMD_SIZE_F*4);
		simd_load(s52, y+base+SIMD_SIZE_F*4);
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);

		simd_load(s61, x+base+SIMD_SIZE_F*5);
		simd_load(s62, y+base+SIMD_SIZE_F*5);
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x+base+SIMD_SIZE_F*6);
		simd_load(s72, y+base+SIMD_SIZE_F*6);
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x+base+SIMD_SIZE_F*7);
		simd_load(s82, y+base+SIMD_SIZE_F*7);
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);

		simd_load(s91, x+base+SIMD_SIZE_F*8);
		simd_load(s92, y+base+SIMD_SIZE_F*8);
		d9 = s91 - s92;
		s_sum_9 = simd_vmas(d9, d9, s_sum_9);

		simd_load(s101, x+base+SIMD_SIZE_F*9);
		simd_load(s102, y+base+SIMD_SIZE_F*9);
		d10 = s101 - s102;
		s_sum_10 = simd_vmas(d10, d10, s_sum_10);

		simd_load(s111, x+base+SIMD_SIZE_F*10);
		simd_load(s112, y+base+SIMD_SIZE_F*10);
		d11 = s111 - s112;
		s_sum_11 = simd_vmas(d11, d11, s_sum_11);

		simd_load(s121, x+base+SIMD_SIZE_F*11);
		simd_load(s122, y+base+SIMD_SIZE_F*11);
		d12 = s121 - s122;
		s_sum_12 = simd_vmas(d12, d12, s_sum_12);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_9 += s_sum_10;
	s_sum_11 += s_sum_12;

	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_9 += s_sum_11;

	s_sum_1 += s_sum_5;
	s_sum_1 += s_sum_9;

	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float euler_dist_8_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);
	
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_8_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*8;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F);
		simd_load(s22, y+base+SIMD_SIZE_F);
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x+base+SIMD_SIZE_F*2);
		simd_load(s32, y+base+SIMD_SIZE_F*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x+base+SIMD_SIZE_F*3);
		simd_load(s42, y+base+SIMD_SIZE_F*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);

		simd_load(s51, x+base+SIMD_SIZE_F*4);
		simd_load(s52, y+base+SIMD_SIZE_F*4);
		d5 = s51 - s52;
		s_sum_5 = simd_vmas(d5, d5, s_sum_5);

		simd_load(s61, x+base+SIMD_SIZE_F*5);
		simd_load(s62, y+base+SIMD_SIZE_F*5);
		d6 = s61 - s62;
		s_sum_6 = simd_vmas(d6, d6, s_sum_6);

		simd_load(s71, x+base+SIMD_SIZE_F*6);
		simd_load(s72, y+base+SIMD_SIZE_F*6);
		d7 = s71 - s72;
		s_sum_7 = simd_vmas(d7, d7, s_sum_7);

		simd_load(s81, x+base+SIMD_SIZE_F*7);
		simd_load(s82, y+base+SIMD_SIZE_F*7);
		d8 = s81 - s82;
		s_sum_8 = simd_vmas(d8, d8, s_sum_8);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float euler_dist_4_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,d1,d2,d3,d4;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_1 += s_sum_3;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_4_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,d1,d2,d3,d4;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*4;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F);
		simd_load(s22, y+base+SIMD_SIZE_F);
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);

		simd_load(s31, x+base+SIMD_SIZE_F*2);
		simd_load(s32, y+base+SIMD_SIZE_F*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmas(d3, d3, s_sum_3);

		simd_load(s41, x+base+SIMD_SIZE_F*3);
		simd_load(s42, y+base+SIMD_SIZE_F*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmas(d4, d4, s_sum_4);
	}
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_1 += s_sum_3;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float euler_dist_2_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,s21,s22,d1,d2;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);
	}
	s_sum_1 += s_sum_2;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_2_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,s21,s22,d1,d2;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*2;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F);
		simd_load(s22, y+base+SIMD_SIZE_F);
		d2 = s21 - s22;
		s_sum_2 = simd_vmas(d2, d2, s_sum_2);
	}
	s_sum_1 += s_sum_2;
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float euler_dist_1_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	floatv8 s11,s12,d1;
	floatv8 s_sum_1 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_F;
		y_offset_addr += SIMD_SIZE_F;
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);
	}
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

inline float origin_euler_dist_1_way(float* x, float* y, int simd_group)
{
	float dist = 0.0;
	int i;
	int base;
	floatv8 s11,s12,d1;
	floatv8 s_sum_1 = 0.0;
	for(i=0;i<simd_group;++i)
	{
		base = i*SIMD_SIZE_F*1;
		simd_load(s11, x+base);
		simd_load(s12, y+base);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);
	}
	dist = simd_reduc_pluss(s_sum_1);
	return dist;
}

/*
inline float16 euler_dist_608_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	int base;
	int simd_len = len / SIMD_SIZE_H;
	float16v32 s_sum=0.0;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_H*8;
		simd_load(s11, &(x[base]));
		simd_load(s12, &(y[base]));
		d1 = s11 - s12;
		// d1 = d1 * d1;
		// s_sum_1 += d1;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, &(x[base+SIMD_SIZE_H*1]));
		simd_load(s22, &(y[base+SIMD_SIZE_H*1]));
		d2 = s21 - s22;
		// d2 = d2 * d2;
		// s_sum_2 += d2;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, &(x[base+SIMD_SIZE_H*2]));
		simd_load(s32, &(y[base+SIMD_SIZE_H*2]));
		d3 = s31 - s32;
		// d3 = d3 * d3;
		// s_sum_3 += d3;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, &(x[base+SIMD_SIZE_H*3]));
		simd_load(s42, &(y[base+SIMD_SIZE_H*3]));
		d4 = s41 - s42;
		// d4 = d4 * d4;
		// s_sum_4 += d4;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);
		simd_load(s51, &(x[base+SIMD_SIZE_H*4]));
		simd_load(s52, &(y[base+SIMD_SIZE_H*4]));
		d5 = s51 - s52;
		// d5 = d5 * d5;
		// s_sum_5 += d5;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);
		simd_load(s61, &(x[base+SIMD_SIZE_H*5]));
		simd_load(s62, &(y[base+SIMD_SIZE_H*5]));
		d6 = s61 - s62;
		// d6 = d6 * d6;
		// s_sum_6 += d6;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);
		simd_load(s71, &(x[base+SIMD_SIZE_H*6]));
		simd_load(s72, &(y[base+SIMD_SIZE_H*6]));
		d7 = s71 - s72;
		// d7 = d7 * d7;
		// s_sum_7 += d7;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);
		simd_load(s81, &(x[base+SIMD_SIZE_H*7]));
		simd_load(s82, &(y[base+SIMD_SIZE_H*7]));
		d8 = s81 - s82;
		// d8 = d8 * d8;
		// s_sum_8 += d8;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);
	}
	base = i*SIMD_SIZE_H*8;
	simd_load(s11, &(x[base]));
	simd_load(s12, &(y[base]));
	d1 = s11 - s12;
	// d1 = d1 * d1;
	// s_sum_1 += d1;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, &(x[base+SIMD_SIZE_H*1]));
	simd_load(s22, &(y[base+SIMD_SIZE_H*1]));
	d2 = s21 - s22;
	// d2 = d2 * d2;
	// s_sum_2 += d2;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);
	simd_load(s31, &(x[base+SIMD_SIZE_H*2]));
	simd_load(s32, &(y[base+SIMD_SIZE_H*2]));
	d3 = s31 - s32;
	// d3 = d3 * d3;
	// s_sum_3 += d3;
	s_sum_3 = simd_vmah(d3, d3, s_sum_3);
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = (simd_reduc_plush(s_sum_1));
	return dist;
}

inline float16 euler_dist_320_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	int base;
	int simd_len = len / SIMD_SIZE_H;
	float16v32 s_sum=0.0;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_H*8;
		simd_load(s11, &(x[base]));
		simd_load(s12, &(y[base]));
		d1 = s11 - s12;
		// d1 = d1 * d1;
		// s_sum_1 += d1;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, &(x[base+SIMD_SIZE_H*1]));
		simd_load(s22, &(y[base+SIMD_SIZE_H*1]));
		d2 = s21 - s22;
		// d2 = d2 * d2;
		// s_sum_2 += d2;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, &(x[base+SIMD_SIZE_H*2]));
		simd_load(s32, &(y[base+SIMD_SIZE_H*2]));
		d3 = s31 - s32;
		// d3 = d3 * d3;
		// s_sum_3 += d3;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, &(x[base+SIMD_SIZE_H*3]));
		simd_load(s42, &(y[base+SIMD_SIZE_H*3]));
		d4 = s41 - s42;
		// d4 = d4 * d4;
		// s_sum_4 += d4;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);
		simd_load(s51, &(x[base+SIMD_SIZE_H*4]));
		simd_load(s52, &(y[base+SIMD_SIZE_H*4]));
		d5 = s51 - s52;
		// d5 = d5 * d5;
		// s_sum_5 += d5;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);
		simd_load(s61, &(x[base+SIMD_SIZE_H*5]));
		simd_load(s62, &(y[base+SIMD_SIZE_H*5]));
		d6 = s61 - s62;
		// d6 = d6 * d6;
		// s_sum_6 += d6;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);
		simd_load(s71, &(x[base+SIMD_SIZE_H*6]));
		simd_load(s72, &(y[base+SIMD_SIZE_H*6]));
		d7 = s71 - s72;
		// d7 = d7 * d7;
		// s_sum_7 += d7;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);
		simd_load(s81, &(x[base+SIMD_SIZE_H*7]));
		simd_load(s82, &(y[base+SIMD_SIZE_H*7]));
		d8 = s81 - s82;
		// d8 = d8 * d8;
		// s_sum_8 += d8;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);
	}
	base = i*SIMD_SIZE_H*8;
	simd_load(s11, &(x[base]));
	simd_load(s12, &(y[base]));
	d1 = s11 - s12;
	// d1 = d1 * d1;
	// s_sum_1 += d1;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, &(x[base+SIMD_SIZE_H*1]));
	simd_load(s22, &(y[base+SIMD_SIZE_H*1]));
	d2 = s21 - s22;
	// d2 = d2 * d2;
	// s_sum_2 += d2;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = (simd_reduc_plush(s_sum_1));
	return dist;
}

inline float16 euler_dist_128_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	int base;
	int simd_len = len / SIMD_SIZE_H;
	float16v32 s_sum=0.0;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;

	i = 0;

	base = i*SIMD_SIZE_H*8;
	simd_load(s11, &(x[base]));
	simd_load(s12, &(y[base]));
	d1 = s11 - s12;
	// d1 = d1 * d1;
	// s_sum_1 += d1;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, &(x[base+SIMD_SIZE_H*1]));
	simd_load(s22, &(y[base+SIMD_SIZE_H*1]));
	d2 = s21 - s22;
	// d2 = d2 * d2;
	// s_sum_2 += d2;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);
	simd_load(s31, &(x[base+SIMD_SIZE_H*2]));
	simd_load(s32, &(y[base+SIMD_SIZE_H*2]));
	d3 = s31 - s32;
	// d3 = d3 * d3;
	// s_sum_3 += d3;
	s_sum_3 = simd_vmah(d3, d3, s_sum_3);
	simd_load(s41, &(x[base+SIMD_SIZE_H*3]));
	simd_load(s42, &(y[base+SIMD_SIZE_H*3]));
	d4 = s41 - s42;
	// d4 = d4 * d4;
	// s_sum_4 += d4;
	s_sum_4 = simd_vmah(d4, d4, s_sum_4);	
	
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_1 += s_sum_3;
	dist = (simd_reduc_plush(s_sum_1));
	return dist;
}
*/

int test_float(Param *param)
{
    int thread_id = athread_get_tid();
	int row_id = thread_id / 8;
	int col_id = thread_id % 8;
    float * ldm_x;
    float * ldm_y;
    volatile athread_rply_t reply_get = 0;
    unsigned long comp_st, comp_ed;
    unsigned long comp_acc = 0;
    double comp_time;
    int n_neighbor = param->n_neighbor;
    int vector_len = param->vector_len;
	int i,j,k;
	float tmp; 
	float out_sum = 0.0;
	int loop=10;

    // if(n_neighbor==1) vector_len = 112;
    // else if(n_neighbor==2) vector_len = 304;
    // else vector_len = 592;

    ldm_x = (float*)aligned_malloc(vector_len * sizeof(float), 64);
    ldm_y = (float*)aligned_malloc(vector_len * sizeof(float), 64);
    memset(ldm_x, 0, vector_len * sizeof(float));
    memset(ldm_y, 0, vector_len * sizeof(float));

    reply_get = 0;
    athread_dma_iget(ldm_x, param->x, vector_len * sizeof(float), &reply_get);
    athread_dma_iget(ldm_y, param->y, vector_len * sizeof(float), &reply_get);
    athread_dma_wait_value(&reply_get, 2);

    // if(n_neighbor == 1) 
    // {
    //     for(i=0;i<loop;++i)
    //     {
    //         comp_st = rpcc_slave();
    //         tmp = euler_dist_304(ldm_x, ldm_y, vector_len);
    //         comp_ed = rpcc_slave();
    //         comp_acc += comp_ed - comp_st;
    //     }
    // }
    // else if(n_neighbor == 2)
    // {
    //     for(i=0;i<loop;++i)
    //     {
    //         comp_st = rpcc_slave();
    //         tmp = euler_dist_304(ldm_x, ldm_y, vector_len);
    //         comp_ed = rpcc_slave();
    //         comp_acc += comp_ed - comp_st;
    //     }
    // }
    // else
    // {
    //     for(i=0;i<loop;++i)
    //     {
    //         comp_st = rpcc_slave();
    //         // tmp = euler_dist_592(ldm_x, ldm_y, vector_len);
	// 		tmp = euler_dist_8_way(ldm_x, ldm_y, 200);
    //         comp_ed = rpcc_slave();
    //         comp_acc += comp_ed - comp_st;
	// 		out_sum += tmp;
	// 		if(thread_id==0)
	// 		{
	// 			printf("loop %d, cycle %ld\n",i,comp_ed-comp_st);
	// 		}
    //     }
    // }

	for(i=8;i<1200;i+=8)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_1_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("1 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*8, (double)3.0*6*64*(i*8)*loop/((double)comp_acc/2.25));
		}
	} 

	for(i=4;i<600;i+=4)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_2_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("2 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*16, (double)3.0*6*64*(i*16)*loop/((double)comp_acc/2.25));
		}
	} 

	for(i=2;i<300;i+=2)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_4_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("4 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*32, (double)3.0*6*64*(i*32)*loop/((double)comp_acc/2.25));
		}
	}

	for(i=1;i<150;++i)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_8_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("8 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*64, (double)3.0*6*64*(i*64)*loop/((double)comp_acc/2.25));
		}
	}

	for(i=1;i<100;++i)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_12_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("12 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*96, (double)3.0*6*64*(i*96)*loop/((double)comp_acc/2.25));
		}
	}

	
	for(i=1;i<75;++i)
	{
		out_sum = 0.0;
		comp_acc = 0;
		for(j=0;j<loop;++j)
		{
			comp_st = rpcc_slave();
			tmp = euler_dist_16_way(ldm_x, ldm_y, i);
			comp_ed = rpcc_slave();
			comp_acc += comp_ed - comp_st;
			out_sum += tmp;
		}
		if(thread_id==0)
		{
			printf("16 way size = %d val = %f out_sum = %f time = %f, cycle = %ld, size = %d Gflops = %f\n",
			i, tmp, out_sum, ((double)comp_acc/2.25/1e9), comp_acc, i*128, (double)3.0*6*64*(i*128)*loop/((double)comp_acc/2.25));
		}
	}




    // if(thread_id==0)
    // {
    //     // printf("n_neighbor = %d, vector_len = %d, val = %f, time = %f, cycle = %ld, Gflops = %f\n",
	// 	// n_neighbor, vector_len, tmp, (comp_acc/2.25/1e9), comp_acc, 3.0*6*64*vector_len*loop/(comp_acc/2.25));

	// 	// printf("n_neighbor = %d, vector_len = %d, val = %f, out_sum = %f, time = %f, cycle = %ld, Gflops = %f\n",
	// 	// n_neighbor, vector_len, tmp, out_sum, (comp_acc/2.25/1e9), comp_acc, 3.0*6*64*(1000*64+16)*loop/(comp_acc/2.25));

	// 	printf("n_neighbor = %d, vector_len = %d, val = %f, out_sum = %f, time = %f, cycle = %ld, Gflops = %f\n",
	// 	n_neighbor, vector_len, tmp, out_sum, (comp_acc/2.25/1e9), comp_acc, (double)3.0*6*64*loop/((double)comp_acc/(200*64)/2.25));
    // }
    // time comp_acc/2.25/1e9

    aligned_free(ldm_x, vector_len * sizeof(float), 64);
    aligned_free(ldm_y, vector_len * sizeof(float), 64);
}






