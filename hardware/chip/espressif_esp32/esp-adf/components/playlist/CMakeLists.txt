
# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES audio_sal nvs_flash)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS ./playlist.c
                   ./playlist_operator/dram_list.c
                   ./playlist_operator/flash_list.c
                   ./playlist_operator/partition_list.c
                   ./playlist_operator/sdcard_list.c
                   ./sdcard_scan/sdcard_scan.c
                   )
                   
set(COMPONENT_ADD_INCLUDEDIRS ./include)

register_component()