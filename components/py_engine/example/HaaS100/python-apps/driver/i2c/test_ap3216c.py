"""
Testing ap3216c python driver

The below i2c configuration is needed in your board.json.

"ap3216c": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 100000,
    "mode": "master",
    "devAddr": 30
}
"""
print("testing ap3216c...")
from ap3216c import AP3216C

ap3216cDev = AP3216C()
ap3216cDev.open("ap3216c")
ap3216cDev.init()
print("ap3216c init finished")

brightness = ap3216cDev.ap3216c_read_ambient_light()
print("ap3216c brightness is:" , brightness)

ir_data = ap3216cDev.ap3216c_read_ir_data()
print("ap3216c ir_data is:" , ir_data)

ps_data = ap3216cDev.ap3216c_read_ps_data()
print("ap3216c ps_data is:" , ps_data)

if ((ps_data >> 15) & 1):
    print("near !")
else:
    print("far !")

ap3216cDev.close()
print("test ap3216c success!");
