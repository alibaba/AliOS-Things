import os, time

#allocate devices
def allocate_devices(at, model, number, timeout, purpose='general'):
    models = ['mk3060', 'esp32']
    if not model or model.lower() not in models:
        print "error: unsupported model {0}".format(repr(model))
        return []
    allocated = at.device_allocate(model, number, timeout, purpose)
    if len(allocated) != number:
        print "error: request device allocation failed"
        return []
    return allocated

def print_device_list(devices):
    device_list = list(devices)
    device_list.sort()
    print 'allocated devices:'
    for device in device_list:
        print '  {0}:{1}'.format(device, devices[device])
    print ''

#subscribe and reboot devices
def subscribe_and_reboot_devices(at, devices):
    if at.device_subscribe(devices) == False:
        print 'error: subscribe to device failed, some devices may not exist in testbed'
        return False
    for device in list(devices):
        at.device_control(device, 'reset')
    time.sleep(5)
    return True

#program devices
def program_devices(at, devices, model, firmware):
    print 'program devices ...',
    models = {'mk3060':'0x13200', 'esp32':'0x10000'}
    if model not in models:
        print "error: unsupported device model {0}".format(repr(model))
        return False
    device_list = list(devices)
    device_list.sort()
    addr = models[model]
    for device in device_list:
        succeed = False; retry = 5
        for i in range(retry):
            if at.device_program(device, addr, firmware) == True:
                succeed = True
                break
            time.sleep(1)
        if succeed == False:
            print 'error: program device {0}:{1} failed'.format(device, devices[device])
            return False
    time.sleep(5)
    print 'succeed\n'
    return True

#reboot and get device mac address
def reboot_and_get_mac(at, device_list, device_attr):
    retry = 5
    for device in device_list:
        succeed = False
        for i in range(retry):
            at.device_run_cmd(device, 'netmgr clear')
            at.device_run_cmd(device, 'kv del alink')
            mac = at.device_run_cmd(device, 'mac', 1, 0.8, ['MAC address:'])
            at.device_control(device, 'reset')
            if mac and len(mac) == 1:
                mac = mac[0].split()[-1]
                mac = mac.replace('-', '') + '0000'
                device_attr[device] = {'mac':mac}
                succeed = True
                break;
            time.sleep(5)
        if succeed == False:
            print 'error: reboot and get mac addr for device {0} failed'.format(device)
            return False
    time.sleep(5)
    return True

#set random extnetid to isolate the network
def set_random_extnetid(at, device_list):
    bytes = os.urandom(6)
    extnetid = ''
    for byte in bytes:
        extnetid = extnetid + '{0:02x}'.format(ord(byte))
    for device in device_list:
        at.device_run_cmd(device, 'umesh extnetid {0}'.format(extnetid))

#start devices
def start_devices(at, device_list, device_attr, ap_ssid, ap_pass):
    filter = [
        'state\tdisabled',
        'state\tdetached',
        'state\tattached',
        'state\tleaf',
        'state\trouter',
        'state\tsuper_router',
        'state\tleader',
        'state\tunknown'
        ]
    for device in device_list:
        at.device_run_cmd(device, 'umesh stop')
    for device in device_list:
        succeed = False; retry = 5
        role = device_attr[device]['role']
        state = []
        while retry > 0:
            at.device_run_cmd(device, 'umesh stop')
            for nbr in device_attr[device]['nbrs']:
                at.device_run_cmd(device, 'umesh whitelist add {0}'.format(nbr))
            at.device_run_cmd(device, 'umesh whitelist enable')
            at.device_run_cmd(device, 'umesh whitelist')
            if role == 'leader':
                at.device_run_cmd(device, 'netmgr connect {0} {1}'.format(ap_ssid, ap_pass))
                time.sleep(30)
                uuid = at.device_run_cmd(device, 'uuid', 1, 1.5, ['uuid:', 'not connected'])
                state = at.device_run_cmd(device, 'umesh status', 1, 1, filter)
                if uuid and 'uuid:' in uuid[0] and state == ['state\t' + role]:
                    succeed = True
                    break
            else:
                at.device_run_cmd(device, 'umesh start')
                time.sleep(30)
                state = at.device_run_cmd(device, 'umesh status', 1, 1, filter)
                if state == ['state\t' + role]:
                    succeed = True
                    break
            at.device_run_cmd(device, 'netmgr clear')
            at.device_run_cmd(device, 'kv del alink')
            at.device_control(device, 'reset')
            time.sleep(5)
            retry -= 1
        if succeed == False:
            print "error: start {0} as {1} failed, state={2}".format(device, role, state)
            return False
    print "form desired mesh network succeed\n"
    return True


#get device ips
def get_device_ips(at, device_list, device_attr):
    for device in device_list:
        succeed = False; retry = 3
        while retry > 0:
            ipaddr = at.device_run_cmd(device, 'umesh ipaddr', 2, 1.5, ['.'])
            if ipaddr == False or ipaddr == [] or len(ipaddr) != 2:
                retry -= 1
                continue
            ipaddr[0] = ipaddr[0].replace('\t', '')
            ipaddr[1] = ipaddr[1].replace('\t', '')
            device_attr[device]['ipaddr'] = ipaddr[0:2]
            succeed = True
            break;
        if succeed == False:
            print 'error: get ipaddr for device {0} failed'.format(device)
            return False
    return True

