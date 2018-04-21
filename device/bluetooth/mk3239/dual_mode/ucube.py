#temporarily disable .a

#in dual_mode_src.mk
src =Split(''' 
    ../BTE/mico_bt_api/mico_bt_main.c
    ../BTE/Components/gki/common/gki_buffer.c
    ../BTE/Components/gki/common/gki_debug.c
    ../BTE/Components/gki/common/gki_time.c
    ../BTE/Components/gki/mico/gki_mico.c
    ../BTE/Components/hcis/hcill_proto.c
    ../BTE/Components/hcis/hcilp.c
    ../BTE/Components/hcis/hcisu_h4.c
    ../BTE/Components/hcis/hciutil.c
    ../BTE/Components/stack/brcm/brcm_ble.c
    ../BTE/Components/stack/brcm/brcm_prm.c
    ../BTE/Components/stack/brcm/brcm_tbfc.c
    ../BTE/Components/stack/brcm/bte_brcm.c
    ../BTE/Components/stack/btm/btm_acl.c
    ../BTE/Components/stack/btm/btm_cfg.c
    ../BTE/Components/stack/btm/btm_dev.c
    ../BTE/Components/stack/btm/btm_devctl.c
    ../BTE/Components/stack/btm/btm_inq.c
    ../BTE/Components/stack/btm/btm_main.c
    ../BTE/Components/stack/btm/btm_pm.c
    ../BTE/Components/stack/btm/btm_sco.c
    ../BTE/Components/stack/btm/btm_sec.c
    ../BTE/Components/stack/btm/btm_sec_nvram.c
    ../BTE/Components/stack/btu/btu_hcif.c
    ../BTE/Components/stack/btu/btu_init.c
    ../BTE/Components/stack/btu/btu_task.c
    ../BTE/Components/stack/hcic/hcicmds.c
    ../BTE/Components/stack/l2cap/l2c_api.c
    ../BTE/Components/stack/l2cap/l2c_csm.c
    ../BTE/Components/stack/l2cap/l2c_fcr.c
    ../BTE/Components/stack/l2cap/l2c_fcr_patch.c
    ../BTE/Components/stack/l2cap/l2c_link.c
    ../BTE/Components/stack/l2cap/l2c_main.c
    ../BTE/Components/stack/l2cap/l2c_utils.c
    ../BTE/Components/stack/sdp/sdp_api.c
    ../BTE/Components/stack/sdp/sdp_db.c
    ../BTE/Components/stack/sdp/sdp_discovery.c
    ../BTE/Components/stack/sdp/sdp_main.c
    ../BTE/Components/stack/sdp/sdp_server.c
    ../BTE/Components/stack/sdp/sdp_utils.c
    ../BTE/Components/stack/wbt/wbt_ext.c
    ../BTE/Projects/bte/main/bte_hcisu.c
    ../BTE/Projects/bte/main/bte_init.c
    ../BTE/Projects/bte/main/bte_load.c
    ../BTE/Projects/bte/main/bte_version.c
    ../BTE/Components/stack/gatt/att_protocol.c
    ../BTE/Components/stack/gatt/gatt_attr.c
    ../BTE/Components/stack/gatt/gatt_cl.c
    ../BTE/Components/stack/gatt/gatt_sr.c
    ../BTE/Components/stack/gatt/gatt_api.c
    ../BTE/Components/stack/gatt/gatt_auth.c
    ../BTE/Components/stack/gatt/gatt_main.c
    ../BTE/Components/stack/gatt/gatt_utils.c
    ../BTE/Components/stack/gatt/legattdb/legattdb.c
    ../BTE/Components/stack/smp/aes.c
    ../BTE/Components/stack/smp/p_256_curvepara.c
    ../BTE/Components/stack/smp/p_256_ecc_pp.c
    ../BTE/Components/stack/smp/p_256_multprecision.c
    ../BTE/Components/stack/smp/smp_act.c
    ../BTE/Components/stack/smp/smp_api.c
    ../BTE/Components/stack/smp/smp_br_main.c
    ../BTE/Components/stack/smp/smp_cmac.c
    ../BTE/Components/stack/smp/smp_keys.c
    ../BTE/Components/stack/smp/smp_l2c.c
    ../BTE/Components/stack/smp/smp_main.c
    ../BTE/Components/stack/smp/smp_utils.c
    ../BTE/Components/stack/hcic/hciblecmds.c
    ../BTE/Components/stack/l2cap/l2c_ble.c
    ../BTE/Components/stack/l2cap/l2c_ble_conn.c
    ../BTE/Components/stack/l2cap/l2c_ble_utils.c
    ../BTE/Components/stack/btm/btm_ble.c
    ../BTE/Components/stack/btm/btm_ble_addr.c
    ../BTE/Components/stack/btm/btm_ble_bgconn.c
    ../BTE/Components/stack/btm/btm_ble_gap.c
    ../BTE/Components/stack/btm/btm_ble_privacy.c
    ../BTE/Components/stack/gap/gap_api.c
    ../BTE/Components/stack/gap/gap_ble.c
    ../BTE/Components/stack/gap/gap_conn.c
    ../BTE/Components/stack/gap/gap_utils.c
    ../BTE/mico_bt_api/mico_bt_l2c_le_wrapper.c
    ../BTE/mico_bt_api/mico_bt_ble_wrapper.c
    ../BTE/mico_bt_api/mico_bt_dev_wrapper.c
    ../BTE/mico_bt_api/mico_bt_gatt_wrapper.c
    ../BTE/mico_bt_api/mico_bt_rfcomm_wrapper.c
    ../BTE/mico_bt_api/mico_bt_sdp_wrapper.c
    ../BTE/mico_bt_api/mico_bt_gatt.c
    ../BTE/mico_bt_api/mico_bt_sdp.c
    ../BTE/Components/stack/rfcomm/port_api.c
    ../BTE/Components/stack/rfcomm/port_rfc.c
    ../BTE/Components/stack/rfcomm/port_utils.c
    ../BTE/Components/stack/rfcomm/rfc_l2cap_if.c
    ../BTE/Components/stack/rfcomm/rfc_mx_fsm.c
    ../BTE/Components/stack/rfcomm/rfc_port_fsm.c
    ../BTE/Components/stack/rfcomm/rfc_port_if.c
    ../BTE/Components/stack/rfcomm/rfc_ts_frames.c
    ../BTE/Components/stack/rfcomm/rfc_utils.c
    ../BTE/proto_disp/hcidisp.c
    ../BTE/proto_disp/sdpdisp.c
    ../BTE/Components/udrv/mico/userial_mico.c
    ../BTE/Projects/bte/embedded/mico/lib/mico_post_reset.c
    ../BTE/Projects/bte/embedded/mico/lib/mico_stubs.c
''')
component =aos_component('BTE_dual_mode', src)



