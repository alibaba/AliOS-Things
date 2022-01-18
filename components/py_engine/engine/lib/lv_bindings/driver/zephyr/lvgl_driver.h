#define CONFIG_LVGL_BITS_PER_PIXEL 32
#define CONFIG_LVGL_VDB_SIZE 16
#define CONFIG_LVGL_DISPLAY_DEV_NAME "ELCDIF_1" // choose from Zephyr display drivers
#define CONFIG_LVGL_BUFFER_ALLOC_STATIC 1 // LVGL buffer settings, see other options in Zephyr's Kconfig
#define CONFIG_LVGL_POINTER_KSCAN 1 // enabling/disabling kernel scan for input queue
#define CONFIG_LVGL_POINTER_KSCAN_MSGQ_COUNT 10
#define CONFIG_LVGL_POINTER_KSCAN_DEV_NAME "FT5336" // choose from Zephyr input drivers


int lvgl_init(const struct device *dev);
