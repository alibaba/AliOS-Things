import os, sys, time
from autotest import Testcase

required_devices = [ [1, 'general'] ]

def main(firmware='helloworld@mk3060-general.bin', model='mk3060'):
    tc = Testcase('helloworld')
    at = tc.at

    number, purpose = required_devices[0]
    [ret, result] = tc.start(number, purpose)
    if ret != 0:
        tc.stop()
        print '\nhelloworld test failed'
        return [ret, result]

    number_of_responses = 10; timeout = 60
    filter = ['helloworld', 'hello world!']
    responses = at.device_read_log('A', number_of_responses, timeout, filter)

    tc.stop() #disconnect testbed server

    ret = 0; result = 'succeed'
    if len(responses) < number_of_responses:
        print 'error: can not get {} lines of log containing {}'.format(number_of_responses, filter)
        print "responses:\n{}".format('\n'.join(responses))
        ret = 1; result = 'failed'

    print '\nlogs:'
    tc.dump_log()

    print '\nhelloworld test {}'.format(result)

    return [ret, result]

if __name__ == '__main__':
    [code, msg] = main()
    sys.exit(code)
