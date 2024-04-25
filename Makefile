LINK = mpicc
SWCC = mpicc
MFLAGS= -mhost -O2 -lm -msimd -Iinclude -Iutils/trans/include/ -mieee -g -w
SFLAGS= -mslave -O3 -lm_slave -fbranch-predict-hint -mfma -funroll-all-loops -msimd -Iinclude -Iutils/trans/include/ #-faddress_align=64
TRANSSFLAGS= -msimd -Iutils/trans/include/ -mieee -g -w -O2 -fPIC -mslave
TRANSMFLAGS= -msimd -Iutils/trans/include/ -mieee -g -w -O2 -fPIC -mhost

all: kmeans
kmeans:master.o sw_hf_trans.o slave.o sw_slave_hf_trans.o trans_datatype.o
	$(LINK) master.o sw_hf_trans.o slave.o sw_slave_hf_trans.o trans_datatype.o -o kmeans -lm -Wl,-zmuldefs

master.o:src/master.c
	$(SWCC) $(MFLAGS) -c src/master.c
sw_hf_trans.o:utils/trans/sw_hf_trans.c
	$(SWCC) $(TRANSMFLAGS) -c utils/trans/sw_hf_trans.c

slave.o:src/slave.c
	$(SWCC) $(SFLAGS) -c src/slave.c
sw_slave_hf_trans.o:utils/trans/sw_slave_hf_trans.c
	$(SWCC) $(TRANSSFLAGS) -c utils/trans/sw_slave_hf_trans.c
trans_datatype.o:utils/trans/trans_datatype.c
	$(SWCC) $(TRANSSFLAGS) -c utils/trans/trans_datatype.c

# test_96_3:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 1 2 0 ./save_labels/

# test_96_5:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 2 2 0 ./save_labels/

# test_96_7:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 3 2 0 ./save_labels/


# test_304_3:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 1 2 0 ./save_labels/

# test_304_5:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 2 2 0 ./save_labels/

# test_304_7:
# 	bsub -b -I -q q_sw_expr -n 32 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 3 2 0 ./save_labels/


############################### float ###########################
test_96_3:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 1 2 0 ./save_labels/

test_96_5:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 2 2 0 ./save_labels/

test_96_7:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 3 2 0 ./save_labels/


test_304_3:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 1 2 0 ./save_labels/

test_304_5:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 2 2 0 ./save_labels/

test_304_7:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 3 2 0 ./save_labels/


test_96_3_o:
	bsub -b -o ./96_3.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 1 2 0 ./save_labels/

test_96_5_o:
	bsub -b -o ./96_5.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 2 2 0 ./save_labels/

test_96_7_o:
	bsub -b -o ./96_7.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 3 2 0 ./save_labels/

test_304_3_o:
	bsub -b -o ./304_3.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 1 2 0 ./save_labels/

test_304_5_o:
	bsub -b -o ./304_5.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 2 2 0 ./save_labels/

test_304_7_o:
	bsub -b -o ./304_7.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 3 2 0 ./save_labels/

############################### half ###########################
test_96_3_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 1 2 1 ./save_labels/

test_96_5_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 2 2 1 ./save_labels/

test_96_7_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 3 2 1 ./save_labels/


test_304_3_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 1 2 1 ./save_labels/

test_304_5_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 2 2 1 ./save_labels/

test_304_7_h:
	bsub -b -I -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 3 2 1 ./save_labels/


test_96_3_o_h:
	bsub -b -o ./96_3_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 1 2 1 ./save_labels/

test_96_5_o_h:
	bsub -b -o ./96_5_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 2 2 1 ./save_labels/

test_96_7_o_h:
	bsub -b -o ./96_7_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 96 3 2 1 ./save_labels/

test_304_3_o_h:
	bsub -b -o ./304_3_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 1 2 1 ./save_labels/

test_304_5_o_h:
	bsub -b -o ./304_5_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 2 2 1 ./save_labels/

test_304_7_o_h:
	bsub -b -o ./304_7_h.txt -q q_share -N 1 -np 6 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans ./config_file/one.txt 1 6 304 3 2 1 ./save_labels/



clean:
	rm -rf *.o kmeans
