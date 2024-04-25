#include <stdio.h>
#include <simd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <crts.h>
#include <slave.h>
#include "kmeans_param.h"
#include "trans_datatype.h"


inline int CORD(int row, int col, int col_size) {return row*col_size+col;}

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

inline void mb()
{
    asm volatile("memb");
    asm volatile("":::"memory");
}

void swap_ptr(float **a, float **b){
	float* tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void swap_ptr_16(float16 **a, float16 **b){
	float16 * tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

inline unsigned long rpcc_slave() {return  athread_stime_cycle();}


inline float euler_dist_304(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_F / 8;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len;++i)
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

inline float euler_dist_592(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_F / 8;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len;++i)
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

// 重测一下 旧的float


inline float euler_dist_304_origin(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	int base;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_F;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_F*8;
		simd_load(s11, x+base+SIMD_SIZE_F*0);
		simd_load(s12, y+base+SIMD_SIZE_F*0);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F*1);
		simd_load(s22, y+base+SIMD_SIZE_F*1);
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
	base = i*SIMD_SIZE_F*8;
	simd_load(s11, x+base+SIMD_SIZE_F*0);
	simd_load(s12, y+base+SIMD_SIZE_F*0);
	d1 = s11 - s12;
	s_sum_1 = simd_vmas(d1, d1, s_sum_1);

	simd_load(s21, x+base+SIMD_SIZE_F*1);
	simd_load(s22, y+base+SIMD_SIZE_F*1);
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

inline float euler_dist_592_origin(float* x, float* y, int len)
{
	float dist = 0.0;
	int i;
	int base;
	float * x_offset_addr = x;
	float * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_F;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len/8;++i)
	{
		base = i*SIMD_SIZE_F*8;
		simd_load(s11, x+base+SIMD_SIZE_F*0);
		simd_load(s12, y+base+SIMD_SIZE_F*0);
		d1 = s11 - s12;
		s_sum_1 = simd_vmas(d1, d1, s_sum_1);

		simd_load(s21, x+base+SIMD_SIZE_F*1);
		simd_load(s22, y+base+SIMD_SIZE_F*1);
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
	base = i*SIMD_SIZE_F*8;
	simd_load(s11, x+base+SIMD_SIZE_F*0);
	simd_load(s12, y+base+SIMD_SIZE_F*0);
	d1 = s11 - s12;
	s_sum_1 = simd_vmas(d1, d1, s_sum_1);

	simd_load(s21, x+base+SIMD_SIZE_F*1);
	simd_load(s22, y+base+SIMD_SIZE_F*1);
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

// group_euler_dist_304(ldm_data_cal+j*simd_gdm, ldm_cluster_center_old, simd_gdm);


void group_euler_dist_112(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	float * x_offset_addr = data;
	float * y_offset_addr = center;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		x_offset_addr = data;
		for(j=0;j<2;++j)
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
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;
		s_sum_1 += s_sum_5;
		candidate[i] = simd_reduc_pluss(s_sum_1);
	}
}

void group_euler_dist_320(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	float * x_offset_addr = data;
	float * y_offset_addr = center;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	floatv8 s91,s92,s101,s102,d9,d10;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7,s_sum_8;
	floatv8 s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
		s_sum_9 = 0.0, s_sum_10 = 0.0;
		x_offset_addr = data;
		for(j=0;j<4;++j)
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
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;

		candidate[i] = simd_reduc_pluss(s_sum_1);
	}
}

void group_euler_dist_616(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	float * x_offset_addr = data;
	float * y_offset_addr = center;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		x_offset_addr = data;
		for(j=0;j<11;++j)
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
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;
		s_sum_1 += s_sum_5;
		candidate[i] = simd_reduc_pluss(s_sum_1);
	}

}




void group_euler_dist_112_origin(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		for(j=0;j<2;++j)
		{
			base_x = j*SIMD_SIZE_F*7;
			base_y = i*112 + j*SIMD_SIZE_F*7;
			simd_load(s11, data+base_x);
			simd_load(s12, center+base_y);
			d1 = s11 - s12;
			s_sum_1 = simd_vmas(d1, d1, s_sum_1);

			simd_load(s21, data+base_x+SIMD_SIZE_F);
			simd_load(s22, center+base_y+SIMD_SIZE_F);
			d2 = s21 - s22;
			s_sum_2 = simd_vmas(d2, d2, s_sum_2);

			simd_load(s31, data+base_x+SIMD_SIZE_F*2);
			simd_load(s32, center+base_y+SIMD_SIZE_F*2);
			d3 = s31 - s32;
			s_sum_3 = simd_vmas(d3, d3, s_sum_3);

			simd_load(s41, data+base_x+SIMD_SIZE_F*3);
			simd_load(s42, center+base_y+SIMD_SIZE_F*3);
			d4 = s41 - s42;
			s_sum_4 = simd_vmas(d4, d4, s_sum_4);

			simd_load(s51, data+base_x+SIMD_SIZE_F*4);
			simd_load(s52, center+base_y+SIMD_SIZE_F*4);
			d5 = s51 - s52;
			s_sum_5 = simd_vmas(d5, d5, s_sum_5);

			simd_load(s61, data+base_x+SIMD_SIZE_F*5);
			simd_load(s62, center+base_y+SIMD_SIZE_F*5);
			d6 = s61 - s62;
			s_sum_6 = simd_vmas(d6, d6, s_sum_6);

			simd_load(s71, data+base_x+SIMD_SIZE_F*6);
			simd_load(s72, center+base_y+SIMD_SIZE_F*6);
			d7 = s71 - s72;
			s_sum_7 = simd_vmas(d7, d7, s_sum_7);
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;
		s_sum_1 += s_sum_5;
		candidate[i] = simd_reduc_pluss(s_sum_1);
	}
}

void group_euler_dist_320_origin(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	floatv8 s81,s82,s91,s92,s101,s102,d8,d9,d10;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	floatv8 s_sum_8, s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		s_sum_8 = 0.0, s_sum_9 = 0.0, s_sum_10 = 0.0;
		for(j=0;j<4;++j)
		{
			base_x = j*SIMD_SIZE_F*10;
			base_y = i*320 + j*SIMD_SIZE_F*10;
			simd_load(s11, data+base_x);
			simd_load(s12, center+base_y);
			d1 = s11 - s12;
			s_sum_1 = simd_vmas(d1, d1, s_sum_1);

			simd_load(s21, data+base_x+SIMD_SIZE_F);
			simd_load(s22, center+base_y+SIMD_SIZE_F);
			d2 = s21 - s22;
			s_sum_2 = simd_vmas(d2, d2, s_sum_2);

			simd_load(s31, data+base_x+SIMD_SIZE_F*2);
			simd_load(s32, center+base_y+SIMD_SIZE_F*2);
			d3 = s31 - s32;
			s_sum_3 = simd_vmas(d3, d3, s_sum_3);

			simd_load(s41, data+base_x+SIMD_SIZE_F*3);
			simd_load(s42, center+base_y+SIMD_SIZE_F*3);
			d4 = s41 - s42;
			s_sum_4 = simd_vmas(d4, d4, s_sum_4);

			simd_load(s51, data+base_x+SIMD_SIZE_F*4);
			simd_load(s52, center+base_y+SIMD_SIZE_F*4);
			d5 = s51 - s52;
			s_sum_5 = simd_vmas(d5, d5, s_sum_5);

			simd_load(s61, data+base_x+SIMD_SIZE_F*5);
			simd_load(s62, center+base_y+SIMD_SIZE_F*5);
			d6 = s61 - s62;
			s_sum_6 = simd_vmas(d6, d6, s_sum_6);

			simd_load(s71, data+base_x+SIMD_SIZE_F*6);
			simd_load(s72, center+base_y+SIMD_SIZE_F*6);
			d7 = s71 - s72;
			s_sum_7 = simd_vmas(d7, d7, s_sum_7);

			simd_load(s81, data+base_x+SIMD_SIZE_F*7);
			simd_load(s82, center+base_y+SIMD_SIZE_F*7);
			d8 = s81 - s82;
			s_sum_8 = simd_vmas(d8, d8, s_sum_8);

			simd_load(s91, data+base_x+SIMD_SIZE_F*8);
			simd_load(s92, center+base_y+SIMD_SIZE_F*8);
			d9 = s91 - s92;
			s_sum_9 = simd_vmas(d9, d9, s_sum_9);

			simd_load(s101, data+base_x+SIMD_SIZE_F*9);
			simd_load(s102, center+base_y+SIMD_SIZE_F*9);
			d10 = s101 - s102;
			s_sum_10 = simd_vmas(d10, d10, s_sum_10);
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;
		candidate[i] = simd_reduc_pluss(s_sum_1);
	}
}

