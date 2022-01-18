REM ~ ..\cpp -D__i386__ -I"D:\eli\cpp_stuff\libc_include" -D__extension__ example_c_file.c > example_c_file_pp.c
REM ~ ..\cpp -D__i386__ -I"D:\eli\c_analyzing\pycparser-trunk\utils\fake_libc_include" example_c_file.c > example_c_file_pp.c
..\cpp -D__i386__ -I"D:\eli\c_analyzing\pycparser-trunk\utils\fake_libc_include" zc.c > zc_pp.c
