BLUETOOTH_LIB_TYPE ?= dual_mode

NAME := BTE_$(BLUETOOTH_LIB_TYPE)

################################################################################
# Supported variants                                                           #
################################################################################

SUPPORTED_BT_CHIPS           := 43438A0 43438A1

################################################################################
# Default settings                                                             #
################################################################################

#COMPLETE_BT_CHIP_NAME := BCM$(BT_CHIP)$(BT_CHIP_REVISION)

BTEDIR := ../BTE
PROTO_DISP_DIR := ../BTE/proto_disp

BTESOURCES := \
		$(BTEDIR)/Components/hcis/hcill_proto.c \
		$(BTEDIR)/Components/hcis/hcilp.c \
		$(BTEDIR)/Components/hcis/hcisu_h4.c \
		$(BTEDIR)/Components/hcis/hciutil.c \
		$(BTEDIR)/Components/stack/brcm/brcm_ble.c \
		$(BTEDIR)/Components/stack/brcm/brcm_prm.c \
		$(BTEDIR)/Components/stack/brcm/brcm_tbfc.c \
		$(BTEDIR)/Components/stack/brcm/bte_brcm.c \
		$(BTEDIR)/Components/stack/btm/btm_acl.c \
		$(BTEDIR)/Components/stack/btm/btm_cfg.c \
		$(BTEDIR)/Components/stack/btm/btm_dev.c \
		$(BTEDIR)/Components/stack/btm/btm_devctl.c \
		$(BTEDIR)/Components/stack/btm/btm_inq.c \
		$(BTEDIR)/Components/stack/btm/btm_main.c \
		$(BTEDIR)/Components/stack/btm/btm_pm.c \
		$(BTEDIR)/Components/stack/btm/btm_sco.c \
		$(BTEDIR)/Components/stack/btm/btm_sec.c \
		$(BTEDIR)/Components/stack/btm/btm_sec_nvram.c \
		$(BTEDIR)/Components/stack/btu/btu_hcif.c \
		$(BTEDIR)/Components/stack/btu/btu_init.c \
		$(BTEDIR)/Components/stack/btu/btu_task.c \
		$(BTEDIR)/Components/stack/hcic/hcicmds.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_api.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_csm.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_fcr.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_fcr_patch.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_link.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_main.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_utils.c \
		$(BTEDIR)/Components/stack/sdp/sdp_api.c \
		$(BTEDIR)/Components/stack/sdp/sdp_db.c \
		$(BTEDIR)/Components/stack/sdp/sdp_discovery.c \
		$(BTEDIR)/Components/stack/sdp/sdp_main.c \
		$(BTEDIR)/Components/stack/sdp/sdp_server.c \
		$(BTEDIR)/Components/stack/sdp/sdp_utils.c \
		$(BTEDIR)/Components/stack/wbt/wbt_ext.c \
		$(BTEDIR)/Projects/bte/main/bte_hcisu.c \
		$(BTEDIR)/Projects/bte/main/bte_init.c \
		$(BTEDIR)/Projects/bte/main/bte_load.c \
		$(BTEDIR)/Projects/bte/main/bte_version.c

BTE_RFCOMM_SOURCES := \
		$(BTEDIR)/Components/stack/rfcomm/port_api.c \
		$(BTEDIR)/Components/stack/rfcomm/port_rfc.c \
		$(BTEDIR)/Components/stack/rfcomm/port_utils.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_l2cap_if.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_mx_fsm.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_port_fsm.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_port_if.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_ts_frames.c \
		$(BTEDIR)/Components/stack/rfcomm/rfc_utils.c


#####ifndef MINDRAY                    ####oscar for mindray

BTE_AVRC_SOURCES := \
		$(BTEDIR)/Components/stack/avrc/avrc_api.c \
		$(BTEDIR)/Components/stack/avrc/avrc_sdp.c \
		$(BTEDIR)/Components/stack/avrc/avrc_opt.c \
		$(BTEDIR)/Components/stack/avrc/avrc_bld_ct.c \
		$(BTEDIR)/Components/stack/avrc/avrc_bld_tg.c \
		$(BTEDIR)/Components/stack/avrc/avrc_pars_ct.c \
		$(BTEDIR)/Components/stack/avrc/avrc_pars_tg.c \
		$(BTEDIR)/Components/stack/avrc/avrc_utils.c \
		$(BTEDIR)/mico_bt_api/mico_bt_avrc_wrapper.c

