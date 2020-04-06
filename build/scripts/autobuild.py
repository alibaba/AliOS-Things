#!/usr/bin/env python

import os, sys, re, json, platform, signal
import traceback, hashlib, subprocess

keep_running = True

def signal_handler(sig, frame):
    global keep_running
    keep_running = False
    print("received SIGINT, aborting build ...")

def search_apps():
    app_dirs = []
    apps = {}

    if os.path.isdir('application/example'):
        if os.path.isdir('application/example/example_legacy'):
            app_dirs.append('application/example/example_legacy')
        app_dirs.append('application/example')
    elif os.path.isdir('app/example'):
        app_dirs.append('app/example')
    elif os.path.isdir('example'):
        app_dirs.append('example')

    for app_dir in app_dirs:
        dirs = os.listdir(app_dir)
        for dir in dirs:
            app = None
            mkfile1 = '{0}/{1}/{1}.mk'.format(app_dir, dir)
            mkfile2 = '{0}/{1}/aos.mk'.format(app_dir, dir)
            if os.path.isfile(mkfile1) or os.path.isfile(mkfile2):
                app = dir
                apps[app] = '{}/{}'.format(app_dir, dir)
            elif os.path.isdir('{}/{}'.format(app_dir, dir)):
                if dir == 'example_legacy': continue
                subdirs = os.listdir('{}/{}'.format(app_dir, dir))
                for subdir in subdirs:
                    mkfile1 = '{0}/{1}/{2}/{2}.mk'.format(app_dir, dir, subdir)
                    mkfile2 = '{0}/{1}/{2}/aos.mk'.format(app_dir, dir, subdir)
                    if os.path.isfile(mkfile1) == False and os.path.isfile(mkfile2) == False:
                        continue
                    app = '{}.{}'.format(dir, subdir)
                    apps[app] = '{}/{}/{}'.format(app_dir, dir, subdir)
    return apps

def search_boards():
    board_dirs = []
    boards = {}

    if os.path.isdir('platform/board'):
        if os.path.isdir('platform/board/board_legacy'):
            board_dirs.append('platform/board/board_legacy')
        board_dirs.append('platform/board')
    else:
        board_dirs.append('board')

    for board_dir in board_dirs:
        dirs = os.listdir(board_dir)
        for dir in dirs:
            mkfile1 = '{0}/{1}/{1}.mk'.format(board_dir, dir)
            mkfile2 = '{}/{}/aos.mk'.format(board_dir, dir)
            if os.path.isfile(mkfile1) == False and os.path.isfile(mkfile2) == False:
                continue
            boards[dir] = '{}/{}'.format(board_dir, dir)
    return boards

def parse_autobuild_target_for_board(path):
    targets = []
    cubefile = os.path.join(path,  'ucube.py')
    if os.path.exists(cubefile) == False:
        return targets
    f = open(cubefile, 'rt')
    lines = f.readlines()
    f.close()

    target_filter = re.compile('(supported_targets|linux_only_targets|windows_only_targets)')
    double_qoute = re.compile('"(.*)"')
    single_qoute = re.compile("'(.*)'")
    for line in lines:
       if target_filter.search(line) == None:
           continue
       if '=' not in line:
           continue
       d_match = double_qoute.search(line)
       s_match = single_qoute.search(line)
       if d_match:
           match = d_match
       elif s_match:
           match = s_match
       else:
           continue
       supported_apps = ' '.join(match.group(1).strip(' ').split()).split()
       for index in range(len(supported_apps)):
           args = supported_apps[index].split('|')
           app = args[0];
           args = ' '.join(args[1:])
           supported_apps[index] = {'app': app, 'params': args}
           if 'supported_targets' in line:
               targets.append({'app': app, 'os': 'linux', 'params': args})
               targets.append({'app': app, 'os': 'windows', 'params': args})
           elif 'linux_only_targets' in line:
               targets.append({'app': app, 'os': 'linux', 'params': args})
           elif 'windows_only_targets' in line:
               targets.append({'app': app, 'os': 'windows', 'params': args})
    return targets

def parse_autobuild_target_for_app(path):
    config_file = os.path.join(path, 'autobuild.json')
    targets = []
    try:
        with open(config_file, 'rt') as f:
            targets = json.load(f)
    except:
        pass
    return targets

def parse_autobuild_targets(os=None):
    import subprocess
    cmd = 'find application/example/ -name "autobuild.json" | grep -q "json"'
    config_in_application = (subprocess.call(cmd, shell=True) == 0)
    app_list = search_apps()
    board_list = search_boards()
    targets = []
    if config_in_application:
        for app in sorted(list(app_list)):
            app_path = app_list[app]
            parsed_targets = parse_autobuild_target_for_app(app_path)
            for target in parsed_targets:
                if target['board'] not in board_list:
                    print('Warning: board {} for app {} does not exist'.format(target['board'], app))
                    continue
                if os != None and target['os'] != os:
                    continue
                board_path = board_list[target['board']]
                target['app'] = app
                target['app_path'] = app_path
                target['board_path'] = board_path
                targets.append(target)
    else:
        for board in sorted(list(board_list)):
            board_path = board_list[board]
            parsed_targets = parse_autobuild_target_for_board(board_path)
            for target in parsed_targets:
                if target['app'] not in app_list:
                    print('Warning: app {} for board {} does not exist'.format(target['app'], board))
                if os != None and target['os'] != os:
                    continue
                app_path = app_list[target['app']]
                target['board'] = board
                target['app_path'] = app_path
                target['board_path'] = board_path
                targets.append(target)
    return targets

