import os
import sys
import string
import re, shutil

import xml.etree.ElementTree as etree
import config_mk
import gen_sensor_cb_3rd

from xml.etree.ElementTree import SubElement
from xml_format import gen_indent
from config_mk import Projects


#
# config_mk.py was generated while building, it defines:
#
# Projects = [
#  { 'name':'alicrypto',
#    'src':[ sources ... ],
#    'include': [ include_dirs ... ],
#    'c_opts_iar': '...',
#    'as_opts_iar': '...',
#    'c_opts_keil': '...',
#    'as_opts_keil': '...',
#  },
#  ...
# ]                      -> <GroupName>name</GroupName>
#                             <Files> ... </Files>
# keil_device = ...      -> Device
# keil_vendor = ...      -> Vendor
# global_cflags = ...    -> MiscControls (cflags)
# global_ldflags = ...   -> ScatterFile, Misc
# global_asmflags = ...  -> MiscControls (asmflags)
# global_includes = ...  -> IncludePath
# global_defines = ...   -> Define
#

# Global Definitions
AOS_RELATIVE_PATH = "../../../../"
OPT_DIR = "opts/"
TEMPLATE_PROJX = "build/scripts/template.uvprojx"
TEMPLATE_OPTX = "build/scripts/template.uvoptx"

# Elements need to be changed:
element_dict = {
    "TargetName": { "xpath": "Targets/Target/TargetName" },
    "Device": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/Device" },
    "Vendor": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/Vendor" },
    "OutputName": { "xpath": "Targets/Target/TargetOption/TargetCommonOption/OutputName" },
    "ScatterFile": { "xpath": "Targets/Target/TargetOption/TargetArmAds/LDads/ScatterFile" },
    "Misc": { "xpath": "Targets/Target/TargetOption/TargetArmAds/LDads/Misc" },
    "AdsCpuType": { "xpath": "Targets/Target/TargetOption/TargetArmAds/ArmAdsMisc/AdsCpuType" },
    "Define": { "xpath": "Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/Define" },
    "IncludePath": { "xpath": "Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/IncludePath" },
    "MiscControls_cflags": { "xpath": "Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/MiscControls" },
    "MiscControls_asmflags": { "xpath": "Targets/Target/TargetOption/TargetArmAds/Aads/VariousControls/MiscControls" },
}

def create_file(data, filename):
    """ Create *_opts files """
    with open(filename, "w") as f:
        f.write(data)

def get_element_value(element_dict, buildstring):
    """ Get elements value """

    # TargetName, OutputName = app@board
    element_dict["TargetName"]["value"] = buildstring
    element_dict["OutputName"]["value"] = buildstring

    # Device = $(NAME)_KEIL_DEVICE that defined in board makefile
    element_dict["Device"]["value"] = config_mk.keil_device

    # Vendor = $(NAME)_KEIL_VENDOR that defined in board makefile
    element_dict["Vendor"]["value"] = config_mk.keil_vendor

    # ScatterFile = the matched part in LDFLAGS "--scatter=(*.sct)"
    # Misc = global ldflags
    patten = re.compile(r"(.*)-L\s+--scatter=(.*\.sct)(.*)")
    match = patten.match(config_mk.global_ldflags)
    if match:
        scatterfile = AOS_RELATIVE_PATH + match.group(2)
        element_dict["ScatterFile"]["value"] = scatterfile

        tmp_ldflags = match.group(1) + match.group(3)
        element_dict["Misc"]["value"] = tmp_ldflags.replace("-L ", "")
    else:
        element_dict["ScatterFile"]["value"] = ""
        element_dict["Misc"]["value"] = config_mk.global_ldflags.replace("-L ", "")

    # AdsCpuType = HOST_ARCH that defined in board makefile
    element_dict["AdsCpuType"]["value"] = config_mk.host_arch

    # Define = global defines splitted by ","
    element_dict["Define"]["value"] = config_mk.global_defines.replace(" ",",")

    # IncludePath = global include dirs splitted by ";"
    include_path = config_mk.global_includes.replace("./", "")
    include_path = include_path.replace("-I","")
    include_path = ";".join([AOS_RELATIVE_PATH + item for item in include_path.split()])
    element_dict["IncludePath"]["value"] = include_path

    # MiscControls_cflags = global cflags
    element_dict["MiscControls_cflags"]["value"] = config_mk.global_cflags

    # MiscControls_asmflags = global asmflags
    element_dict["MiscControls_asmflags"]["value"] = config_mk.global_asmflags

def file_type_value(fn):
    """ Mapping Number and Filetype """
    if fn.endswith('.h'):
        return 5        
    if fn.endswith('.s') or fn.endswith('.S'):
        return 2
    if fn.endswith('.lib') or fn.endswith('.a'):
        return 4        
    if fn.endswith('.cpp') or fn.endswith('.cxx'):
        return 8
    if fn.endswith('.c') or fn.endswith('.C'):
        return 1
    
    return 5