BTE_AVCT_SOURCES := \
		$(BTEDIR)/Components/stack/avct/avct_api.c \
		$(BTEDIR)/Components/stack/avct/avct_ccb.c \
		$(BTEDIR)/Components/stack/avct/avct_l2c.c \
		$(BTEDIR)/Components/stack/avct/avct_l2c_br.c \
		$(BTEDIR)/Components/stack/avct/avct_lcb.c \
		$(BTEDIR)/Components/stack/avct/avct_bcb_act.c \
		$(BTEDIR)/Components/stack/avct/avct_lcb_act.c

BTE_A2DP_SOURCES := \
		$(BTEDIR)/Components/stack/a2dp/a2d_api.c \
		$(BTEDIR)/Components/stack/a2dp/a2d_sbc.c \
		$(BTEDIR)/Components/stack/a2dp/a2d_m12.c \
		$(BTEDIR)/Components/stack/a2dp/a2d_m24.c \
		$(BTEDIR)/mico_bt_api/mico_bt_a2d_wrapper.c

BTE_AVDT_SOURCES := \
		$(BTEDIR)/Components/stack/avdt/avdt_ad.c \
		$(BTEDIR)/Components/stack/avdt/avdt_api.c \
		$(BTEDIR)/Components/stack/avdt/avdt_ccb.c \
		$(BTEDIR)/Components/stack/avdt/avdt_l2c.c \
		$(BTEDIR)/Components/stack/avdt/avdt_scb.c \
		$(BTEDIR)/Components/stack/avdt/avdt_scb_act.c \
		$(BTEDIR)/Components/stack/avdt/avdt_msg.c \
		$(BTEDIR)/Components/stack/avdt/avdt_ccb_act.c \
		$(BTEDIR)/mico_bt_api/mico_bt_avdt_wrapper.c

#####endif


BTE_GATT_SMP_SOURCES := \
		$(BTEDIR)/Components/stack/gatt/att_protocol.c \
		$(BTEDIR)/Components/stack/gatt/gatt_attr.c \
		$(BTEDIR)/Components/stack/gatt/gatt_cl.c \
		$(BTEDIR)/Components/stack/gatt/gatt_sr.c \
		$(BTEDIR)/Components/stack/gatt/gatt_api.c \
		$(BTEDIR)/Components/stack/gatt/gatt_auth.c \
		$(BTEDIR)/Components/stack/gatt/gatt_main.c \
		$(BTEDIR)/Components/stack/gatt/gatt_utils.c \
		$(BTEDIR)/Components/stack/gatt/legattdb/legattdb.c \
		$(BTEDIR)/Components/stack/smp/aes.c \
		$(BTEDIR)/Components/stack/smp/p_256_curvepara.c \
		$(BTEDIR)/Components/stack/smp/p_256_ecc_pp.c \
		$(BTEDIR)/Components/stack/smp/p_256_multprecision.c \
		$(BTEDIR)/Components/stack/smp/smp_act.c \
		$(BTEDIR)/Components/stack/smp/smp_api.c \
		$(BTEDIR)/Components/stack/smp/smp_br_main.c \
		$(BTEDIR)/Components/stack/smp/smp_cmac.c \
		$(BTEDIR)/Components/stack/smp/smp_keys.c \
		$(BTEDIR)/Components/stack/smp/smp_l2c.c \
		$(BTEDIR)/Components/stack/smp/smp_main.c \
		$(BTEDIR)/Components/stack/smp/smp_utils.c \
		$(BTEDIR)/Components/stack/hcic/hciblecmds.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_ble.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_ble_conn.c \
		$(BTEDIR)/Components/stack/l2cap/l2c_ble_utils.c \
		$(BTEDIR)/Components/stack/btm/btm_ble.c \
		$(BTEDIR)/Components/stack/btm/btm_ble_addr.c \
		$(BTEDIR)/Components/stack/btm/btm_ble_bgconn.c \
		$(BTEDIR)/Components/stack/btm/btm_ble_gap.c \
		$(BTEDIR)/Components/stack/btm/btm_ble_privacy.c \
		$(BTEDIR)/Components/stack/gap/gap_api.c \
		$(BTEDIR)/Components/stack/gap/gap_ble.c \
		$(BTEDIR)/Components/stack/gap/gap_conn.c \
		$(BTEDIR)/Components/stack/gap/gap_utils.c \
		$(BTEDIR)/mico_bt_api/mico_bt_l2c_le_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_ble_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_dev_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_gatt_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_rfcomm_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_sdp_wrapper.c \
		$(BTEDIR)/mico_bt_api/mico_bt_gatt.c \
		$(BTEDIR)/mico_bt_api/mico_bt_sdp.c


