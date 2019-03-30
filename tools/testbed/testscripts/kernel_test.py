import os, sys, time
from autotest import Testcase

required_devices = [ [1, 'general'] ]

def main(firmware='yts@esp32-general.bin', model='esp32'):
    tc = Testcase('kernel')
    at = tc.at

    number, purpose = required_devices[0]
    [ret, result] = tc.start(number, purpose)
    if ret != 0:
        tc.stop()
        print '\nkernel test failed'
        return [ret, result]

    timeout = 300
    at.device_run_cmd('A', 'yts_run') #run yts test
    responses = at.device_read_log('A', 1, timeout, ['Tests completed with return value'])

    tc.stop()

    if len(responses) < 1:
        print 'error: yts_run did not finish in {}S'.format(timeout)
        ret = 1; result = 'failed'
    elif 'return value 0' not in responses[0]:
        print 'error: {}'.format(responses[0])
        ret = 1; result = 'failed'
    else:
        ret = 0; result = 'succeed'

    print '\nlogs:'
    tc.dump_log()

    print '\nkernel test {}'.format(result)
    return [ret, result]

if __name__ == '__main__':
    [code, msg] = main()
    sys.exit(code)
