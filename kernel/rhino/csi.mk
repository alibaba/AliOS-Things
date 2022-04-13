##
 # Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##

INCDIR += -I$(KERNELDIR)/rhino/arch/include
INCDIR += -I$(KERNELDIR)/rhino/core/include
ifeq ($(CONFIG_KERNEL_PWR_MGMT), y)
INCDIR += -I$(KERNELDIR)/rhino/common
INCDIR += -I$(KERNELDIR)/rhino/pwrmgmt
endif
INCDIR += -I$(LIBSDIR)/mm
INCDIR += -I$(LIBSDIR)/trace/include

KERNEL_CSRC += $(KERNELDIR)/rhino/driver/systick.c
KERNEL_CSRC += $(KERNELDIR)/rhino/driver/hook_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/driver/hook_weak.c
KERNEL_CSRC += $(KERNELDIR)/rhino/driver/yoc_impl.c

KERNEL_CSRC += $(KERNELDIR)/rhino/adapter/csi_rhino.c

KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_buf_queue.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_dyn_mem_proc.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_err.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_event.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_idle.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm_debug.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm_blk.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm_region.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm_firstfit.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mm_bestfit.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_ringbuf.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_mutex.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_obj.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_pend.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_queue.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_sem.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_stats.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_sys.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_task.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_task_sem.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_tick.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_time.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_timer.c
KERNEL_CSRC += $(KERNELDIR)/rhino/core/k_workqueue.c

KERNEL_CSRC += $(KERNELDIR)/rhino/common/k_atomic.c
KERNEL_CSRC += $(KERNELDIR)/rhino/common/k_ffs.c

ifeq ($(CONFIG_KERNEL_PWR_MGMT), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/board/board_cpu_pwr.c
KERNEL_CSRC += $(KERNELDIR)/rhino/board/board_cpu_pwr_systick.c
KERNEL_CSRC += $(KERNELDIR)/rhino/board/board_cpu_pwr_timer.c

KERNEL_CSRC += $(KERNELDIR)/rhino/pwrmgmt/cpu_pwr_hal_lib.c
KERNEL_CSRC += $(KERNELDIR)/rhino/pwrmgmt/cpu_pwr_lib.c
KERNEL_CSRC += $(KERNELDIR)/rhino/pwrmgmt/cpu_pwr_show.c
KERNEL_CSRC += $(KERNELDIR)/rhino/pwrmgmt/cpu_tickless.c
endif

ifeq ($(CONFIG_SUPPORT_TSPEND), y)
IS_TSPEND=tspend
else
IS_TSPEND=non_tspend
endif

ifeq ($(CONFIG_SYSTEM_SECURE), y)
IS_SECURE=security
else
IS_SECURE=non_security
endif

ifeq ($(CONFIG_CPU_E802)$(CONFIG_CPU_S802)$(CONFIG_CPU_E802T)$(CONFIG_CPU_S802T)$(CONFIG_CPU_CK802), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/802/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/802/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/802/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/802/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/802/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_E803)$(CONFIG_CPU_S803)$(CONFIG_CPU_E803T)$(CONFIG_CPU_S803T)$(CONFIG_CPU_CK803), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/803/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_E804D)$(CONFIG_CPU_E804DT)$(CONFIG_CPU_CK803ER1)$(CONFIG_CPU_CK803ER2)$(CONFIG_CPU_CK803ER3), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804d/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804d/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804d/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/804d/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804d/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_E804F)$(CONFIG_CPU_E804FT)$(CONFIG_CPU_CK803F), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804f/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804f/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804f/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/804f/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804f/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_CK803EF), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803ef_32gpr/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803ef_32gpr/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803ef_32gpr/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/803ef_32gpr/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/803ef_32gpr/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_E804DF)$(CONFIG_CPU_E804DFT)$(CONFIG_CPU_CK804EF)$(CONFIG_CPU_CK803EFR1)$(CONFIG_CPU_CK803EFR2)$(CONFIG_CPU_CK803EFR3), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804df/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804df/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804df/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/804df/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/804df/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_I805)$(CONFIG_CPU_I805F)$(CONFIG_CPU_CK805)$(CONFIG_CPU_CK805F), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/805/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/805/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/805/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/805/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/805/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_C807)$(CONFIG_CPU_R807), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/807/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_C807F)$(CONFIG_CPU_C807FV)$(CONFIG_CPU_R807F), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807f/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807f/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807f/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/807f/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/807f/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_C810)$(CONFIG_CPU_C810V)$(CONFIG_CPU_C810T)$(CONFIG_CPU_C810TV), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/810/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/810/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/810/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/810/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/810/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_CK610E), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/ck610/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_CK610EF), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610f/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610f/csky_sched.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610f/dump_backtrace.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/csky/ck610f/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/csky/ck610f/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_ARCH_RV32)$(CONFIG_ARCH_RV32EMC), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/riscv/rv32_16gpr/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/riscv/rv32_16gpr/csky_sched.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/riscv/rv32_16gpr/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/riscv/rv32_16gpr/$(IS_SECURE)/port_c.c
endif

ifeq ($(CONFIG_CPU_CM0), y)
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/arm/cm0/cpu_impl.c
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/arm/cm0/csky_sched.c
KERNEL_SSRC += $(KERNELDIR)/rhino/arch/arm/cm0/$(IS_TSPEND)/port_s.S
KERNEL_CSRC += $(KERNELDIR)/rhino/arch/arm/cm0/$(IS_SECURE)/port_c.c
endif