GKI_SOURCES := \
		$(BTEDIR)/Components/gki/common/gki_buffer.c \
		$(BTEDIR)/Components/gki/common/gki_debug.c \
		$(BTEDIR)/Components/gki/common/gki_time.c \
		$(BTEDIR)/Components/gki/mico/gki_mico.c

UDRV_SOURCES := \
		$(BTEDIR)/Components/udrv/mico/userial_mico.c
		
PROTODISP_SOURCES := \
		$(PROTO_DISP_DIR)/hcidisp.c \
		$(PROTO_DISP_DIR)/sdpdisp.c

PROTODISP_INCLUDES := \
		$(PROTO_DISP_DIR)


BTEINCLUDES := \
		$(BTEDIR)/Components/hcis \
		$(BTEDIR)/Components/hcis/include \
		$(BTEDIR)/Components/stack/include \
		$(BTEDIR)/Components/stack/avrc \
		$(BTEDIR)/Components/stack/avct \
		$(BTEDIR)/Components/stack/avdt \
		$(BTEDIR)/Components/stack/a2dp \
		$(BTEDIR)/Components/stack/btm \
		$(BTEDIR)/Components/stack/brcm \
		$(BTEDIR)/Components/stack/l2cap \
		$(BTEDIR)/Components/stack/sdp \
		$(BTEDIR)/Components/stack/gap \
		$(BTEDIR)/Components/stack/gatt \
		$(BTEDIR)/Components/stack/gatt/legattdb/inc \
		$(BTEDIR)/Components/stack/smp \
		$(BTEDIR)/Components/stack/hid \
		$(BTEDIR)/Components/stack/wbt \
		$(BTEDIR)/Components/rpc/include \
        $(BTEDIR)/Components/gki/mico \
		$(BTEDIR)/Components/gki/common \
		$(BTEDIR)/Components/udrv/include 
		

$(NAME)_SOURCES := 	\
					$(BTEDIR)/mico_bt_api/mico_bt_main.c \
					$(GKI_SOURCES) \
					$(BTESOURCES) \
					$(BTE_GATT_SMP_SOURCES) \
					$(BTE_RFCOMM_SOURCES) \
					$(PROTODISP_SOURCES) \
					$(UDRV_SOURCES)
					

$(NAME)_SOURCES +=	$(BTEDIR)/Projects/bte/embedded/mico/lib/mico_post_reset.c \
					$(BTEDIR)/Projects/bte/embedded/mico/lib/mico_stubs.c
	

$(NAME)_INCLUDES := $(BTEINCLUDES) \
					$(PROTODISP_INCLUDES) \
                    $(BTEDIR)/Projects/bte/main \
					../include \
					.
                    
$(NAME)_CFLAGS += -Wno-unused-function -Wno-unused-variable -Wno-unused-value -Wno-unused-label    

$(NAME)_DEFINES := MICO_BTE_LIB

GLOBAL_DEFINES += 	BUILDCFG BLUETOOTH_BTE GATT_ENABLE

#GLOBAL_DEFINES += 	BUILDCFG BLUETOOTH_BTE \
#GATT_ENABLE WBT_EXT_ENABLE SCO_ENABLE A2DP_ENABLE

                   
                   
