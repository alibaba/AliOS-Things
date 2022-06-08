include("manifest.py")

freeze("$(MPY_DIR)/modules/urequests", "urequests.py")

# freeze("$(MPY_LIB_DIR)/micropython/upysh", "upysh.py")
freeze("$(MPY_DIR)/modules/umqtt.simple", "umqtt/simple.py")
freeze("$(MPY_DIR)/modules/umqtt.robust", "umqtt/robust.py")
