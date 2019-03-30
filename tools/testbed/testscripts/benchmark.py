#!/usr/bin/python

import os, sys, time, re
from autotest import Testcase

required_devices = [ [1, 'general'] ]

ret = 0; result = 'succeed'

#realtime  start

max_permit_absolute = float(0.6)
max_permit_relative = float(0.1)

#if log with maxint_distime not include base_time
base_time = 'basetime'
maxint_distime = 'Max intrpt disable time'
realtime_standard_values = {
    'developerkit':{
        base_time:{
            'sched_yield':'2.27',
            'sched_preempt':'4.51',
            'sync_sem_shuffling':'6.83',
            'sync_mutex_shuffling': '7.96',
            'message_queue_send': '1.60',
            'message_queue_rev': '1.61',
            'mem_tlf_alloc': '16.17',
            'mem_tlf_free': '8.56',
        },
        maxint_distime:'5.30',
    }

}

def realtime_deal(model,responses):
    tmp_value = realtime_standard_values[model]
    compare_value = tmp_value[base_time]

    for res in responses :
        if maxint_distime in res:
            compare_value = tmp_value


    for res in responses :
        for key,values in compare_value.items():
            if key in res :
                testval = re.search("\s\d+(\.\d+)?",res).group(0).strip()
                cmpval = compare_value[key]
                if (float(testval) > float(cmpval)) and (float(testval) - float(cmpval) > max_permit_absolute) \
                and ((float(testval) - float(cmpval))/float(cmpval) > max_permit_relative):
                    print '\n\033[1;31m {}:fail, run val: {}, compare val: {} \033[0m!'.format(key, testval, cmpval)
                    ret = 1; result = 'failed'
                else:
                    print '\n\033[1;30m {}:success, run val: {}, compare val: {} \033[0m!'.format(key, testval, cmpval)



#realtime  end


#pwr  start

def pwr_deal(model,responses):
    idletmp = 0;
    for res in responses :
        if r'idle' in res:
            idle = re.search("idle\s=\s\d+",res).group(0).strip()
            idle = re.sub(r'idle\s=',"",idle)
            idle = idle.strip()
            if (int(idle) - int(idletmp)) > 10:
                print '\n\033[1;31m pwr test fail, last count: {}, current val: {} \033[0m!'.format(idletmp, idle)
                ret = 1; result = 'failed'
            else:
                print '\n\033[1;30m success, last count: {}, current val: {} \033[0m!'.format(idletmp, idle)

            idletmp  = idle

#pwr  end


def main(firmware='benchmark.realtime_test@mk3060-general.bin', model='mk3060'):
    tc = Testcase('benchmark.realtime_test')
    at = tc.at

    dealfun = 0
    number, purpose = required_devices[0]
    [ret, result] = tc.start(number, purpose)
    if ret != 0:
        tc.stop()
        print '\nbenchmark test failed'
        return [ret, result]

    model_test = tc.get_model()
    firmware_test = tc.get_firmware()
    number_of_responses = 200; timeout = 6
    filter = [""]

    if 'realtime_test' in firmware_test:
        number_of_responses = 200; timeout = 6
        dealfun = realtime_deal
    elif 'pwr_test' in firmware_test:
        number_of_responses = 5; timeout = 20
        dealfun = pwr_deal
        filter = ['idle =']

    responses = at.device_read_log('A', number_of_responses, timeout, filter)

    tc.stop() #disconnect testbed server

    ret = 0; result = 'succeed'
    if len(responses) <= 0:
        print 'error: can not get {} lines of log containing {}'.format(number_of_responses, filter)
        ret = 1; result = 'failed'

    dealfun(model_test,responses)

    print '\nlogs:'
    tc.dump_log()

    print '\n{} {}'.format(firmware_test,result)

    return [ret, result]

if __name__ == '__main__':
    [code, msg] = main()
    sys.exit(code)
