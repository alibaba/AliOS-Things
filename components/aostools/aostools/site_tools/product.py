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
    """ Try to detect the PRODUCT compiler """
    try:
        return env['PRODUCT']
    except KeyError:
        pass

    return env.WhereIs('product') or env.WhereIs('/usr/local/bin/product')

    raise SCons.Errors.StopError(
        ObjcopyCompilerNotFound,
        "Could not detect product")


_hex_builder = SCons.Builder.Builder(
    action=SCons.Action.Action('$HEXCOM', '$HEXCOMSTR'),
    src_suffix='$PROGSUFFIX',
    suffix='$HEXSUFFIX',
    single_source=1)

_image_builder = SCons.Builder.Builder(
    action=SCons.Action.Action('$IMAGECOM', '$IMAGECOMSTR'),
    src_suffix='$PROGSUFFIX',
    suffix='$IMAGESUFFIX',
    single_source=1)


def generate(env):
    env['PRODUCT'] = _detect(env)
    env.SetDefault(
        IMAGEFLAGS=SCons.Util.CLVar(''),
        HEXFLAGS=SCons.Util.CLVar(''),

        IMAGESUFFIX='.zip',
        HEXSUFFIX='',

        # product image $(MK_GENERATED_IMGS_PATH)/images.zip -e $(MK_GENERATED_IMGS_PATH) -x
        HEXCOM='$PRODUCT image $HEXFLAGS ${SOURCE} -e ${PATH} -x',
        HEXCOMSTR='',

        # product image $(MK_GENERATED_IMGS_PATH)/images.zip -i $(MK_GENERATED_IMGS_PATH)/data -l -p
        IMAGECOM='$PRODUCT image ${TARGET} -i ${PATH} -l -p',
        IMAGECOMSTR='',
    )

    env.Append(BUILDERS={
        'Zip': _image_builder,
        'Hex': _hex_builder,
    })


def exists(env):
    return _detect(env)