global_macros =Split(''' 
    BUILDCFG
    BLUETOOTH_BTE
    GATT_ENABLE
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ../BTE/Components/hcis
    ../BTE/Components/hcis/include
    ../BTE/Components/stack/include
    ../BTE/Components/stack/avrc
    ../BTE/Components/stack/avct
    ../BTE/Components/stack/avdt
    ../BTE/Components/stack/a2dp
    ../BTE/Components/stack/btm
    ../BTE/Components/stack/brcm
    ../BTE/Components/stack/l2cap
    ../BTE/Components/stack/sdp
    ../BTE/Components/stack/gap
    ../BTE/Components/stack/gatt
    ../BTE/Components/stack/gatt/legattdb/inc
    ../BTE/Components/stack/smp
    ../BTE/Components/stack/hid
    ../BTE/Components/stack/wbt
    ../BTE/Components/rpc/include
    ../BTE/Components/gki/mico
    ../BTE/Components/gki/common
    ../BTE/Components/udrv/include
    ../BTE/proto_disp
    ../BTE/Projects/bte/main
    ../include
    .
''')
for i in includes:
    component.add_includes(i)

macros =Split(''' 
    MICO_BTE_LIB
''')
for i in macros:
    component.add_macros(i)

cflags =Split(''' 
    -Wno-unused-function
    -Wno-unused-variable
    -Wno-unused-value
    -Wno-unused-label
''')
for i in cflags:
    component.add_cflags(i)

#in dual_mode.mk    
includes =Split(''' 
    .
    ../include   
''')
for i in includes:
    component.add_global_includes(i)
    
component.add_comp_deps('device/bluetooth/mk3239/firmware')

src =Split(''' 
    ../BTE_platform/mico_bt_bus.c
    ../BTE_platform/mico_bt_hcd.c
    ../BTE_platform/mico_bt_logmsg.c
    ../BTE_platform/mico_bt_nvram_access.c
    ../BTE_platform/mico_upio.c
''')
for i in src:
    component.add_sources(i)
    
aos_global_config.set('VALID_PLATFORMS',['MK3238','MK3239'])    
    
    
    
    
    