#print device attributes
def print_device_attrs(device_attr):
    device_list = list(device_attr)
    device_list.sort()
    for device in device_list:
        attr_str  = "{0}: ".format(device)
        attr_str += "mac-{0} ".format(device_attr[device]['mac'])
        attr_str += "ip-{0} ".format(device_attr[device]['ipaddr'][0])
        attr_str += "role-{0} ".format(device_attr[device]['role'])
        print attr_str
    print ''

#ping test
def ping_test(at, device_list, device_attr):
    retry = 5
    print 'test connectivity with icmp:'
    pass_num = 0; fail_num = 0
    for device in device_list:
        for other in device_list:
            if device == other:
                continue
            for pkt_len in ['20', '500', '1000']:
                filter = ['bytes from']
                dst_ip = device_attr[other]['ipaddr'][0]
                for i in range(retry):
                    response = at.device_run_cmd(device, 'umesh ping {0} {1}'.format(dst_ip, pkt_len), 1, 0.5, filter)
                    expected_response = '{0} bytes from {1}'.format(pkt_len, dst_ip)
                    if response == False or response == [] or expected_response not in response[0]:
                        if i < retry - 1:
                            continue
                        else:
                            print '{0} ping {1} with {2} bytes by local ip addr failed'.format(device, other, pkt_len)
                            fail_num += 1
                            break
                    else:
                        pass_num += 1
                        break
    print 'ping: pass-{0}, fail-{1}\n'.format(pass_num, fail_num)
    return [pass_num, fail_num]

#ping external ip test
def ping_ext_test(at, device_list, dst_ip):
    retry = 5
    print 'test connectivity of external ip with icmp:'
    pass_num = 0
    fail_num = 0
    for device in device_list:
        for pkt_len in ['20', '500', '1000']:
            filter = ['bytes from']
            for i in range(retry):
                response = at.device_run_cmd(device, 'umesh ping {0} {1}'.format(dst_ip, pkt_len), 1, 0.5, filter)
                expected_response = '{0} bytes from {1}'.format(pkt_len, dst_ip)
                if response is False or response == [] or expected_response not in response[0]:
                    if i < retry - 1:
                        continue
                    else:
                        print '{0} ping {1} with {2} bytes by local ip addr failed'.format(device, dst_ip, pkt_len)
                        fail_num += 1
                        break
                else:
                    pass_num += 1
                    break
    print 'ping: pass-{0}, fail-{1}\n'.format(pass_num, fail_num)
    return [pass_num, fail_num]

#udp test
def udp_test(at, device_list, device_attr):
    for device in device_list:
        ret = at.device_run_cmd(device, 'umesh help', 1, 1, ['autotest'])
        if ret != ['autotest']:
            print "udp test aborted: 'umesh autotest' operation not supported\n"
            return [0, 0]
    retry = 5
    print 'test connectivity with udp:'
    pass_num = 0; fail_num = 0
    for device in device_list:
        for other in device_list:
            if device == other:
                continue
            for pkt_len in ['20', '500', '1000']:
                dst_ip = device_attr[other]['ipaddr'][0]
                filter = ['bytes autotest echo reply from']
                for i in range(retry):
                    response = at.device_run_cmd(device, 'umesh autotest {0} 1 {1}'.format(dst_ip, pkt_len), 1, 0.5, filter)
                    expected_response = '{0} bytes autotest echo reply from {1}'.format(pkt_len, dst_ip)
                    if response == False or response == [] or expected_response not in response[0]:
                        if i < retry - 1:
                            continue
                        else:
                            print '{0} send {1} with {2} bytes by local ip addr failed'.format(device, other, pkt_len)
                            fail_num += 1
                            break
                    else:
                        pass_num += 1
                        break
    print 'udp: pass-{0}, fail-{1}\n'.format(pass_num, fail_num)
    return [pass_num, fail_num]

def multicast_test(at, device_list, device_attr):
    for device in device_list:
        ret = at.device_run_cmd(device, 'umesh help', 1, 1, ['autotest'])
        if ret != ['autotest']:
            print "multicast test aborted: 'umesh autotest' operation not supported\n"
            return [0, 0]
    retry = 20
    print 'test multicast connectivity:'
    pass_num = 0; fail_num = 0
    response_num = str(len(device_list) - 1)
    for pkt_len in ['20', '400']:
        for device in device_list:
            dst_ip = device_attr[device]['ipaddr'][1]
            for index in range(retry):
                at.device_run_cmd(device, 'umesh autotest {0} 1 {1}'.format(dst_ip, pkt_len))
                time.sleep(4)
                response = at.device_run_cmd(device, 'umesh testcmd autotest_acked', 1, 1, [response_num])
                if response == [] or len(response) != 1 or response_num not in response[0]:
                    if index < retry - 1:
                        continue
                    else:
                        print '{0} multicast {1} bytes failed'.format(device, pkt_len)
                        fail_num += 1
                        break
                else:
                    pass_num += 1
                    break
    print 'udp: pass-{0}, fail-{1}'.format(pass_num, fail_num)
    return [pass_num, fail_num]

#restore state to default
def restore_device_status(at, device_list):
    extnetid = '010203040506'
    for device in device_list:
        at.device_run_cmd(device, 'umesh extnetid {0}'.format(extnetid))
        at.device_run_cmd(device, 'umesh whitelist disable')
