.PHONY: clean all

all:
	@gcc main.c traverse.c ../kv.c ../kv_util.c ../sim/kv_bd.c ../sim/aos/hal/flash.c -I.. -I../sim -DKV_SIM_DEBUG -DCONFIG_KV_WRITE_IMMEDIATE_VERIFY -g -o test
	@gcc bin_test.c traverse.c ../kv.c ../kv_util.c ../sim/kv_bd.c ../sim/aos/hal/flash.c -I.. -I../sim -DKV_SIM_DEBUG -DCONFIG_KV_WRITE_IMMEDIATE_VERIFY -g -o bin.test

clean:
	@rm -fr *.o a.out ./test ./bin.test
