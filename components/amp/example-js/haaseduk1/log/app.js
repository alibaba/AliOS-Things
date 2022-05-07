import * as ulog from 'ulog'

//set local terminal output log level to debug
ulog.stdloglevel('debug')

//set local log file log level info
ulog.fsloglevel('warning')

//set local file save path
ulog.setlogfilepath('/data/ulog/')

//set log fize size to 8k
ulog.setlogfilesize(8192)

//ulog.cloudloglevel('info')

ulog.debug('test for debug');

ulog.info('test for info');

ulog.warn('test for warning');

ulog.error('test for error log');

ulog.fatal('test for fatal log');