def do_build_target(target, jnum):
    code_dir = os.getcwd();

    flog = open('temp.log', 'wt')

    #clean up
    cmd = 'rm -rf out .config* .defconfig'
    subprocess.call(cmd , stdout=flog, stderr=flog, shell=True)

    #check if using two_step build
    two_step = subprocess.call('grep "call NO_CONFIG_ERROR" build/Makefile', stdout=flog, stderr=flog, shell=True)
    create_then_make = subprocess.call('grep "^update:" build/build_rules/aos_kconfig.mk', stdout=flog, stderr=flog, shell=True)
    if two_step != 0:  #traditional build
        cmd = 'timeout 480 aos make {}@{} {} JOBS={}'.format(target['app'], target['board'], target['params'], jnum)
    else:  #two step build
        if create_then_make == 0:
            dst_dir = hashlib.md5('{}'.format(target)).hexdigest()
            app = target['app'].split('.')[-1]
            board = target['board']
            cmd = 'export AOS_SDK_PATH={}/ && '.format(code_dir)
            cmd += "timeout 60 aos create project -b {0} -t {1} -d /tmp/{2} {1} && ".format(board, app, dst_dir)
            cmd += "cd /tmp/{}/{} && ".format(dst_dir, app)
        else:
            cmd = "timeout 60 aos make {}@{} -c config && ".format(target['app'], target['board'])
        if target['params']:
            cmd += 'timeout 480 aos make JOBS={} {}'.format(jnum, target['params'])
        else:
            cmd += 'timeout 480 aos make JOBS={}'.format(jnum)
    flog.close()
    subprocess.call('rm -f temp.log', shell=True)

    #print cmd
    try:
        log = subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
        result = 'success'
        print("build {}@{} succeed".format(target['app'], target['board']))
    except subprocess.CalledProcessError as e:
        log = e.output
        result = 'fail'
        print("build {}@{} failed, log:".format(target['app'], target['board']))
        print(log)
        print("build {}@{} failed".format(target['app'], target['board']))
    except:
        log = "build error:" + traceback.format_exc()
        result = 'fail'
        print("build {}@{} encontered error, info:\n{}".format(target['app'], target['board'], traceback.format_exc()))
    if create_then_make == 0:
        subprocess.call('rm -rf /tmp/{}'.format(dst_dir), shell=True)
    if len(log) > 65500: log = log[-65500:]
    return result

def build():
    global keep_running
    if len(sys.argv) < 2:
        board_file = None
    else:
        board_file = sys.argv[1]

    #get cpu cores
    try:
        jnum = int(subprocess.check_output('cat /proc/cpuinfo | grep processor | wc -l', shell=True))
    except:
        jnum = 4

    if os.name == 'posix':
        system = 'linux'
    elif os.name == 'nt':
        system = 'windows'
    else:
        print('error: unkonw OS')
        sys.exit(1)
    print('OS: {}'.format(platform.system()))

    if board_file != None:
        if os.path.isfile(board_file) == False:
            print("error: boards list file {} does not existed!".format(board_file))
            sys.exit(1)
        try:
            with open(board_file, 'r') as f:
                boards = f.read().replace('\n', ' ')
                boards = boards.strip(' ')
                boards = ' '.join(boards.split(' ')).split(' ')
        except:
            traceback.print_exc()
            print("error: parse boards list file {} failed!".format(board_file))
            sys.exit(1)

    org_dir = os.getcwd()
    aos_dir = os.path.dirname(os.path.abspath(__file__)) + '/../../'
    os.chdir(aos_dir)

    if board_file == None:
        boards = search_boards()

    targets = parse_autobuild_targets(system)

    ret = 0
    failed = []
    signal.signal(signal.SIGINT, signal_handler)
    for board in sorted(list(boards)):
        for target in targets:
            if target['board'] != board:
                continue
            if keep_running == False:
                break
            result = do_build_target(target, jnum)
            if result != 'success':
                failed.append(target)
                ret += 1
        if keep_running == False:
            break

    if keep_running and ret > 0:
        print('\nfailed list:')
        for index in range(len(failed)):
            target = failed[index]
            cmd = '{}@{}'.format(target['app'], target['board'])
            if target['params']: cmd += ' ' + target['params']
            print('  {}. {}'.format(index+1, cmd))

    if keep_running == False:
        result = 'aborted'
    elif ret > 0:
        result = 'failed'
    else:
        result = 'succeed'
    print('autobuild {}').format(result)
    return ret

if __name__ == '__main__':
    sys.exit(build())
