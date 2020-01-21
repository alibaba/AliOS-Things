import os
import shutil
import re
import abc
import toolchain
import sys
import time


class aos_global_config:
    global_config_dict = os.environ.copy()

    aos_env = None
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
    current_time = time.strftime("%Y%m%d.%H%M", time.localtime())
    ld_files = []
    ld_targets = []
    testcases = []
    arch = ''
    mcu_family = ''
    config_observers = {}
    enable_vfp = 0
    project_path = Dir('#').abspath
    max_files = 9999

    @staticmethod
    def set_append(key, value):
        if key in aos_global_config.global_config_dict:
            aos_global_config.global_config_dict[key] += ' ' + value

        aos_global_config.global_config_override(key, value)

    @staticmethod
    def set_override(key, value):
        aos_global_config.global_config_dict[key] = value

    @staticmethod
    def get(key, default=None):
        ret_value = default
        if key in aos_global_config.global_config_dict:
            ret_value = aos_global_config.global_config_dict[key]

        return ret_value

    @staticmethod
    def set(key, value, append=False):
        last_value = aos_global_config.get(key, None)

        if append:
            aos_global_config.set_append(key, value)
        else:
            aos_global_config.set_override(key, value)

        if last_value != value and key in aos_global_config.config_observers:
            func_comp = aos_global_config.config_observers[key]
            for func, comp in list(func_comp.items()):
                func(comp)
            func_comp.clear()

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

    @staticmethod
    def add_ld_files(*lds):
        for ld in lds:
            path = ld
            if not os.path.isabs(path) and not path.startswith('#'):
                path = os.path.join(os.getcwd(), path)
            aos_global_config.ld_files.append(path)


class aos_component:
    def __init__(self, name, src):
        self.name = name
        self.component_dependencis = []
        self.dir = Dir('.').srcnode().path
        self.src = [str(s) for s in src]

        self.include_directories = []
        self.macros = []
        self.targets = []
        self.cflags = []
        self.asflags = []

        aos_global_config.components.append(self)
        aos_global_config.component_includes.append('#' + self.dir)

    def get_comp_deps(self):
        return self.component_dependencis

    def add_comp_deps(self, *dependencis):
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

    def add_macros(self, *macros):
        for macro in macros:
            self.macros.append(macro)

    def add_cflags(self, *cflags):
        for cflag in cflags:
            self.cflags.append(cflag)

    def add_asflags(self, *asflags):
        for asflag in asflags:
            self.asflags.append(asflag)

    def add_prebuilt_libs(self, *libs):
        for lib in libs:
            if not os.path.isabs(lib) and not lib.startswith('#'):
                lib = os.path.join(self.dir, lib)
                lib = lib.replace('\\', '/')
            aos_global_config.prebuilt_libs.append(lib)

    def add_prebuilt_objs(self, *objs):
        for obj in objs:
            if not os.path.isabs(obj) and not obj.startswith('#'):
                bdir = os.path.join(aos_global_config.out_dir, 'modules', self.dir)
                lib = os.path.join(bdir, obj)
            aos_global_config.external_obj.append(lib)

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
    def add_global_macros(*macros):
        for macro in macros:
            aos_global_config.aos_env.Append(CPPDEFINES=macro)

    @staticmethod
    def get_arch():
        return aos_global_config.arch

    @staticmethod
    def get_mcu_family():
        return aos_global_config.mcu_family

    @staticmethod
    def get_compiler():
        compiler = aos_global_config.compiler
        if compiler == 'armcc': compiler = 'keil'
        return compiler


class aos_arch_component(aos_component):
    def __init__(self, name, src):
        aos_component.__init__(self, name, src)

    @staticmethod
    def add_global_cflags(*cflags):
        for cflag in cflags:
            aos_global_config.cflags.append(cflag)

    @staticmethod
    def add_global_asflags(*asflags):
        for asflag in asflags:
            aos_global_config.asflags.append(asflag)

    @staticmethod
    def add_global_ldflags(*ldflags):
        for ldflag in ldflags:
            aos_global_config.ldflags.append(ldflag)


class aos_mcu_component(aos_arch_component):
    def __init__(self, name, prefix, src):
        aos_arch_component.__init__(self, name, src)
        tool_chain = aos_global_config.create_tool_chain()
        tool_chain.set_prefix(prefix)
        aos_global_config.tool_chain_config(tool_chain)

    @staticmethod
    def set_global_arch(arch):
        aos_global_config.arch = arch


