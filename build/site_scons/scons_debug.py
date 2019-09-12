from scons_util import *
import subprocess
from collections import OrderedDict
from scons_upload import aos_upload

try:
    import urllib.request as urllib
except ImportError:
    import urllib

def taskkill(host_os, aos_path, exec_name):
    info("Killing %s\n" % exec_name)

    ret = 0
    if host_os == "Win32":
        # Don't display taskkill.exe output
        proc = subprocess.Popen("%s /F /IM %s" % (os.path.join(aos_path, "build", "cmd", "win32", "taskkill.exe"), exec_name), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        proc.wait()
    else:
        ret = subprocess.call("killall %s" % exec_name, shell=True, stdout=None, stderr=None)

    return ret

def process_cmd(cmd_data, host_os, aos_path, target, program_path=None, bin_dir=None):
    """ Replace hardcode strings from commands """

    exec_cmd = []
    port = 4242
    # url = ""
    for item in cmd_data:
        if type(item) == dict:
            if host_os in item:
                item = item[host_os]
                # item = tmp['cmd']
                # url = tmp['url']
            else:
                error("Debug command is not defined for %s!" % host_os)

        if '@AOSROOT@/out/@TARGET@' in item:
            if program_path:
                item = item.replace('@AOSROOT@', program_path)

        item = item.replace('@AOSROOT@', aos_path)
        item = item.replace('@TARGET@', target)
        exec_cmd += [item]
    exec_name = os.path.basename(exec_cmd[0])

    # Support user defined dir
    if bin_dir and os.path.isdir(bin_dir):
        for index, value in enumerate(exec_cmd):
            if '.elf' in value:
                binname = os.path.basename(value)
                exec_cmd[index] = os.path.join(bin_dir, binname)

    return exec_cmd, exec_name

def un_tar(filename, path):
    """ Extract files from tarball """
    import tarfile

    # Fix codec issue while extracting files
    import sys
    reload(sys)
    sys.setdefaultencoding('utf-8')

    try:
        tar = tarfile.open(filename)
        tar.extractall(path)
    except Exception as err:
        print(("[ERROR]: %s" % err))
        raise Exception
    finally:
        tar.close()

def download_tool(tool, url):
    """ Download tool from url if the tool is not existing """
    if not url:
        error("URL for debug tool is not defined!")

    info("Trying to download %s from %s" % (tool, url))
    filename = os.path.basename(url)
    try:
        urllib.urlretrieve(url, filename)
    except:
        pass

    if 'compiler' in tool:
        # Strip "$toolchain/$arch/bin/$gdb"
        path = '/'.join(tool.split('/')[:-4])
    elif 'openocd_master' in tool:
        # Strip "openocd_master/bin/openocd_*"
        path = '/'.join(tool.split('/')[:-3])
    else:
        path = os.path.dirname(tool)

    if os.path.isfile(filename):
        un_tar(filename, path)
        os.remove(filename)

    if not os.path.isfile(tool):
        error("Can't find tool %s" % tool)

def update_launch(aos_path, target, port):
    host_os = get_host_os()
    downloads = []
    gdb_path = ''
    command = 'arm-none-eabi-gdb'
    toolchain_config = os.path.join(aos_path, 'build', 'toolchain_config.py')
    if os.path.exists(toolchain_config) == False:
        error("Can't get toolchain config, missing %s" % toolchain_config)
    from toolchain_config import boards

    (app, board) = target.split('@')
    if board in boards:
        for toolchain in boards[board]:
            # name = toolchain['name']
            command = toolchain['command'].replace('gcc', 'gdb')
            if 'path_specific' in toolchain:
                gdb_path = '{}/bin/{}'.format(toolchain['path'], command)
            else:
                gdb_path = '{}/{}/bin/{}'.format(toolchain['path'], host_os, command)

    gdb_path = os.path.join(aos_path, '', gdb_path)
    if host_os == 'Win32':
        gdb_path = '%s.exe' % gdb_path
    
    if os.path.isfile(gdb_path) == False:
        # toolchain not exist, using gdb name
        log("[WARNING] gdb not EXIST: %s" % gdb_path)
        gdb_path = command
        if host_os == 'Win32':
            gdb_path = '%s.exe' % gdb_path
    
    launch_path = os.path.join(aos_path, '.vscode', 'launch.json')
    try:
        with open(launch_path, 'r') as f:
            launch_file = json.load(f, object_pairs_hook=OrderedDict)
    except IOError as e:
        error(e)
    
    new_launch = {}
    #print new_launch
    new_launch['version'] = launch_file['version']
    #print new_launch
    new_launch['configurations'] = []
    #print new_launch
    for c in launch_file['configurations']:
        if c['name'] != 'Debug':
            return

        c['program'] = '${workspaceRoot}/out/%s/binary/%s.elf' % (target, target)
        c['miDebuggerServerAddress'] = 'localhost:%d' % port
        c['setupCommands'][1]['text'] = 'target remote localhost:%d' % port
        c['setupCommands'][2]['text'] = 'file out/%s/binary/%s.elf' % (target, target)

        if host_os == 'Win32':
            c['windows']['miDebuggerPath'] = gdb_path
        elif host_os == 'Linux32' or host_os == 'Linux64':
            c['linux']['miDebuggerPath'] = gdb_path
        elif host_os == 'OSX':
            c['osx']['miDebuggerPath'] = gdb_path
        new_launch['configurations'].append(c)

    try:
        write_json(launch_path, new_launch)
    except IOError as e:
        error(e)

def _run_debug_cmd(target, aos_path, cmd_file, program_path=None, bin_dir=None, startclient=False, gdb_args=None):
    """ Run the command from cmd file """
    ret = 0

    host_os = get_host_os()
    if not host_os:
        error("Unsupported Operating System!")

    configs = read_json(cmd_file)

    if not configs:
        error("Can not read flash configs from %s" % cmd_file)

    if 'port' not in configs:
        error("need PORT in debug config file: %s" % cmd_file)

    # upload firmware first
    if 'upload' in configs and configs['upload'] == True:
        if aos_upload(target, aos_path):
            error("Upload firmware error")

    # update_launch(aos_path, target, configs['port'])
    start_server,exec_name = process_cmd(configs['cmd'], host_os, aos_path, target, program_path, bin_dir)
    info("Running cmd:\n\t'%s', Exec: %s\n" % (' '.join(start_server), exec_name))

    taskkill(host_os, aos_path, exec_name)

    if host_os == 'Win32':
        ret = subprocess.Popen(start_server, shell=True,stdout=sys.stdout, stderr=sys.stderr, creationflags=subprocess.CREATE_NEW_CONSOLE)
    else:
        ret = subprocess.Popen(start_server, stdout=sys.stdout, stderr=sys.stderr, preexec_fn=os.setsid)

    if ret != 0 and configs['prompt']:
        info("%s" % configs['prompt'])

    return ret

def _debug_app(target, aos_path, registry_file, program_path=None, bin_dir=None, startclient=False, gdb_args=None):
    """ Debug app according to configs """
    (app, board) = target.split('@')
    cmd_file_dir = os.path.dirname(registry_file)
    cmd_files = None
    ret = 0

    # Check elf exist
    elf_file = os.path.join(program_path if program_path else aos_path, "out", target, "binary", "%s.elf" % target)
    if not os.path.exists(elf_file):
        error("Please build target[%s] first" % target)

    # Get valid board from registry file
    registry_board = read_json(registry_file)

    if not registry_board:
        error("Can not read registered boards from %s" % registry_file)

    # 1. Fully match with board name: key = board
    for key in registry_board:
        if key == board:
            cmd_files = registry_board[key]

    # 2. Part match with board name: key in board
    if not cmd_files:
        for key in registry_board:
            if key in board:
                cmd_files = registry_board[key]

    # 3. Use 'ALLOTHERS' if cmd_files is still None
    if not cmd_files:
        if 'ALLOTHERS' in registry_board:
            cmd_files = registry_board['ALLOTHERS']

    if cmd_files:
        for cmd_file in cmd_files:
            ret = _run_debug_cmd(target, aos_path, os.path.join(cmd_file_dir, cmd_file), program_path, bin_dir, startclient, gdb_args)
    else:
        error("The board %s is not registered in %s" % (board, registry_file))

    return ret

def aos_debug(target, work_path=None, bin_dir=None, startclient=False, gdb_args=None):
    program_path = None

    if '@' not in target or len(target.split('@')) != 2:
        error("Target invalid!")

    if work_path:
        aos_path = get_config_value('AOS_SDK_PATH')
        program_path = os.getcwd()
    else:
        if os.path.isdir('./kernel/rhino') or os.path.isdir('./include/aos'):
            info("Currently in aos_sdk_path: '%s'\n" % os.getcwd())
            aos_path = os.getcwd()
        else:
            info("Not in aos_sdk_path, curr_path:'%s'\n" % os.getcwd())
            aos_path = get_config_value('os_path')
            if not aos_path:
                error("aos_sdk is unavailable, please run 'aos new $prj_name'!")
            else:
                info("Load aos configs success, set '%s' as sdk path\n" % aos_path)

    # program_path = get_config_value('program_path')
    registry_file = os.path.split(os.path.realpath(__file__))[0] + '/debug/registry_board.json'
    if os.path.isfile(registry_file):
        ret = _debug_app(target, aos_path, registry_file, program_path, bin_dir, startclient, gdb_args)
    else:
        error("Can not find file: %s" % registry_file)

    return ret