def add_group(parent, name, files, project_path):
    """ Create xml SubElenent:
    <Group>
      <GroupName>group_name.text</GroupName>
      <Files>
        <File>
          <FileName>file_name.text</FileName>
          <FileType>file_type.text</FileType>
          <FilePath>file_path.text</FilePath>
        </File>
      </Files>
    """
    group = SubElement(parent, 'Group')
    group_name = SubElement(group, 'GroupName')
    group_name.text = name

    for f in files:
        files = SubElement(group, 'Files')
        file = SubElement(files, 'File')
        file_name = SubElement(file, 'FileName')
        name = os.path.basename(f)

        file_name.text = name
        file_type = SubElement(file, 'FileType')
        file_type.text = '%d' % file_type_value(name)
        file_path = SubElement(file, 'FilePath')
        file_path.text = AOS_RELATIVE_PATH + f

    return group

def changeItemForMcu(tree, xpath, value):
    """ Set Element's value """
    element = tree.find(xpath)
    element.text = value

def gen_projxfile(tree, target, buildstring, Projects):
    project_path = os.path.dirname(os.path.abspath(target))
    boardname = buildstring.split("@")[1]

    project_opts_path = os.path.join(project_path, "opts")
    if not os.path.isdir(project_opts_path):
        os.makedirs(project_opts_path)

    root = tree.getroot()
    get_element_value(element_dict, buildstring)

    for key in element_dict:
        xpath = element_dict[key]["xpath"]
        value = element_dict[key]["value"]
        changeItemForMcu(tree, xpath, value)

    # add group
    groups = tree.find('Targets/Target/Groups')
    if groups is None:
        groups = SubElement(tree.find('Targets/Target'), 'Groups')
    groups.clear() # clean old groups
    for group in Projects:
        # don't add an empty group
        if len(group['src']) != 0:
            if group['name'] == 'sensor':
                gen_sensor_cb_3rd.process(group['src'],'./projects/Keil/gen_sensor.c')
                group['src'].append('./projects/Keil/gen_sensor.c')

            group_tree = add_group(groups, group['name'], group['src'], project_path)

            # add GroupOption
            GroupOption     = SubElement(group_tree,  'GroupOption')
            GroupArmAds     = SubElement(GroupOption, 'GroupArmAds')
            Cads            = SubElement(GroupArmAds, 'Cads')
            VariousControls = SubElement(Cads, 'VariousControls')
            MiscControls    = SubElement(VariousControls, 'MiscControls')
            MiscControls.text = "--via %s%s.c_opts" % (OPT_DIR, group["name"])
            
            Aads            = SubElement(GroupArmAds, 'Aads')
            VariousControls = SubElement(Aads, 'VariousControls')
            MiscControls    = SubElement(VariousControls, 'MiscControls')
            MiscControls.text = "--via %s%s.as_opts" % (OPT_DIR, group["name"])

        if group['c_opts_keil']:
            filename = os.path.join(project_opts_path, "%s.c_opts" % group['name'])
            create_file(group['c_opts_keil'], filename)

        if group['as_opts_keil']:
            filename = os.path.join(project_opts_path, "%s.as_opts" % group['name'])
            create_file(group['as_opts_keil'], filename)
    
    gen_indent(root)
    
    with open(target, 'wb') as f:
        f.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n'.encode())
        f.write(etree.tostring(root, encoding='utf-8'))

def gen_optxfile(optx_tree, optx_file, buildstring):
    TargetName = optx_tree.find('Target/TargetName')
    TargetName.text = buildstring

    with open(optx_file, "wb") as f:
        f.write(etree.tostring(optx_tree.getroot(), encoding='utf-8'))

def main():
    # buildstring, eg: nano@b_l475e
    buildstring = sys.argv[1]

    projx_file = "projects/Keil/%s/keil_project/%s.uvprojx" % (buildstring, buildstring)
    optx_file = projx_file.replace('.uvprojx', '.uvoptx')

    print("Creating keil project %s" % (buildstring))
    projx_tree = etree.parse(TEMPLATE_PROJX)
    optx_tree = etree.parse(TEMPLATE_OPTX)

    # create uvprojx file
    gen_projxfile(projx_tree, projx_file, buildstring, Projects)

    # create uvoptx file
    gen_optxfile(optx_tree, optx_file, buildstring)

    # copy out/config/autoconf.h to project dir
    autoconf_h = "out/config/autoconf.h"
    if os.path.isfile(autoconf_h):
        shutil.copyfile(autoconf_h, os.path.join(os.path.dirname(projx_file), "autoconf.h"))

    print("Keil project created at %s" % (projx_file))

if __name__ == "__main__":
    main()
