#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <crts.h>
#include <sys/time.h>
#include <athread.h>
#include <simd.h>
#include <math.h>
#include "mpi.h"
#include "kmeans_param.h"
#include "trans.h"

#define THRESHOLD 1e-6
#define MAXITER 100
#define CHECK(err, err_code) if(err) { return err_code; }
#define CHECK_ERROR(ret, err_code) CHECK(ret != 0, err_code)

extern void SLAVE_FUN(swKmeans_float());
extern void SLAVE_FUN(swKmeans_half());
inline int CORD(int row, int col, int col_size) {return row*col_size+col;}

inline int int_min(int a, int b) {return a>b?b:a;}

inline int int_max(int a, int b) {return a>b?a:b;}

void swap_ptr(float **a, float **b){
	float* tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

int cal_avg(int sum, int n, int id) {return sum/n+(id<(sum%n)?1:0);}

int cal_avg_offset(int sum, int n, int id) {return (sum/n)*id+int_min(id,sum%n);}

unsigned long rpcc_usec()
{
	struct timeval tm;
	gettimeofday(&tm, NULL);
	return (tm.tv_sec*1e6 + tm.tv_usec);
}


int parse_arg(KmeansParam *param, char ** argv)
{
    // ./kmeans 24.txt 24 6 304 1 ./dummy_centers.txt ./dummy_labels.txt ./dummy_info.txt 2 0
    // ./kemans 24.txt       24        6      304      1        2          0         ./save_labels/
    // ./kmeans 图片目录   图片数量   通道数   类别数   邻居数   季节数   是否混合精度     输出目录
    // argv        1          2        3        4       5        6          7             8
    param->data_config_file = (char*)malloc(sizeof(char)*64);
    param->output_dir = (char*)malloc(sizeof(char)*64);
    strcpy(param->data_config_file, argv[1]); 
    param->nlines = atoi(argv[2]);
	param->n_dims = atoi(argv[3]);
	param->n_clusters = atoi(argv[4]);
	param->n_neighbors = atoi(argv[5]);
	param->n_mix = atoi(argv[6]);
	param->is_half = atoi(argv[7]);
    strcpy(param->output_dir, argv[8]);
}

void setup_parameters(KmeansParam *param)
{
    int i, count;
    assert(param->n_mix == 2);
    assert(param->p_num >= param->nlines);
    // CPU setup
    param->cpe_group_num = 8;
    param->cpe_group_size = 64 / param->cpe_group_num;

    // Auxiliary variable
	param->grid_sum = (2*param->n_neighbors+1)*(2*param->n_neighbors+1);
	param->nc_nd_ = param->n_clusters*param->n_dims*param->grid_sum*param->n_mix;
	param->local_n_clusters = param->n_clusters / param->cpe_group_size;
	param->local_ncnd = param->nc_nd_ / param->cpe_group_size;
	param->origin_gdm = param->grid_sum*param->n_dims*param->n_mix;
	// param->simd_gdm = ((param->origin_gdm - 1) / SIMD_SIZE_F + 1) * SIMD_SIZE_F;
	if(param->grid_sum==9) param->simd_gdm = 112;
	else if(param->grid_sum==25) param->simd_gdm = 320;
	else param->simd_gdm = 616;

	// param->half_gdm = ((param->origin_gdm - 1) / SIMD_SIZE_H + 1) * SIMD_SIZE_H;
	if(param->grid_sum==9) param->half_gdm = 128;
	else if(param->grid_sum==25) param->half_gdm = 320;
	else param->half_gdm = 640;

    // param->row_max = (LDM_CAPACITY - (2*param->local_n_clusters*(param->grid_sum*param->n_dims*param->n_mix+4)+param->local_n_clusters))/((param->grid_sum*param->n_dims*param->n_mix+4)+2+(2*8+1+1)/2);
    // param->row_max = 
	// (LDM_CAPACITY - (2*param->local_n_clusters*
	// (param->grid_sum*param->n_dims*param->n_mix+4)+param->local_n_clusters))
	// /((param->grid_sum*param->n_dims*param->n_mix+4)+2+(2*8+1+1)/2);
	if(param->is_half == 0)
	{
		// label, se, buf, check
		param->row_max = (LDM_CAPACITY-2*param->simd_gdm*param->local_n_clusters-2*param->local_n_clusters)/
		(param->simd_gdm+3+2*param->cpe_group_size);
	}
	else
	{
		param->row_max = (HALF_CAPACITY-3*param->half_gdm*param->local_n_clusters-4*param->local_n_clusters)/
		(param->half_gdm+6+2*param->cpe_group_size);
		//param->row_max /= 2;
	}

    param->row_max = param->row_max / 4 * 4; 
	if(param->p_id==0) printf("row_max=%d\n",param->row_max);

    // Malloc filename & Process data config file
    for(i=0;i<param->nlines;++i)
	{
		param->grow_t_file_name[i]=(char*)malloc(sizeof(char)*128);
		param->grow_f_file_name[i]=(char*)malloc(sizeof(char)*128);
	}
	param->local_grow_t_file_name=(char*)malloc(sizeof(char)*128);
	param->local_grow_f_file_name=(char*)malloc(sizeof(char)*128);
	param->center_file_name=(char*)malloc(sizeof(char)*128);
	param->labels_file_name=(char*)malloc(sizeof(char)*128);
    param->all_width = (int*)malloc(sizeof(int) * param->nlines);
	param->all_height = (int*)malloc(sizeof(int) * param->nlines);

    FILE * fp;
    char buf[1024];
    fp = fopen(param->data_config_file, "r");
	const char split[2] = " ";
	count = 0;
    while(fgets(buf,sizeof(buf),fp)!=NULL)
	{
		strcpy(param->grow_t_file_name[count],strtok(buf,split));
		strcpy(param->grow_f_file_name[count],strtok(NULL,split));
		param->all_width[count]=atoi(strtok(NULL,split));
		param->all_height[count]=atoi(strtok(NULL,split));
		count++;
	}
    fclose(fp);

    // Processes id for each file : [st, ed)
    param->local_p_num_st = (int*)malloc(param->nlines*sizeof(int));
	param->local_p_num_ed = (int*)malloc(param->nlines*sizeof(int));
    param->total_n_samples = 0;

    // maybe not necessary
    param->total_offset_n_samples = (int*)malloc(sizeof(int) * param->nlines);

    // File global setup 
    param->local_p_num=param->p_num / param->nlines;
	param->local_id = param->p_id % param->local_p_num;
	for(i=0;i<param->nlines;++i)
	{
		param->local_p_num_st[i]=i*param->local_p_num;
		param->local_p_num_ed[i]=(i+1)*param->local_p_num;
		param->total_n_samples+=param->all_width[i]*param->all_height[i];
		// total_offset_n_samples 每个文件第一个数的偏移
		if (i == 0)
		{
			param->total_offset_n_samples[i] = 0;
		}
		else
		{
			param->total_offset_n_samples[i] = param->total_offset_n_samples[i-1]+param->all_width[i - 1] * param->all_height[i - 1];
		}
		if(param->p_id>=param->local_p_num_st[i]&&
		   param->p_id<param->local_p_num_ed[i])
		{
			param->local_file_id=i;
		}
	}

    // File local setup
    param->width=param->all_width[param->local_file_id];
	param->height=param->all_height[param->local_file_id];
	param->n_samples=param->width*param->height;
    strcpy(param->local_grow_t_file_name,"data/");
	strcpy(param->local_grow_f_file_name,"data/");
	strcat(param->local_grow_t_file_name,
		   param->grow_t_file_name[param->local_file_id]);
	strcat(param->local_grow_f_file_name,
		   param->grow_f_file_name[param->local_file_id]);
}

void setup_storage(KmeansParam *param)
{
    // assert main memory is adequate, thus all batchsize == 1
    int local_id = param->local_id;
    param->row_batch_count=1;
	param->col_batch_count=1;
	param->row_batch_size = cal_avg(param->height, param->local_p_num, local_id);
	param->col_batch_size = param->width;
	param->row_st = cal_avg_offset(param->height, param->local_p_num, local_id);
	param->row_ed = param->row_st + param->row_batch_size;
	param->col_st = 0;
	param->col_ed = param->width;
	param->max_batch_size = param->row_batch_size * param->col_batch_size;
	param->n_batch=1;

    // keep in main memory
    param->local_data_mpiin = (float*)malloc((param->row_batch_size+2*param->n_neighbors)*(param->col_batch_size+2*param->n_neighbors)*param->n_mix*param->n_dims*sizeof(float));
	param->local_data_mpiin_16 = (short*)malloc((param->row_batch_size+2*param->n_neighbors)*(param->col_batch_size+2*param->n_neighbors)*param->n_mix*param->n_dims*sizeof(short));
    param->labels=(int*)malloc(param->max_batch_size*sizeof(int));
	param->se=(float*)malloc(64*param->row_max*sizeof(float));
	param->se_reduce=(float*)malloc(param->row_max*sizeof(float));

    // malloc for single and mixed precision
    param->memory_pool_center_data = (float*)malloc((64+2)*param->nc_nd_*sizeof(float));
	param->memory_pool_center_num = (int*)malloc((64+1)*param->n_clusters*sizeof(int));
	param->cluster_center_num = param->memory_pool_center_num;
	param->cluster_center_num_buf = param->cluster_center_num+param->n_clusters;
 	param->cluster_center_old = param->memory_pool_center_data;
 	param->cluster_center_new = param->cluster_center_old+param->nc_nd_;
 	param->cluster_center_buf = param->cluster_center_new+param->nc_nd_;

	param->memory_pool_center_data_16 = (short*)malloc((64+2)*param->nc_nd_*sizeof(short));
	param->cluster_center_old_16 = param->memory_pool_center_data_16;
	param->cluster_center_new_16 = param->cluster_center_old_16+param->nc_nd_;
 	param->cluster_center_buf_16 = param->cluster_center_new_16+param->nc_nd_;
	// param->se_16 = (short*)malloc(64*param->row_max*sizeof(short));
	// param->se_reduce_16 = (short*)malloc(param->row_max*sizeof(short));

	param->slave_time_array = (double*)malloc(16*sizeof(double));
}

void openfile(KmeansParam *param, MPI_File* fp_t, MPI_File* fp_f, MPI_File* fp_l, MPI_Comm* new_comm)
{
	int ret_t = MPI_File_open(*new_comm, param->local_grow_t_file_name, MPI_MODE_RDONLY, MPI_INFO_NULL,fp_t);
	if (ret_t != MPI_SUCCESS){
 		printf("MPI file T open failure! id = %d\n",param->p_id);
 	}

	int ret_f = MPI_File_open(*new_comm, param->local_grow_f_file_name, MPI_MODE_RDONLY, MPI_INFO_NULL,fp_f);
	if (ret_f != MPI_SUCCESS){
 		printf("MPI file F open failure! id = %d\n",param->p_id);
 	}

    strcpy(param->labels_file_name, param->output_dir);
    strcat(param->labels_file_name, param->grow_t_file_name[param->local_file_id]);
	int ret = MPI_File_open(MPI_COMM_SELF, param->labels_file_name, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, fp_l);
	if(ret!=MPI_SUCCESS){
        printf("MPI file L open failure! id = %d open error code = %d\n",param->p_id, ret);
    }
}

void closefile(KmeansParam *param, MPI_File* fp_t, MPI_File* fp_f, MPI_File* fp_l)
{
	MPI_File_close(fp_t);
	MPI_File_close(fp_f);
	MPI_File_close(fp_l);
}

int init_cluster_center(KmeansParam *param, MPI_File *fp_t, MPI_File *fp_f)
{
    // related to data structure
    int i, j, k, row, col;

	int n_neighbors = param->n_neighbors;
	int grid_sum = param->grid_sum;
	int n_mix = param->n_mix;
	int n_dims = param->n_dims;
	int row_batch_size = param->row_batch_size;
	int col_batch_size = param->col_batch_size;
	int max_batch_size = param->max_batch_size;
	float * cluster_center_old = param->cluster_center_old;
	float * local_data_mpiin = param->local_data_mpiin;

    srand((unsigned)time(NULL));
    for(i=0; i<param->n_clusters; ++i)
    {
        j = rand()%max_batch_size;
		row = j / col_batch_size + n_neighbors;
		col = j % col_batch_size + n_neighbors;
        for(k=-(n_neighbors);k<=n_neighbors;++k)
		{
			memcpy(cluster_center_old+i*n_dims*grid_sum*n_mix + (k+n_neighbors)*(2*n_neighbors+1)*n_dims,
				   local_data_mpiin + CORD(row+k,col-n_neighbors,(col_batch_size+2*n_neighbors))*n_dims,
				   sizeof(float)*(2*n_neighbors+1)*n_dims
			);
			memcpy(cluster_center_old+i*n_dims*grid_sum*n_mix + (k+n_neighbors)*(2*n_neighbors+1)*n_dims+n_dims*grid_sum,
				   local_data_mpiin + (row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors) * n_dims
				   +CORD(row+k,col-n_neighbors,(col_batch_size+2*n_neighbors))*n_dims,
				   sizeof(float)*(2*n_neighbors+1)*n_dims
			);
		}
    }
    return 1;
}

int read_data(KmeansParam *param, MPI_File *fp_t, MPI_File *fp_f)
{
	int i,j,k,ret,row,col,pos_row,pos_col,local_row,local_col_st,local_col_len;
	MPI_Status status_t;
	MPI_Status status_f;
	// row_st row_ed col_st col_ed 参数提前算好
	// row_batch_count col_batch_count 总batch 数为这两项的积
	// row_batch_size col_batch_size
	// 当前batch : row_batch_id col_batch_id
	int width = param->width;
	int height = param->height;
	int n_dims = param->n_dims;
	int n_neighbors = param->n_neighbors;
	int n_mix = param->n_mix;
	int grid_sum = param->grid_sum;
	int row_batch_size = param->row_batch_size;
	int col_batch_size = param->col_batch_size;
	
	int row_st = param->row_st;
	int row_ed = param->row_ed;
	int col_st = param->col_st;
	int col_ed = param->col_ed;

	// int anchor_x_cord = row_st + row_batch_id * row_batch_size;
	// int anchor_y_cord = col_st + col_batch_id * col_batch_size;
	int anchor_x_cord = row_st;
	int anchor_y_cord = col_st;
	int x_st = anchor_x_cord - n_neighbors;
	int x_ed = anchor_x_cord + row_batch_size + n_neighbors;
	// local_data_mpiin 中的实际位置

	// int y_st = int_max(anchor_y_cord - n_neighbors, 0);
	// int y_ed = int_min(anchor_y_cord + col_batch_size + n_neighbors, width);
	param->n_load_row = row_batch_size * col_batch_size;
	memset(param->local_data_mpiin,0,(row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors)*n_mix*n_dims*sizeof(float));
	for(row=x_st;row<x_ed;++row)
	{
		if(row<0||row>=height) continue;
		local_row = row-x_st;
		// local_col_st = n_neighbors - (anchor_y_cord - y_st);
		local_col_st = n_neighbors;
		// local_col_ed = n_neighbors + col_batch_size + (y_ed - (anchor_y_cord + col_batch_size));
		local_col_len = col_batch_size;
		ret = MPI_File_read_at(*fp_t,
							   (row*width + 0)*n_dims*sizeof(float),
							   param->local_data_mpiin + (local_row*(col_batch_size+2*n_neighbors) + local_col_st)*n_dims,
							   local_col_len * n_dims,	
							   MPI_FLOAT, &status_t);	
		ret = MPI_File_read_at(*fp_f,
							   (row*width + 0)*n_dims*sizeof(float),
							   param->local_data_mpiin + \
							   (local_row*(col_batch_size+2*n_neighbors)+local_col_st+(row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors))*n_dims,
							   local_col_len * n_dims,	
							   MPI_FLOAT, &status_f);	
		// if(param->p_id==0 && row%100==0) printf("row = %d st = %d ed = %d\n",row,x_st,x_ed);
	}
	if(param->is_half)
	{
		sw_float_trans_half(
			param->local_data_mpiin,
			param->local_data_mpiin_16,
			(row_batch_size+2*n_neighbors)*(col_batch_size+2*n_neighbors)*n_mix*n_dims
		);
	}
	return 1;
}

void train(KmeansParam *param,MPI_File *fp_t,MPI_File *fp_f)
{
    int i, j;
	unsigned long f2h_st, f2h_ed, h2f_st, h2f_ed, slave_st, slave_ed, mpereduce_st, mpereduce_ed;
	unsigned long reducebuf_st, reducebuf_ed, reducenum_st, reducenum_ed, reducese_st, reducese_ed;
	unsigned long reducenew_st, reducenew_ed;
	double f2h_time, h2f_time, slave_time, mpereduce_time, reducebuf_time, reducenum_time, reducese_time, reducenew_time;

    if(param->is_half == 0)
    {
        memset(param->cluster_center_buf,0,64*sizeof(float)*param->nc_nd_);
        memset(param->se,0,64*sizeof(float)*param->row_max);
    }
    else
    {
        // memset(param->cluster_center_buf_16,0,64*sizeof(short)*param->nc_nd_);
		memset(param->cluster_center_buf,0,64*sizeof(float)*param->nc_nd_);
        // memset(param->se_16,0,64*sizeof(short)*param->row_max);
        memset(param->se,0,64*sizeof(float)*param->row_max);
    }
    memset(param->cluster_center_num_buf,0,64*sizeof(int)*param->n_clusters);

    assert(param->n_batch == 1);

	f2h_st = rpcc_usec();
	if(param->is_half)//&& iter==0)
	{
		sw_float_trans_half(
			param->cluster_center_old,
			param->cluster_center_old_16,
			param->nc_nd_
		);
	}
	f2h_ed = rpcc_usec();

	slave_st = rpcc_usec();
	if(param->is_half==0)
	{
    	athread_spawn(swKmeans_float, param);
		athread_join();
	}
	else
	{
		athread_spawn(swKmeans_half, param);
		athread_join();
	}
	slave_ed = rpcc_usec();

    // half2float for MPI OP
    int reduce_size = param->cpe_group_num;
	h2f_st = rpcc_usec();
    // if(param->is_half)
    // {
    //     // sw_half_trans_float(param->cluster_center_buf_16,param->cluster_center_buf,reduce_size*param->nc_nd_);
    //     // sw_half_trans_float(param->se_16,param->se,reduce_size*param->row_max);
    // }
	h2f_ed = rpcc_usec();

	mpereduce_st = rpcc_usec();
    for(i=1; i<reduce_size; ++i)
    {
        for(j=0; j<param->nc_nd_; ++j)
        {
            param->cluster_center_buf[j]+=param->cluster_center_buf[i*param->nc_nd_+j];
        }
        for(j=0; j<param->row_max; ++j)
        {
            param->se[j]+=param->se[i*param->row_max+j];
        }
        for(j=0; j<param->n_clusters; ++j)
        {
            param->cluster_center_num_buf[j]+=param->cluster_center_num_buf[i*param->n_clusters+j];
        }
    }
	mpereduce_ed = rpcc_usec();
    // MPI_Barrier(MPI_COMM_WORLD);

	reducebuf_st = rpcc_usec();
	MPI_Allreduce(param->cluster_center_buf, param->cluster_center_new, param->n_clusters*param->n_dims * param->grid_sum*param->n_mix, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
	reducebuf_ed = rpcc_usec();
	reducenum_st = rpcc_usec();
	MPI_Allreduce(param->cluster_center_num_buf, param->cluster_center_num, param->n_clusters, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	reducenum_ed = rpcc_usec();
	reducese_st = rpcc_usec();
	MPI_Reduce(param->se,param->se_reduce,param->row_max,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);
	reducese_ed = rpcc_usec();
    // MPI_Barrier(MPI_COMM_WORLD);

	reducenew_st = rpcc_usec();
	for(i=0; i<param->n_clusters; i++){
		if(param->cluster_center_num[i] == 0) continue;
		for(j=0;j<param->n_dims*param->grid_sum*param->n_mix;j++){
			param->cluster_center_new[i*param->n_dims*param->grid_sum*param->n_mix+j] /= param->cluster_center_num[i];
		}
	}  
	reducenew_ed = rpcc_usec();  
    swap_ptr(&param->cluster_center_new, &param->cluster_center_old);

	f2h_time = (f2h_ed-f2h_st)/1e6;
	slave_time = (slave_ed-slave_st)/1e6;
	h2f_time = (h2f_ed-h2f_st)/1e6;
	mpereduce_time = (mpereduce_ed-mpereduce_st)/1e6;
	reducebuf_time = (reducebuf_ed-reducebuf_st)/1e6;
	reducenum_time = (reducenum_ed-reducenum_st)/1e6;
	reducese_time = (reducese_ed-reducese_st)/1e6;
	reducenew_time = (reducenew_ed-reducenew_st)/1e6;

	if(param->p_id==0)
	{
		printf("f2h       = %f\n",f2h_time);
		printf("slave     = %f\n",slave_time);
		printf("h2f       = %f\n",h2f_time);
		printf("mpereduce = %f\n",mpereduce_time);
		printf("reducebuf = %f\n",reducebuf_time);
		printf("reducenum = %f\n",reducenum_time);
		printf("reducese  = %f\n",reducese_time);
		printf("reducenew = %f\n",reducenew_time);

		printf("***** slave inner *****\n");
		printf("slave_centerin time = %f\n", param->slave_time_array[0]);
		printf("slave_comp time     = %f\n", param->slave_time_array[1]);
		printf("slave_resout time   = %f\n", param->slave_time_array[2]);

		printf("***** comp inner *****\n");

		printf("datain time     = %f\n", param->slave_time_array[3]);
		printf("localcomp time  = %f\n", param->slave_time_array[4]);
		printf("sendres time    = %f\n", param->slave_time_array[5]);
		printf("globalcomp time = %f\n", param->slave_time_array[6]);
		printf("bcast time      = %f\n", param->slave_time_array[7]);
		printf("verify time     = %f\n", param->slave_time_array[8]);
		printf("write time      = %f\n", param->slave_time_array[9]);
	}

}

void write_data(KmeansParam *param,MPI_File *fp_l)
{
    int ret,batch,i,j,k,reduce_size,row,local_row;
    MPI_Status status;
	int width = param->width;
	int row_st = param->row_st;
	int row_ed = param->row_ed;
	int col_st = param->col_st;
	int col_ed = param->col_ed;
	int row_batch_size = param->row_batch_size;
	int col_batch_size = param->col_batch_size;
    if(param->is_half == 0)
    {
        memset(param->cluster_center_buf,0,64*sizeof(float)*param->nc_nd_);
        memset(param->se,0,64*sizeof(float)*param->row_max);
    }
    else
    {
        memset(param->cluster_center_buf_16,0,64*sizeof(short)*param->nc_nd_);
        // memset(param->se_16,0,64*sizeof(short)*param->row_max);
        memset(param->se,0,64*sizeof(float)*param->row_max);
    }
    memset(param->cluster_center_num_buf,0,64*sizeof(int)*param->n_clusters);

    assert(param->n_batch == 1);
	if(param->is_half)//&& iter==0)
	{
		sw_float_trans_half(
			param->cluster_center_old,
			param->cluster_center_old_16,
			param->nc_nd_
		);
	}

	if(param->is_half==0)
	{
    	athread_spawn(swKmeans_float, param);
		athread_join();
	}
	else
	{
		athread_spawn(swKmeans_half, param);
		athread_join();
	}

    ret = MPI_File_write_at(*fp_l,
							row_st*width*sizeof(int),
							param->labels,
							row_batch_size * col_batch_size,
							MPI_INT, &status);
	if(ret!=MPI_SUCCESS) printf("MPI write at error, pid = %d, error code = %d\n",param->p_id, ret);

    reduce_size = param->cpe_group_num;
    // if(param->is_half) sw_half_trans_float(param->se_16,param->se,reduce_size*param->row_max);
    for(i=1; i<reduce_size; ++i) param->se[j]+=param->se[i*param->row_max+j];
    MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(param->se,param->se_reduce,param->row_max,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
}




int main(int argc, char **argv)
{
    KmeansParam *param = (KmeansParam*)malloc(sizeof(KmeansParam));
    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &(param->p_num));
	MPI_Comm_rank(MPI_COMM_WORLD, &(param->p_id));
	athread_init();

    parse_arg(param, argv);
    setup_parameters(param);
    setup_storage(param);

    MPI_Barrier(MPI_COMM_WORLD);
    if(param->p_id==0)
    {
        printf("after setup\n");
    }

    MPI_File fp_t, fp_f, fp_c, fp_l;
    int i, j, k;

	unsigned long over_st, over_ed;
	double over_time;

    int colour, key;
    colour = param->p_id / param->nlines;
	key = param->p_id % param->nlines;
	MPI_Comm new_comm;
	MPI_Comm_split(MPI_COMM_WORLD, colour, key, &new_comm);
	int my_new_comm_rank;
	MPI_Comm_rank(new_comm, &my_new_comm_rank);
    openfile(param, &fp_t, &fp_f, &fp_l, &new_comm);


    MPI_Barrier(MPI_COMM_WORLD);
    if(param->p_id==0)
    {
        printf("before read data\n");
    }

    // Read data
    // assert only read data once
    int ret;
    MPI_Barrier(MPI_COMM_WORLD);
    ret = read_data(param, &fp_t, &fp_f);
	param->n_load_row = param->row_batch_size * param->col_batch_size;
    MPI_Barrier(MPI_COMM_WORLD);
    if(param->p_id==0) printf("Finish read!\n");

    // assert only train
    if(param->p_id==0) ret = init_cluster_center(param, &fp_t, &fp_f);
    MPI_Bcast(param->cluster_center_old, 
			  param->nc_nd_, 
			  MPI_FLOAT, 0, MPI_COMM_WORLD);

    // begin train
    int iter = 0;
    param->is_write_labels = 0;
    while(iter < MAXITER)
    {
		// 计时代码 		
		over_st = rpcc_usec();
        train(param, &fp_t, &fp_f);
		over_ed = rpcc_usec();
		over_time = (over_ed-over_st)/1e6;

        if(param->p_id == 0)
        {
            for(i=1;i<param->row_max;++i) param->se_reduce[0]+=param->se_reduce[i];
			printf("iter %d,sse %f\n",iter,param->se_reduce[0]);
			printf("time = %f\n",over_time);
			if(param->is_half==0)
			{
				printf("Gflops = %f\n",13568*13568/1e9*3*param->simd_gdm*param->n_clusters*param->nlines/over_time);
				printf("Tflops = %f\n",13568*13568/1e12*3*param->simd_gdm*param->n_clusters*param->nlines/over_time);
			}
			else
			{
				printf("Gflops = %f\n",13568*13568/1e9*3*param->half_gdm*param->n_clusters*param->nlines/over_time);
				printf("Tflops = %f\n",13568*13568/1e12*3*param->half_gdm*param->n_clusters*param->nlines/over_time);
			}

        }
        iter++;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // begin generate label
    param->is_write_labels = 1;

    write_data(param, &fp_l);
    MPI_Barrier(MPI_COMM_WORLD);
    if(param->p_id == 0)
    {
        // why row_max / 2; double buffer
        for(i=1;i<param->row_max;++i) param->se_reduce[0]+=param->se_reduce[i];
		printf("Final, sse %f\n",param->se_reduce[0]);
    }
	closefile(param,&fp_t,&fp_f,&fp_l);
	athread_halt();
	MPI_Finalize();
	return 0;    
}












