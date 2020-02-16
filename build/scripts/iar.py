import os
import sys
import string
import shutil
import re

import xml.etree.ElementTree as etree
import config_mk
from xml.etree.ElementTree import SubElement
from os.path import basename
from xml_format import gen_indent
from config_mk import Projects

AOS_RELATIVE_PATH = '$PROJ_DIR$/../../../../'
OPT_DIR = '$PROJ_DIR$/opts/'

element_dict = {
    "OGChipSelectEditMenu": "",
    "IlinkOutputFile": "",
    "IlinkIcfFile": "",
}

def create_file(data, filename):
    """ Create *_opts files """
    with open(filename, "w") as f:
        f.write(data)

def get_element_value(element_dict, buildstring):
    element_dict["OGChipSelectEditMenu"] = config_mk.iar_ogcmenu.replace(' ', '\t', 1)
    element_dict["IlinkOutputFile"] = buildstring

    patten = re.compile(r".*--config\s+(.*\.icf).*")
    match = patten.match(config_mk.global_ldflags)
    if match:
        element_dict["IlinkIcfFile"] = AOS_RELATIVE_PATH + match.group(1)

def add_group(parent, name, files, includes, project_path):
    group = SubElement(parent, 'group')
    group_name = SubElement(group, 'name')
    group_name.text = name
    
    for f in files:
        file = SubElement(group, 'file')
        file_name = SubElement(file, 'name')
        if repeat_path.count(f):
            includes.append(os.path.dirname(f))
            fnewName = f.replace('./','')
            fnewName = fnewName.replace('/','_')
            fnewPath = project_path+'/'+fnewName
            #print 'copy', f, 'to', fnewPath
            shutil.copyfile(f,fnewPath)
            f = "$PROJ_DIR$/"+fnewName
            file_name.text = f
        else:
            file_name.text = AOS_RELATIVE_PATH + f
    
    
    group_config = SubElement(group, 'configuration')
    group_config_name = SubElement(group_config, 'name')
    group_config_name.text = 'Debug'
    
    group_config_settings = SubElement(group_config, 'settings')
    group_settings_name = SubElement(group_config_settings, 'name')
    group_settings_name.text = 'ICCARM'
    
    group_settings_data = SubElement(group_config_settings, 'data')
    group_data_option = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option, 'name')
    group_option_name.text = 'IExtraOptionsCheck'
    group_option_state = SubElement(group_data_option, 'state')
    group_option_state.text = '1'
    
    group_data_option2 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option2, 'name')
    group_option_name.text = 'IExtraOptions'
    group_option_state = SubElement(group_data_option2, 'state')
    group_option_state.text = '-f '+OPT_DIR+name+".c_opts"
    
    group_data_option3 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option3, 'name')
    group_option_name.text = 'CCIncludePath2'
  
    for i in includes:
        stateTemp = SubElement(group_data_option3, 'state')
        stateTemp.text = AOS_RELATIVE_PATH + i
    
    
    group_config_settings2 = SubElement(group_config, 'settings')
    group_settings_name = SubElement(group_config_settings2, 'name')
    group_settings_name.text = 'AARM'
    
    group_settings_data = SubElement(group_config_settings2, 'data')
    group_data_option = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option, 'name')
    group_option_name.text = 'AExtraOptionsCheckV2'
    group_option_state = SubElement(group_data_option, 'state')
    group_option_state.text = '0'
    
    group_data_option2 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option2, 'name')
    group_option_name.text = 'AExtraOptionsV2'
    group_option_state = SubElement(group_data_option2, 'state')
    group_option_state.text = '-f '+OPT_DIR+name+".as_opts"
    
    group_data_option3 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option3, 'name')
    group_option_name.text = 'AUserIncludes'
    for i in includes:
        stateTemp = SubElement(group_data_option3, 'state')
        stateTemp.text = AOS_RELATIVE_PATH + i

# automation to do
def changeItemForMcu( tree, element_dict, buildstring ):
    for config in tree.findall('configuration'):
        for settings in config.findall('settings'):
            if settings.find('name').text == 'ILINK':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'IlinkOutputFile':
                        option.find('state').text = buildstring + '.out'
                    if option.find('name').text == 'IlinkIcfFile': 
                        option.find('state').text = element_dict["IlinkIcfFile"]

            if settings.find('name').text == 'General':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'OGChipSelectEditMenu':
                        option.find('state').text = element_dict["OGChipSelectEditMenu"]

work_space_content = '''<?xml version="1.0" encoding="UTF-8"?>

<workspace>
  <project>
    <path>$WS_DIR$\%s</path>
  </project>
  <batchBuild/>
</workspace>


'''
                            
def gen_workspace(target, buildstring):
    # make an workspace 
    workspace = target.replace('.ewp', '.eww')
    xml = work_space_content % (buildstring+'.ewp')
    with open (workspace, "w") as out:
        out.write(xml)
    
repeat_path=[]
def gen_project(target, script, buildstring):
    project_path = os.path.dirname(os.path.abspath(target))

    project_opts_path = os.path.join(project_path, "opts")
    if not os.path.isdir(project_opts_path):
        os.makedirs(project_opts_path)

    get_element_value(element_dict, buildstring)
    
    boardname = buildstring.split("@")[1]
    projfilename = 'build/scripts/template_%s.ewp' % (boardname)
    if os.path.exists(projfilename) == False:
        projfilename = 'build/scripts/template.ewp'
    tree = etree.parse(projfilename)
    root = tree.getroot()

    existedFileNameString=[]
    # copy repeat source file and replace old one
    for group in script:
        for filePath in group['src']:
            filename = os.path.splitext(basename(filePath))
            if existedFileNameString.count(filename):
                repeat_path.append(filePath)
            else:
                existedFileNameString.append(filename)        
        if group['c_opts_iar']:
            filename = os.path.join(project_opts_path, "%s.c_opts" % group['name'])
            create_file(group['c_opts_iar'], filename)

        if group['as_opts_iar']:
            filename = os.path.join(project_opts_path, "%s.as_opts" % group['name'])
            # strip '--cpu xxx' from as_opts_iar which causes IAR IDE build failed
            as_opts_tmp = re.sub(r'^\s*Cortex-M\d+ ','',group['as_opts_iar'])
            create_file(as_opts_tmp, filename)
     
    if len(repeat_path):
        print('repeat name files:', repeat_path)
        print('will copy them to '+project_path+'/ !')
    
    # add group
    for group in script:
        add_group(root, group['name'], group['src'], group['include'], project_path)       
    
    changeItemForMcu(tree, element_dict, buildstring)
    gen_indent(root)
    projString = etree.tostring(root, encoding='utf-8')
    with open(target, "w") as out:
        out.write(projString.decode())

    gen_workspace(target, buildstring)

def main():
    #argv[1]: buildstring, eg: nano@b_l475e
    buildstring = sys.argv[1]
    projectPath = "projects/IAR/%s/iar_project/%s.ewp" % (buildstring, buildstring)

    print('Making iar project '+buildstring)
    gen_project(projectPath, Projects, buildstring)

    # copy aos_config.h to project dir
    aos_config_h = "aos_config.h"
    if os.path.isfile(aos_config_h):
        shutil.copyfile(aos_config_h, os.path.join(os.path.dirname(projectPath), "aos_config.h"))

    print('iar project: '+ projectPath + ' has generated over')

if __name__ == "__main__":
    main()

