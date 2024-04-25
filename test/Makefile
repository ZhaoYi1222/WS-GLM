LINK = mpicc
SWCC = mpicc
MFLAGS= -mhost -O2 -lm -msimd -mieee -g -w
SFLAGS= -mslave -O3 -lm_slave -fbranch-predict-hint -mfma -funroll-all-loops -msimd #-faddress_align=64

all: kmeans
kmeans:master.o slave.o
	$(LINK) master.o slave.o -o kmeans -lm -Wl,-zmuldefs

master.o:master.c
	$(SWCC) $(MFLAGS) -c master.c

slave.o:slave.c
	$(SWCC) $(SFLAGS) -c slave.c

test_float:
	bsub -b -I -q q_share -n 1 -cgsp 64 -shared -share_size 15500 -cache_size 0 -host_stack 1024 ./kmeans 0

clean:
	rm -rf *.o kmeans