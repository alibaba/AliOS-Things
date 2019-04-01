
XTAL := -DXTAL=25
ifeq ($(strip $(BOARD)), EVB_v1_25M)
XTAL := -DXTAL=25
endif
ifeq ($(strip $(BOARD)), EVB_v1_26M)
XTAL := -DXTAL=26
endif

ifeq ($(strip $(BOARD)), EVB_v1_40M)
XTAL := -DXTAL=40
endif
ifeq ($(strip $(BOARD)), EVB_v1_38M)
XTAL := -DXTAL=384
endif
ifeq ($(strip $(BOARD)), EVB_v1_19M)
XTAL := -DXTAL=192
endif
ifeq ($(strip $(BOARD)), EVB_v1_20M)
XTAL := -DXTAL=20
endif
ifeq ($(strip $(BOARD)), EVB_v1_24M)
XTAL := -DXTAL=24
endif
ifeq ($(strip $(BOARD)), EVB_v1_52M)
XTAL := -DXTAL=52
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S197_25M)
XTAL := -DXTAL=25
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S197_40M)
XTAL := -DXTAL=40
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_19M)
XTAL := -DXTAL=192
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_20M)
XTAL := -DXTAL=20
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_24M)
XTAL := -DXTAL=24
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_25M)
XTAL := -DXTAL=25
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_26M)
XTAL := -DXTAL=26
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_38M)
XTAL := -DXTAL=384
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_40M)
XTAL := -DXTAL=40
endif

ifeq ($(strip $(BOARD)), SSV6006_MP_S236_52M)
XTAL := -DXTAL=52
endif


ifeq ($(strip $(BOARD)), SSV6006_SHUTTLE_S197_25M)
XTAL := -DXTAL=25
endif

ifeq ($(strip $(BOARD)), SSV6006_SHUTTLE_S197_40M)
XTAL := -DXTAL=25
endif
