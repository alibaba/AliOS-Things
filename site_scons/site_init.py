import os
import shutil
import re 
import abc
import toolchain
import sys


class aos_global_config:
    global_config_dict = {}
    aos_env = Environment(ENV=os.environ, CPPPATH=['#include'])
    toolchain = None
    compiler = ''
    out_dir = ''
    app = None
    board = None
    build_type = 'debug'
    components = []
    component_targets = []
    component_includes = []
    component_directories = []
    prebuilt_libs = []
    cflags = []
    asflags = []
    ldflags = []
    external_obj = []
    verbose = ''
    ide = ''
    current_time = '123'
    ld_files = []
    ld_targets = []
    testcases = []
    arch = ''
    mcu_family = ''

    @staticmethod
    def set_global_config_append(key, value):
        if key in aos_global_config.global_config_dict:
            aos_global_config.global_config_dict[key] += ' ' + value

        aos_global_config.global_config_override(key, value)

    @staticmethod
    def set_global_config_override(key, value):
        aos_global_config.global_config_dict[key] = value

    @staticmethod
    def get_aos_global_config(key, default=None):
        ret_value = default
        if key in aos_global_config.global_config_dict:
            ret_value = aos_global_config.global_config_dict[key]

        return ret_value

    @staticmethod
    def set_aos_global_config(key, value, append=False):
        if append:
            aos_global_config.set_global_config_append(key, value)
        else:
            aos_global_config.set_global_config_override(key, value)

    @staticmethod
    def set_build_type(build_type):
        aos_global_config.build_type = build_type

    @staticmethod
    def get_build_type():
        return aos_global_config.build_type

    @staticmethod
    def create_tool_chain():
        aos_global_config.toolchain = toolchain.create_tool_chain(aos_global_config)
        return aos_global_config.toolchain

    @staticmethod
    def tool_chain_config(tool_chain):
        tool_chain.tools_config()


class aos_component:
    def __init__(self, name, src):
        self.name = name
        self.component_dependencis = []
        self.dir = Dir('.').srcnode().path
        self.src = [str(s) for s in src]

        self.include_directories = []
        self.macros = []
        self.target = ''
        self.cflags = []
        self.asflags = []

        aos_global_config.components.append(self)
        aos_global_config.component_includes.append('#' + self.dir)

    def get_component_dependencis(self):
        return self.dependencis

    def add_component_dependencis(self, *dependencis):
        for dependency in dependencis:
            self.component_dependencis.append(dependency)

    def add_sources(self, *sources):
        for source in sources:
            self.src.append(source)

    def add_includes(self, *directories):
        for directory in directories:
            directory = os.path.join('#' + self.dir, directory)
            self.include_directories.append(directory)

    def add_global_includes(self, *directories):
        for directory in directories:
            if not os.path.isabs(directory) and not directory.startswith('#'):
                directory = os.path.join('#' + self.dir, directory)
            aos_global_config.component_includes.append(directory)

    def add_macro(self, value):
        self.macros.append(value)

    def add_cflags(self, value):
        self.cflags.append(value)    

    def add_asflags(self, value):
        self.asflags.append(value)  

    def add_prebuilt_lib(self, path):
        if not os.path.isabs(path) and not path.startswith('#'):
            path = os.path.join(self.dir, path)
        aos_global_config.prebuilt_libs.append(path)

    def add_external_obj(self, path):
        if not os.path.isabs(path) and not path.startswith('#'):
            bdir = os.path.join(aos_global_config.out_dir, 'modules', self.dir)
            path = os.path.join(bdir, path)
        aos_global_config.external_obj.append(path)

    def get_self_env(self):
        env = aos_global_config.aos_env.Clone()
        env.Prepend(CPPPATH=self.include_directories)
        env.Append(CPPDEFINES=self.macros)
        env.Append(CCFLAGS=self.cflags)
        env.Append(ASFLAGS=self.asflags)
        return env

    def add_command(self, target, source, command):
        target = '#' + target
        source = '#' + source
        cmd = aos_global_config.aos_env.Command(target, source, command.dispatch_action)
        AlwaysBuild(cmd)

    @staticmethod
    def add_global_macro(value):
        aos_global_config.aos_env.Append(CPPDEFINES=value)

    @staticmethod
    def get_global_arch():
        return aos_global_config.arch

    @staticmethod
    def get_global_mcu_family():
        return aos_global_config.mcu_family


