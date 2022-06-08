include("$(MPY_DIR)/extmod/uasyncio/manifest.py")
freeze("$(MPY_DIR)/drivers/dht", "dht.py")
# freeze("$(MPY_DIR)/drivers/display", ("lcd160cr.py", "lcd160cr_test.py"))
freeze("$(MPY_DIR)/drivers/onewire", "onewire.py")
freeze("$(MPY_DIR)/ports/stm32/modules", "neopixel.py")


# boardparser
freeze("$(MPY_DIR)/modules/boardparser", "boardparser.py")

# driver
include("$(MPY_DIR)/modules/driver/manifest.py")

# modbus
include("$(MPY_DIR)/modules/modbus/manifest.py")