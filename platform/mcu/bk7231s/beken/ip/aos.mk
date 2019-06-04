NAME := ip

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := 

$(NAME)_INCLUDES := ke \
					mac \
					lmac/src/hal \
					lmac/src/mm \
					lmac/src/ps \
					lmac/src/rd \
					lmac/src/rwnx \
					lmac/src/rx \
					lmac/src/scan \
					lmac/src/sta \
					lmac/src/tx \
					lmac/src/vif \
					lmac/src/rx/rxl \
					lmac/src/tx/txl \
					lmac/src/tpc \
					lmac/src/tdls \
					lmac/src/p2p \
					lmac/src/chan \
					lmac/src/td \
					umac/src/bam \
					umac/src/llc \
					umac/src/me \
					umac/src/rxu \
					umac/src/scanu \
					umac/src/sm \
					umac/src/txu \
					umac/src/apm \
					umac/src/mesh \
					umac/src/rc 
					
$(NAME)_INCLUDES += ../app/standalone-ap \
                    ../app/standalone-station \
					../driver/sdio \
                    ../driver/uart \
                    ../driver/sys_ctrl \
                    ../driver/gpio \
                    ../driver/common/reg \
                    ../driver/dma \
                    ../driver/intc \
                    ../driver/phy \
                    ../driver/rc_beken \
                    ../driver/general_dma \
                    ../driver/spidma \
                    ../driver/rw_pub \
                    ../func/sdio_intf \
                    ../func/power_save \
					../func/temp_detect \
                    ../func/spidma_intf \
                    ../func/ethernet_intf \
                    ../func/rwnx_intf \
					../func/rf_test 
					
$(NAME)_INCLUDES += ../func/hostapd-2.5/src/utils \
                    ../func/hostapd-2.5/src \
                    ../func/hostapd-2.5/bk_patch \
                    ../func/hostapd-2.5/src/ap \
                    ../func/hostapd-2.5/hostapd \
                    ../func/hostapd-2.5/src/common \
                    ../func/hostapd-2.5/src/drivers 
					
$(NAME)_SOURCES	:=  common/co_dlist.c \
					common/co_list.c \
					common/co_math.c \
					common/co_pool.c \
					common/co_ring.c \
					ke/ke_env.c \
					ke/ke_event.c \
					ke/ke_msg.c \
					ke/ke_queue.c \
					ke/ke_task.c \
					ke/ke_timer.c \
					lmac/src/chan/chan.c \
					lmac/src/hal/hal_desc.c \
					lmac/src/hal/hal_dma.c \
					lmac/src/hal/hal_machw.c \
					lmac/src/hal/hal_mib.c \
					lmac/src/mm/mm.c \
					lmac/src/mm/mm_bcn.c \
					lmac/src/mm/mm_task.c \
					lmac/src/mm/mm_timer.c \
					lmac/src/p2p/p2p.c \
					lmac/src/ps/ps.c \
					lmac/src/rd/rd.c \
					lmac/src/rwnx/rwnx.c \
					lmac/src/rx/rx_swdesc.c \
					lmac/src/rx/rxl/rxl_cntrl.c \
					lmac/src/rx/rxl/rxl_hwdesc.c \
					lmac/src/scan/scan.c \
					lmac/src/scan/scan_shared.c \
					lmac/src/scan/scan_task.c \
					lmac/src/sta/sta_mgmt.c \
					lmac/src/td/td.c \
					lmac/src/tdls/tdls.c \
					lmac/src/tdls/tdls_task.c \
					lmac/src/tpc/tpc.c \
					lmac/src/tx/tx_swdesc.c \
					lmac/src/tx/txl/txl_buffer.c \
					lmac/src/tx/txl/txl_buffer_shared.c \
					lmac/src/tx/txl/txl_cfm.c \
					lmac/src/tx/txl/txl_cntrl.c \
					lmac/src/tx/txl/txl_frame.c \
					lmac/src/tx/txl/txl_frame_shared.c \
					lmac/src/tx/txl/txl_hwdesc.c \
					lmac/src/vif/vif_mgmt.c \
					mac/mac.c \
					mac/mac_ie.c \
					umac/src/apm/apm.c \
					umac/src/apm/apm_task.c \
					umac/src/bam/bam.c \
					umac/src/bam/bam_task.c \
					umac/src/me/me.c \
					umac/src/me/me_mgmtframe.c \
					umac/src/me/me_mic.c \
					umac/src/me/me_task.c \
					umac/src/me/me_utils.c \
					umac/src/rc/rc.c \
					umac/src/rc/rc_basic.c \
					umac/src/rxu/rxu_cntrl.c \
					umac/src/scanu/scanu.c \
					umac/src/scanu/scanu_shared.c \
					umac/src/scanu/scanu_task.c \
					umac/src/sm/sm.c \
					umac/src/sm/sm_task.c \
					umac/src/txu/txu_cntrl.c 
