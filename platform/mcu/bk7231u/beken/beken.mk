
BEKEN_DIR := ./beken

$(NAME)_INCLUDES += $(BEKEN_DIR)/app/standalone-ap \
                    $(BEKEN_DIR)/app/standalone-station \
					$(BEKEN_DIR)/driver/sdio \
                    $(BEKEN_DIR)/driver/uart \
                    $(BEKEN_DIR)/driver/sys_ctrl \
                    $(BEKEN_DIR)/driver/gpio \
                    $(BEKEN_DIR)/driver/common/reg \
                    $(BEKEN_DIR)/driver/dma \
                    $(BEKEN_DIR)/driver/intc \
                    $(BEKEN_DIR)/driver/phy \
                    $(BEKEN_DIR)/driver/rc_beken \
                    $(BEKEN_DIR)/driver/general_dma \
                    $(BEKEN_DIR)/driver/spidma \
                    $(BEKEN_DIR)/driver/rw_pub \
                    $(BEKEN_DIR)/func/sdio_intf \
                    $(BEKEN_DIR)/func/power_save \
					$(BEKEN_DIR)/func/temp_detect \
                    $(BEKEN_DIR)/func/spidma_intf \
                    $(BEKEN_DIR)/func/ethernet_intf \
                    $(BEKEN_DIR)/func/rwnx_intf \
					$(BEKEN_DIR)/func/rf_test \
                    $(BEKEN_DIR)/func/saradc_intf

$(NAME)_INCLUDES += $(BEKEN_DIR)/ip/ke \
                    $(BEKEN_DIR)/ip/mac \
                    $(BEKEN_DIR)/ip/lmac/src/hal \
                    $(BEKEN_DIR)/ip/lmac/src/mm \
                    $(BEKEN_DIR)/ip/lmac/src/ps \
                    $(BEKEN_DIR)/ip/lmac/src/rd \
                    $(BEKEN_DIR)/ip/lmac/src/rwnx \
                    $(BEKEN_DIR)/ip/lmac/src/rx \
                    $(BEKEN_DIR)/ip/lmac/src/scan \
                    $(BEKEN_DIR)/ip/lmac/src/sta \
                    $(BEKEN_DIR)/ip/lmac/src/tx \
                    $(BEKEN_DIR)/ip/lmac/src/vif \
                    $(BEKEN_DIR)/ip/lmac/src/rx/rxl \
                    $(BEKEN_DIR)/ip/lmac/src/tx/txl \
                    $(BEKEN_DIR)/ip/lmac/src/tpc \
                    $(BEKEN_DIR)/ip/lmac/src/tdls \
                    $(BEKEN_DIR)/ip/lmac/src/p2p \
                    $(BEKEN_DIR)/ip/lmac/src/chan \
                    $(BEKEN_DIR)/ip/lmac/src/td \
                    $(BEKEN_DIR)/ip/umac/src/bam \
                    $(BEKEN_DIR)/ip/umac/src/llc \
                    $(BEKEN_DIR)/ip/umac/src/me \
                    $(BEKEN_DIR)/ip/umac/src/rxu \
                    $(BEKEN_DIR)/ip/umac/src/scanu \
                    $(BEKEN_DIR)/ip/umac/src/sm \
                    $(BEKEN_DIR)/ip/umac/src/txu \
                    $(BEKEN_DIR)/ip/umac/src/apm \
                    $(BEKEN_DIR)/ip/umac/src/mesh \
                    $(BEKEN_DIR)/ip/umac/src/rc

$(NAME)_INCLUDES += $(BEKEN_DIR)/func/hostapd-2.5/src/utils \
                    $(BEKEN_DIR)/func/hostapd-2.5/src \
                    $(BEKEN_DIR)/func/hostapd-2.5/bk_patch \
                    $(BEKEN_DIR)/func/hostapd-2.5/src/ap \
                    $(BEKEN_DIR)/func/hostapd-2.5/hostapd \
                    $(BEKEN_DIR)/func/hostapd-2.5/src/common \
                    $(BEKEN_DIR)/func/hostapd-2.5/src/drivers

$(NAME)_PREBUILT_LIBRARY := $(BEKEN_DIR)/beken.a
$(NAME)_PREBUILT_LIBRARY += $(BEKEN_DIR)/ip/ip.a
$(NAME)_PREBUILT_LIBRARY += $(BEKEN_DIR)/driver/ble/ble.a
$(NAME)_PREBUILT_LIBRARY += $(BEKEN_DIR)/alios/entry/entry.a

#BLE
GLOBAL_DEFINES += CFG_SUPPORT_BLE
$(NAME)_INCLUDES += $(BEKEN_DIR)/driver/ble \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ahi/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/inc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gap/gapc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gap/gapm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gap/smpc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gap/smpm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gap \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt/attc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt/attm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt/atts \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt/gattc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/gatt/gattm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/l2c/l2cc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/hl/src/l2c/l2cm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/ll/src/em \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/ll/src/llc \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/ll/src/lld \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/ll/src/llm \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/ll/src/rwble \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/ancs/ancsc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/ancs \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/anp/anpc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/anp/anpc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/anp/anps/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/anp/anps/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/anp \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/bas/basc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/bas/basc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/bas/bass/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/bas/bass/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/blp/blpc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/blp/blpc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/blp/blps/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/blp/blps/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/blp \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cpp/cppc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cpp/cppc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cpp/cpps/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cpp/cpps/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cpp \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cscp/cscpc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cscp/cscpc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cscp/cscps/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cscp/cscps/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/cscp \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/dis/disc/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/dis/disc/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/dis/diss/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/dis/diss/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/FFE0/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/FFE0/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/FFF0/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/FFF0/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/AIS/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ble/profiles/media/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/ea/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/em/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/hci/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/ip/hci/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/app/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/app/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/ali_sdk/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/ali_sdk/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/common/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/dbg/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/h4tl/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/ke/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/ke/src \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/rwip/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/rf/api \
                    $(BEKEN_DIR)/driver/ble/rw_ble/modules/media \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/arch \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/arch/boot \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/arch/compiler \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/arch/ll \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/arch \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/build/ble_full/reg/fw \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/driver/reg \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/rom/common \
                    $(BEKEN_DIR)/driver/ble/rw_ble/plf/refip/src/driver/ble_icu

