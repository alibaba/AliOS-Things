import os
import sys
import string

import xml.etree.ElementTree as etree
from xml.etree.ElementTree import SubElement
from xml_format import gen_indent
from config_mk import Projects

def file_type_value(fn):
    if fn.endswith('.h'):
        return 5        
    if fn.endswith('.s') or fn.endswith('.S'):
        return 2
    if fn.endswith('.lib'):
        return 4        
    if fn.endswith('.cpp') or fn.endswith('.cxx'):
        return 8
    if fn.endswith('.c') or fn.endswith('.C'):
        return 1
    
    return 5

    
#ProjectFiles use for remove same name files
#add files
def add_group(parent, name, files, project_path):
    cur_encoding = sys.getfilesystemencoding()
    group = SubElement(parent, 'Group')
    group_name = SubElement(group, 'GroupName')
    group_name.text = name

    for f in files:
        files = SubElement(group, 'Files')
        file = SubElement(files, 'File')
        file_name = SubElement(file, 'FileName')
        name = os.path.basename(f)

        file_name.text = name.decode(cur_encoding)
        file_type = SubElement(file, 'FileType')
        file_type.text = '%d' % file_type_value(name)
        file_path = SubElement(file, 'FilePath')
        file_path.text = (aos_relative_path + f).decode(cur_encoding)

    return group

# automation to do
def changeItemForMcu( tree ):
    ScatterFile = tree.find('Targets/Target/TargetOption/TargetArmAds/LDads/ScatterFile')
    if 'starterkit' in buildstring:
        ScatterFile.text = '..\..\..\..\platform\mcu\stm32l4xx\src\STM32L433RC-Nucleo\STM32L433.sct'
    if 'stm32l432' in buildstring:
        ScatterFile.text = '..\..\..\..\platform\mcu\stm32l4xx\src\STM32L432KC-Nucleo\STM32L432.sct'
    
# change key word in project file. automation to do
def ModifyProjString( projString ):
    if 'starterkit' in buildstring:
        projString = projString.replace('STM32L475VGTx','STM32L433RCTx')
    if 'stm32l432' in buildstring:
        projString = projString.replace('STM32L475VGTx','STM32L432KCTx')
    return  projString   
    
def gen_project(tree, target, script):
    project_path = os.path.dirname(os.path.abspath(target))

    root = tree.getroot()
    out = file(target, 'wb')
    out.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')
    
    #change target name
    TargetName = tree.find('Targets/Target/TargetName')
    TargetName.text = buildstring
    OutputName = tree.find('Targets/Target/TargetOption/TargetCommonOption/OutputName')
    OutputName.text = buildstring
    
    # add group
    groups = tree.find('Targets/Target/Groups')
    if groups is None:
        groups = SubElement(tree.find('Targets/Target'), 'Groups')
    groups.clear() # clean old groups
    for group in script:
        # don't add an empty group
        if len(group['src']) != 0:
            group_tree = add_group(groups, group['name'], group['src'], project_path)

            # add GroupOption
            GroupOption     = SubElement(group_tree,  'GroupOption')
            GroupArmAds     = SubElement(GroupOption, 'GroupArmAds')
            Cads            = SubElement(GroupArmAds, 'Cads')
            VariousControls = SubElement(Cads, 'VariousControls')
            MiscControls    = SubElement(VariousControls, 'MiscControls')
            MiscControls.text = '--via '+opt_dir+group['name']+'.c_opts'
            
            Aads            = SubElement(GroupArmAds, 'Aads')
            VariousControls = SubElement(Aads, 'VariousControls')
            MiscControls    = SubElement(VariousControls, 'MiscControls')
            MiscControls.text = '--via '+opt_dir+group['name']+'.as_opts'
    
    # set <OutputName>B-L475E-IOT01</OutputName> 
    
    gen_indent(root)
    
    changeItemForMcu(tree)
    projString = ModifyProjString( etree.tostring(root, encoding='utf-8') )
    out.write(projString)
    out.close()

def gen_main(target, script):
    template_tree = etree.parse('build/scripts/template.uvprojx')
    # create uvprojx file
    gen_project(template_tree, target, script)
    
    # create uvoptx file
    opt_file = target.replace('.uvprojx', '.uvoptx')
    opt_tree = etree.parse('build/scripts/template.uvoptx')
    TargetName = opt_tree.find('Target/TargetName')
    TargetName.text = buildstring
    out = file(opt_file, 'wb')
    projString = ModifyProjString( etree.tostring(opt_tree.getroot(), encoding='utf-8') )
    out.write(projString)
    out.close()

'''
Projects = [
{'name':'alicrypto', 
'src':[ 
'a.c',
'a_1.s',
]
},
{'name':'alinkapp', 
'src':[ 
'./example/alinkapp/alink_sample.c',
]
}
]
'''

#argv[1]: buildstring, eg: nano@b_l475e
buildstring = sys.argv[1]
proj_output_dir = 'projects/autogen/'+buildstring+'/keil_project'
#use in xml text
aos_relative_path = '../../../../'
projectPath = proj_output_dir+'/'+buildstring+'.uvprojx'
opt_dir = 'opts/'

print 'Making keil project '+buildstring
gen_main(projectPath, Projects)
print 'keil project: '+ projectPath + ' has generated over'
