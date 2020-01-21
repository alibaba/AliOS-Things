## Contents

```sh
debug_app
├── debug_app.c    # helloworld source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **debug_app** example will show how-to-use api in aos/debug.h  by cli cmd, the example is simple.

### Dependencies

* debug
* cli
* kv

### Supported Boards

- arch for ARM and Xtensa

### Build

```sh
# generate helloworld@developerkit default config
aos make debug_app@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result
debug app show demo example:
1. aos_debug_mm_overview
            ========== Heap Info  ==========
            ---------------------------------------------------------------------------
            [HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
            | 0x00100000 | 0x000DC040 | 0x00023FC0 | 0x000DBE30 | 0x000DA0C0    |
            ---------------------------------------------------------------------------
            [POOL]| PoolSz     | FreeSz     | UsedSz     | BlkSz      | MaxFreeBlkSz  |
            | 0x00002000 | 0x00001F80 | 0x00000080 | 0x00000020 | 0x00000020    |
            ---------------------------------------------------------------------------

2. aos_debug_task_overview
            ========== Task Info  ==========
            --------------------------------------------------------------------------
            TaskName             State    Prio       Stack      StackSize (MinFree)
            --------------------------------------------------------------------------
            dyn_mem_proc_task    PEND     0x00000005 0x0806BDA0 0x00004000(0x00003BD0)
            idle_task            RDY      0x0000008C 0x0806FF20 0x00004000(0x0000367C)
            idle_task            RDY      0x0000008C 0x080740A0 0x00004000(0x000036CC)
            DEFAULT-WORKQUEUE    PEND     0x00000014 0x08078220 0x00004000(0x00003CCC)
            timer_task           PEND     0x00000005 0x0807C3A0 0x00004000(0x00003D3C)
            app_task             SLP      0x00000014 0x08082620 0x00004000(0x000037C8)
            cpu_intr             PEND     0x00000000 0x080867A0 0x00004000(0x00003D6C)
            cli                  RDY      0x0000003C 0x0808B708 0x00004000(0x00001340)

 3. aos_debug_queue_overview
            ========== Queue Info ==========
            -------------------------------------------------------
            QueAddr    TotalSize  PeakNum    CurrNum    TaskWaiting
            -------------------------------------------------------

4. aos_debug_buf_queue_overview
            ======== Buf Queue Info ========
            ------------------------------------------------------------------
            BufQueAddr TotalSize  PeakNum    CurrNum    MinFreeSz  TaskWaiting
            ------------------------------------------------------------------
            0x0816AB00 0x000001E0 0x00000000 0x00000000 0x000001E0 timer_task
            0x0808A988 0x00000100 0x00000001 0x00000000 0x000000FB

5. aos_debug_sem_overview
            =========== Sem Info ===========
            --------------------------------------------
            SemAddr    Count      PeakCount  TaskWaiting
            --------------------------------------------
            0x0816A180 0x00000000 0x00000000 dyn_mem_proc_task
            0x0816A3B4 0x00000000 0x00000000 DEFAULT-WORKQUEUE
            0x08069620 0x00000000 0x00000000 cpu_intr
            0x0808F908 0x00000000 0x00000000

6.  aos_debug_overview will show all above

7. aos_debug_backtrace_now or aos_debug_backtrace_now(task)
            ========== Call stack ==========
            backtrace : 0x080071A4
            backtrace : 0x08007374
            backtrace : 0x08010942
            backtrace : 0x08006834
            backtrace : 0x08006A50
            backtrace : ^task entry^
            ==========    End     ==========

```sh
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
