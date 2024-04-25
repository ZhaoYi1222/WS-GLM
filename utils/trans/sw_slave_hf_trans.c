#include <stdio.h>
#include <math.h>
#include <string.h>
#include <crts.h>
#include <simd.h>
#include "trans.h"
#include "trans_datatype.h"
void sw_slave_float_trans_half(float_trans_half *p)
{
	crts_rply_t get_reply_a,put_reply_a;
	int core_number=64;
	int vec=32;
	int forward_size_half=98304;//98kb
        int i,j,a_start,MAX,k,remain_data_p,each_data;
        float *a,*a_slave;
        unsigned long all_count,ldm_all_count;
        _Float16 *b,*b_slave;
        all_count=p->num;
        a=(float *)p->A;
        b=(_Float16 *)p->HA;
        MAX=forward_size_half/sizeof(float);
        int my_id=(int)CRTS_smng_get_tid();
        get_reply_a=0;
        put_reply_a=0;
	int N=all_count/(core_number*2);
        int N_count=all_count%(core_number*2);
        int N_count_remain=N_count/2;
	N=N*2;
        if(my_id<=N_count_remain)
        {
                a_start=my_id*(N+2);
        }
        else{
                a_start=(N+2)*N_count_remain+(my_id-N_count_remain)*N;
        }
        if(my_id < N_count_remain)
        {
                N=N+2;
        }
        if(N==0)
        {
                return;
        }
	int sig_idx;
#ifdef DEBUG_SLAVE_VAL
        p->sig_slave[my_id] = 0;
#endif

 	a_slave=(float *)ldm_malloc(MAX*sizeof(float));
        b_slave=(_Float16 *)ldm_malloc(MAX*sizeof(_Float16));

        int remain_data=N%MAX;
        int ve_count=N/MAX;
	for(i=0;i<ve_count;i++)
        {
  		CRTS_dma_iget(a_slave,a+a_start+i*MAX,MAX*sizeof(float),&get_reply_a);
                CRTS_dma_wait_value(&get_reply_a,1);
                simd_trans_a(MAX,a_slave,b_slave);
                CRTS_dma_iput(b+a_start+i*MAX,b_slave,MAX*sizeof(_Float16),&put_reply_a);
                CRTS_dma_wait_value(&put_reply_a,1);
#ifdef DEBUG_SLAVE_VAL
                for(sig_idx=0; sig_idx<MAX; ++sig_idx)
                {
                        if(b_slave[sig_idx]!=b_slave[sig_idx])
                        {
                                p->sig_slave[my_id] = 1;
                                break;
                        }
                }
#endif

                get_reply_a=0;
                put_reply_a=0;
//		if(my_id==0)
//		{		
//			printf("mode 1\n");
//		}
	}
 	if(remain_data!=0)
        {
      		CRTS_dma_iget(a_slave,a+a_start+ve_count*MAX,remain_data*sizeof(float),&get_reply_a);
                CRTS_dma_wait_value(&get_reply_a,1);
                simd_trans_a(remain_data,a_slave,b_slave);
		CRTS_dma_iput(b+a_start+ve_count*MAX,b_slave,remain_data*sizeof(_Float16),&put_reply_a);
                CRTS_dma_wait_value(&put_reply_a,1);
#ifdef DEBUG_SLAVE_VAL          
                for(sig_idx=0; sig_idx<remain_data; ++sig_idx)
                {
                        if(b_slave[sig_idx]!=b_slave[sig_idx])
                        {
                                p->sig_slave[my_id] = 1;
                                break;
                        }
                }
#endif
//		if(my_id==0)
//		{
//			printf("mode 2\n");
//		}
                get_reply_a=0;
                put_reply_a=0;
		
	}
        ldm_free(a_slave,MAX*sizeof(float));
        ldm_free(b_slave,MAX*sizeof(_Float16));
}

void sw_slave_half_trans_float(half_trans_float *p)
{
    // if(CRTS_tid==0)printf("Run sw_slave_half_trans_float\n");
    crts_rply_t get_reply_a,put_reply_a;
	int core_number=64;
	int vec=32;
	int forward_size_half=98304;//98kb
        int i,j,a_start,MAX,k,remain_data_p,each_data;
        _Float16 *a,*a_slave;
        unsigned long all_count,ldm_all_count;
        float *b,*b_slave;
        all_count=p->num;
        a=(_Float16 *)p->HA;
        b=p->A;
        MAX=forward_size_half/sizeof(float);
        int my_id=(int)CRTS_smng_get_tid();
        get_reply_a=0;
        put_reply_a=0;
 	int N=all_count/(core_number*2);
        int N_count=all_count%(core_number*2);
	int N_count_remain=N_count/2;
	N=N*2;
	if(my_id<=N_count_remain)
        {
                a_start=my_id*(N+2);
        }
        else{
                a_start=(N+2)*N_count_remain+(my_id-N_count_remain)*N;
        }
        if(my_id < N_count_remain)
        {
                N=N+2;
        }
        if(N==0)
        {
                return;
        }
        int sig_idx;

#ifdef DEBUG_SLAVE_VAL
        p->sig_slave[my_id] = 0;
#endif
 	b_slave=(float *)ldm_malloc(MAX*sizeof(float));
        a_slave=(_Float16 *)ldm_malloc(MAX*sizeof(_Float16));

        int remain_data=N%MAX;
        int ve_count=N/MAX;
	float aa;
	for(i=0;i<ve_count;i++)
        {
  		CRTS_dma_iget(a_slave,a+a_start+i*MAX,MAX*sizeof(_Float16),&get_reply_a);
                CRTS_dma_wait_value(&get_reply_a,1);
                simd_trans_hf_sf(MAX,a_slave,b_slave);
                CRTS_dma_iput(b+a_start+i*MAX,b_slave,MAX*sizeof(float),&put_reply_a);
                CRTS_dma_wait_value(&put_reply_a,1);
#ifdef DEBUG_SLAVE_VAL
                for(sig_idx=0; sig_idx<MAX; ++sig_idx)
                {
                        if(b_slave[sig_idx]!=b_slave[sig_idx])
                        {
                                p->sig_slave[my_id] = 1;
                                break;
                        }
                }
#endif

                get_reply_a=0;
                put_reply_a=0;
//		if(my_id==0)
//		{
//			printf("mode 3\n");
//		}
	}
 	if(remain_data!=0)
        {
      		CRTS_dma_iget(a_slave,a+a_start+ve_count*MAX,remain_data*sizeof(_Float16),&get_reply_a);
                CRTS_dma_wait_value(&get_reply_a,1);
                simd_trans_hf_sf(remain_data,a_slave,b_slave);
		CRTS_dma_iput(b+a_start+ve_count*MAX,b_slave,remain_data*sizeof(float),&put_reply_a);
#ifdef DEBUG_SLAVE_VAL          
                for(sig_idx=0; sig_idx<remain_data; ++sig_idx)
                {
                        if(b_slave[sig_idx]!=b_slave[sig_idx])
                        {
                                p->sig_slave[my_id] = 1;
                                break;
                        }
                }
#endif

                CRTS_dma_wait_value(&put_reply_a,1);
                get_reply_a=0;
                put_reply_a=0;
//		if(my_id==0)
//		{
//			printf("mode 4\n");
//		}
	}
        ldm_free(b_slave,MAX*sizeof(float));
        ldm_free(a_slave,MAX*sizeof(_Float16));
}