class aos_board_component(aos_component):
    def __init__(self, name, mcu, src):
        aos_component.__init__(self, name, src)
        self.set_global_mcu_family(mcu)
        self.add_global_macros( 'MCU_FAMILY=\\"' + mcu + '\\"' )
        self.add_comp_deps('platform/mcu/'+ mcu)

    @staticmethod
    def set_global_testcases(testcases):
        aos_global_config.testcases = testcases

    @staticmethod
    def set_global_mcu_family(mcu_family):
        aos_global_config.mcu_family = mcu_family

    @staticmethod
    def set_enable_vfp():
        aos_global_config.enable_vfp = 1


def do_process(process):
    process.do_action()


class process(object, metaclass=abc.ABCMeta):
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

        if self.args.get('LINK_CLOUD_ENV') != None:
            aos_global_config.aos_env.Append(CPPDEFINES=self.args.get('LINK_CLOUD_ENV'))

        if self.args.get('osal'):
            aos_global_config.set('osal', self.args.get('osal'))

        if self.args.get('test'):
            aos_global_config.set('test', self.args.get('test'))

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

        #temporarily add include path aos.h need
        aos_global_config.component_includes.append('#kernel/fs/kv/include')
        aos_global_config.component_includes.append('#network/yloop/include')
        aos_global_config.component_includes.append('#network/include')

def pre_config(config):
    def __config(func):
        def __decorator(component):
            if config in aos_global_config.config_observers:
                aos_global_config.config_observers[config][func] = component
            else:
                aos_global_config.config_observers[config] = {func: component}
        return __decorator
    return __config


def post_config(func):
    def __decorator(component):
        component.post_config = func
    return __decorator


