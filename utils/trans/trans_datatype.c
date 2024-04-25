#include <stdio.h>
#include <math.h>
#include <string.h>
#include <simd.h>
#include "sf.h"

//trans float to half-float
void simd_trans_a(int num,float *a_slave, _Float16 *a_slaveH){
	floatv8 v1,v2,v3,v4;
	float16v32 vh1,vh2,vh3,vh4,vsh1,vsh2,vhf;
	intv16 sh1 = simd_set_intv16(0x51349141,0x99617595,0xF79D71B6,0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh2 = simd_set_intv16(0x440C2040,0xA2481C61,0x3CE34C2C,0x648E2860,0xAA689E69,0xBEEB6CAE,0,0,0,0,0,0,0,0,0,0);
	int i;
	for(i=0; i+31<num;i+=32){
		simd_load(v1, &a_slave[i]);
		simd_load(v2, &a_slave[i+8]);
		simd_load(v3, &a_slave[i+2*8]);
		simd_load(v4, &a_slave[i+3*8]);
		vh1 = simd_vfcvtsh(v1, 1);
		vh2 = simd_vfcvtsh(v2, 1);
		vh3 = simd_vfcvtsh(v3, 1);
		vh4 = simd_vfcvtsh(v4, 1);
		vsh1=simd_vshfh(vh1,vh2,sh1);
		vsh2=simd_vshfh(vh3,vh4,sh1);
		vhf=simd_vshfh(vsh1,vsh2,sh2);
		simd_store(vhf, &a_slaveH[i]);
	}
	int j;
        for(j=i;j<num;j++)
        {
                a_slaveH[j]=(_Float16)a_slave[j];
        }
}

//trans half-float to float
void simd_trans_hf_sf(int num,_Float16 *c_slaveH, float *c_slave){
	floatv8 v1,v2,v3,v4,vsh1,vsh2,vsh3,vsh4,vsh21,vsh22,vsh23,vsh24;
	float16v32 vhf;
	intv16 sh11 = simd_set_intv16(0x8628c020, 0x6AD0A49C, 0xBD8E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh12 = simd_set_intv16(0x96ACE128, 0xEEF1ACDE, 0xFF9E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh21 = simd_set_intv16(0xA3018820, 0x4398A49C, 0xBDAB, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh22 = simd_set_intv16(0xB385A928, 0xC7B9ACDE, 0xFFBB, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	int i;
	for(i=0; i+31<num; i+=32){
		simd_load(vhf, c_slaveH+i);
		v1 = simd_vfcvths(vhf, 0);
		v2 = simd_vfcvths(vhf, 1);
		v3 = simd_vfcvths(vhf, 2);
		v4 = simd_vfcvths(vhf, 3);
		vsh1 = simd_vshuffles(v1,v2,sh11);	
		vsh2 = simd_vshuffles(v1,v2,sh12);	
		vsh3 = simd_vshuffles(v3,v4,sh11);	
		vsh4 = simd_vshuffles(v3,v4,sh12);	
		
		vsh21 = simd_vshuffles(vsh1,vsh3,sh21);
		vsh22 = simd_vshuffles(vsh1,vsh3,sh22); 
		vsh23 = simd_vshuffles(vsh2,vsh4,sh21);
		vsh24 = simd_vshuffles(vsh2,vsh4,sh22);

		simd_store(vsh21, &c_slave[i+0]);	
		simd_store(vsh22, &c_slave[i+8]);
		simd_store(vsh23, &c_slave[i+16]);
		simd_store(vsh24, &c_slave[i+24]);
	}
    int j;
    for(j=i;j<num;j++)
    {
        c_slave[j]=(float)c_slaveH[j];
    }
}






float simd_trans_hf_sf_add(int num,_Float16 *c_slaveH)
{

        floatv8 v1,v2,v3,v4,vsh1,vsh2,vsh3,vsh4,vsh21,vsh22,vsh23,vsh24;
        float16v32 vhf;
        intv16 sh11 = simd_set_intv16(0x8628c020, 0x6AD0A49C, 0xBD8E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
        intv16 sh12 = simd_set_intv16(0x96ACE128, 0xEEF1ACDE, 0xFF9E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
        intv16 sh21 = simd_set_intv16(0xA3018820, 0x4398A49C, 0xBDAB, 0,0,0,0,0,0,0,0,0,0,0,0,0);
        intv16 sh22 = simd_set_intv16(0xB385A928, 0xC7B9ACDE, 0xFFBB, 0,0,0,0,0,0,0,0,0,0,0,0,0);
        int i;
        float sum=0.0;
        for(i=0; i+31<num; i+=32){
                simd_load(vhf, c_slaveH+i);
                v1 = simd_vfcvths(vhf, 0);
                v2 = simd_vfcvths(vhf, 1);
                v3 = simd_vfcvths(vhf, 2);
                v4 = simd_vfcvths(vhf, 3);
                vsh1 = simd_vshuffles(v1,v2,sh11);
                vsh2 = simd_vshuffles(v1,v2,sh12);
                vsh3 = simd_vshuffles(v3,v4,sh11);
                vsh4 = simd_vshuffles(v3,v4,sh12);

                vsh21 = simd_vshuffles(vsh1,vsh3,sh21);
                vsh22 = simd_vshuffles(vsh1,vsh3,sh22);
                vsh23 = simd_vshuffles(vsh2,vsh4,sh21);
                vsh24 = simd_vshuffles(vsh2,vsh4,sh22);
                sum=sum+simd_reduc_pluss(vsh21);
		//printf("sum is %f\n",sum);
                sum=sum+simd_reduc_pluss(vsh22);
                sum=sum+simd_reduc_pluss(vsh23);
                sum=sum+simd_reduc_pluss(vsh24);
	}
	int j;
	for(j=i;j<num;j++)
	{               
                sum=sum+(float)c_slaveH[j];
	}
	return sum;
}

void simd_trans_hf_sf_v2v(float16v32 *vc_slaveH, floatv8 *vc_slave)
{
	floatv8 v1,v2,v3,v4,vsh1,vsh2,vsh3,vsh4,vsh21,vsh22,vsh23,vsh24;
	float16v32 vhf;
	intv16 sh11 = simd_set_intv16(0x8628c020, 0x6AD0A49C, 0xBD8E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh12 = simd_set_intv16(0x96ACE128, 0xEEF1ACDE, 0xFF9E, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh21 = simd_set_intv16(0xA3018820, 0x4398A49C, 0xBDAB, 0,0,0,0,0,0,0,0,0,0,0,0,0);
	intv16 sh22 = simd_set_intv16(0xB385A928, 0xC7B9ACDE, 0xFFBB, 0,0,0,0,0,0,0,0,0,0,0,0,0);

	// simd_load(vhf, vc_slaveH);
	v1 = simd_vfcvths(vc_slaveH[0], 0);
	v2 = simd_vfcvths(vc_slaveH[0], 1);
	v3 = simd_vfcvths(vc_slaveH[0], 2);
	v4 = simd_vfcvths(vc_slaveH[0], 3);
	vsh1 = simd_vshuffles(v1,v2,sh11);	
	vsh2 = simd_vshuffles(v1,v2,sh12);	
	vsh3 = simd_vshuffles(v3,v4,sh11);	
	vsh4 = simd_vshuffles(v3,v4,sh12);	
	vsh21 = simd_vshuffles(vsh1,vsh3,sh21);
	vsh22 = simd_vshuffles(vsh1,vsh3,sh22); 
	vsh23 = simd_vshuffles(vsh2,vsh4,sh21);
	vsh24 = simd_vshuffles(vsh2,vsh4,sh22);
	vc_slave[0] = vsh21;
	vc_slave[1] = vsh22;
	vc_slave[2] = vsh23;
	vc_slave[3] = vsh24;
}















