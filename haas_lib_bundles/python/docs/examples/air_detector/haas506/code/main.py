# coding=utf-8
# This is a sample Python script.
from aliyunIoT import Device
import ujson
import network
import utime as time
from driver import GPIO
from driver import UART

t1 = 30
gas_threshold = 5.0
liq_mdcn_alarm = False
gas_alarm = False
version = 'v0.0.1'
uart1 = UART('serail1')
liq_level = GPIO()
gpio = GPIO()
'''0 1 means cloud ctrl,2 local ctrl'''
cloud_ctrl = 2
g_connect_status = False
ini_file_name = '/user/cfg.txt'

def on_4g_cb(args):
    global g_connect_status
    pdp = args[0]
    netwk_sta = args[1]
    if netwk_sta == 1:
        g_connect_status = True
    else:
        g_connect_status = False


def connect_network():
    global on_4g_cb,g_connect_status

    net = network.NetWorkClient()
    g_register_network = False
    if net._stagecode is not None and net._stagecode == 3 and net._subcode == 1:
        g_register_network = True
    else:
        g_register_network = False
    if g_register_network:
        net.on(1,on_4g_cb)
        net.connect(None)
    else:
        print('connect network failed')
    for i in range(30):
        if g_connect_status:
            print('connect network success')
            return True
        time.sleep(1)
    return False

def read_cfg_file():
    global t1,gas_threshold,ini_file_name
    try:
        f = open(ini_file_name,'r')
    except OSError:
        cfg_dict = {'gasstr':1.0,'t1':60}
        print('write',cfg_dict)
        f = open(ini_file_name,'w+')
        print(f)
        f.write(ujson.dumps(cfg_dict))
    else:
        cfg_txt = f.read()
        cfg_dict = ujson.loads(cfg_txt)
        if isinstance(cfg_dict,dict) == False:
            print('cfg_dict not a dict')
            return
        print('read',cfg_dict)
        gas_threshold = cfg_dict['gasstr']
        t1 = cfg_dict['t1']
        print('gas',gas_threshold,'t1',t1)
    finally:
        f.close()
        print('close')
    return 0

    
def write_cfg_file(cloudstr):
    global t1,gas_threshold,ini_file_name
    
    if isinstance(cloudstr,str) == False:
        return
    try:
        f = open(ini_file_name,'r')
    except OSError:
        pass
    else:
        cfg_txt = f.read()
        f.close()
    finally:
        pass
        
    try:
        f = open(ini_file_name,'w+')
    except OSError:
        pass
    else:
        cfg_dict = ujson.loads(cfg_txt)
        cloud_dict = ujson.loads(cloudstr)
        if isinstance(cfg_dict,dict) == False:
            print('cfg_dict not a dict')
            return
        
        if isinstance(cloud_dict,dict) == False:
            print('cloud_dict not a dict')
            return
        
        for key in cloud_dict.keys():
            if cfg_dict.get(key) != None:
                cfg_dict[key] = cloud_dict[key]
                if key == 'gasstr':
                    gas_threshold = cfg_dict[key]
                if key == 't1':
                    t1 = cfg_dict[key]
                    
        f.seek(0)
        f.write(ujson.dumps(cfg_dict))
        print(cfg_dict)
        pass
    finally:
        f.close()
        print('cloud cfg file close')
    return
    
def on_connect():
    print('linkkit is connected')
    
def on_disconnect():
    print('linkkit is disconnected')
    
def on_props(request):
    print('clound req data is {}'.format(request))
    global gpio
    global cloud_ctrl
    
    cloudmsg = ujson.loads(request)
    if 'powerstate' in cloudmsg:
        if cloudmsg['powerstate'] == 0:
            gpio.write(0)
            #pass
            cloud_ctrl = 0
            print('led state {}'.format(cloudmsg['powerstate']))
        else:
            cloud_ctrl = 1
            gpio.write(1)
            #pass
            print('led state {}'.format(cloudmsg['powerstate']))
    else:
        write_cfg_file(request)
    
def on_service(id,request):
    print('clound req id  is {} , req is {}'.format(id,request))

def on_error(err):
    print('err msg is {} '.format(err))
    
def gas_detec():
    gas_val = 0.0
    dotnum = 0
    global uart1
    len1 = 0
    #sign = 0
    uart1.open('serial1')
    readbuf1 = bytearray(9)
    writebuf1 = bytearray([0xd7])
    readbuf2 = bytearray(13)
    writebuf2 = bytearray([0xff,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78])
    
    uart1.write(writebuf1)
    len1 = uart1.read(readbuf1)
    print('dotnum:',end='')
    print(readbuf1)
    if len1 != len(readbuf1):
        print('read dotnum err')
        uart1.close()
        return gas_val
    
    uart1.write(writebuf2)
    len1 = uart1.read(readbuf2)
    print('readlen:',len1,'dotnum:',end='')
    print(readbuf2)
    if len1 != len(readbuf2):
        print('read gas err')
        uart1.close()
        return gas_val
        
    uart1.close()
    dotnum = (readbuf1[6]&0xf0)>> 4
    #sign = readbuf1[6]&0x0f
    gas_val = (readbuf2[2]*256.000 + readbuf2[3])*1.000/10**dotnum
    print('gasvalue:',end='')
    print(gas_val)
    return gas_val
	