class dependency_process_impl(process):
    def __init__(self, aos_global_config):
        self.config = aos_global_config

    def do_action(self):
        self.__generate_all_components()
        return

    def __normalize(self, p):
        p = p.replace('/', os.sep)
        p = p.replace('\\', os.sep)
        return p.replace('.', os.sep)

    def __search_dfl(self, mod_dir):
        dfl_paths = [ 'device', 'framework', 'kernel', 'platform', 'experimental', '3rdparty.experimental' ]

        # for ./ situation
        if os.path.exists(mod_dir):
            return mod_dir

        mod_dir = mod_dir.replace('.', os.sep)
        if os.path.exists(mod_dir):
            return mod_dir

        for p in dfl_paths:
            n = self.__normalize(p + '.' + mod_dir)
            if not os.path.exists(n):
                continue
            return n
        return None

    def __load_one_component(self, mod_dir):
        m = self.__search_dfl(mod_dir)
        if not m:
            print('module %s not found'%mod_dir)
            return

        py_path = os.path.join(m, 'ucube.py')
        if not os.path.exists(py_path):
            print('file %s not found'%py_path)
            return

        self.config.component_directories.append(m)
        self.config.aos_env.SConscript(py_path)

    def __add_components_dependency(self, dependency):
        if dependency not in aos_global_config.component_directories:
            component_number = len(aos_global_config.components)
            self.__load_one_component(dependency)

            for c in aos_global_config.components[component_number:]:
                component_dependencis = c.component_dependencis
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
        source_codes += "#include \"aos/kernel.h\"\n\n"

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

    def __generate_auto_component(self):
        auto_component_dir = os.path.join(self.config.out_dir, 'auto_component')
        if not os.path.exists(auto_component_dir):
            os.makedirs(auto_component_dir)

        src = []
        src_file = self.__generate_testcase_register_c(auto_component_dir)
        if src_file != '':
            src.append(src_file)

        aos_component('auto_component', src)

    def __pre_config(self):
        for config, func_comp in list(self.config.config_observers.items()):
            for func, comp in list(func_comp.items()):
                len_deps = len(comp.get_comp_deps())
                func(comp)
                if len_deps != len(comp.get_comp_deps()):
                    for dep in comp.get_comp_deps():
                        self.__add_components_dependency(dep)
            func_comp.clear()

    def __post_config(self):
        for component in self.config.components:
            if hasattr(component, 'post_config'):
                len_deps = len(component.get_comp_deps())
                component.post_config(component)

                if len_deps != len(component.get_comp_deps()):
                    for dep in component.get_comp_deps():
                        self.__add_components_dependency(dep)

    def __generate_all_components(self):
        print('app=' + aos_global_config.app + ', board=' + aos_global_config.board + ', out_dir=' + aos_global_config.out_dir)

        # board config tool chain, must add first.
        self.__load_one_component('platform/board/' + aos_global_config.board)
        if (len(aos_global_config.components)) != 1:
			self.__load_one_component('platform/board/board_legacy/' + aos_global_config.board)
			if (len(aos_global_config.components)) != 1:	
				print('Unsupported board, make sure %s in board directory...' % aos_global_config.board)
				exit(-1)

        self.__load_one_component('application/example/example_legacy/' + aos_global_config.app)
        if (len(aos_global_config.components)) != 2:
			self.__load_one_component('application/example/' + aos_global_config.app)
			if (len(aos_global_config.components)) != 2:
				print('Unsupported app, make sure %s in example directory...' % aos_global_config.app)
				exit(-1)

        if aos_global_config.app == 'yts':
            if aos_global_config.get('test'):
                test_mod_list = aos_global_config.get('test').strip(',').split(',')
                print ('test case list: %s'%test_mod_list)

                for i in test_mod_list:
                    for testcase in aos_global_config.testcases:
                        if testcase.find(i) != -1:
                            print('Select Special Testcase: %s'%testcase)
                            self.__load_one_component(testcase)
                            break
            else:
                for testcase in aos_global_config.testcases:
                    self.__load_one_component(testcase)

        for component in aos_global_config.components:
            for dependency in component.component_dependencis:
                self.__add_components_dependency(dependency)

        self.__generate_auto_component()

        self.__pre_config()
        self.__post_config()

        print("all components: %s " % ' '.join([component.name for component in aos_global_config.components]))

        self.config.aos_env.Append(CPPPATH=aos_global_config.component_includes)
        self.config.aos_env.Append(CCFLAGS=aos_global_config.cflags)
        for inc in aos_global_config.component_includes:
            inc_replace = inc.replace( "#", "" )
            aos_global_config.asflags.append( "-I"+inc_replace )
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

        if self.config.ide == 'iar':
            proj_gen_dir = 'projects/IAR/'+buildstring+'/'+self.config.ide+'_project'
        elif self.config.ide == 'keil':
            proj_gen_dir = 'projects/Keil/'+buildstring+'/'+self.config.ide+'_project'
        else:
            pass

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
                if self.config.ide == 'keil':
                    as_opt_str = as_opt_str.replace('-I','-I../../../../')
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

            #add lib
            if len(aos_global_config.prebuilt_libs):
                f.write('{\'name\':\''+'external_lib'+'\',\n')
                f.write('\'src\':[\n' )
                for lib in aos_global_config.prebuilt_libs:
                    f.write( '\'' + lib+ '\',\n'  )
                f.write('],\n')
                f.write('\'include\':[\n' )
                f.write('],\n')
                f.write('},\n')

            f.write(']\n')

            for flag in aos_global_config.ldflags:
                if "=" in flag:
                    flag=flag.split("=")[1]
                if self.config.ide == 'iar' and flag.endswith(".icf"):
                    f.write('ld_script = "{}"\n'.format(flag))
                    break
                elif self.config.ide == 'keil' and flag.endswith(".sct"):
                    f.write('ld_script = "{}"\n'.format(flag))
                    break

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
            if len(component.src) < 1:
                continue

            env = component.get_self_env()
            objdir = os.path.join(self.config.out_dir, 'modules')
            bdir = os.path.join(objdir, component.dir)
            src = [os.path.join(bdir, s) for s in component.src]
            env.VariantDir(objdir, '#', duplicate=0)
            target = os.path.join(self.config.out_dir, 'libraries', component.name)
            #will be used in furure for windows MSVS debug.
            #env['PDB'] = target+'.pdb'
            if self.config.verbose == '1':
                cccomstr = ''
                arcomstr = ''
                ascomstr = ''
            else:
                cccomstr = 'Compiling $SOURCE'
                arcomstr = 'Making $TARGET'
                ascomstr = 'Assembling $SOURCE'

            if len(component.src) < aos_global_config.max_files:
                lib = env.Library(target=target, source=src, CCCOMSTR=cccomstr, ASCOMSTR=ascomstr,ARCOMSTR=arcomstr,LIBPREFIX='')
                component.targets.append(lib)
                if self.config.compiler == 'gcc':
                    lib_src = str(lib[0])
                    lib_tgt = lib_src.replace('.a', '.stripped.a')
                    strip_tool = self.config.aos_env['STRIP']
                    strip_cmd = strip_tool + ' -g -o $TARGET $SOURCE'
                    env.Command(lib_tgt, lib_src, strip_cmd)
                continue
            i = 0
            while i < len(component.src):
                tsrc = src[i : i + aos_global_config.max_files]
                lib = env.Library(target="%s_%d"%(target,i), source=tsrc, CCCOMSTR=cccomstr, ARCOMSTR=arcomstr,LIBPREFIX='')
                component.targets.append(lib)
                i += aos_global_config.max_files

        self.config.component_targets = []
        for component in aos_global_config.components:
            if len(component.src):
                self.config.component_targets += component.targets

        aos_global_config.set('cppLinkFlag', 0)
        for component in aos_global_config.components:
            if aos_global_config.get('cppLinkFlag') == 1:
                break;
            for src_file in component.src:
                if '.cpp' in src_file:
                    aos_global_config.set('cppLinkFlag', 1)

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
                        for t in component.targets:
                            libs_tmp += (' --whole_archive '+str(t[0])+' ' )
                    else:
                        for t in component.targets:
                            libs_tmp += (str(t[0])+' ')
            linkcom = 'ilinkarm -o $TARGET $LDFLAGS' + libs_tmp + '$LIBS $LINKFLAGS'
        elif aos_global_config.compiler == 'armcc':
            objs_tmp = ''
            for obj in aos_global_config.external_obj:
                objs_tmp += (' '+str(obj)+' ')

            mapfile = binary.replace('.axf','.map')
            linkcom = 'armcc  -L --library_type=microlib -o $TARGET  -L --map -L --list='+ mapfile +' $LDFLAGS' + objs_tmp + '$SOURCES $LIBS $LINKFLAGS'
        elif aos_global_config.compiler == 'rvct':
            objs_tmp = ''
            for obj in aos_global_config.external_obj:
                objs_tmp += (' '+str(obj)+' ')

            mapfile = binary.replace('.axf','.map')
            linkcom = 'armlink  -o $TARGET  --map --list='+ mapfile +' $LDFLAGS' + objs_tmp + '$SOURCES $LIBS $LINKFLAGS'
        elif aos_global_config.compiler == 'gcc':
            if  aos_global_config.toolchain.prefix == 'arm-none-eabi-':
                if aos_global_config.get('cppLinkFlag') == 1:
                    link_tmp = 'arm-none-eabi-g++'
                else:
                    link_tmp = '$LINK'
                linkcom = link_tmp + ' -o $TARGET -Wl,-Map,$MAPFILE  -Wl,--whole-archive -Wl,--start-group $LIBS  -Wl,--end-group -Wl,--no-whole-archive -Wl,--gc-sections -Wl,--cref $LDFLAGS $LINKFLAGS'
            else:
                linkcom = '$LINK -o $TARGET -Wl,-Map,$MAPFILE  -Wl,--start-group $LIBS  -Wl,--end-group  -Wl,--gc-sections -Wl,--cref $LDFLAGS $LINKFLAGS'
        elif aos_global_config.compiler == 'cl':
            dummyobj = os.path.join(sys.path[0],'site_scons','dummy.obj')
            linkcom = 'LINK /OUT:$TARGET /INCREMENTAL "kernel32.lib" "ws2_32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "odbc32.lib" "odbccp32.lib" /SUBSYSTEM:CONSOLE /MACHINE:X86 $LIBS '+dummyobj

        env.Append(LIBS=aos_global_config.prebuilt_libs)
        env.Append(LIBS=[component.targets for component in aos_global_config.components])

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
        link_flags = ''
        if len(self.config.ld_files):
            link_flags = ' -L ' + os.path.join(self.config.out_dir, 'ld')
            for ld in self.config.ld_files:
                if ld.endswith('.S'):
                    tool_chain = aos_global_config.toolchain
                    aos_global_config.aos_env['CPP'] = os.path.join(tool_chain.tools_path, tool_chain.prefix + 'cpp')
                    target = os.path.join(self.config.out_dir, 'ld', os.path.basename(ld)[:-2])
                    aos_global_config.aos_env.Command(target, ld, aos_global_config.aos_env['CPP']
                                                    + ' -P $_CPPINCFLAGS $SOURCE -o $TARGET')
                    link_flags += ' -T ' + os.path.basename(target)
                else:
                    link_flags += ' -T ' + os.path.basename(ld) + ' -L ' + os.path.dirname(ld)
                    target = ld

                self.config.ld_targets.append(target)
        else:
            print('Pleas config ld file in mcu component...')

        self.config.aos_env.Append(LINKFLAGS=link_flags)


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

        elif aos_global_config.compiler == 'rvct':
            stripped_file = binary.replace('.axf', '.stripped.axf')
            strip_cmd = strip_tool + ' --output=' + stripped_file + ' --strip=debug,symbols --elf ' + binary
            bin_file = binary.replace('.axf', '.bin')
            bin_cmd = objcopy_tool + ' --bin ' + stripped_file + ' --output=' + bin_file

        elif aos_global_config.compiler == 'iar':
            stripped_file = binary.replace('.iarElf', '.stripped.iarElf')
            strip_cmd = strip_tool + ' --strip --silent ' + binary + ' ' + stripped_file
            bin_file = binary.replace('.iarElf', '.bin')
            bin_cmd = objcopy_tool + ' --bin --silent ' + stripped_file + ' ' + bin_file

        elif aos_global_config.compiler == 'cl':
            stripped_file = binary.replace('.exe', '.stripped.exe')
            strip_cmd = strip_tool + ' -o ' + stripped_file + ' ' + binary
            bin_file = binary.replace('.elf', '.bin')
            bin_cmd = objcopy_tool + ' -O binary ' + stripped_file + ' ' + bin_file

        else:
            stripped_file = binary.replace('.elf', '.stripped.elf')
            strip_cmd = strip_tool + ' -o ' + stripped_file + ' ' + binary
            bin_file = binary.replace('.elf', '.bin')
            bin_cmd = objcopy_tool + ' -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes ' + stripped_file + ' ' + bin_file

        if aos_global_config.verbose == '1':
            linkcomstr = ''
        else:
            linkcomstr = 'Making $TARGET'
        #msvs not support striped and make bin temporary.
        if aos_global_config.compiler != 'cl':
            env = aos_global_config.aos_env
            env.Program(stripped_file, binary, LINKCOM=strip_cmd, LINKCOMSTR=linkcomstr)
            env.Program(bin_file, stripped_file, LINKCOM=bin_cmd, LINKCOMSTR=linkcomstr)


