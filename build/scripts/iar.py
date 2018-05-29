import os
import sys
import string
import shutil 

import xml.etree.ElementTree as etree
from xml.etree.ElementTree import SubElement
from os.path import basename
from xml_format import gen_indent
from config_mk import Projects


def add_group(parent, name, files, includes, project_path):
    cur_encoding = sys.getfilesystemencoding()
    group = SubElement(parent, 'group')
    group_name = SubElement(group, 'name')
    group_name.text = name
    
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
    group_option_state.text = '-f '+opt_dir+name+".c_opts"
    
    group_data_option3 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option3, 'name')
    group_option_name.text = 'CCIncludePath2'
  
    for i in includes:
        stateTemp = SubElement(group_data_option3, 'state')
        stateTemp.text = (aos_relative_path + i).decode(cur_encoding)
    
    
    group_config_settings2 = SubElement(group_config, 'settings')
    group_settings_name = SubElement(group_config_settings2, 'name')
    group_settings_name.text = 'AARM'
    
    group_settings_data = SubElement(group_config_settings2, 'data')
    group_data_option = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option, 'name')
    group_option_name.text = 'AExtraOptionsCheckV2'
    group_option_state = SubElement(group_data_option, 'state')
    group_option_state.text = '1'
    
    group_data_option2 = SubElement(group_settings_data, 'option')
    group_option_name = SubElement(group_data_option2, 'name')
    group_option_name.text = 'AExtraOptionsV2'
    group_option_state = SubElement(group_data_option2, 'state')
    group_option_state.text = '-f '+opt_dir+name+".as_opts"
    
    
    for f in files:
        if repeat_path.count(f):
            fnewName = f.replace('./','')
            fnewName = fnewName.replace('/','_')
            fnewPath = proj_output_dir+'/'+fnewName
            #print 'copy', f, 'to', fnewPath
            shutil.copyfile(f,fnewPath)
            f = fnewPath
        file = SubElement(group, 'file')
        file_name = SubElement(file, 'name')
        file_name.text = (aos_relative_path + f).decode(cur_encoding)

# automation to do
def changeItemForMcu( tree ):
    for config in tree.findall('configuration'):
        for settings in config.findall('settings'):
            if settings.find('name').text == 'ILINK':
                data = settings.find('data')
                for option in data.findall('option'):
                    if option.find('name').text == 'IlinkOutputFile':
                        option.find('state').text = buildstring + '.out'
                    if option.find('name').text == 'IlinkIcfFile': 
                        if 'starterkit' in buildstring:
                            option.find('state').text = '$PROJ_DIR$\../../../../'+'platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf'
                        if 'stm32l432' in buildstring:
                            option.find('state').text = '$PROJ_DIR$\../../../../'+'platform/mcu/stm32l4xx/src/STM32L432KC-Nucleo/STM32L432.icf'

                          
work_space_content = '''<?xml version="1.0" encoding="UTF-8"?>

<workspace>
  <project>
    <path>$WS_DIR$\%s</path>
  </project>
  <batchBuild/>
</workspace>


'''
                            
def gen_workspace(target):
    # make an workspace 
    workspace = target.replace('.ewp', '.eww')
    out = file(workspace, 'wb')
    xml = work_space_content % (buildstring+'.ewp')
    out.write(xml)
    out.close()
    
repeat_path=[]
def gen_project(target, script):
    project_path = os.path.dirname(os.path.abspath(target))

    tree = etree.parse('build/scripts/template.ewp')
    root = tree.getroot()

    out = file(target, 'wb')
    
    existedFileNameString=[]
    # copy repeat source file and replace old one
    for group in script:
        for filePath in group['src']:
            filename = os.path.splitext(basename(filePath))
            if existedFileNameString.count(filename):
                repeat_path.append(filePath)
            else:
                existedFileNameString.append(filename)        
     
    if len(repeat_path):
        print 'repeat name files:', repeat_path
        print 'will copy them to '+proj_output_dir+'/ !'
    
    # add group
    for group in script:
        add_group(root, group['name'], group['src'], group['include'], project_path)       
    
    changeItemForMcu(tree)
    gen_indent(root)
    projString = etree.tostring(root, encoding='utf-8')
    if 'starterkit' in buildstring:
        projString = projString.replace('STM32L475VG','STM32L433RC')
    if 'stm32l432' in buildstring:
        projString = projString.replace('STM32L475VG','STM32L432KC')
    out.write(projString)
    out.close()

    gen_workspace(target)

#argv[1]: buildstring, eg: nano@b_l475e
buildstring = sys.argv[1]
proj_output_dir = 'projects/autogen/'+buildstring+'/iar_project'
#use in xml text
aos_relative_path = '$PROJ_DIR$\\' + '../../../../'
projectPath = proj_output_dir+'/'+buildstring+'.ewp'
opt_dir = '$PROJ_DIR$\\opts/'

print 'Making iar project '+buildstring
gen_project(projectPath, Projects)
print 'iar project: '+ projectPath + ' has generated over'



