#!/usr/bin/env python

import os, sys, subprocess, json

def cd_aos_root():
    import platform, re
    if platform.system() == 'Windows':
        sys_root = re.compile(r'^[A-Z]{1}:\\$')
    else:
        sys_root = re.compile('^/$')
    while os.path.isdir('./kernel/rhino') == False:
        os.chdir('../')
        if sys_root.match(os.getcwd()):
            return 'fail'
    return 'success'

def get_compiled_boards(app_name):
    cmd = "grep -rl '{}' --include='ucube.py' board | sed 's/board\///g' | sed 's/\/ucube.py//g' | sort | tr '\n' ' '".format(app_name)
    try:
        boards = subprocess.check_output(cmd, shell=True)[:-1]
        boards = boards.split(' ')
    except:
        traceback.print_exc()
        boards = []
    return boards

def get_validated_boards(app_name, compiled_boards):
    import pv_config
    builds = {}
    for target in pv_config.buildtargets:
        builds[pv_config.buildtargets[target]['bincopy'][1]] = pv_config.buildtargets[target]['buildargs']
    #print builds

    tests = {}
    for model in pv_config.models:
        for [suit, test] in pv_config.models[model]:
            if suit not in pv_config.testsuits:
                continue
            if test not in pv_config.testsuits[suit]['tests']:
                continue
            firmware = pv_config.testsuits[suit]['tests'][test]['firmware_prefix'] + model + pv_config.testsuits[suit]['tests'][test]['firmware_suffix']
            if firmware not in builds:
                #print "warning: firmware {} not compiled".format(firmware)
                continue
            info = ':'.join([model, suit, test])
            tests[info] = builds[firmware]
    #print json.dumps(tests, indent=4, sort_keys=True)

    boards = []
    for board in compiled_boards:
        for test in tests:
            cmd = '@'.join([app_name, board])
            if cmd in tests[test]:
                if board not in boards:
                    boards.append(board)
                break

    boards = sorted(boards)
    return boards

def main(apps):
    work_dir = os.getcwd()
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    cd_aos_root()
    sys.path.append('tools/testbed/testscripts')
    info = {}
    try:
        for app in apps:
            compiled_boards = get_compiled_boards(app)
            validated_boards = get_validated_boards(app, compiled_boards)
            info[app] = {'compiled': compiled_boards, 'validated': validated_boards}
        with open('{}/app_validate_status.json'.format(work_dir), 'wt') as f:
            json.dump(info, f, indent=4, sort_keys=True)
        print "app compile & validate info writen to app_validate_status.json"
        return 0
    except:
        traceback.print_exc()
        return 1

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "usage: python {} app1 [app2 ... appN]".format(sys.argv[0])
        sys.exit(1)
    apps = sys.argv[1:]
    ret = main(apps)
    sys.exit(ret)

