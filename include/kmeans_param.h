
// 5a
// #define SIMD_SIZE_I 8
// #define SIMD_SIZE_F 4

// 9a
#define SIMD_SIZE_I 16
#define SIMD_SIZE_F 8
#define SIMD_SIZE_H 32

// #define LDM_CAPACITY 16000 // 62.5KB 16000 float
#define LDM_CAPACITY 64000 // 250KB 64000 float
#define HALF_CAPACITY 128000

#define SLAVE_NUM 64

// #define CORD(row, col, col_size) (row*col_size+col)

typedef struct KmeansParam_{
	int p_id;
	int p_num;
	int local_p_num;
	int local_id;
	char * data_config_file;
	char * output_dir;
	// char * local_file_name;

	int * offset_local_p_num;
	int * local_p_num_st;
	int * local_p_num_ed;

	size_t nlines;
	size_t local_n_samples; // l_n
	size_t offset_n_samples;
	size_t global_offset_n_samples;
	size_t n_dims; // d
	size_t width;
	size_t height;
	size_t n_samples; // n
	size_t total_n_samples;
	size_t n_clusters; // k
	size_t n_neighbors; //neighbors
	size_t grid_sum;
	int n_mix;
	int max_batch_size;
	int n_batch;
	int n_load_row;
	int nc_nd_;
	int row_max;
	int is_write_labels;
	int cpe_group_num;
	int cpe_group_size;
	int local_n_clusters;
	int local_ncnd;
	// int level;
	int is_half;
	
	int row_batch_size;
	int col_batch_size;
	int row_batch_count;
	int col_batch_count;
	int row_st;
	int row_ed;
	int col_st;
	int col_ed;

	int origin_gdm;
	int simd_gdm;
	int half_gdm;

	// MPI_Comm new_comm;
	// int my_new_comm_rank;

	// MPI_File fp_t, fp_f;

	// char *file_name[2];
	char * grow_t_file_name[1600];
	char * grow_f_file_name[1600];
	// char ** grow_t_file_name;
	// char ** grow_f_file_name;

	char * local_grow_t_file_name;
	char * local_grow_f_file_name;
	char * center_file_name;
	char * labels_file_name;

	int * stencil_idx;
	size_t local_file_id;
	int *all_width;
	int *all_height;
	int *total_offset_n_samples;
	int *cluster_center_num; // k
	float * memory_pool_center_data;
	int * memory_pool_center_num;
	float *cluster_center_new; // k*d
	float *cluster_center_old; // k*d
	float *cluster_center_buf; // k*d float
	int *cluster_center_num_buf; // d int
	float *local_data; // l_n*d
	float *local_data_mpiin;
	int *labels; // l_n
	float *se; // l_n
	float *se_reduce;
	double * slave_time_array;

	short * local_data_mpiin_16;
	short * cluster_center_old_16;
	short * cluster_center_new_16;
	short * cluster_center_buf_16;
	short * se_16;
	short * se_reduce_16;
	short * memory_pool_center_data_16;

} KmeansParam;