def liquid_level_detec():
    lowval = liq_level.read()
    print('lowval',lowval)
    liq_meicn_remain = False

    if lowval == 1:
        liq_meicn_remain = True
    else:
        liq_meicn_remain = False
        
    return liq_meicn_remain
 
def main():
    global liq_level,cloud_ctrl,t1,liq_mdcn_alarm,gas_alarm
    ret = connect_network()
    print('network register sta {}'.format(ret))
    
    productKey = 'a1iJzDtH0Hy'
    productSecret = ''
    deviceName = 'haas505_demo_sn1'
    deviceSecret = '7e7ea7025c837fb57f73d7b3ff0d50a6'
    
    key_info = {
        'region' : 'cn-shanghai',
        'productKey' : productKey,
        'deviceName' : deviceName,
        'deviceSecret' : deviceSecret,
        'productSecret' : productSecret,
        'keepaliveSec': 60
    }
    
    device = Device()
    
    device.on(device.ON_CONNECT,on_connect)
    device.on(device.ON_DISCONNECT,on_disconnect)
    device.on(device.ON_PROPS,on_props)
    device.on(device.ON_SERVICE,on_service)
    device.on(device.ON_ERROR,on_error)
    
    device.connect(key_info) 
    send_info = {'ver':version,'name':key_info['deviceName']}
    post_data = {'params':ujson.dumps(send_info)}
    device.postProps(post_data)
    
    read_cfg_file()
    
    time.sleep(2)
    led1 = GPIO()
    pump = GPIO()

    '''liqid level detec prompt led'''
    led1.open('led1')
    '''liquid level detec io'''
    liq_level.open('liq_level')

    '''control pump relay'''
    pump.open('pump')
    pump.write(1)
    '''cloud_flg is cloud down data led'''
    gpio.open('cloud_flg') 
    time_cnt = 0
    gas_value = 0.00
    liq_mdcn_re_flg_chg = False
    need_send = False
    
    while True:
        time.sleep_ms(1000)
        time_cnt += 1
        liq_mdcn_re_flg = liquid_level_detec()
        if liq_mdcn_re_flg == False:
            led1.write(0)
            if liq_mdcn_re_flg_chg == True:
                liq_mdcn_re_flg_chg = False
                need_send = True
            pass
        else:
            led1.write(1)
            need_send = True
            liq_mdcn_re_flg_chg = True
            print('need send')
            '''need send data to cloud'''
            pass
        
        if time_cnt%10 == 0:
            gas_value = gas_detec()
            if gas_value > gas_threshold:
                '''need send data to cloud'''
                gas_alarm = True
                need_send = True
                print('need send')
            else:
                gas_alarm = False
            pass
        
        if liq_mdcn_re_flg == True:
            need_send = False
            pump.write(1)
            cloud_ctrl = 2
            print('close pump')
            post_data = {'params':{'liq_mdcn_re':0,'gasval':100,'gasalarm':0,'powerstate':0}}
            post_data['params']['liq_mdcn_re'] = 0
            gas_value = gas_detec()
            post_data['params']['gasval'] = int(gas_value*100)
            
            if gas_alarm == True:
                post_data['params']['gasalarm'] = 1

            post_data['params']['powerstate'] = gpio.read()
            post_data_dict = {'params':ujson.dumps(post_data['params'])}
                
            device.postProps(post_data_dict)
            continue
        if gas_alarm == False:
            if time_cnt%t1 == 0:
                if pump.read() == 1 :
                    pump.write(0)
                    print('open pump')
                else:
                    pump.write(1)
                    print('close pump')
            else:
                pass
                
            if cloud_ctrl == 0:
                pump.write(1)
                cloud_ctrl = 2
                time_cnt = 0
                print('cloud close pump')
            elif cloud_ctrl == 1:
                pump.write(0)
                cloud_ctrl = 2
                time_cnt = 0
                print('cloud open pump')
        elif gas_alarm == True:
            pump.write(1)
            print('gas alarm close pump')
            
        if need_send == True:
            need_send = False
            post_data1 = {'params':{'liq_mdcn_re':0,'gasval':100,'gasalarm':0,'powerstate':0}}
            if liq_mdcn_re_flg == True:
                post_data1['params']['liq_mdcn_re'] = 0
            else:
                post_data1['params']['liq_mdcn_re'] = 1
                
            post_data1['params']['gasval'] = int(gas_value*100)
            
            if gas_alarm == True:
                post_data1['params']['gasalarm'] = 1
                
            post_data1['params']['powerstate'] = gpio.read()
            post_data1_dict = {'params':ujson.dumps(post_data1['params'])}
            device.postProps(post_data1_dict)
            
        
if __name__ == '__main__':
    main()