void group_euler_dist_616_origin(float* data, float* center, float* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	floatv8 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	floatv8 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		for(j=0;j<11;++j)
		{
			base_x = j*SIMD_SIZE_F*7;
			base_y = i*616 + j*SIMD_SIZE_F*7;
			simd_load(s11, data+base_x);
			simd_load(s12, center+base_y);
			d1 = s11 - s12;
			s_sum_1 = simd_vmas(d1, d1, s_sum_1);

			simd_load(s21, data+base_x+SIMD_SIZE_F);
			simd_load(s22, center+base_y+SIMD_SIZE_F);
			d2 = s21 - s22;
			s_sum_2 = simd_vmas(d2, d2, s_sum_2);

			simd_load(s31, data+base_x+SIMD_SIZE_F*2);
			simd_load(s32, center+base_y+SIMD_SIZE_F*2);
			d3 = s31 - s32;
			s_sum_3 = simd_vmas(d3, d3, s_sum_3);

			simd_load(s41, data+base_x+SIMD_SIZE_F*3);
			simd_load(s42, center+base_y+SIMD_SIZE_F*3);
			d4 = s41 - s42;
			s_sum_4 = simd_vmas(d4, d4, s_sum_4);

			simd_load(s51, data+base_x+SIMD_SIZE_F*4);
			simd_load(s52, center+base_y+SIMD_SIZE_F*4);
			d5 = s51 - s52;
			s_sum_5 = simd_vmas(d5, d5, s_sum_5);

			simd_load(s61, data+base_x+SIMD_SIZE_F*5);
			simd_load(s62, center+base_y+SIMD_SIZE_F*5);
			d6 = s61 - s62;
			s_sum_6 = simd_vmas(d6, d6, s_sum_6);

			simd_load(s71, data+base_x+SIMD_SIZE_F*6);
			simd_load(s72, center+base_y+SIMD_SIZE_F*6);
			d7 = s71 - s72;
			s_sum_7 = simd_vmas(d7, d7, s_sum_7);
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;
		s_sum_1 += s_sum_5;
		candidate[i] = simd_reduc_pluss(s_sum_1);
	}
}



inline float16 euler_dist_608_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	float16 * x_offset_addr = x;
	float16 * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_H / 8;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);
		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d5 = s51 - s52;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d6 = s61 - s62;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);
		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d7 = s71 - s72;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);
		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d8 = s81 - s82;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);
	}
	simd_load(s11, x_offset_addr);
	simd_load(s12, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d1 = s11 - s12;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, x_offset_addr);
	simd_load(s22, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d2 = s21 - s22;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);
	simd_load(s31, x_offset_addr);
	simd_load(s32, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d3 = s31 - s32;
	s_sum_3 = simd_vmah(d3, d3, s_sum_3);

	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_plush(s_sum_1);
	return dist;
}

inline float16 euler_dist_320_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	float16 * x_offset_addr = x;
	float16 * y_offset_addr = y;
	int simd_len = len / SIMD_SIZE_H / 8;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
	for(i=0;i<simd_len;++i)
	{
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);
		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d5 = s51 - s52;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d6 = s61 - s62;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);
		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d7 = s71 - s72;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);
		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d8 = s81 - s82;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);
	}
	simd_load(s11, x_offset_addr);
	simd_load(s12, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d1 = s11 - s12;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, x_offset_addr);
	simd_load(s22, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d2 = s21 - s22;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);

	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_5 += s_sum_6;
	s_sum_7 += s_sum_8;
	s_sum_1 += s_sum_3;
	s_sum_5 += s_sum_7;
	s_sum_1 += s_sum_5;
	dist = simd_reduc_plush(s_sum_1);
	return dist;
}