class aos_command(object, metaclass=abc.ABCMeta):
    @abc.abstractmethod
    def dispatch_action(self, target, source, env):
        return

from scons_upload import aos_upload
from scons_debug import aos_debug
def ucube_main(args):
    for key,value in ARGLIST:
        aos_global_config.set(key,value)
    if args.get('COMMAND')== 'upload':
        print('[INFO]: Scons to upload!args:%s\n' % args)
        app=args.get('APPLICATION')
        board=args.get('BOARD')
        target=app+'@'+board
        workdir = None
        bindir = None
        if 'WORKPATH' in args:
            workdir=args.get('WORKPATH')
        if 'BINDIR' in args:
            bindir=args.get('BINDIR')
        ret = aos_upload(target, workdir, bindir)
        return ret

    if args.get('COMMAND')== 'debug':
        print('[INFO]: Scons to debug!args:%s\n' % args)
        app=args.get('APPLICATION')
        board=args.get('BOARD')
        target=app+'@'+board
        workdir=None
        bindir=None
        startclient=False
        gdb_args=None
        if 'WORKPATH' in args:
            workdir=args.get('WORKPATH')
        if 'BINDIR' in args:
            bindir=args.get('BINDIR')
        if 'STARTCLIENT' in args:
            startclient=args.get('STARTCLIENT')
        if 'GDBARGS' in args:
            gdb_args=args.get('GDBARGS')
        ret = aos_debug(target, workdir, bindir, startclient, gdb_args)
        return ret

    if args.get('COMPILER')=='cl':
        print('>>>MSVS Tool Environment')
        aos_global_config.aos_env = Environment(ENV=os.environ, CPPPATH=['#include'], TARGET_ARCH='x86')
    else:
        aos_global_config.aos_env = Environment(ENV=os.environ, CPPPATH=['#include'], TOOLS=['mingw', 'gcc', 'g++'])

    #one component support limit source files in windows
    if sys.platform.startswith('win'):
        aos_global_config.max_files = 30
    do_process(base_process_impl(aos_global_config, args))
    do_process(dependency_process_impl(aos_global_config))
    do_process(ld_file_process_impl(aos_global_config))

    if aos_global_config.ide != '':
        do_process(ide_transfer_process_impl(aos_global_config))
    else:
        do_process(build_rule_process_impl(aos_global_config))
        do_process(link_process_impl(aos_global_config))
        do_process(create_bin_process_impl(aos_global_config))
