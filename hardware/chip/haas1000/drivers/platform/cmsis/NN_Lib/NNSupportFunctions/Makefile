cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
#obj-y := $(filter-out %arm_nn_vec_mat_mult_t_s8.c,$(obj-y))
#obj-y := $(filter-out %arm_nn_mat_mult_nt_t_s8.c,$(obj-y))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)
