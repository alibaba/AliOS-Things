#
# build script written by : Michael Brockus.
# github repo author: Mark VanderVoord.
#
# license: MIT
#
project('CException', 'c',
    license: 'MIT',
    meson_version: '>=0.53.0',
    default_options: ['layout=flat', 'warning_level=3', 'werror=true', 'c_std=c11']
)
lang = 'c'
cc = meson.get_compiler(lang)

#
# Meson: Add compiler flags
if cc.get_id() == 'clang'
    add_project_arguments(cc.get_supported_arguments(
            [
                '-Wexit-time-destructors',
                '-Wglobal-constructors',
                '-Wmissing-prototypes',
                '-Wmissing-noreturn',
                '-Wno-missing-braces',
                '-Wold-style-cast', '-Wpointer-arith', '-Wweak-vtables',
                '-Wcast-align', '-Wconversion', '-Wcast-qual', '-Wshadow'
            ]
        ), language: lang)
endif

if cc.get_argument_syntax() == 'gcc'
    add_project_arguments(cc.get_supported_arguments(
            [
                '-Wformat', '-Waddress', '-Winit-self', '-Wno-multichar',
                '-Wpointer-arith'       , '-Wwrite-strings'              ,
                '-Wno-parentheses'      , '-Wno-type-limits'             ,
                '-Wformat-security'     , '-Wunreachable-code'           ,
                '-Waggregate-return'    , '-Wformat-nonliteral'          ,
                '-Wmissing-declarations', '-Wmissing-include-dirs'       ,
                '-Wno-unused-parameter'
            ]
        ), language: lang)
endif

#
# Sub directory to project source code
subdir('lib')
cexception_dep = declare_dependency(link_with: cexception_lib, include_directories: cexception_dir)
