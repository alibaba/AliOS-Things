NAME := ble

$(NAME)_TYPE := kernel

GLOBAL_DEFINES += CFG_SUPPORT_BLE
$(NAME)_INCLUDES := . \
					rw_ble/ip/ahi/api \
					rw_ble/ip/ble/hl/api \
					rw_ble/ip/ble/hl/inc \
					rw_ble/ip/ble/hl/src/gap/gapc \
					rw_ble/ip/ble/hl/src/gap/gapm \
					rw_ble/ip/ble/hl/src/gap/smpc \
					rw_ble/ip/ble/hl/src/gap/smpm \
					rw_ble/ip/ble/hl/src/gap \
					rw_ble/ip/ble/hl/src/gatt/attc \
					rw_ble/ip/ble/hl/src/gatt/attm \
					rw_ble/ip/ble/hl/src/gatt/atts \
					rw_ble/ip/ble/hl/src/gatt \
					rw_ble/ip/ble/hl/src/gatt/gattc \
					rw_ble/ip/ble/hl/src/gatt/gattm \
					rw_ble/ip/ble/hl/src/l2c/l2cc \
					rw_ble/ip/ble/hl/src/l2c/l2cm \
					rw_ble/ip/ble/ll/src/em \
					rw_ble/ip/ble/ll/src/llc \
					rw_ble/ip/ble/ll/src/lld \
					rw_ble/ip/ble/ll/src/llm \
					rw_ble/ip/ble/ll/src/rwble \
					rw_ble/ip/ble/profiles/ancs/ancsc/api \
					rw_ble/ip/ble/profiles/ancs \
					rw_ble/ip/ble/profiles/anp/anpc/api \
					rw_ble/ip/ble/profiles/anp/anpc/src \
					rw_ble/ip/ble/profiles/anp/anps/api \
					rw_ble/ip/ble/profiles/anp/anps/src \
					rw_ble/ip/ble/profiles/anp \
					rw_ble/ip/ble/profiles/bas/basc/api \
					rw_ble/ip/ble/profiles/bas/basc/src \
					rw_ble/ip/ble/profiles/bas/bass/api \
					rw_ble/ip/ble/profiles/bas/bass/src \
					rw_ble/ip/ble/profiles/blp/blpc/api \
					rw_ble/ip/ble/profiles/blp/blpc/src \
					rw_ble/ip/ble/profiles/blp/blps/api \
					rw_ble/ip/ble/profiles/blp/blps/src \
					rw_ble/ip/ble/profiles/blp \
					rw_ble/ip/ble/profiles/cpp/cppc/api \
					rw_ble/ip/ble/profiles/cpp/cppc/src \
					rw_ble/ip/ble/profiles/cpp/cpps/api \
					rw_ble/ip/ble/profiles/cpp/cpps/src \
					rw_ble/ip/ble/profiles/cpp \
					rw_ble/ip/ble/profiles/cscp/cscpc/api \
					rw_ble/ip/ble/profiles/cscp/cscpc/src \
					rw_ble/ip/ble/profiles/cscp/cscps/api \
					rw_ble/ip/ble/profiles/cscp/cscps/src \
					rw_ble/ip/ble/profiles/cscp \
					rw_ble/ip/ble/profiles/dis/disc/api \
					rw_ble/ip/ble/profiles/dis/disc/src \
					rw_ble/ip/ble/profiles/dis/diss/api \
					rw_ble/ip/ble/profiles/dis/diss/src \
					rw_ble/ip/ble/profiles/FFE0/api \
					rw_ble/ip/ble/profiles/FFE0/api \
					rw_ble/ip/ble/profiles/FFF0/api \
					rw_ble/ip/ble/profiles/FFF0/api \
					rw_ble/ip/ble/profiles/AIS/api \
					rw_ble/ip/ble/profiles/media/api \
					rw_ble/ip/ea/api \
					rw_ble/ip/em/api \
					rw_ble/ip/hci/api \
					rw_ble/ip/hci/src \
					rw_ble/modules/app/api \
					rw_ble/modules/app/src \
					rw_ble/modules/ali_sdk/api \
					rw_ble/modules/ali_sdk/src \
					rw_ble/modules/common/api \
					rw_ble/modules/dbg/api \
					rw_ble/modules/h4tl/api \
					rw_ble/modules/ke/api \
					rw_ble/modules/ke/src \
					rw_ble/modules/rwip/api \
					rw_ble/modules/rf/api \
					rw_ble/modules/media \
					rw_ble/plf/refip/src/arch \
					rw_ble/plf/refip/src/arch/boot \
					rw_ble/plf/refip/src/arch/compiler \
					rw_ble/plf/refip/src/arch/ll \
					rw_ble/plf/refip/src/arch \
					rw_ble/plf/refip/src/build/ble_full/reg/fw \
					rw_ble/plf/refip/src/driver/reg \
					rw_ble/plf/refip/src/rom/common \
					rw_ble/plf/refip/src/driver/ble_icu

