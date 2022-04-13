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
    """ Try to detect the OBJCOPY compiler """
    try:
        return env['OBJCOPY']
    except KeyError:
        pass

    objcopy = env.WhereIs('objcopy') or env.WhereIs('objcopy')
    if objcopy:
        return objcopy

    raise SCons.Errors.StopError(
        ObjcopyCompilerNotFound,
        "Could not detect objcopy")


_srec_builder = SCons.Builder.Builder(
    action=SCons.Action.Action('$SRECCOM', '$SRECCOMSTR'),
    src_suffix='$PROGSUFFIX',
    suffix='$SRECSUFFIX',
    single_source=1)

_bin_builder = SCons.Builder.Builder(
    action=SCons.Action.Action('$BINCOM', '$BINCOMSTR'),
    src_suffix='$PROGSUFFIX',
    suffix='$BINSUFFIX',
    single_source=1)


def generate(env):
    env['OBJCOPY'] = _detect(env)
    env.SetDefault(
        SRECFLAGS=SCons.Util.CLVar(''),
        BINFLAGS=SCons.Util.CLVar(''),

        SRECSUFFIX='.srec',
        BINSUFFIX='',

        # OBJCOPY commands
        SRECCOM='$OBJCOPY $SRECFLAGS -O srec $SOURCE $TARGET',
        SRECCOMSTR='',

        BINCOM='$OBJCOPY $BINFLAGS -O binary $SOURCE $TARGET',
        BINCOMSTR='',
    )

    env['BUILDERS']['Srec'] = _srec_builder
    env['BUILDERS']['Binary'] = _bin_builder


def exists(env):
    return _detect(env)
