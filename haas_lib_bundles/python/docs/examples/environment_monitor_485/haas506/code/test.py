import utime
import modbus as mb
import yuanda_htb485
import zzio606

def test_htb485():
    ret = mb.init('modbus_485_4800')
    if ret < 0:
        raise ValueError("init modbus failed.")
    htb485Obj = yuanda_htb485.HTB485(mb, 1)
    
    humidity = htb485Obj.getHumidity()
    temperature =  htb485Obj.getTemperature()
    brightness = htb485Obj.getBrightness()
    htb = htb485Obj.getHTB()
    
    print("temperature:", temperature)
    print("humidity:", humidity)
    print("brightness:",brightness)
    print("htb:",htb)
    mb.deinit()
    
def test_zzio606():
    # mb.writeHoldingRegister(1, 0x03e8,2,200)
    ret = mb.init('modbus_485_4800')
    if ret < 0:
        raise ValueError("init modbus failed.")
    zzio606Obj = zzio606.ZZIO606(mb, 3)
    zzio606Obj.openChannel(0)
    utime.sleep(1)
    zzio606Obj.openChannel(1)
    utime.sleep(1)
    zzio606Obj.openChannel(2)
    utime.sleep(1)
    zzio606Obj.openChannel(3)
    utime.sleep(1)
    zzio606Obj.openChannel(4)
    utime.sleep(1)
    zzio606Obj.openChannel(5)
    status = zzio606Obj.getChannelStatus()
    print("status", status)
    
    utime.sleep(5)
    
    zzio606Obj.closeChannel(0)
    utime.sleep(1)
    zzio606Obj.closeChannel(1)
    utime.sleep(1)
    zzio606Obj.closeChannel(2)
    utime.sleep(1)
    zzio606Obj.closeChannel(3)
    utime.sleep(1)
    zzio606Obj.closeChannel(4)
    utime.sleep(1)
    zzio606Obj.closeChannel(5)
    utime.sleep(1)
    status = zzio606Obj.getChannelStatus()
    print("status", status)