$(NAME)_INCLUDES += ../../ip/ke \
					../../ip/lmac/src/rwnx \
					../../ip/mac \
					../../ip/umac/src/apm \
					../../ip/lmac/src/scan \
					../../ip/lmac/src/hal \
					../../ip/lmac/src/mm \
					../../ip/umac/src/me \
					../../ip/umac/src/rc \
					../../ip/lmac/src/sta \
					../../ip/umac/src/rxu \
					../../ip/lmac/src/rx \
					../../ip/umac/src/bam \
					../../ip/lmac/src/vif \
					../../ip/lmac/src/chan \
					../../ip/lmac/src/tx \
					../../ip/lmac/src/rx/rxl \
					../../ip/lmac/src/tx/txl \
					../../ip/umac/src/scanu

$(NAME)_INCLUDES +=	../../func/rwnx_intf \
					../../driver/dma \
					../../driver/common/reg \
					../../driver/phy \
					../../driver/uart
					
#$(NAME)_SOURCES	:=  ble.c \
#					rw_ble/ip/ahi/src/ahi.c \
#					rw_ble/ip/ahi/src/ahi_task.c \
#					rw_ble/ip/ble/hl/src/gap/gapc/gapc.c \
#					rw_ble/ip/ble/hl/src/gap/gapc/gapc_hci.c \
#					rw_ble/ip/ble/hl/src/gap/gapc/gapc_sig.c \
#					rw_ble/ip/ble/hl/src/gap/gapc/gapc_task.c \
#					rw_ble/ip/ble/hl/src/gap/gapm/gapm.c \
#					rw_ble/ip/ble/hl/src/gap/gapm/gapm_hci.c \
#					rw_ble/ip/ble/hl/src/gap/gapm/gapm_task.c \
#					rw_ble/ip/ble/hl/src/gap/gapm/gapm_util.c \
#					rw_ble/ip/ble/hl/src/gap/smpc/smpc.c \
#					rw_ble/ip/ble/hl/src/gap/smpc/smpc_api.c \
#					rw_ble/ip/ble/hl/src/gap/smpc/smpc_crypto.c \
#					rw_ble/ip/ble/hl/src/gap/smpc/smpc_util.c \
#					rw_ble/ip/ble/hl/src/gap/smpm/smpm_api.c \
#					rw_ble/ip/ble/hl/src/gatt/attc/attc.c \
#					rw_ble/ip/ble/hl/src/gatt/attm/attm.c \
#					rw_ble/ip/ble/hl/src/gatt/attm/attm_db.c \
#					rw_ble/ip/ble/hl/src/gatt/atts/atts.c \
#					rw_ble/ip/ble/hl/src/gatt/gattc/gattc.c \
#					rw_ble/ip/ble/hl/src/gatt/gattc/gattc_task.c \
#					rw_ble/ip/ble/hl/src/gatt/gattm/gattm.c \
#					rw_ble/ip/ble/hl/src/gatt/gattm/gattm_task.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cc/l2cc.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cc/l2cc_lecb.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cc/l2cc_pdu.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cc/l2cc_sig.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cc/l2cc_task.c \
#					rw_ble/ip/ble/hl/src/l2c/l2cm/l2cm.c \
#					rw_ble/ip/ble/hl/src/prf/prf.c \
#					rw_ble/ip/ble/hl/src/prf/prf_utils.c \
#					rw_ble/ip/ble/hl/src/rwble_hl/rwble_hl.c \
#					rw_ble/ip/ble/ll/src/em/em_buf.c \
#					rw_ble/ip/ble/ll/src/llc/llc.c \
#					rw_ble/ip/ble/ll/src/llc/llc_ch_asses.c \
#					rw_ble/ip/ble/ll/src/llc/llc_hci.c \
#					rw_ble/ip/ble/ll/src/llc/llc_llcp.c \
#					rw_ble/ip/ble/ll/src/llc/llc_task.c \
#					rw_ble/ip/ble/ll/src/llc/llc_util.c \
#					rw_ble/ip/ble/ll/src/lld/lld.c \
#					rw_ble/ip/ble/ll/src/lld/lld_evt.c \
#					rw_ble/ip/ble/ll/src/lld/lld_pdu.c \
#					rw_ble/ip/ble/ll/src/lld/lld_sleep.c \
#					rw_ble/ip/ble/ll/src/lld/lld_util.c \
#					rw_ble/ip/ble/ll/src/lld/lld_wlcoex.c \
#					rw_ble/ip/ble/ll/src/llm/llm.c \
#					rw_ble/ip/ble/ll/src/llm/llm_hci.c \
#					rw_ble/ip/ble/ll/src/llm/llm_task.c \
#					rw_ble/ip/ble/ll/src/llm/llm_util.c \
#					rw_ble/ip/ble/ll/src/rwble/rwble.c \
#					rw_ble/ip/ble/profiles/ancs/ancsc/src/ancsc.c \
#					rw_ble/ip/ble/profiles/ancs/ancsc/src/ancsc_task.c \
#					rw_ble/ip/ble/profiles/anp/anpc/src/anpc.c \
#					rw_ble/ip/ble/profiles/anp/anpc/src/anpc_task.c \
#					rw_ble/ip/ble/profiles/anp/anps/src/anps.c \
#					rw_ble/ip/ble/profiles/anp/anps/src/anps_task.c \
#					rw_ble/ip/ble/profiles/bas/basc/src/basc.c \
#					rw_ble/ip/ble/profiles/bas/basc/src/basc_task.c \
#					rw_ble/ip/ble/profiles/bas/bass/src/bass.c \
#					rw_ble/ip/ble/profiles/bas/bass/src/bass_task.c \
#					rw_ble/ip/ble/profiles/blp/blpc/src/blpc.c \
#					rw_ble/ip/ble/profiles/blp/blpc/src/blpc_task.c \
#					rw_ble/ip/ble/profiles/blp/blps/src/blps.c \
#					rw_ble/ip/ble/profiles/blp/blps/src/blps_task.c \
#					rw_ble/ip/ble/profiles/cpp/cppc/src/cppc.c \
#					rw_ble/ip/ble/profiles/cpp/cppc/src/cppc_task.c \
#					rw_ble/ip/ble/profiles/cpp/cpps/src/cpps.c \
#					rw_ble/ip/ble/profiles/cpp/cpps/src/cpps_task.c \
#					rw_ble/ip/ble/profiles/cscp/cscpc/src/cscpc.c \
#					rw_ble/ip/ble/profiles/cscp/cscpc/src/cscpc_task.c \
#					rw_ble/ip/ble/profiles/cscp/cscps/src/cscps.c \
#					rw_ble/ip/ble/profiles/cscp/cscps/src/cscps_task.c \
#					rw_ble/ip/ble/profiles/dis/disc/src/disc.c \
#					rw_ble/ip/ble/profiles/dis/disc/src/disc_task.c \
#					rw_ble/ip/ble/profiles/dis/diss/src/diss.c \
#					rw_ble/ip/ble/profiles/dis/diss/src/diss_task.c \
#					rw_ble/ip/ble/profiles/FFE0/src/ffe0s.c \
#					rw_ble/ip/ble/profiles/FFE0/src/ffe0s_task.c \
#					rw_ble/ip/ble/profiles/FFF0/src/fff0s.c \
#					rw_ble/ip/ble/profiles/FFF0/src/fff0s_task.c \
#					rw_ble/ip/ble/profiles/AIS/src/AIS.c \
#					rw_ble/ip/ble/profiles/AIS/src/AIS_task.c \
#					rw_ble/ip/ble/profiles/media/src/ms.c \
#					rw_ble/ip/ble/profiles/media/src/ms_task.c \
#					rw_ble/ip/ea/src/ea.c \
#					rw_ble/ip/hci/src/hci.c \
#					rw_ble/ip/hci/src/hci_fc.c \
#					rw_ble/ip/hci/src/hci_msg.c \
#					rw_ble/ip/hci/src/hci_tl.c \
#					rw_ble/ip/hci/src/hci_util.c \
#					rw_ble/modules/app/src/app.c \
#					rw_ble/modules/app/src/app_sec.c \
#					rw_ble/modules/app/src/app_ffe0.c \
#					rw_ble/modules/app/src/app_fff0.c \
#					rw_ble/modules/app/src/app_task.c \
#					rw_ble/modules/ali_sdk/src/ais_sdk.c \
#					rw_ble/modules/media/app_ms.c \
#					rw_ble/modules/common/src/common_list.c \
#					rw_ble/modules/common/src/common_utils.c \
#					rw_ble/modules/common/src/RomCallFlash.c \
#					rw_ble/modules/dbg/src/dbg.c \
#					rw_ble/modules/dbg/src/dbg_mwsgen.c \
#					rw_ble/modules/dbg/src/dbg_swdiag.c \
#					rw_ble/modules/dbg/src/dbg_task.c \
#					rw_ble/modules/h4tl/src/h4tl.c \
#					rw_ble/modules/ke/src/kernel.c \
#					rw_ble/modules/ke/src/kernel_event.c \
#					rw_ble/modules/ke/src/kernel_mem.c \
#					rw_ble/modules/ke/src/kernel_msg.c \
#					rw_ble/modules/ke/src/kernel_queue.c \
#					rw_ble/modules/ke/src/kernel_task.c \
#					rw_ble/modules/ke/src/kernel_timer.c \
#					rw_ble/modules/rwip/src/rwip.c \
#					rw_ble/plf/refip/src/arch/main/arch_main.c \
#					rw_ble/modules/rf/src/ble_rf_xvr.c \
#					rw_ble/plf/refip/src/driver/uart/uart.c

$(NAME)_PREBUILT_LIBRARY := ble.a