class aos_arch_component(aos_component):
    def __init__(self, name, src):
        aos_component.__init__(self, name, src)

    def add_global_ld_file(self, file):
        if not os.path.isabs(file) and not file.startswith('#'):
            file = os.path.join(self.dir, file)
        aos_global_config.ld_files.append(file)

    @staticmethod
    def add_global_cflags(cflags):
        aos_global_config.cflags.append(cflags)

    @staticmethod
    def add_global_asflags(asflags):
        aos_global_config.asflags.append(asflags)

    @staticmethod
    def add_global_ldflags(ldflags):
        aos_global_config.ldflags.append(ldflags)

    @staticmethod
    def set_global_arch(arch):
        aos_global_config.arch = arch

    @staticmethod
    def set_global_mcu_family(mcu_family):
        aos_global_config.mcu_family = mcu_family


class aos_board_component(aos_component):
    def __init__(self, name, src):
        aos_component.__init__(self, name, src)

    @staticmethod
    def set_global_testcases(testcases):
        aos_global_config.testcases = testcases


def do_process(process):
    process.do_action()


class process(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def do_action(aos_global_config):
        return


class base_process_impl(process):
    def __init__(self, aos_global_config, args):
        self.config = aos_global_config
        self.args = args

    def do_action(self):
        self.__base_config()
        return

    def __base_config(self):
        aos_global_config.app = self.args.get('APPLICATION')
        aos_global_config.board = self.args.get('BOARD')

        if not aos_global_config.app or not aos_global_config.board:
            sys.exit(-1)

        aos_global_config.verbose = self.args.get('VERBOSE')
        aos_global_config.compiler = self.args.get('COMPILER') if self.args.get('COMPILER') else 'gcc'
        aos_global_config.out_dir = self.args.get('OUT_DIR') if self.args.get('OUT_DIR') else 'out'
        aos_global_config.ide = self.args.get('IDE') if self.args.get('IDE') else ''
        
        if aos_global_config.ide == 'keil':
            aos_global_config.compiler = 'armcc'
        elif aos_global_config.ide == 'iar':
            aos_global_config.compiler = 'iar'

        build_type = self.args.get('build_type')
        if build_type:
            aos_global_config.type = build_type
            aos_global_config.out_dir = os.path.join(aos_global_config.out_dir,
                                                     aos_global_config.app + '@' + aos_global_config.board + '@' + aos_global_config.build_type)
        else:
            aos_global_config.out_dir = os.path.join(aos_global_config.out_dir,
                                                     aos_global_config.app + '@' + aos_global_config.board)
        #temporarily disable muti bin
        aos_global_config.aos_env.Append(CPPDEFINES='BUILD_BIN')


class dependency_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__generate_all_components()
        return

    def __load_one_component(self, mod_dir):
        self.config.component_directories.append(mod_dir)
        self.config.aos_env.SConscript(os.path.join(mod_dir, 'ucube.py'))

    def __add_components_dependency(self, dependency):
        if dependency not in aos_global_config.component_directories:
            component_number = len(aos_global_config.components)
            self.__load_one_component(dependency)
            if (component_number+1) != len(aos_global_config.components):
                print('Can\'t find %s, make sure component is exist!!!' % dependency)
                exit(-1)

            component_dependencis = aos_global_config.components[-1].component_dependencis
            for dependency in component_dependencis:
                self.__add_components_dependency(dependency)
    
    def  __generate_testcase_register_c(self, auto_component_dir):
        test_function = []
        for component in aos_global_config.components:
            if component.name.endswith('_test'):
                function_name = 'test_' + component.name[:-5]
                test_function.append(function_name)

        if len(test_function) == 0:
            return ''

        testcase_file = os.path.join(auto_component_dir,'testcase_register.c')
        source_codes = "/*\n * warning: testcase collection code is auto generate, please don't change!!!\n */\n\n"
        source_codes += "#include <aos/aos.h>\n\n"

        for function_name in test_function:
            source_codes +=  "extern void %s(void);\n"%(function_name)

        source_codes += "\nvoid add_test(void) {\n\n"

        # temporary work around for the process sequence of mesh & netmgr testcase
        mesh_test_function = ""
        netmgr_test_function = ""

        for function_name in test_function:
            if function_name.find("mesh") == -1 and function_name.find("netmgr") == -1 :
                source_codes += "    %s();\n\n"%(function_name)
            else:
                if function_name.find("mesh") == -1:
                    netmgr_test_function = function_name
                else:
                    mesh_test_function = function_name
        
        if mesh_test_function:
            source_codes += "    %s();\n\n"%(mesh_test_function)

        if netmgr_test_function:
            source_codes += "    %s();\n\n"%(netmgr_test_function)

        source_codes += "}"

        with open(testcase_file, "w") as f:
            f.write(source_codes)

        return testcase_file

    def  __generate_auto_component(self):      
        auto_component_dir = os.path.join(self.config.out_dir, 'auto_component')
        if not os.path.exists(auto_component_dir):
            os.makedirs(auto_component_dir)
        
        src = []
        src_file = self.__generate_testcase_register_c(auto_component_dir)
        if src_file != '':
            src.append(src_file)

        component = aos_component('auto_component', src)


    def __generate_all_components(self):
        print('app=' + aos_global_config.app + ', board=' + aos_global_config.board + ', out_dir=' + aos_global_config.out_dir)

        # board config tool chain, must add first.
        self.__load_one_component('board/' + aos_global_config.board)
        if (len(aos_global_config.components)) != 1:
            print('Unsupported board, make sure %s in board directory...' % aos_global_config.board)
            exit(-1)

        self.__load_one_component('example/' + aos_global_config.app)
        if (len(aos_global_config.components)) != 2:
            print('Unsupported app, make sure %s in example directory...' % aos_global_config.app)
            exit(-1)

        self.__load_one_component('kernel/init')
        self.__load_one_component('kernel/vcall')
        if aos_global_config.app == 'yts':          
            for testcase in aos_global_config.testcases:
                self.__load_one_component(testcase)

        for component in aos_global_config.components:
            for dependency in component.component_dependencis:
                self.__add_components_dependency(dependency)

        self.__generate_auto_component()

        print("all components: %s " % ' '.join([component.name for component in aos_global_config.components]))

        self.config.aos_env.Append(CPPPATH=aos_global_config.component_includes)
        self.config.aos_env.Append(CCFLAGS=aos_global_config.cflags)
        self.config.aos_env.Append(ASFLAGS=aos_global_config.asflags)
        self.config.aos_env.Append(LDFLAGS=aos_global_config.ldflags)


def system_exec(cmd):
    print(cmd)
    os.system(cmd)


class ide_transfer_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__ide_transfer()
        return

    def __ide_transfer(self):        
        
        if self.config.ide != 'iar' and self.config.ide != 'keil':
            print('IDE '+self.config.ide+' not support!!')
            exit(-1)
        
        buildstring = self.config.app + '@' + self.config.board
        proj_gen_dir = 'projects/autogen/'+buildstring+'/'+self.config.ide+'_project'  
        transfer_cmd = 'python build/scripts/'+self.config.ide+'.py ' + buildstring
        opts_dir = proj_gen_dir+'/opts' 
        
        if not os.path.exists(proj_gen_dir):
            os.makedirs(proj_gen_dir)
        if not os.path.exists(opts_dir):
            os.makedirs(opts_dir)    
        
        # output options to file, $CFLAGS $CCFLAGS $_CCCOMCOM
        for component in self.config.components:
            c_opt_str=''
            as_opt_str=''
            c_opt_file_path = os.path.join(opts_dir, component.name+".c_opts")
            as_opt_file_path = os.path.join(opts_dir, component.name+".as_opts")
            if len(component.src):
                env = component.get_self_env()
                c_opt_str += '-c'                  
                c_opt_str += ' '+env.subst('$CFLAGS')
                c_opt_str += ' '+env.subst('$CCFLAGS')
                c_opt_str += ' '+env.subst('$_CCCOMCOM')

                if self.config.ide == 'keil': 
                    c_opt_str = c_opt_str.replace('-I#','-I../../../../')
                elif self.config.ide == 'iar':
                    c_opt_str = re.subn('\s-I\S+','',c_opt_str)[0]
                    c_opt_str = re.subn('\s--cpu=\S+','',c_opt_str)[0]
                    c_opt_str = re.subn('\s--endian\S+','',c_opt_str)[0]
                    c_opt_str = re.subn('\s--dlib_config\S+','',c_opt_str)[0]  
                    c_opt_str = c_opt_str.replace( r'=\"', r'="\"')
                    c_opt_str = c_opt_str.replace( r'\" ', r'\"" ')
                    c_opt_str = re.subn('\"$', '\""', c_opt_str)[0]    

                with open(c_opt_file_path, 'w') as f:
                    f.write(c_opt_str)

                as_opt_str += env.subst('$ASFLAGS')
                with open(as_opt_file_path, 'w') as f:
                    as_opt_str=as_opt_str.replace('--cpu Cortex-M4', '')
                    f.write(as_opt_str)

        # create config_mk.py
        config_py_file = 'build/scripts/config_mk.py'
        with open(config_py_file, 'w') as f:
            f.write('Projects = [\n')
            for component in self.config.components:
                comp_dir = component.dir.replace('\\','/')
                f.write('{\'name\':\''+component.name+'\',\n')
                f.write('\'src\':[\n' )
                for source in component.src:
                    f.write( '\'' + comp_dir+'/'+source+ '\',\n'  )
                f.write('],\n')

                f.write('\'include\':[\n' )
                env = component.get_self_env()
                inc_list = env['CPPPATH']
                for inc in inc_list:
                    inc = inc.replace('#','')
                    inc = inc.replace('\\','/')
                    f.write( '\'' +inc+ '\',\n'  )

                f.write('],\n')
                f.write('},\n')
            f.write(']\n')

        # excute transfer
        print('Makeing IDE project')
               
        if self.config.ide == 'iar':
            debug_template = 'build/scripts/template.ewd'
            debug_file = proj_gen_dir + '/' + buildstring+'.ewd'
            shutil.copyfile(debug_template, debug_file)
         
        system_exec(transfer_cmd)
        print(' ----------- project has generated in ' + proj_gen_dir + ' -----------')


class build_rule_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__build_rule_components()
        return

    def __build_rule_components(self):
        for component in self.config.components:
            if len(component.src):                
                env = component.get_self_env()
                objdir = os.path.join(self.config.out_dir, 'modules')
                bdir = os.path.join(objdir, component.dir)
                src = [os.path.join(bdir, s) for s in component.src]
                env.VariantDir(objdir, '#', duplicate=0)
                target = os.path.join(self.config.out_dir, 'libraries', component.name)
                if self.config.verbose == '1':
                    cccomstr = ''
                    arcomstr = ''
                else:
                    cccomstr = 'Compiling $SOURCE'
                    arcomstr = 'Making $TARGET'
                
                component.target = env.Library(target=target, source=src, CCCOMSTR=cccomstr, ARCOMSTR=arcomstr,LIBPREFIX='')

        self.config.component_targets = []
        for component in aos_global_config.components:
            if len(component.src):
                self.config.component_targets.append(component.target) 


class link_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__link_elf()
        return

    def __link_elf(self):
        env = aos_global_config.aos_env
        binary = aos_global_config.toolchain.binary

        if aos_global_config.compiler == 'iar':
            libs_tmp = ''
            for component in aos_global_config.components:
                if len(component.src): 
                    if component.name != 'alicrypto':
                        libs_tmp += (' --whole_archive '+str(component.target[0])+' ' )
                    else:
                        libs_tmp += (str(component.target[0])+' ')
            linkcom = 'ilinkarm -o $TARGET $LDFLAGS' + libs_tmp + '$LIBS $LINKFLAGS'
        elif aos_global_config.compiler == 'armcc':
            objs_tmp = ''
            for obj in aos_global_config.external_obj:
                objs_tmp += (' '+str(obj)+' ')

            mapfile = binary.replace('.axf','.map')
            linkcom = 'armcc  -L --library_type=microlib -o $TARGET  -L --map -L --list='+ mapfile +' $LDFLAGS' + objs_tmp + '$SOURCES $LIBS $LINKFLAGS'
        else:
            linkcom = aos_global_config.toolchain.linkcom

        env.Append(LIBS=aos_global_config.prebuilt_libs)
        env.Append(LIBS=[component.target for component in aos_global_config.components])

        if aos_global_config.verbose == '1':
            linkcomstr = ''
        else:
            linkcomstr = 'Linking $TARGET'

        env.Program(binary, aos_global_config.ld_targets, LINKCOM=linkcom, LINKCOMSTR=linkcomstr)


class ld_file_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__set_ld_file()
        return

    def __set_ld_file(self):
        ld_target = ''
        for ld in self.config.ld_files:
            if ld.endswith('.S'):
                tool_chain = aos_global_config.toolchain
                aos_global_config.aos_env['CPP'] = os.path.join(tool_chain.tools_path, tool_chain.prefix + 'cpp')
                target = os.path.join(self.config.out_dir, 'ld', os.path.basename(ld)[:-2])
                aos_global_config.aos_env.Command(target, ld, aos_global_config.aos_env['CPP'] + ' -P $SOURCE -o $TARGET')
            else:
                target = os.path.join(self.config.out_dir, 'ld', os.path.basename(ld))
                aos_global_config.aos_env.Command(target, ld, Copy('$TARGET', '$SOURCE'))

            self.config.ld_targets.append(target)

            if 'platform' in ld:
                ld_target = target

        if ld_target:
            self.config.aos_env.Append(LINKFLAGS='-T ' + os.path.basename(ld_target) + ' -L ' + os.path.dirname(ld_target))
        else:
            print('Pleas config ld file in mcu component...')


class create_bin_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__create_bin()
        return

    def __create_bin(self):
        strip_tool = self.config.aos_env['STRIP']
        objcopy_tool = self.config.aos_env['OBJCOPY']
        binary = self.config.toolchain.binary

        if aos_global_config.compiler == 'armcc':
            stripped_file = binary.replace('.axf', '.stripped.axf')
            strip_cmd = strip_tool + ' --output=' + stripped_file + ' --strip=debug,symbols --elf ' + binary
            bin_file = binary.replace('.axf', '.bin')
            bin_cmd = objcopy_tool + ' --bin ' + stripped_file + ' --output=' + bin_file

        elif aos_global_config.compiler == 'iar':
            stripped_file = binary.replace('.iarElf', '.stripped.iarElf')
            strip_cmd = strip_tool + ' --strip --silent ' + binary + ' ' + stripped_file
            bin_file = binary.replace('.iarElf', '.bin')
            bin_cmd = objcopy_tool + ' --bin --silent ' + stripped_file + ' ' + bin_file

        else:
            stripped_file = binary.replace('.elf', '.stripped.elf')
            strip_cmd = strip_tool + ' -o ' + stripped_file + ' ' + binary
            bin_file = binary.replace('.elf', '.bin')
            bin_cmd = objcopy_tool + ' -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes ' + stripped_file + ' ' + bin_file

        if aos_global_config.verbose == '1':
            linkcomstr = ''
        else:
            linkcomstr = 'Making $TARGET'

        env = aos_global_config.aos_env
        env.Program(stripped_file, binary, LINKCOM=strip_cmd, LINKCOMSTR=linkcomstr)
        env.Program(bin_file, stripped_file, LINKCOM=bin_cmd, LINKCOMSTR=linkcomstr)


class aos_command(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def dispatch_action(self, target, source, env):
        return


def ucube_main(args):
    do_process(base_process_impl(aos_global_config, args))
    do_process(dependency_process_impl(aos_global_config))
    do_process(ld_file_process_impl(aos_global_config))

    if aos_global_config.ide != '':
        do_process(ide_transfer_process_impl(aos_global_config))
    else:
        do_process(build_rule_process_impl(aos_global_config))
        do_process(link_process_impl(aos_global_config))
        do_process(create_bin_process_impl(aos_global_config))
