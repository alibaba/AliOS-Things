NAME := beken

$(NAME)_TYPE := kernel

$(NAME)_INCLUDES := app/standalone-ap \
                    app/standalone-station \
					driver/sdio \
                    driver/uart \
                    driver/sys_ctrl \
                    driver/gpio \
                    driver/common/reg \
                    driver/dma \
                    driver/intc \
                    driver/phy \
                    driver/rc_beken \
                    driver/general_dma \
                    driver/spidma \
                    driver/rw_pub \
                    func/sdio_intf \
                    func/power_save \
					func/temp_detect \
                    func/spidma_intf \
                    func/ethernet_intf \
                    func/rwnx_intf \
					func/rf_test \
                    func/saradc_intf
					
$(NAME)_INCLUDES += ip/ke \
					ip/mac \
					ip/lmac/src/hal \
					ip/lmac/src/mm \
					ip/lmac/src/ps \
					ip/lmac/src/rd \
					ip/lmac/src/rwnx \
					ip/lmac/src/rx \
					ip/lmac/src/scan \
					ip/lmac/src/sta \
					ip/lmac/src/tx \
					ip/lmac/src/vif \
					ip/lmac/src/rx/rxl \
					ip/lmac/src/tx/txl \
					ip/lmac/src/tpc \
					ip/lmac/src/tdls \
					ip/lmac/src/p2p \
					ip/lmac/src/chan \
					ip/lmac/src/td \
					ip/umac/src/bam \
					ip/umac/src/llc \
					ip/umac/src/me \
					ip/umac/src/rxu \
					ip/umac/src/scanu \
					ip/umac/src/sm \
					ip/umac/src/txu \
					ip/umac/src/apm \
					ip/umac/src/mesh \
					ip/umac/src/rc
					
#$(NAME)_SOURCES +=  app/app.c \
#					app/ate_app.c \
#                    app/config/param_config.c \
#                    app/standalone-ap/sa_ap.c \
#                    app/standalone-station/sa_station.c \
#                    driver/common/dd.c \
#                    driver/common/drv_model.c \
#                    driver/dma/dma.c \
#                    driver/driver.c \
#                    driver/fft/fft.c \
#                    driver/flash/flash.c \
#                    driver/general_dma/general_dma.c \
#                    driver/gpio/gpio.c \
#                    driver/i2s/i2s.c \
#                    driver/icu/icu.c \
#                    driver/irda/irda.c \
#                    driver/macphy_bypass/mac_phy_bypass.c \
#                    driver/phy/phy_trident.c \
#                    driver/pwm/pwm.c \
#					driver/pwm/bk_timer.c \
#                    driver/pwm/mcu_ps_timer.c \
#                    driver/saradc/saradc.c \
#                    driver/sdio/sdio.c \
#                    driver/sdio/sdma.c \
#                    driver/sdio/sutil.c \
#                    driver/spi/spi.c \
#                    driver/spidma/spidma.c \
#                    driver/sys_ctrl/sys_ctrl.c \
#                    driver/uart/Retarget.c \
#                    driver/uart/uart.c \
#                    driver/wdt/wdt.c \
#                    driver/rw_pub/rw_platf_pub.c \
#                    func/bk7011_cal/bk7231_cal.c \
#					func/bk7011_cal/bk7231U_cal.c \
#					func/bk7011_cal/bk7221U_cal.c \
#					func/bk7011_cal/manual_cal_bk7231.c \
#                    func/bk7011_cal/manual_cal_bk7231U.c \
#                    func/func.c \
#                    func/hostapd_intf/hostapd_intf.c \
#                    func/misc/fake_clock.c \
#                    func/misc/target_util.c \
#                    func/rf_test/rx_sensitivity.c \
#                    func/rf_test/tx_evm.c \
#                    func/rwnx_intf/rw_ieee80211.c \
#                    func/rwnx_intf/rw_msdu.c \
#                    func/rwnx_intf/rw_msg_rx.c \
#                    func/rwnx_intf/rw_msg_tx.c \
#                    func/sdio_intf/sdio_intf.c \
#                    func/sdio_trans/sdio_trans.c \
#                    func/sim_uart/gpio_uart.c \
#                    func/sim_uart/pwm_uart.c \
#                    func/spidma_intf/spidma_intf.c \
#                    func/temp_detect/temp_detect.c \
#                    func/uart_debug/cmd_evm.c \
#                    func/uart_debug/cmd_help.c \
#                    func/uart_debug/cmd_reg.c \
#                    func/uart_debug/cmd_rx_sensitivity.c \
#                    func/uart_debug/command_line.c \
#                    func/uart_debug/command_table.c \
#                    func/uart_debug/udebug.c \
#                    func/power_save/power_save.c \
#                    func/power_save/mcu_ps.c \
#                    func/power_save/manual_ps.c \
#                    func/wlan_ui/wlan_ui.c \
#                    func/wlan_ui/extral_cli.c \
#                    func/wlan_ui/lsig_monitor.c \
#                    func/net_param_intf/net_param.c \
#                    func/lwip_intf/dhcpd/dhcp-server-main.c \
#                    func/lwip_intf/dhcpd/dhcp-server.c \
#                    func/saradc_intf/saradc_intf.c

$(NAME)_INCLUDES += func/hostapd-2.5/src/utils \
                    func/hostapd-2.5/src \
                    func/hostapd-2.5/bk_patch \
                    func/hostapd-2.5/src/ap \
                    func/hostapd-2.5/hostapd \
                    func/hostapd-2.5/src/common \
                    func/hostapd-2.5/src/drivers 

