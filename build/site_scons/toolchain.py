import os
import sys

class tool_chain:
    def __init__(self, config, tools_path=''):
        self.config = config
        self.tools_path = tools_path
        self.binary_raw = os.path.join(self.config.out_dir, 'binary', self.config.app + '@' + self.config.board)

    @staticmethod
    def __get_os():
        os = ''
        if sys.platform.startswith('linux'):
            os = 'Linux64' if sys.maxsize > 2**32 else 'Linux32'
        elif sys.platform.startswith('darwin'):
            os = 'OSX'
        elif sys.platform.startswith('win'):
            os = 'Win32'
        else:
            print('%s encrypt unsupported...' % sys.platform)
        return os

    def __tools_path(self):
        tools_chain_root = os.path.join(self.config.project_path, 'build/compiler')
        path = ''

        if not self.tools_path:
            if self.prefix == 'arm-none-eabi-':
                path = os.path.join(tools_chain_root, 'gcc-arm-none-eabi', self.__get_os(), 'bin')
            elif self.prefix == 'xtensa-esp32-elf-':
                path = os.path.join(tools_chain_root, 'gcc-xtensa-esp32', self.__get_os(), 'bin')
            elif self.prefix == 'xtensa-lx106-elf-':
                path = os.path.join(self.config.project_path, 'gcc-xtensa-lx106', self.__get_os(), 'bin')
            elif self.prefix == 'csky-abiv2-elf-':
                path = os.path.join(tools_chain_root, 'gcc-csky-abiv2', self.__get_os(), 'bin')
            elif self.prefix == '' and self.config.compiler=='gcc':
                path = ''
            elif self.prefix == '' and self.config.compiler=='cl':
                path = ''
            else:               
                print("tool chain is not support")

        if os.path.isdir(path):
            self.tools_path = path
        elif os.getenv('TOOLCHAIN_PATH'):
            self.tools_path = os.getenv('TOOLCHAIN_PATH')

    def tools_name_config(self):
        self.__tools_path()
        self.config.aos_env['CC'] = os.path.join(self.tools_path, self.prefix + self.cc)
        self.config.aos_env['CXX'] = os.path.join(self.tools_path, self.prefix + self.cxx)
        self.config.aos_env['AS'] = os.path.join(self.tools_path, self.prefix + self.ass)
        #msvs not support here temporary
        if self.config.compiler != 'cl':
            self.config.aos_env['AR'] = os.path.join(self.tools_path, self.prefix + self.ar)
        self.config.aos_env['LD'] = os.path.join(self.tools_path, self.prefix + self.ld)
        self.config.aos_env['OBJDUMP'] = os.path.join(self.tools_path, self.prefix + self.objdump)
        self.config.aos_env['OBJCOPY'] = os.path.join(self.tools_path, self.prefix + self.objcopy)
        self.config.aos_env['STRIP'] = os.path.join(self.tools_path, self.prefix + self.strip)
        self.config.aos_env['NM'] = os.path.join(self.tools_path, self.prefix + self.nm)
        self.config.aos_env['READELF'] = os.path.join(self.tools_path, self.prefix + self.readelf)
        self.config.aos_env['RANLIBCOM'] = ''

        for tool, name in list(self.extend_name_dict.items()):
            self.config.aos_env[tool] = os.path.join(self.tools_path, self.prefix + name)

    def tools_flag_config(self):
        self.config.aos_env.Append(CPPFLAGS=self.cppflags)
        self.config.aos_env.Append(CFLAGS=self.cflags)
        self.config.aos_env.Append(CXXFLAGS=self.cxxflags)
        self.config.aos_env.Append(ASFLAGS=self.asflags)
        self.config.aos_env.Append(LINKFLAGS=self.ldflags)
        self.config.aos_env.Replace(ARFLAGS=self.arflags)

        for flag, value in list(self.extend_flag_dict.items()):
            self.config.aos_env.Append(flag=value)

    def tools_config(self):
        self.tools_name_config()
        self.tools_flag_config()

    def extend_tool_name(self, tool, name):
        self.extend_name_dict[tool] = name

    def extend_tool_flag(self, key, value):
        self.extend_flag_dict[key] = value

    def set_tools_path(self, tools_path):
        self.tools_path = tools_path if os.path.isabs(tools_path) else os.path.abspath(tools_path)

    def set_prefix(self, prefix):
        self.prefix = prefix

    def set_cc(self, cc):
        self.cc = cc

    def set_cxx(self, cxx):
        self.cxx = cxx

    def set_as(self, ass):
        self.ass = ass

    def set_ar(self, ar):
        self.ar = ar

    def set_ld(self, ld):
        self.ld = ld

    def set_objdump(self, objcump):
        self.objdump = objcump

    def set_objcopy(self, objcopy):
        self.objcopy = objcopy

    def set_strip(self, strip):
        self.strip = strip

    def set_nm(self, nm):
        self.nm = nm

    def set_readelf(self, readelf):
        self.readelf = readelf

    def set_cflags(self, cflags):
        self.cflags = cflags

    def set_cxxflags(self, cxxflags):
        self.cxxflags = cxxflags

    def set_cppflags(self, cppflags):
        self.cppflags = cppflags

    def set_asflags(self, asflags):
        self.asflags = asflags

    def set_ldflags(self, ldflags):
        self.ldflags = ldflags

    def set_linkcom(self, linkcom):
        self.linkcom = linkcom


