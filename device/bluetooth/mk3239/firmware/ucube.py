src =Split(''' 
''')
component =aos_component('lib_mico_ble_firmware', src)

BT_CHIP = aos_global_config.get('BT_CHIP')
BT_CHIP_REVISION = aos_global_config.get('BT_CHIP_REVISION')
BT_CHIP_XTAL_FREQUENCY = aos_global_config.get('BT_CHIP_XTAL_FREQUENCY')
if BT_CHIP_XTAL_FREQUENCY == None:
    src.add_sources( BT_CHIP+BT_CHIP_REVISION+'/bt_firmware_image.c')
else:
    src.add_sources( BT_CHIP+BT_CHIP_REVISION+'/'+BT_CHIP_XTAL_FREQUENCY+'/bt_firmware_image.c')