#$(NAME)_SOURCES	 += func/hostapd-2.5/bk_patch/ddrv.c \
#                    func/hostapd-2.5/bk_patch/signal.c \
#                    func/hostapd-2.5/bk_patch/sk_intf.c \
#                    func/hostapd-2.5/bk_patch/fake_socket.c \
#                    func/hostapd-2.5/src/common/hw_features_common.c \
#                    func/hostapd-2.5/src/common/ieee802_11_common.c \
#                    func/hostapd-2.5/src/common/wpa_common.c \
#                    func/hostapd-2.5/src/crypto/aes-unwrap.c \
#                    func/hostapd-2.5/src/crypto/rc4.c \
#                    func/hostapd-2.5/src/crypto/sha1-pbkdf2.c \
#                    func/hostapd-2.5/src/crypto/sha1-prf.c \
#                    func/hostapd-2.5/src/crypto/tls_none.c \
#                    func/hostapd-2.5/src/crypto/aes-wrap.c \
#                    func/hostapd-2.5/src/drivers/driver_beken.c \
#                    func/hostapd-2.5/src/drivers/driver_common.c \
#                    func/hostapd-2.5/src/drivers/drivers.c \
#                    func/hostapd-2.5/src/eap_common/eap_common.c \
#                    func/hostapd-2.5/src/eap_server/eap_server.c \
#                    func/hostapd-2.5/src/eap_server/eap_server_methods.c \
#                    func/hostapd-2.5/src/eapol_auth/eapol_auth_sm.c \
#                    func/hostapd-2.5/src/l2_packet/l2_packet_none.c \
#                    func/hostapd-2.5/src/rsn_supp/preauth.c \
#                    func/hostapd-2.5/src/rsn_supp/wpa.c \
#                    func/hostapd-2.5/src/rsn_supp/wpa_ie.c \
#                    func/hostapd-2.5/src/utils/common.c \
#                    func/hostapd-2.5/src/utils/eloop.c \
#                    func/hostapd-2.5/src/utils/os_none.c \
#                    func/hostapd-2.5/src/utils/wpabuf.c \
#					func/hostapd-2.5/src/ap/ap_config.c \
#                    func/hostapd-2.5/src/ap/ap_drv_ops.c \
#                    func/hostapd-2.5/src/ap/ap_list.c \
#                    func/hostapd-2.5/src/ap/ap_mlme.c \
#                    func/hostapd-2.5/src/ap/authsrv.c \
#                    func/hostapd-2.5/src/ap/beacon.c \
#                    func/hostapd-2.5/src/ap/bss_load.c \
#                    func/hostapd-2.5/src/ap/dfs.c \
#                    func/hostapd-2.5/src/ap/drv_callbacks.c \
#                    func/hostapd-2.5/src/ap/eap_user_db.c \
#                    func/hostapd-2.5/src/ap/hostapd.c \
#                    func/hostapd-2.5/src/ap/hw_features.c \
#                    func/hostapd-2.5/src/ap/ieee802_11.c \
#                    func/hostapd-2.5/src/ap/ieee802_11_auth.c \
#                    func/hostapd-2.5/src/ap/ieee802_11_ht.c \
#                    func/hostapd-2.5/src/ap/ieee802_11_shared.c \
#                    func/hostapd-2.5/src/ap/ieee802_1x.c \
#                    func/hostapd-2.5/src/ap/pmksa_cache_auth.c \
#                    func/hostapd-2.5/src/ap/sta_info.c \
#                    func/hostapd-2.5/src/ap/tkip_countermeasures.c \
#                    func/hostapd-2.5/src/ap/utils.c \
#                    func/hostapd-2.5/src/ap/wmm.c \
#                    func/hostapd-2.5/src/ap/wpa_auth.c \
#                    func/hostapd-2.5/src/ap/wpa_auth_glue.c \
#                    func/hostapd-2.5/src/ap/wpa_auth_ie.c \
#                    func/hostapd-2.5/hostapd/main_none.c \
#                    func/hostapd-2.5/wpa_supplicant/ap.c \
#                    func/hostapd-2.5/wpa_supplicant/blacklist.c \
#                    func/hostapd-2.5/wpa_supplicant/bss.c \
#                    func/hostapd-2.5/wpa_supplicant/config.c \
#                    func/hostapd-2.5/wpa_supplicant/config_none.c \
#                    func/hostapd-2.5/wpa_supplicant/eap_register.c \
#                    func/hostapd-2.5/wpa_supplicant/events.c \
#                    func/hostapd-2.5/wpa_supplicant/main_supplicant.c \
#                    func/hostapd-2.5/wpa_supplicant/notify.c \
#                    func/hostapd-2.5/wpa_supplicant/wmm_ac.c \
#                    func/hostapd-2.5/wpa_supplicant/wpa_scan.c \
#                    func/hostapd-2.5/wpa_supplicant/wpa_supplicant.c \
#                    func/hostapd-2.5/wpa_supplicant/wpas_glue.c
					
#$(NAME)_SOURCES	 += func/hostapd-2.5/src/crypto/crypto_ali.c \
#                    alios/lwip-2.0.2/port/ethernetif.c \
#                    alios/lwip-2.0.2/port/net.c \
#                    alios/os/mem_arch.c \
#                    alios/os/str_arch.c 

$(NAME)_PREBUILT_LIBRARY := beken.a