class gcc_tool_chain(tool_chain):
    def __init__(self, config, tools_path=''):
        tool_chain.__init__(self, config, tools_path)
        board_name = config.board
        self.prefix = ''
        self.cppflags = ''
        self.cc = 'gcc'
        self.cxx = 'g++'
        self.ass = 'gcc'
        self.ar = 'ar'
        self.ld = 'ld'
        self.objdump = 'objdump'
        self.objcopy = 'objcopy'
        self.strip = 'strip'
        self.nm = 'nm'
        self.readelf = 'readelf'
        self.extend_name_dict = {}
        self.cflags = '-Wfatal-errors -ggdb -Os  -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11'
        self.cxxflags = '-Wfatal-errors -ggdb -Os  -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11'

        self.asflags = '-c'
        self.ldflags = ''
        self.arflags = 'rc'
        self.extend_flag_dict = {}
        self.binary = self.binary_raw + '.elf'
        env = self.config.aos_env
        env['MAPFILE'] = self.binary_raw + '.map'

class win32msvs_tool_chain(tool_chain):
    def __init__(self, config, tools_path=''):        
        tool_chain.__init__(self, config, tools_path)       
        self.linkcom = ''
        board_name = config.board
        self.prefix = ''
        self.cppflags = ''
       
        self.cc = 'cl'
        self.cxx = 'cl'
        self.ass = 'cl'
        self.ar = 'cl'
        self.ld = 'cl'
        self.objdump = 'objdump'
        self.objcopy = 'objcopy'
        self.strip = 'strip'
        self.nm = 'nm'
        self.readelf = 'readelf'
        self.extend_name_dict = {}
        self.cflags = '-DWIN32 -D_WIN32 -D_DEBUG -MDd /utf-8 /c'
        self.cxxflags = ''
        self.asflags = ''
        self.ldflags = ''
        self.arflags = ''
        self.extend_flag_dict = {}
        self.binary = self.binary_raw + '.exe'
        env = self.config.aos_env
        env['MAPFILE'] = self.binary_raw + '.map'
        
class iar_tool_chain(tool_chain):
    def __init__(self, config, tools_path=''):
        tool_chain.__init__(self, config, tools_path)
        self.prefix = ''
        self.cc = 'iccarm'
        self.cxx = 'iccarm'
        self.ass = 'iasmarm'
        self.ar = 'iarchive'
        self.ld = 'ilinkarm'
        self.objdump = 'ielfdumparm'
        self.objcopy = 'ielftool'
        self.strip = 'ielftool'
        self.extend_name_dict = {}
        self.cflags = '-e --dlib_config=full -D_TIMESPEC_DEFINED --silent --only_stdout --no_warnings --diag_warning=Pe167,Pe144,Pe513'
        self.cxxflags = ''
        self.cppflags = ''
        self.asflags = ''
        self.ldflags = ''
        self.arflags = '--create'
        self.extend_flag_dict = {}
        self.nm = ''
        self.readelf = ''
        self.binary = self.binary_raw + '.iarElf'


class armcc_tool_chain(tool_chain):
    def __init__(self,config,tools_path=''):
        tool_chain.__init__(self,config,tools_path)
        self.prefix = ''
        self.cc = 'armcc'
        self.cxx = 'armcc'
        self.ass = 'armasm'
        self.ar = 'armar'
        self.ld = 'armlink'
        self.objdump = 'fromelf'
        self.objcopy = 'fromelf'
        self.strip = 'fromelf'
        self.extend_name_dict = {}
        self.cflags = '--c90 --gnu --library_type=microlib -W'
        self.cxxflags = ''
        self.cppflags = ''
        self.asflags = '--library_type=microlib'
        self.ldflags = ''
        self.arflags = '-rcs'
        self.extend_flag_dict = {}
        self.nm = ''
        self.readelf = ''
        self.binary = self.binary_raw + '.axf'


class rvct_tool_chain(tool_chain):
    def __init__(self,config,tools_path=''):
        tool_chain.__init__(self,config,tools_path)
        self.prefix = ''
        self.cc = 'armcc'
        self.cxx = 'armcc'
        self.ass = 'armasm'
        self.ar = 'armar'
        self.ld = 'armlink'
        self.objdump = 'fromelf'
        self.objcopy = 'fromelf'
        self.strip = 'fromelf'
        self.extend_name_dict = {}
        self.cflags = '--c90 --gnu -W'
        self.cxxflags = ''
        self.cppflags = ''
        self.asflags = ''
        self.ldflags = ''
        self.arflags = '-rcs'
        self.extend_flag_dict = {}
        self.nm = ''
        self.readelf = ''
        self.binary = self.binary_raw + '.axf'


def create_tool_chain(config):
    compiler = config.compiler
    if compiler == 'gcc':
        return gcc_tool_chain(config)
    elif compiler == 'cl':
        return win32msvs_tool_chain(config)
    elif compiler == 'iar':
        return iar_tool_chain(config)
    elif compiler == 'armcc':
        return armcc_tool_chain(config)
    elif compiler == 'rvct':
        return rvct_tool_chain(config)
    else:
        print("%s is not support new, please contact aos build engineer." % compiler)
        exit(-1)
