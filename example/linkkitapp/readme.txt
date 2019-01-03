
1.modify device&product info at the header of linkkit_example_*.c or newapi/*.c

2.modify tsl string at data/*.data

3.make:
    Notice: Please be sure to use git bash when build on Windows! Otherwise may resulting in run problem for linkkit. git bash download url https://git-scm.com/download/win

    1).linkkit_example_solo.c(default):
        aos make clean
        aos make linkkitapp@xxxx   

    2).linkkit_example_sched.c:
        aos make clean
        aos make linkkitapp@xxxx case=sched

    3).linkkit_example_cntdown.c:
        aos make clean
        aos make linkkitapp@xxxx case=cntdown

    4).deprecated/solo.c
        aos make clean
        aos make linkkitapp@xxxx deprecated=1      

    5).deprecated/cntdown.c
        aos make clean
        aos make linkkitapp@xxxx deprecated=1  case=cntdown  

    5).deprecated/sched.c
        aos make clean
        aos make linkkitapp@xxxx deprecated=1  case=sched    

4.extended compile options
    1)use print_heap=1 to print free heap size periodically  
        example:aos make linkkitapp@espmk3080 print_heap=1 
   
    2)append macro ON_DAILY/ON_PRE to GLOBAL_DEFINES to enable daily/pre env
