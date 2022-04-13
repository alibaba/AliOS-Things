# -*- coding:utf-8 -*-
#

import SCons.Action
import SCons.Builder
import SCons.Util


class ToolObjcopyWarning(SCons.Warnings.Warning):
    pass


class ObjcopyCompilerNotFound(ToolObjcopyWarning):
    pass


SCons.Warnings.enableWarningClass(ToolObjcopyWarning)


def _detect(env):
    """ Try to detect the OBJDUMP compiler """
    try:
        return env['OBJDUMP']
    except KeyError:
        pass

    objdump = env.WhereIs('objdump') or env.WhereIs('objdump')
    if objdump:
        return objdump

    raise SCons.Errors.StopError(
        ObjcopyCompilerNotFound,
        "Could not detect objdump")


_dump_builder = SCons.Builder.Builder(
    action=SCons.Action.Action('$DUMPCOM', '$DUMPCOMSTR'),
    src_suffix='$PROGSUFFIX',
    suffix='$DUMPSUFFIX',
    single_source=1)


def generate(env):
    env['OBJDUMP'] = _detect(env)
    env.SetDefault(
        DUMPFLAGS=SCons.Util.CLVar(''),

        DUMPSUFFIX='.asm',

        # OBJDUMP commands
        DUMPCOM='$OBJDUMP $DUMPFLAGS -d $SOURCE > $TARGET',
        DUMPCOMSTR='',
    )

    env['BUILDERS']['Dump'] = _dump_builder


def exists(env):
    return _detect(env)
