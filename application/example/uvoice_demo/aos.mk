NAME := uvoice_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uvoice demo Application

$(NAME)_COMPONENTS := components/service/uvoice netmgr cli fatfs littlefs mbedtls

GLOBAL_DEFINES     += SOUND_APP

GLOBAL_DEFINES     += OSAL_RHINO

GLOBAL_INCLUDES    += .

$(NAME)_SOURCES    := uvoice_demo.c maintask.c

GLOBAL_DEFINES     += CLI_CONFIG_STACK_SIZE=8192 CONFIG_AOS_FATFS_SUPPORT_MMC HAAS_AUDIO_DEMO

uvoice_project_build ?= 1

uvoice_build_type ?= debug
uvoice_ws_enable ?= 0
uvoice_nopoll_enable ?= 0
uvoice_tts_enable ?= 1
uvoice_player_enable ?= 1
uvoice_recorder_enable ?= 1
uvoice_file_enable ?= 1
uvoice_mlist_enable ?= 1

uvoice_resample_enable ?= 0

uvoice_anc_enable ?= 0
uvoice_aec_enable ?= 0
uvoice_agc_enable ?= 0
uvoice_vad_enable ?= 0
uvoice_eq_enable ?= 0

uvoice_http_enable ?= 1
uvoice_https_enable ?= 1
uvoice_download_enable ?= 0
uvoice_hls_enable ?= 0

uvoice_partition_enable ?= 0
uvoice_event_enable ?= 1

dec_pvmp3_enable ?= 1
dec_pvaac_enable ?= 1
enc_speex_enable ?= 0
dec_speex_enable ?= 0
enc_opus_enable ?= 0
dec_opus_enable ?= 0
dec_amr_enable ?= 0
enc_amr_enable ?= 0
dec_amrwb_enable ?= 0
enc_amrwb_enable ?= 0

uvoice_offlineasr_enable ?= 0
uvoice_swid_enable ?= 0


