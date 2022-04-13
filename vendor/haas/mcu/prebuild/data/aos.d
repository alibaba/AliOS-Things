# Add cli cma here to execute the cmd after OS is ready
# note: line start with with '#' means it's commented

# Any valid cli command can be executed by the script.

# For example: load app at startup

#loadm /data/autotest_user@haas100-mk.app.bin

# load driver process
loadm /data/main_driver@haas100-mk.app.bin

# load netservice process
loadm /data/ulwip_service@haas100-mk.app.bin

# load link sdk demos process
loadm /data/link_sdk_demos@haas100-mk.app.bin
