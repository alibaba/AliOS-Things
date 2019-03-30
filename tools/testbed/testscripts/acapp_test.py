import os, sys, time
from autotest import Testcase

required_devices = [ [1, 'general'] ]

def main(firmware='acapp@mk3060-general.bin', model='mk3060'):
    tc = Testcase('activation')
    at = tc.at

    number, purpose = required_devices[0]
    [ret, result] = tc.start(number, purpose)
    if ret != 0:
        tc.stop()
        print '\nactivation test failed'
        return [ret, result]

    keyword_filter = ['version_verify result:']
    number_of_responses = 1
    timeout = 20
    response = at.device_read_log('A', number_of_responses, timeout, keyword_filter)

    tc.stop()

    ret = 0; result = 'succeed'
    if len(response) < number_of_responses or 'successed' not in response[0]:
        print 'response:', repr(response)
        ret = 1; result = 'failed'

    print '\nlogs:'
    tc.dump_log()

    print '\nactivation test {}'.format(result)

    return [ret, result]

if __name__ == '__main__':
    [code, msg] = main()
    sys.exit(code)