inline float16 euler_dist_128_half(float16 * x, float16 * y, int len)
{
	float16 dist = 0.0;
	int i;
	float16 * x_offset_addr = x;
	float16 * y_offset_addr = y;
	// int simd_len = len / SIMD_SIZE_H / 8;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,d1,d2,d3,d4;
	float16v32 s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0;
	simd_load(s11, x_offset_addr);
	simd_load(s12, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;	
	y_offset_addr += SIMD_SIZE_H;
	d1 = s11 - s12;
	s_sum_1 = simd_vmah(d1, d1, s_sum_1);
	simd_load(s21, x_offset_addr);
	simd_load(s22, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d2 = s21 - s22;
	s_sum_2 = simd_vmah(d2, d2, s_sum_2);
	simd_load(s31, x_offset_addr);
	simd_load(s32, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d3 = s31 - s32;
	s_sum_3 = simd_vmah(d3, d3, s_sum_3);
	simd_load(s41, x_offset_addr);
	simd_load(s42, y_offset_addr);
	x_offset_addr += SIMD_SIZE_H;
	y_offset_addr += SIMD_SIZE_H;
	d4 = s41 - s42;
	s_sum_4 = simd_vmah(d4, d4, s_sum_4);	
	
	s_sum_1 += s_sum_2;
	s_sum_3 += s_sum_4;
	s_sum_1 += s_sum_3;
	dist = simd_reduc_plush(s_sum_1);
	return dist;
}



void group_euler_dist_128_half(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	float16 * x_offset_addr = data;
	float16 * y_offset_addr = center;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,d1,d2,d3,d4;
	float16v32 s_sum_1, s_sum_2, s_sum_3, s_sum_4;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0;
		x_offset_addr = data;
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);

		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_1 += s_sum_3;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}

void group_euler_dist_320_half(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	float16 * x_offset_addr = data;
	float16 * y_offset_addr = center;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s91,s92,s101,s102,d9,d10;
	float16v32 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7,s_sum_8;
	float16v32 s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
		s_sum_9 = 0.0, s_sum_10 = 0.0;
		x_offset_addr = data;
		simd_load(s11, x_offset_addr);
		simd_load(s12, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);
		simd_load(s21, x_offset_addr);
		simd_load(s22, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);
		simd_load(s31, x_offset_addr);
		simd_load(s32, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);
		simd_load(s41, x_offset_addr);
		simd_load(s42, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);
		simd_load(s51, x_offset_addr);
		simd_load(s52, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d5 = s51 - s52;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);
		simd_load(s61, x_offset_addr);
		simd_load(s62, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d6 = s61 - s62;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);
		simd_load(s71, x_offset_addr);
		simd_load(s72, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d7 = s71 - s72;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);
		simd_load(s81, x_offset_addr);
		simd_load(s82, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d8 = s81 - s82;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);
		simd_load(s91, x_offset_addr);
		simd_load(s92, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d9 = s91 - s92;
		s_sum_9 = simd_vmah(d9, d9, s_sum_9);
		simd_load(s101, x_offset_addr);
		simd_load(s102, y_offset_addr);
		x_offset_addr += SIMD_SIZE_H;
		y_offset_addr += SIMD_SIZE_H;
		d10 = s101 - s102;
		s_sum_10 = simd_vmah(d10, d10, s_sum_10);

		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}

void group_euler_dist_640_half(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	float16 * x_offset_addr = data;
	float16 * y_offset_addr = center;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,s81,s82,d1,d2,d3,d4,d5,d6,d7,d8;
	float16v32 s91,s92,s101,s102,d9,d10;
	float16v32 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7,s_sum_8;
	float16v32 s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0,s_sum_8 = 0.0;
		s_sum_9 = 0.0, s_sum_10 = 0.0;
		x_offset_addr = data;
		for(j=0;j<2;++j)
		{
			simd_load(s11, x_offset_addr);
			simd_load(s12, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d1 = s11 - s12;
			s_sum_1 = simd_vmah(d1, d1, s_sum_1);
			simd_load(s21, x_offset_addr);
			simd_load(s22, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d2 = s21 - s22;
			s_sum_2 = simd_vmah(d2, d2, s_sum_2);
			simd_load(s31, x_offset_addr);
			simd_load(s32, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d3 = s31 - s32;
			s_sum_3 = simd_vmah(d3, d3, s_sum_3);
			simd_load(s41, x_offset_addr);
			simd_load(s42, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d4 = s41 - s42;
			s_sum_4 = simd_vmah(d4, d4, s_sum_4);
			simd_load(s51, x_offset_addr);
			simd_load(s52, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d5 = s51 - s52;
			s_sum_5 = simd_vmah(d5, d5, s_sum_5);
			simd_load(s61, x_offset_addr);
			simd_load(s62, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d6 = s61 - s62;
			s_sum_6 = simd_vmah(d6, d6, s_sum_6);
			simd_load(s71, x_offset_addr);
			simd_load(s72, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d7 = s71 - s72;
			s_sum_7 = simd_vmah(d7, d7, s_sum_7);
			simd_load(s81, x_offset_addr);
			simd_load(s82, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d8 = s81 - s82;
			s_sum_8 = simd_vmah(d8, d8, s_sum_8);
			simd_load(s91, x_offset_addr);
			simd_load(s92, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d9 = s91 - s92;
			s_sum_9 = simd_vmah(d9, d9, s_sum_9);
			simd_load(s101, x_offset_addr);
			simd_load(s102, y_offset_addr);
			x_offset_addr += SIMD_SIZE_H;
			y_offset_addr += SIMD_SIZE_H;
			d10 = s101 - s102;
			s_sum_10 = simd_vmah(d10, d10, s_sum_10);
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}




void group_euler_dist_128_half_origin(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,d1,d2,d3,d4;
	float16v32 s_sum_1, s_sum_2, s_sum_3,s_sum_4;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0;

		// base_x = j*SIMD_SIZE_H*10;
		// base_y = i*640 + j*SIMD_SIZE_H*10;
		base_x = 0;
		base_y = i*128;
		simd_load(s11, data+base_x);
		simd_load(s12, center+base_y);
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);

		simd_load(s21, data+base_x+SIMD_SIZE_H);
		simd_load(s22, center+base_y+SIMD_SIZE_H);
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);

		simd_load(s31, data+base_x+SIMD_SIZE_H*2);
		simd_load(s32, center+base_y+SIMD_SIZE_H*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);

		simd_load(s41, data+base_x+SIMD_SIZE_H*3);
		simd_load(s42, center+base_y+SIMD_SIZE_H*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);

		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_1 += s_sum_3;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}

void group_euler_dist_320_half_origin(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	float16v32 s81,s82,s91,s92,s101,s102,d8,d9,d10;
	float16v32 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	float16v32 s_sum_8, s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		s_sum_8 = 0.0, s_sum_9 = 0.0, s_sum_10 = 0.0;

		// base_x = j*SIMD_SIZE_H*10;
		// base_y = i*320 + j*SIMD_SIZE_H*10;
		base_x = 0;
		base_x = i*320;
		simd_load(s11, data+base_x);
		simd_load(s12, center+base_y);
		d1 = s11 - s12;
		s_sum_1 = simd_vmah(d1, d1, s_sum_1);

		simd_load(s21, data+base_x+SIMD_SIZE_H);
		simd_load(s22, center+base_y+SIMD_SIZE_H);
		d2 = s21 - s22;
		s_sum_2 = simd_vmah(d2, d2, s_sum_2);

		simd_load(s31, data+base_x+SIMD_SIZE_H*2);
		simd_load(s32, center+base_y+SIMD_SIZE_H*2);
		d3 = s31 - s32;
		s_sum_3 = simd_vmah(d3, d3, s_sum_3);

		simd_load(s41, data+base_x+SIMD_SIZE_H*3);
		simd_load(s42, center+base_y+SIMD_SIZE_H*3);
		d4 = s41 - s42;
		s_sum_4 = simd_vmah(d4, d4, s_sum_4);

		simd_load(s51, data+base_x+SIMD_SIZE_H*4);
		simd_load(s52, center+base_y+SIMD_SIZE_H*4);
		d5 = s51 - s52;
		s_sum_5 = simd_vmah(d5, d5, s_sum_5);

		simd_load(s61, data+base_x+SIMD_SIZE_H*5);
		simd_load(s62, center+base_y+SIMD_SIZE_H*5);
		d6 = s61 - s62;
		s_sum_6 = simd_vmah(d6, d6, s_sum_6);

		simd_load(s71, data+base_x+SIMD_SIZE_H*6);
		simd_load(s72, center+base_y+SIMD_SIZE_H*6);
		d7 = s71 - s72;
		s_sum_7 = simd_vmah(d7, d7, s_sum_7);

		simd_load(s81, data+base_x+SIMD_SIZE_H*7);
		simd_load(s82, center+base_y+SIMD_SIZE_H*7);
		d8 = s81 - s82;
		s_sum_8 = simd_vmah(d8, d8, s_sum_8);

		simd_load(s91, data+base_x+SIMD_SIZE_H*8);
		simd_load(s92, center+base_y+SIMD_SIZE_H*8);
		d9 = s91 - s92;
		s_sum_9 = simd_vmah(d9, d9, s_sum_9);

		simd_load(s101, data+base_x+SIMD_SIZE_H*9);
		simd_load(s102, center+base_y+SIMD_SIZE_H*9);
		d10 = s101 - s102;
		s_sum_10 = simd_vmah(d10, d10, s_sum_10);

		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}

void group_euler_dist_640_half_origin(float16* data, float16* center, float16* candidate, int num_clusters)
{
	int i, j;
	int base_x, base_y;
	float16v32 s11,s12,s21,s22,s31,s32,s41,s42,s51,s52,s61,s62,s71,s72,d1,d2,d3,d4,d5,d6,d7;
	float16v32 s81,s82,s91,s92,s101,s102,d8,d9,d10;
	float16v32 s_sum_1, s_sum_2, s_sum_3,s_sum_4,s_sum_5,s_sum_6,s_sum_7;
	float16v32 s_sum_8, s_sum_9, s_sum_10;
	for(i=0;i<num_clusters;++i)
	{
		s_sum_1 = 0.0, s_sum_2 = 0.0, s_sum_3 = 0.0,s_sum_4 = 0.0,s_sum_5 = 0.0,s_sum_6 = 0.0,s_sum_7 = 0.0;
		s_sum_8 = 0.0, s_sum_9 = 0.0, s_sum_10 = 0.0;
		for(j=0;j<2;++j)
		{
			base_x = j*SIMD_SIZE_H*10;
			base_y = i*640 + j*SIMD_SIZE_H*10;
			simd_load(s11, data+base_x);
			simd_load(s12, center+base_y);
			d1 = s11 - s12;
			s_sum_1 = simd_vmah(d1, d1, s_sum_1);

			simd_load(s21, data+base_x+SIMD_SIZE_H);
			simd_load(s22, center+base_y+SIMD_SIZE_H);
			d2 = s21 - s22;
			s_sum_2 = simd_vmah(d2, d2, s_sum_2);

			simd_load(s31, data+base_x+SIMD_SIZE_H*2);
			simd_load(s32, center+base_y+SIMD_SIZE_H*2);
			d3 = s31 - s32;
			s_sum_3 = simd_vmah(d3, d3, s_sum_3);

			simd_load(s41, data+base_x+SIMD_SIZE_H*3);
			simd_load(s42, center+base_y+SIMD_SIZE_H*3);
			d4 = s41 - s42;
			s_sum_4 = simd_vmah(d4, d4, s_sum_4);

			simd_load(s51, data+base_x+SIMD_SIZE_H*4);
			simd_load(s52, center+base_y+SIMD_SIZE_H*4);
			d5 = s51 - s52;
			s_sum_5 = simd_vmah(d5, d5, s_sum_5);

			simd_load(s61, data+base_x+SIMD_SIZE_H*5);
			simd_load(s62, center+base_y+SIMD_SIZE_H*5);
			d6 = s61 - s62;
			s_sum_6 = simd_vmah(d6, d6, s_sum_6);

			simd_load(s71, data+base_x+SIMD_SIZE_H*6);
			simd_load(s72, center+base_y+SIMD_SIZE_H*6);
			d7 = s71 - s72;
			s_sum_7 = simd_vmah(d7, d7, s_sum_7);

			simd_load(s81, data+base_x+SIMD_SIZE_H*7);
			simd_load(s82, center+base_y+SIMD_SIZE_H*7);
			d8 = s81 - s82;
			s_sum_8 = simd_vmah(d8, d8, s_sum_8);

			simd_load(s91, data+base_x+SIMD_SIZE_H*8);
			simd_load(s92, center+base_y+SIMD_SIZE_H*8);
			d9 = s91 - s92;
			s_sum_9 = simd_vmah(d9, d9, s_sum_9);

			simd_load(s101, data+base_x+SIMD_SIZE_H*9);
			simd_load(s102, center+base_y+SIMD_SIZE_H*9);
			d10 = s101 - s102;
			s_sum_10 = simd_vmah(d10, d10, s_sum_10);
		}
		s_sum_1 += s_sum_2;
		s_sum_3 += s_sum_4;
		s_sum_5 += s_sum_6;
		s_sum_7 += s_sum_8;
		s_sum_9 += s_sum_10;

		s_sum_1 += s_sum_3;
		s_sum_5 += s_sum_7;

		s_sum_1 += s_sum_5;
		s_sum_1 += s_sum_9;
		candidate[i] = simd_reduc_plush(s_sum_1);
	}
}





int swKmeans_float(KmeansParam *param)
{

	int thread_id = athread_get_tid();
	int row_id = thread_id / 8;
	int col_id = thread_id % 8;
	int p_id = param->p_id;

	size_t n_dims = param->n_dims;
	size_t n_clusters = param->n_clusters;
	size_t grid_sum = param->grid_sum;
	size_t local_n_samples = param->local_n_samples;
	size_t n_load_row = param->n_load_row;
	size_t n_mix = param->n_mix;
	size_t n_neighbors = param->n_neighbors;
	float *cluster_center_old = param->cluster_center_old;
	
	int is_write_labels = param->is_write_labels;
	int cpe_group_num = param->cpe_group_num;
	int cpe_group_size = param->cpe_group_size;

	int row_st = param->row_st;


	float * cluster_center_buf = param->cluster_center_buf;
	int * cluster_center_num_buf = param->cluster_center_num_buf;
	float * local_data_mpiin = param->local_data_mpiin;
	int * labels = param->labels;
	float * se = param->se;
	double * slave_time_array = param->slave_time_array;

	int workload = n_load_row/(SLAVE_NUM/cpe_group_num) + (row_id<(n_load_row%(SLAVE_NUM/cpe_group_num))?1:0);
	int my_start = row_id * (n_load_row/(SLAVE_NUM/cpe_group_num)) + (row_id < (n_load_row%(SLAVE_NUM/cpe_group_num))?row_id:(n_load_row%(SLAVE_NUM/cpe_group_num)));

	int n_clusters_ = n_clusters;
	int nc_nd_ = param->nc_nd_;

	int local_n_clusters = param->local_n_clusters;
	int local_ncnd = param->local_ncnd;

	int row_max = param->row_max;

	// for mixed precision
	// row_max = row_max - 1;

	int row_round = (workload-1)/row_max+1; 

	// int origin_gdm = grid_sum * n_dims * n_mix;
	// int simd_gdm = ((origin_gdm - 1) / SIMD_SIZE_F + 1) * SIMD_SIZE_F;
	// int half_gdm = ((origin_gdm - 1) / SIMD_SIZE_H + 1) * SIMD_SIZE_H;
	int origin_gdm = param->origin_gdm;
	int simd_gdm = param->simd_gdm;
	int half_gdm = param->half_gdm;

	int row_batch_size = param->row_batch_size;
	int col_batch_size = param->col_batch_size;

	int i,j,k,min_index,min_col_id,this_min_index, post_min_index;
	int row_cnt, row, col, pos_row, pos_col, dma_expect_val;

    int buff_row_max, buff_row_round;
	// buff_row_max = row_max / 2;
    // buff_row_round = (workload-1)/buff_row_max+1;
    // row_cnt = buff_row_max;
	buff_row_max = row_max;
	buff_row_round = (workload-1) / buff_row_max + 1;
	row_cnt = buff_row_max;

	// 重新安排一下rowmax的计算，以及从核的内存分布


	int* ldm_cluster_center_num = (int*)aligned_malloc(local_n_clusters*sizeof(int), 32);
	int* ldm_labels = (int*)ldm_malloc(buff_row_max*sizeof(int));
	// memset(ldm_labels, 0, buff_row_max*sizeof(int));
	memset(ldm_cluster_center_num, 0, local_n_clusters*sizeof(int));
	float * ldm_data;
	float * ldm_cluster_center_old;
	float * ldm_cluster_center_buf;
	float * ldm_se;
	float * rma_buf;
	float * rma_check;
	float * ldm_min_candidate;
	// double * ldm_slave_time_array;


	ldm_data = (float*)aligned_malloc(row_max * simd_gdm * sizeof(float), 64);
	ldm_cluster_center_old = (float*)aligned_malloc(simd_gdm * local_n_clusters * sizeof(float), 64);
	ldm_cluster_center_buf = (float*)aligned_malloc(simd_gdm * local_n_clusters * sizeof(float), 64);
	ldm_se = (float*)ldm_malloc(buff_row_max*sizeof(float));
	rma_buf = (float*)aligned_malloc(buff_row_max*2*cpe_group_size*sizeof(float), 64);
	rma_check = (float*)aligned_malloc(buff_row_max*sizeof(float), 64);
	ldm_min_candidate = (float*)aligned_malloc(local_n_clusters*sizeof(float), 64);
	// ldm_slave_time_array = (double*)aligned_malloc(16*sizeof(double), 64);
	double ldm_slave_time_array[16];
	memset(ldm_data, 0, row_max*simd_gdm*sizeof(float));
	// memset(ldm_cluster_center_old, 0, simd_gdm*local_n_clusters * sizeof(float));
	memset(ldm_cluster_center_buf, 0, simd_gdm*local_n_clusters * sizeof(float));
	memset(ldm_se, 0, buff_row_max*sizeof(float));

	volatile athread_rply_t reply_get_data = 0;
	volatile athread_rply_t reply_get_center = 0;
	volatile athread_rply_t reply_get_center_num = 0;
	volatile athread_rply_t reply_put_center_num = 0;
	volatile athread_rply_t reply_put_center = 0;
	volatile athread_rply_t reply_put_labels = 0;
	volatile athread_rply_t reply_put_se = 0;
	volatile athread_rply_t reply_get_se = 0;
	volatile athread_rply_t reply_rma_l=0;
	volatile athread_rply_t reply_rma_r=0;

	// unsigned long a_dma_center,b_dma_center,a_dma_data,b_dma_data,a_cal,b_cal,a_se,b_se,a_dma_out,b_dma_out;
	// unsigned long dma_center=0,dma_data=0,cal=0,se_out=0,dma_out=0;

	// unsigned long a_cal_cal,b_cal_cal,a_cal_rma_1,b_cal_rma_1,a_cal_rma_2,b_cal_rma_2,a_cal_pro_1,b_cal_pro_1,a_cal_pro_2,b_cal_pro_2;
	// unsigned long cal_cal=0,cal_rma_1=0,cal_rma_2=0,cal_pro_1=0,cal_pro_2=0;
	// unsigned long a_sync_1, b_sync_1, a_sync_2, b_sync_2, a_sync_0, b_sync_0;
	// unsigned long sync_1=0, sync_2=0, sync_0=0;

	unsigned long centerin_st, centerin_ed, comp_st, comp_ed, resout_st, resout_ed;
	double centerin_time, comp_time, resout_time;

	unsigned long datain_st, datain_ed, localcomp_st, localcomp_ed,
	sendres_st, sendres_ed, globalcomp_st, globalcomp_ed,
	bcast_st, bcast_ed, verify_st, verify_ed, write_st, write_ed;
	unsigned long datain_acc = 0.0;
	unsigned long localcomp_acc = 0.0;
	unsigned long sendres_acc = 0.0;
	unsigned long globalcomp_acc = 0.0;
	unsigned long bcast_acc = 0.0;
	unsigned long verify_acc = 0.0;
	unsigned long write_acc = 0.0;
	double datain_time, localcomp_time, sendres_time, globalcomp_time, 
	bcast_time, verify_time, write_time;

	float tmp,min_dist,this_min_dist;
	floatv8 ssrca, ssrcb, sdst;

	centerin_st = rpcc_slave();
	for(i=0;i<local_n_clusters;++i)
	{
		athread_dma_iget(ldm_cluster_center_old+i*simd_gdm, cluster_center_old+(col_id*local_n_clusters+i)*origin_gdm, origin_gdm*sizeof(float), &reply_get_center);
	}
	athread_dma_wait_value(&reply_get_center, local_n_clusters);
	centerin_ed = rpcc_slave();


    float * ldm_data_dma = ldm_data;
    // float * ldm_data_cal = ldm_data + buff_row_max * simd_gdm;
	float * ldm_data_cal = ldm_data;


	comp_st = rpcc_slave();
    for(i=0;i<buff_row_round;++i)
    {
		datain_st = rpcc_slave();
        if(i==buff_row_round-1) row_cnt = workload-i*buff_row_max;
    	reply_get_data=0;
		athread_ssync(ROW_SCOPE,0xff);
    	for(j=0;j<row_cnt;++j)
    	{
        	row = (my_start + i*buff_row_max + j) / col_batch_size + n_neighbors;
			col = (my_start + i*buff_row_max + j) % col_batch_size + n_neighbors;
			if(col_id==row_id){
        	athread_dma_row_ibcast_stride(ldm_data_dma+j*simd_gdm,
								local_data_mpiin+ CORD(row-n_neighbors,col-n_neighbors,col_batch_size+2*n_neighbors)*n_dims,
								n_dims*grid_sum*sizeof(float),
								n_dims*(2*n_neighbors+1)*sizeof(float), //bsize
								n_dims*(col_batch_size-1)*sizeof(float),&reply_get_data);
			athread_dma_row_ibcast_stride(ldm_data_dma+j*simd_gdm + n_dims * grid_sum,
								local_data_mpiin+ CORD(row-n_neighbors,col-n_neighbors,col_batch_size+2*n_neighbors)*n_dims+(row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors)*n_dims,
								n_dims*grid_sum*sizeof(float),
								n_dims*(2*n_neighbors+1)*sizeof(float), //bsize
								n_dims*(col_batch_size-1)*sizeof(float),&reply_get_data);	
			}
    	}
    	athread_dma_wait_value(&reply_get_data, row_cnt*n_mix);
		datain_ed = rpcc_slave();
		datain_acc += datain_ed-datain_st;

        // cal distance & prepare rma buf
		localcomp_st = rpcc_slave();
		////////////////////////////////////////////////////////
        // for(j=0;j<row_cnt;++j)
        // {
		// 	min_dist = 1e9;
		// 	min_index = 0;
		// 	// min_col_id=0;
		// 	for(k=0; k<local_n_clusters; k++)
		// 	{
		// 		if(n_neighbors==1)
		// 		{
		// 				tmp = euler_dist_304(ldm_data_cal+j*simd_gdm,ldm_cluster_center_old+k*simd_gdm,simd_gdm);
		// 		}
		// 		else if(n_neighbors==2)
		// 		{
		// 				tmp = euler_dist_304(ldm_data_cal+j*simd_gdm,ldm_cluster_center_old+k*simd_gdm,simd_gdm);
		// 		}
		// 		else
		// 		{
		// 				tmp = euler_dist_592(ldm_data_cal+j*simd_gdm,ldm_cluster_center_old+k*simd_gdm,simd_gdm);
		// 		} 

		// 		if(tmp < min_dist)
		// 		{
		// 			min_dist = tmp;
		// 			min_index = k;
		// 		}
		// 	}
		// 	rma_buf[col_id*buff_row_max*2+j*2+0] = min_dist;
		// 	rma_buf[col_id*buff_row_max*2+j*2+1] = (float)min_index;
        // }
		//////////////////////////////////////////////////////// 向量融合计算 ///////////////
		if(n_neighbors==1)
		{
			// 在local n clusters个结果中找最小的
			// 存在ldm_min_candidate中
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_112(ldm_data_cal+j*simd_gdm, ldm_cluster_center_old, ldm_min_candidate, local_n_clusters);
				min_dist = 1e9;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate[k] < min_dist) {min_dist = ldm_min_candidate[k]; min_index = k;}
				}
				rma_buf[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf[col_id*buff_row_max*2+j*2+1] = (float)min_index;
			}
		}
		else if(n_neighbors==2)
		{
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_320(ldm_data_cal+j*simd_gdm, ldm_cluster_center_old, ldm_min_candidate, local_n_clusters);
				min_dist = 1e9;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate[k] < min_dist) {min_dist = ldm_min_candidate[k]; min_index = k;}
				}
				rma_buf[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf[col_id*buff_row_max*2+j*2+1] = (float)min_index;
			}
		}
		else
		{
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_616(ldm_data_cal+j*simd_gdm, ldm_cluster_center_old, ldm_min_candidate, local_n_clusters);
				min_dist = 1e9;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate[k] < min_dist) {min_dist = ldm_min_candidate[k]; min_index = k;}
				}
				rma_buf[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf[col_id*buff_row_max*2+j*2+1] = (float)min_index;
			}
		}



		localcomp_ed = rpcc_slave();
		localcomp_acc += localcomp_ed - localcomp_st;

        // send local result
		sendres_st = rpcc_slave();
        reply_rma_l=0;
		reply_rma_r=0;
		athread_ssync(ROW_SCOPE, 0xff);
		if(col_id==0)
		{
			for(k=1;k<8;++k) athread_rma_iget(rma_buf+k*buff_row_max*2,&reply_rma_l,row_cnt*2*sizeof(float),thread_id+k,rma_buf+k*buff_row_max*2,&reply_rma_r);
		}
		else
		{
			athread_rma_wait_value(&reply_rma_r,1);
		}
		if(col_id==0) athread_rma_wait_value(&reply_rma_l,7);
		sendres_ed = rpcc_slave();
		sendres_acc += sendres_ed - sendres_st;

        // cal global result
		globalcomp_st = rpcc_slave();
        if(col_id==0)
		{
			for(j=0;j<row_cnt;++j)
			{
				min_dist = 1e9;
				for(k=0;k<8;++k)
				{
					if(rma_buf[k*buff_row_max*2+j*2+0]<min_dist)
					{
						min_col_id = k;
						min_dist = rma_buf[k*buff_row_max*2+j*2+0];
						min_index = (int)rma_buf[k*buff_row_max*2+j*2+1];
					}
				}
				ldm_se[j] += min_dist;
				ldm_labels[j] = min_col_id*local_n_clusters+min_index;
				rma_check[j] = (float)min_col_id;
			}
		}
		globalcomp_ed = rpcc_slave();
		globalcomp_acc += globalcomp_ed - globalcomp_st;
		
        // broadcast the result
		bcast_st = rpcc_slave();
        reply_rma_l=0;
		reply_rma_r=0;
		athread_ssync(ROW_SCOPE, 0xff);
        if(col_id==0)
		{
			athread_rma_row_ibcast(rma_check,rma_check,row_cnt*sizeof(float),&reply_rma_l,&reply_rma_r);
			athread_rma_wait_value(&reply_rma_l,1);
		}
		else athread_rma_wait_value(&reply_rma_r,1);
		bcast_ed = rpcc_slave();
		bcast_acc += bcast_ed - bcast_st;

        // verify the global result
		verify_st = rpcc_slave();
        for(j=0;j<row_cnt;++j)
		{
			if(rma_check[j]==col_id)
			{
				post_min_index = (int)rma_buf[col_id*buff_row_max*2+j*2+1];
				ldm_cluster_center_num[post_min_index]++;
				for(k=0;k<simd_gdm/SIMD_SIZE_F;++k)
				{
				    simd_load(ssrca, &(ldm_data_cal[j*simd_gdm + k*SIMD_SIZE_F]));
				    simd_load(ssrcb, &(ldm_cluster_center_buf[post_min_index*simd_gdm + k*SIMD_SIZE_F]));
				    sdst = ssrca + ssrcb;
				    simd_store(sdst, &(ldm_cluster_center_buf[post_min_index*simd_gdm + k*SIMD_SIZE_F]));
				}
			}
		}
		verify_ed = rpcc_slave();
		verify_acc += verify_ed - verify_st;

        // write labels
		write_st = rpcc_slave();
        if(col_id==0&&is_write_labels)
		{
			reply_put_labels=0;
			athread_dma_iput(labels+my_start+i*buff_row_max,ldm_labels,row_cnt*sizeof(int),&reply_put_labels);
			athread_dma_wait_value(&reply_put_labels,1);
		}
		write_ed = rpcc_slave();
		write_acc += write_ed - write_st;
    }
	comp_ed = rpcc_slave();

    // output se & center buf & center num buf
	resout_st = rpcc_slave();
    if(col_id==0)
	{
		reply_put_se=0;
		athread_dma_iput(se+row_id*buff_row_max,ldm_se,buff_row_max*sizeof(float),&reply_put_se);
		athread_dma_wait_value(&reply_put_se,1);
	}
    reply_put_center=0;
	for(i=0;i<local_n_clusters;++i)
	{
		athread_dma_iput(cluster_center_buf+row_id*origin_gdm*n_clusters+(col_id*local_n_clusters+i)*origin_gdm, ldm_cluster_center_buf+i*simd_gdm, origin_gdm*sizeof(float), &reply_put_center);
	}
	athread_dma_wait_value(&reply_put_center, local_n_clusters);

	reply_put_center_num=0;
	athread_dma_iput(cluster_center_num_buf+row_id*n_clusters+col_id*local_n_clusters,ldm_cluster_center_num,local_n_clusters*sizeof(int),&reply_put_center_num);
	athread_dma_wait_value(&reply_put_center_num,1);
	resout_ed = rpcc_slave();

	ldm_slave_time_array[0] = (centerin_ed-centerin_st)/2.25/1e9;
	ldm_slave_time_array[1] = (comp_ed-comp_st)/2.25/1e9;
	ldm_slave_time_array[2] = (resout_ed-resout_st)/2.25/1e9;

	ldm_slave_time_array[3] = datain_acc/2.25/1e9;
	ldm_slave_time_array[4] = localcomp_acc/2.25/1e9;
	ldm_slave_time_array[5] = sendres_acc/2.25/1e9;
	ldm_slave_time_array[6] = globalcomp_acc/2.25/1e9;
	ldm_slave_time_array[7] = bcast_acc/2.25/1e9;
	ldm_slave_time_array[8] = verify_acc/2.25/1e9;
	ldm_slave_time_array[9] = write_acc/2.25/1e9;
	
	if(p_id==0&&thread_id==0)
	{
		reply_put_center_num=0;
		athread_dma_iput(slave_time_array, ldm_slave_time_array, 10*sizeof(double),&reply_put_center_num);
		athread_dma_wait_value(&reply_put_center_num,1);		
	}

    aligned_free(ldm_cluster_center_num, local_n_clusters*sizeof(int), 32);
	ldm_free(ldm_labels, buff_row_max*sizeof(int));
	ldm_free(ldm_se, buff_row_max*sizeof(float));
	aligned_free(ldm_data, row_max * simd_gdm * sizeof(float),64);
	aligned_free(ldm_cluster_center_old, simd_gdm*local_n_clusters*sizeof(float),64);
	aligned_free(ldm_cluster_center_buf, simd_gdm*local_n_clusters*sizeof(float),64);
	aligned_free(rma_buf, buff_row_max*2*cpe_group_size*sizeof(float), 64);
	aligned_free(rma_check, buff_row_max*sizeof(float), 64);
	aligned_free(ldm_min_candidate, local_n_clusters*sizeof(float), 64);
	// aligned_free(ldm_slave_time_array, 16*sizeof(double), 64);
}

int swKmeans_half(KmeansParam *param)
{
	int thread_id = athread_get_tid();
	int row_id = thread_id / 8;
	int col_id = thread_id % 8;
	int p_id = param->p_id;

	size_t n_dims = param->n_dims;
	size_t n_clusters = param->n_clusters;
	size_t grid_sum = param->grid_sum;
	size_t local_n_samples = param->local_n_samples;
	size_t n_load_row = param->n_load_row;
	size_t n_mix = param->n_mix;
	size_t n_neighbors = param->n_neighbors;
	float16 *cluster_center_old_16 = param->cluster_center_old_16;
	
	int is_write_labels = param->is_write_labels;
	int cpe_group_num = param->cpe_group_num;
	int cpe_group_size = param->cpe_group_size;

	int row_st = param->row_st;

	// float16 * cluster_center_buf_16 = param->cluster_center_buf_16;
	float * cluster_center_buf = param->cluster_center_buf;

	int * cluster_center_num_buf = param->cluster_center_num_buf;
	float16 * local_data_mpiin_16 = param->local_data_mpiin_16;
	int * labels = param->labels;
	// float16 * se_16 = param->se_16;
	float * se = param->se;
	double * slave_time_array = param->slave_time_array;

	int workload = n_load_row/(SLAVE_NUM/cpe_group_num) + (row_id<(n_load_row%(SLAVE_NUM/cpe_group_num))?1:0);
	int my_start = row_id * (n_load_row/(SLAVE_NUM/cpe_group_num)) + (row_id < (n_load_row%(SLAVE_NUM/cpe_group_num))?row_id:(n_load_row%(SLAVE_NUM/cpe_group_num)));

	int n_clusters_ = n_clusters;
	int nc_nd_ = param->nc_nd_;

	int local_n_clusters = param->local_n_clusters;
	int local_ncnd = param->local_ncnd;

	int row_max = param->row_max;

	// for mixed precision
	// row_max = row_max - 1;

	int row_round = (workload-1)/row_max+1; 
	int origin_gdm = param->origin_gdm;
	int simd_gdm = param->simd_gdm;
	int half_gdm = param->half_gdm;

	int row_batch_size = param->row_batch_size;
	int col_batch_size = param->col_batch_size;

	int i,j,k,min_index,min_col_id,this_min_index, post_min_index;
	int row_cnt, row, col, pos_row, pos_col, dma_expect_val;

    int buff_row_max, buff_row_round;
	// buff_row_max = row_max / 2;
    // buff_row_round = (workload-1)/buff_row_max+1;
    // row_cnt = buff_row_max;
	buff_row_max = row_max;
	buff_row_round = (workload-1) / buff_row_max + 1;
	row_cnt = buff_row_max;

	int* ldm_cluster_center_num = (int*)aligned_malloc(local_n_clusters*sizeof(int), 32);
	int* ldm_labels = (int*)ldm_malloc(buff_row_max*sizeof(int));
	// memset(ldm_labels, 0, buff_row_max*sizeof(int));
	memset(ldm_cluster_center_num, 0, local_n_clusters*sizeof(int));
	float16 * ldm_data_16;
	float16 * ldm_cluster_center_old_16;
	// float16 * ldm_cluster_center_buf_16;
	float * ldm_cluster_center_buf; 
	// float16 * ldm_se_16;
	float * ldm_se;
	float16 * rma_buf_16;
	float16 * rma_check_16;
	float16 * ldm_min_candidate_16;
	// double * ldm_slave_time_array;

	ldm_data_16 = (float16*)aligned_malloc(row_max * half_gdm * sizeof(float16), 64);
	ldm_cluster_center_old_16 = (float16*)aligned_malloc(half_gdm * local_n_clusters * sizeof(float16), 64);
	// ldm_cluster_center_buf_16 = (float16*)aligned_malloc(half_gdm * local_n_clusters * sizeof(float16), 64);
	ldm_cluster_center_buf = (float*)aligned_malloc(half_gdm * local_n_clusters * sizeof(float), 64);
	// ldm_se_16 = (float16*)ldm_malloc(buff_row_max*sizeof(float16));
	ldm_se = (float*)ldm_malloc(buff_row_max*sizeof(float));
	rma_buf_16 = (float16*)aligned_malloc(buff_row_max*2*cpe_group_size*sizeof(float16), 64);
	rma_check_16 = (float16*)aligned_malloc(buff_row_max*sizeof(float16), 64);
	ldm_min_candidate_16 = (float16*)aligned_malloc(local_n_clusters*sizeof(float16), 64);
	// ldm_slave_time_array = (double*)aligned_malloc(16*sizeof(double), 64);
	double ldm_slave_time_array[16];
	memset(ldm_data_16, 0, row_max*half_gdm*sizeof(float16));
	// memset(ldm_cluster_center_old, 0, simd_gdm*local_n_clusters * sizeof(float));
	// memset(ldm_cluster_center_buf_16, 0, half_gdm*local_n_clusters * sizeof(float16));
	memset(ldm_cluster_center_buf, 0, half_gdm*local_n_clusters*sizeof(float));
	// memset(ldm_se_16, 0, buff_row_max*sizeof(float16));
	memset(ldm_se, 0, buff_row_max*sizeof(float));

	volatile athread_rply_t reply_get_data = 0;
	volatile athread_rply_t reply_get_center = 0;
	volatile athread_rply_t reply_get_center_num = 0;
	volatile athread_rply_t reply_put_center_num = 0;
	volatile athread_rply_t reply_put_center = 0;
	volatile athread_rply_t reply_put_labels = 0;
	volatile athread_rply_t reply_put_se = 0;
	volatile athread_rply_t reply_get_se = 0;
	volatile athread_rply_t reply_rma_l=0;
	volatile athread_rply_t reply_rma_r=0;

	unsigned long centerin_st, centerin_ed, comp_st, comp_ed, resout_st, resout_ed;
	double centerin_time, comp_time, resout_time;

	unsigned long datain_st, datain_ed, localcomp_st, localcomp_ed,
	sendres_st, sendres_ed, globalcomp_st, globalcomp_ed,
	bcast_st, bcast_ed, verify_st, verify_ed, write_st, write_ed;
	unsigned long datain_acc = 0.0;
	unsigned long localcomp_acc = 0.0;
	unsigned long sendres_acc = 0.0;
	unsigned long globalcomp_acc = 0.0;
	unsigned long bcast_acc = 0.0;
	unsigned long verify_acc = 0.0;
	unsigned long write_acc = 0.0;
	double datain_time, localcomp_time, sendres_time, globalcomp_time, 
	bcast_time, verify_time, write_time;

	float16 tmp,min_dist,this_min_dist;	
	float16v32 v_ssrca;
	floatv8 ssrca[4];
	floatv8 ssrcb[4];
	floatv8 sdst[4];

	centerin_st = rpcc_slave();
	for(i=0;i<local_n_clusters;++i)
	{
		athread_dma_iget(ldm_cluster_center_old_16+i*half_gdm, cluster_center_old_16+(col_id*local_n_clusters+i)*origin_gdm, origin_gdm*sizeof(float16), &reply_get_center);
	}
	athread_dma_wait_value(&reply_get_center, local_n_clusters);
	centerin_ed = rpcc_slave();

	float16 * ldm_data_dma_16 = ldm_data_16;
    // float16 * ldm_data_cal_16 = ldm_data_16 + buff_row_max * half_gdm;
	float16 * ldm_data_cal_16 = ldm_data_16;

	comp_st = rpcc_slave();
	for(i=0;i<buff_row_round;++i)
	{
		datain_st = rpcc_slave();
        if(i==buff_row_round-1) row_cnt = workload-i*buff_row_max;
    	reply_get_data=0;
		athread_ssync(ROW_SCOPE,0xff);
    	for(j=0;j<row_cnt;++j)
    	{
        	row = (my_start + i*buff_row_max + j) / col_batch_size + n_neighbors;
			col = (my_start + i*buff_row_max + j) % col_batch_size + n_neighbors;
			if(col_id==row_id){
        	athread_dma_row_ibcast_stride(ldm_data_dma_16+j*half_gdm,
								local_data_mpiin_16+ CORD(row-n_neighbors,col-n_neighbors,col_batch_size+2*n_neighbors)*n_dims,
								n_dims*grid_sum*sizeof(float16),
								n_dims*(2*n_neighbors+1)*sizeof(float16), //bsize
								n_dims*(col_batch_size-1)*sizeof(float16),&reply_get_data);
			athread_dma_row_ibcast_stride(ldm_data_dma_16+j*half_gdm + n_dims * grid_sum,
								local_data_mpiin_16+ CORD(row-n_neighbors,col-n_neighbors,col_batch_size+2*n_neighbors)*n_dims+(row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors)*n_dims,
								n_dims*grid_sum*sizeof(float16),
								n_dims*(2*n_neighbors+1)*sizeof(float16), //bsize
								n_dims*(col_batch_size-1)*sizeof(float16),&reply_get_data);	
			}
    	}
    	athread_dma_wait_value(&reply_get_data, row_cnt*n_mix);
		datain_ed = rpcc_slave();
		datain_acc += datain_ed-datain_st;

		// cal distance & prepare rma buf
		localcomp_st = rpcc_slave();
		//////////////////////////////////////////////////////
        // for(j=0;j<row_cnt;++j)
        // {
		// 	min_dist = 65500.0;
		// 	min_index = 0;
		// 	for(k=0; k<local_n_clusters; ++k)
		// 	{
		// 		if(n_neighbors==1) //128
		// 		{
		// 			tmp = euler_dist_128_half(
		// 				ldm_data_cal_16+j*half_gdm,
		// 				ldm_cluster_center_old_16+k*half_gdm,
		// 				half_gdm
		// 			);
		// 		}
		// 		else if(n_neighbors==2) //320
		// 		{
		// 			tmp = euler_dist_320_half(
		// 				ldm_data_cal_16+j*half_gdm,
		// 				ldm_cluster_center_old_16+k*half_gdm,
		// 				half_gdm
		// 			);
		// 		}
		// 		else //608
		// 		{
		// 			tmp = euler_dist_608_half(
		// 				ldm_data_cal_16+j*half_gdm,
		// 				ldm_cluster_center_old_16+k*half_gdm,
		// 				half_gdm
		// 			);
		// 		}

		// 		if(tmp < min_dist)
		// 		{
		// 			min_dist = tmp;
		// 			min_index = k;
		// 		}

		// 	}
		// 	rma_buf_16[col_id*buff_row_max*2+j*2+0] = min_dist;
		// 	rma_buf_16[col_id*buff_row_max*2+j*2+1] = (float16)min_index;
		// }
		////////////////////////////////////////// 半精度向量融合计算 ////////////////////
		if(n_neighbors==1)
		{
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_128_half(
					ldm_data_cal_16+j*half_gdm,
					ldm_cluster_center_old_16,
					ldm_min_candidate_16,
					local_n_clusters
				);
				min_dist = 65500.0;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate_16[k] < min_dist) {min_dist = ldm_min_candidate_16[k]; min_index = k;}
				}
				rma_buf_16[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf_16[col_id*buff_row_max*2+j*2+1] = (float16)min_index;
			}
		}
		else if(n_neighbors==2)
		{
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_320_half(
					ldm_data_cal_16+j*half_gdm,
					ldm_cluster_center_old_16,
					ldm_min_candidate_16,
					local_n_clusters
				);
				min_dist = 65500.0;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate_16[k] < min_dist) {min_dist = ldm_min_candidate_16[k]; min_index = k;}
				}
				rma_buf_16[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf_16[col_id*buff_row_max*2+j*2+1] = (float16)min_index;
			}
		}
		else
		{
			for(j=0;j<row_cnt;++j)
			{
				group_euler_dist_640_half(
					ldm_data_cal_16+j*half_gdm,
					ldm_cluster_center_old_16,
					ldm_min_candidate_16,
					local_n_clusters
				);
				min_dist = 65500.0;
				min_index = 0;
				for(k=0;k<local_n_clusters;++k)
				{
					if(ldm_min_candidate_16[k] < min_dist) {min_dist = ldm_min_candidate_16[k]; min_index = k;}
				}
				rma_buf_16[col_id*buff_row_max*2+j*2+0] = min_dist;
				rma_buf_16[col_id*buff_row_max*2+j*2+1] = (float16)min_index;
			}
		}

		localcomp_ed = rpcc_slave();
		localcomp_acc += localcomp_ed - localcomp_st;

		// send local result
		sendres_st = rpcc_slave();
        reply_rma_l=0;
		reply_rma_r=0;
		athread_ssync(ROW_SCOPE, 0xff);
		if(col_id==0)
		{
			for(k=1;k<8;++k) athread_rma_iget(rma_buf_16+k*buff_row_max*2,&reply_rma_l,row_cnt*2*sizeof(float16),thread_id+k,rma_buf_16+k*buff_row_max*2,&reply_rma_r);
		}
		else
		{
			athread_rma_wait_value(&reply_rma_r,1);
		}
		if(col_id==0) athread_rma_wait_value(&reply_rma_l,7);
		sendres_ed = rpcc_slave();
		sendres_acc += sendres_ed - sendres_st;

        // cal global result
		globalcomp_st = rpcc_slave();
        if(col_id==0)
		{
			for(j=0;j<row_cnt;++j)
			{
				min_dist = 65500.0;
				for(k=0;k<8;++k)
				{
					if(rma_buf_16[k*buff_row_max*2+j*2+0]<min_dist)
					{
						min_col_id = k;
						min_dist = rma_buf_16[k*buff_row_max*2+j*2+0];
						min_index = (int)rma_buf_16[k*buff_row_max*2+j*2+1];
					}
				}
				// ldm_se_16[j] += min_dist;
				ldm_se[j] += (float)min_dist;
				ldm_labels[j] = min_col_id*local_n_clusters+min_index;
				rma_check_16[j] = (float16)min_col_id;
			}
		}
		globalcomp_ed = rpcc_slave();
		globalcomp_acc += globalcomp_ed - globalcomp_st;

		// broadcast the result
		bcast_st = rpcc_slave();
        reply_rma_l=0;
		reply_rma_r=0;
		athread_ssync(ROW_SCOPE, 0xff);
        if(col_id==0)
		{
			athread_rma_row_ibcast(rma_check_16,rma_check_16,row_cnt*sizeof(float16),&reply_rma_l,&reply_rma_r);
			athread_rma_wait_value(&reply_rma_l,1);
		}
		else athread_rma_wait_value(&reply_rma_r,1);
		bcast_ed = rpcc_slave();
		bcast_acc += bcast_ed - bcast_st;

		// verify the global result
		verify_st = rpcc_slave();
        for(j=0;j<row_cnt;++j)
		{
			// float16 vs float
			if(rma_check_16[j]==col_id)
			{
				post_min_index = (int)rma_buf_16[col_id*buff_row_max*2+j*2+1];
				ldm_cluster_center_num[post_min_index]++;
				// float16v32 -> floatv8
				for(k=0;k<half_gdm/SIMD_SIZE_H;++k)
				{
					simd_load(v_ssrca, &(ldm_data_cal_16[j*half_gdm + k*SIMD_SIZE_H]));
					// 一个float16v32转成4个floatv8
					simd_trans_hf_sf_v2v(&v_ssrca, &ssrca);
					simd_load(ssrcb[0], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H]));
					simd_load(ssrcb[1], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+8]));
					simd_load(ssrcb[2], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+16]));
					simd_load(ssrcb[3], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+24]));
					sdst[0] = ssrca[0] + ssrcb[0];
					sdst[1] = ssrca[1] + ssrcb[1];
					sdst[2] = ssrca[2] + ssrcb[2];
					sdst[3] = ssrca[3] + ssrcb[3];
					simd_store(sdst[0], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H]));
					simd_store(sdst[1], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+8]));
					simd_store(sdst[2], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+16]));
					simd_store(sdst[3], &(ldm_cluster_center_buf[post_min_index*half_gdm+k*SIMD_SIZE_H+24]));
				}
			}
		}
		verify_ed = rpcc_slave();
		verify_acc += verify_ed - verify_st;

		// write labels
		write_st = rpcc_slave();
        if(col_id==0&&is_write_labels)
		{
			reply_put_labels=0;
			athread_dma_iput(labels+my_start+i*buff_row_max,ldm_labels,row_cnt*sizeof(int),&reply_put_labels);
			athread_dma_wait_value(&reply_put_labels,1);
		}
		write_ed = rpcc_slave();
		write_acc += write_ed - write_st;
	}
	comp_ed = rpcc_slave();

	// output se & center buf & center num buf
	resout_st = rpcc_slave();
    if(col_id==0)
	{
		reply_put_se=0;
		// athread_dma_iput(se_16+row_id*buff_row_max,ldm_se_16,buff_row_max*sizeof(float16),&reply_put_se);
		athread_dma_iput(se+row_id*buff_row_max,ldm_se,buff_row_max*sizeof(float),&reply_put_se);
		athread_dma_wait_value(&reply_put_se,1);
	}
    reply_put_center=0;
	for(i=0;i<local_n_clusters;++i)
	{
		// athread_dma_iput(cluster_center_buf_16+row_id*origin_gdm*n_clusters+(col_id*local_n_clusters+i)*origin_gdm, ldm_cluster_center_buf_16+i*half_gdm, origin_gdm*sizeof(float16), &reply_put_center);
		athread_dma_iput(cluster_center_buf+row_id*origin_gdm*n_clusters+(col_id*local_n_clusters+i)*origin_gdm, ldm_cluster_center_buf+i*half_gdm, origin_gdm*sizeof(float), &reply_put_center);
	}
	athread_dma_wait_value(&reply_put_center, local_n_clusters);

	reply_put_center_num=0;
	athread_dma_iput(cluster_center_num_buf+row_id*n_clusters+col_id*local_n_clusters,ldm_cluster_center_num,local_n_clusters*sizeof(int),&reply_put_center_num);
	athread_dma_wait_value(&reply_put_center_num,1);
	resout_ed = rpcc_slave();

	ldm_slave_time_array[0] = (centerin_ed-centerin_st)/2.25/1e9;
	ldm_slave_time_array[1] = (comp_ed-comp_st)/2.25/1e9;
	ldm_slave_time_array[2] = (resout_ed-resout_st)/2.25/1e9;

	ldm_slave_time_array[3] = datain_acc/2.25/1e9;
	ldm_slave_time_array[4] = localcomp_acc/2.25/1e9;
	ldm_slave_time_array[5] = sendres_acc/2.25/1e9;
	ldm_slave_time_array[6] = globalcomp_acc/2.25/1e9;
	ldm_slave_time_array[7] = bcast_acc/2.25/1e9;
	ldm_slave_time_array[8] = verify_acc/2.25/1e9;
	ldm_slave_time_array[9] = write_acc/2.25/1e9;

	if(p_id==0&&thread_id==0)
	{
		reply_put_center_num=0;
		athread_dma_iput(slave_time_array, ldm_slave_time_array, 10*sizeof(double),&reply_put_center_num);
		athread_dma_wait_value(&reply_put_center_num,1);
	}

    aligned_free(ldm_cluster_center_num, local_n_clusters*sizeof(int), 32);
	ldm_free(ldm_labels, buff_row_max*sizeof(int));
	// ldm_free(ldm_se_16, buff_row_max*sizeof(float16));
	ldm_free(ldm_se, buff_row_max*sizeof(float));
	aligned_free(ldm_data_16, row_max * half_gdm * sizeof(float16),64);
	aligned_free(ldm_cluster_center_old_16, half_gdm*local_n_clusters*sizeof(float16),64);
	// aligned_free(ldm_cluster_center_buf_16, half_gdm*local_n_clusters*sizeof(float16),64);
	aligned_free(ldm_cluster_center_buf, half_gdm*local_n_clusters*sizeof(float),64);
	aligned_free(rma_buf_16, buff_row_max*2*cpe_group_size*sizeof(float16), 64);
	aligned_free(rma_check_16, buff_row_max*sizeof(float16), 64);
	aligned_free(ldm_min_candidate_16, local_n_clusters*sizeof(float16), 64);
}

