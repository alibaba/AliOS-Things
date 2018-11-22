from scons_util import *
import subprocess
import urllib

def process_cmd(cmd_data, host_os, aos_path, target, program_path=None, bin_dir=None):
    """ Replace hardcode strings from commands """

    exec_cmd = []
    url = ""
    for item in cmd_data:
        if type(item) == dict:
            if host_os in item:
                tmp = item[host_os]
                item = tmp['cmd']
                url = tmp['url']
            else:
                error("Debug command is not defined for %s!" % host_os)

        if '@AOSROOT@/out/@TARGET@' in item:
            if program_path:
                item = item.replace('@AOSROOT@', program_path)

        item = item.replace('@AOSROOT@', aos_path)
        item = item.replace('@TARGET@', target)
        exec_cmd += [item]

    # Support user defined dir
    if bin_dir and os.path.isdir(bin_dir):
        for index, value in enumerate(exec_cmd):
            if '.elf' in value:
                binname = os.path.basename(value)
                exec_cmd[index] = os.path.join(bin_dir, binname)

    return (exec_cmd, url)

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
    except Exception, err:
        print("[ERROR]: %s" % err)
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

def _run_debug_cmd(target, aos_path, cmd_file, program_path=None, bin_dir=None, startclient=False, gdb_args=None):
    """ Run the command from cmd file """
    ret = 0

    host_os = get_host_os()
    if not host_os:
        error("Unsupported Operating System!")

    configs = read_json(cmd_file)

    if not configs:
        error("Can not read flash configs from %s" % cmd_file)

    (start_server, surl) = process_cmd(configs['start_server'], host_os, aos_path, target, program_path, bin_dir)
    (start_client, curl) = process_cmd(configs['start_client'], host_os, aos_path, target, program_path, bin_dir)

    if not os.path.isfile(start_server[0]):
        download_tool(start_server[0], surl)

    if not os.path.isfile(start_client[0]):
        download_tool(start_client[0], curl)

    args_list = []
    if gdb_args:
        args_list = gdb_args.split(' ')

    if startclient:
        start_cmd = start_client + args_list
    else:
        start_cmd = start_server

    info("Running cmd:\n\t'%s'\n" % ' '.join(start_cmd))

    if host_os == 'Win32':
        ret = subprocess.call(start_cmd, shell=True)
    else:
        ret = subprocess.call(start_cmd, stdout=sys.stdout, stderr=sys.stderr)

    return ret

def _debug_app(target, aos_path, registry_file, program_path=None, bin_dir=None, startclient=False, gdb_args=None):
    """ Debug app according to configs """
    (app, board) = target.split('@')
    cmd_file_dir = os.path.dirname(registry_file)
    cmd_files = None
    ret = 0

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
    if '@' not in target or len(target.split('@')) != 2:
        error("Target invalid!")

    if work_path:
        aos_path = work_path
    else:
        if os.path.isdir('./kernel/rhino'):
            info("Currently in aos_sdk_path: '%s'\n" % os.getcwd())
            aos_path = os.getcwd()
        else:
            info("Not in aos_sdk_path, curr_path:'%s'\n" % os.getcwd())
            aos_path = get_config_value('os_path')
            if not aos_path:
                error("aos_sdk is unavailable, please run 'aos new $prj_name'!")
            else:
                info("Load aos configs success, set '%s' as sdk path\n" % aos_path)

    program_path = get_config_value('program_path')
    registry_file = os.path.split(os.path.realpath(__file__))[0] + '/debug/registry_board.json'
    if os.path.isfile(registry_file):
        ret = _debug_app(target, aos_path, registry_file, program_path, bin_dir, startclient, gdb_args)
    else:
        error("Can not find file: %s" % registry_file)

    return ret
