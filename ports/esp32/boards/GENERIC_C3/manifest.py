freeze("$(PORT_DIR)/modules")
freeze("$(MPY_DIR)/tools", ("upip.py", "upip_utarfile.py"))
freeze("$(MPY_DIR)/ports/esp8266/modules", "ntptime.py")
freeze("$(MPY_DIR)/drivers/dht", "dht.py")
freeze("$(MPY_DIR)/drivers/onewire")
include("$(MPY_DIR)/extmod/uasyncio/manifest.py")
include("$(MPY_DIR)/extmod/webrepl/manifest.py")
include("$(MPY_DIR)/drivers/neopixel/manifest.py")

include("$(MPY_DIR)/extmod/uasyncio/manifest.py")
freeze("$(MPY_DIR)/drivers/onewire", "onewire.py")

# upip
freeze("$(MPY_DIR)/tools", ("upip.py", "upip_utarfile.py"))

# boardparser
freeze("$(MPY_DIR)/modules/boardparser", "boardparser.py")

# driver
include("$(MPY_DIR)/modules/driver/manifest.py")

# modbus
include("$(MPY_DIR)/modules/modbus/manifest.py")

# ulinksdk
include("$(MPY_DIR)/modules/ulinksdk/manifest.py")

# http
freeze("$(MPY_DIR)/modules/http", "http.py")

# ukv
freeze("$(MPY_DIR)/modules/ukv", "kv.py")
# ukv
freeze("$(MPY_DIR)/modules/adaptor/esp32/c3", "systemAdaptor.py")
