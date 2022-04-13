# -*- coding:utf-8 -*-
#
# Copyright (C) 2019-2020 Alibaba Group Holding Limited


from __future__ import print_function
from aostools import *

import codecs


class Cdk(Command):
    common = True
    helpSummary = "Generate current solution SDK"
    helpUsage = """
%prog
"""
    helpDescription = """
generate current solution SDK.
"""
    # def _Options(self, p):
    #     p.add_option('-a', '--all',
    #                  dest='show_all', action='store_true',
    #                  help='show the complete list of commands')

    def Execute(self, opt, args):
        yoc = YoC()
        solution = yoc.getSolution()
        if solution:
            packages=''
            for c in solution.components:
                packages += '    <Package ID="%s" Version="%s" IsBasic="false"/>\n' % (c.name, c.version)

            text = temp.format(
                name=solution.solution_component.name,
                description=solution.solution_component.description,
                chip_name=solution.chip_component.name,
                chip_version=solution.chip_component.version,
                board_name=solution.board_component.name,
                board_version=solution.board_component.version,
                packages=packages,
                cpu_name=solution.cpu_name,
                CFLAGS=' '.join(solution.CFLAGS),
                LINKFLAGS=' '.join(solution.LINKFLAGS),
                ld_script=solution.ld_script,
            )

            generate_flash_init(os.path.join(solution.board_component.path, 'configs/config.yaml'),
                os.path.join(solution.solution_component.path, 'script/flash.init')
            )
            try:
                shutil.copytree('/usr/local/lib/aostools/script', os.path.join(solution.solution_component.path, 'script'))
            except:
                pass

            try:
                filename = os.path.join(solution.solution_component.path, 'project.cdkproj')
                with codecs.open(filename, 'w', 'UTF-8') as f:
                    f.write(text)
                return True
            except:
                put_string("Generate %s file failed." % filename)



temp='''
<?xml version="1.0" encoding="UTF-8"?>
<Project Name="{name}" Version="1">
  <Description>{description}</Description>
  <MonitorProgress>
    <FlashOperate>230</FlashOperate>
    <DebugLaunch>42</DebugLaunch>
  </MonitorProgress>
  <Chips>
    <Chip ID="{chip_name}" Version="{chip_version}" IsBasic="false"/>
  </Chips>
  <Boards>
    <Board ID="{board_name}" Version="{board_version}" IsBasic="false"/>
  </Boards>
  <Packages>
{packages}  </Packages>
  <MergedToYaml>yes</MergedToYaml>
  <BuildConfigs>
    <BuildConfig Name="BuildSet">
      <Target>
        <ROMBank Selected="1">
          <ROM1>
            <InUse>yes</InUse>
            <Start>0x20000000</Start>
            <Size>0x1000</Size>
          </ROM1>
          <ROM2>
            <InUse>no</InUse>
            <Start/>
            <Size/>
          </ROM2>
          <ROM3>
            <InUse>no</InUse>
            <Start/>
            <Size/>
          </ROM3>
          <ROM4>
            <InUse>no</InUse>
            <Start/>
            <Size/>
          </ROM4>
          <ROM5>
            <InUse>no</InUse>
            <Start/>
            <Size/>
          </ROM5>
        </ROMBank>
        <RAMBank>
          <RAM1>
            <InUse>no</InUse>
            <Start>0x20001000</Start>
            <Size>0x1000</Size>
            <Init>yes</Init>
          </RAM1>
          <RAM2>
            <InUse>no</InUse>
            <Start/>
            <Size/>
            <Init>yes</Init>
          </RAM2>
          <RAM3>
            <InUse>no</InUse>
            <Start/>
            <Size/>
            <Init>yes</Init>
          </RAM3>
          <RAM4>
            <InUse>no</InUse>
            <Start/>
            <Size/>
            <Init>yes</Init>
          </RAM4>
          <RAM5>
            <InUse>no</InUse>
            <Start/>
            <Size/>
            <Init>yes</Init>
          </RAM5>
        </RAMBank>
        <CPU>{cpu_name}</CPU>
        <UseMiniLib>yes</UseMiniLib>
        <Endian>little</Endian>
        <UseHardFloat>no</UseHardFloat>
        <UseEnhancedLRW>no</UseEnhancedLRW>
        <UseContinueBuild>no</UseContinueBuild>
        <UseSemiHost>no</UseSemiHost>
      </Target>
      <Output>
        <OutputName>$(ProjectName)</OutputName>
        <Type>Executable</Type>
        <CreateHexFile>yes</CreateHexFile>
        <CreateBinFile>no</CreateBinFile>
        <Preprocessor>no</Preprocessor>
        <Disasm>no</Disasm>
        <CallGraph>no</CallGraph>
        <Map>no</Map>
      </Output>
      <User>
        <BeforeCompile>
          <RunUserProg>no</RunUserProg>
          <UserProgName/>
        </BeforeCompile>
        <BeforeMake>
          <RunUserProg>yes</RunUserProg>
          <UserProgName>"$(ProjectPath)/script/pre_build.sh"</UserProgName>
        </BeforeMake>
        <AfterMake>
          <RunUserProg>yes</RunUserProg>
          <UserProgName>"$(ProjectPath)/script/aft_build.sh"</UserProgName>
        </AfterMake>
      </User>
      <Compiler>
        <Define/>
        <Undefine/>
        <Optim>Optimize size (-Os)</Optim>
        <DebugLevel>Default (-g)</DebugLevel>
        <IncludePath>$(ProjectPath)/src/tftp;$(ProjectPath)cb5654/include;$(ProjectPath)include;$(ProjectPath)src/audio;$(ProjectPath)src/wifipair;$(ProjectPath)src/wifipair/ap</IncludePath>
        <OtherFlags>{CFLAGS}</OtherFlags>
        <Verbose>no</Verbose>
        <Ansi>no</Ansi>
        <Syntax>no</Syntax>
        <Pedantic>no</Pedantic>
        <PedanticErr>no</PedanticErr>
        <InhibitWarn>no</InhibitWarn>
        <AllWarn>yes</AllWarn>
        <WarnErr>no</WarnErr>
        <OneElfS>no</OneElfS>
        <Fstrict>no</Fstrict>
      </Compiler>
      <Asm>
        <Define/>
        <Undefine/>
        <IncludePath>$(ComProjectPath)include;$(ProjectPath)/src/tftp;$(ProjectPath)src;$(ProjectPath)src/audio;$(ProjectPath)src/wifipair;$(ProjectPath)src/wifipair/ap</IncludePath>
        <OtherFlags/>
        <DebugLevel>gdwarf2</DebugLevel>
      </Asm>
      <Linker>
        <Garbage>yes</Garbage>
        <LDFile>$(ProjectPath){ld_script}</LDFile>
        <LibName/>
        <LibPath/>
        <OtherFlags>{LINKFLAGS}</OtherFlags>
        <AutoLDFile>no</AutoLDFile>
      </Linker>
      <Debug>
        <LoadApplicationAtStartup>no</LoadApplicationAtStartup>
        <Connector>ICE</Connector>
        <StopAt>no</StopAt>
        <StopAtText>main</StopAtText>
        <InitFile>$(ProjectPath)script/cdkgdbinit</InitFile>
        <AutoRun>no</AutoRun>
        <ResetType>Hard Reset</ResetType>
        <SoftResetVal/>
        <ResetAfterLoad>no</ResetAfterLoad>
        <Dumpcore>no</Dumpcore>
        <DumpcoreText>$(ProjectPath)$(ProjectName).cdkcore</DumpcoreText>
        <ConfigICE>
          <IP>localhost</IP>
          <PORT>1025</PORT>
          <Clock>12000</Clock>
          <Delay>10</Delay>
          <WaitReset>500</WaitReset>
          <DDC>yes</DDC>
          <TRST>no</TRST>
          <DebugPrint>no</DebugPrint>
          <Connect>Normal</Connect>
          <ResetType>Soft Reset</ResetType>
          <SoftResetVal>0</SoftResetVal>
          <RTOSType>Bare Metal</RTOSType>
          <DownloadToFlash>yes</DownloadToFlash>
          <ResetAfterConnect>no</ResetAfterConnect>
          <GDBName/>
          <GDBServerType>Local</GDBServerType>
          <OtherFlags/>
        </ConfigICE>
        <ConfigSIM>
          <SIMTarget>soccfg/cskyv2/rhea802.xml</SIMTarget>
          <OtherFlags/>
          <NoGraphic>yes</NoGraphic>
          <Log>no</Log>
          <SimTrace>no</SimTrace>
        </ConfigSIM>
      </Debug>
      <Flash>
        <InitFile>$(ProjectPath)script/flash.init</InitFile>
        <Erase>Erase Sectors</Erase>
        <Algorithms Path="">SC5654A_SPIFlash_CDK.elf</Algorithms>
        <Program>yes</Program>
        <Verify>yes</Verify>
        <ResetAndRun>no</ResetAndRun>
        <ResetType>Soft Reset</ResetType>
        <SoftResetVal/>
        <External>no</External>
        <Command/>
        <Arguments/>
      </Flash>
    </BuildConfig>
  </BuildConfigs>
  <DebugSessions>
    <watchExpressions/>
    <memoryExpressions/>
    <statistics/>
    <peripheralTabs/>
    <WatchDisplayFormat/>
    <LocalDisplayFormat/>
    <debugLayout/>
    <memoryTabColSizeExpressions/>
  </DebugSessions>
</Project>
'''


def generate_flash_init(config_yaml, filename):
    text = '''
#reset
set $psr=0xc0000000
set *(int*)0x42010300=0x03

{downloads} '''
    v = yaml_load(config_yaml)
    cmd = 'download ihex verify=no $(ProjectPath)generated/{name}.hex'
    downloads=[]
    for k in v['partitions']:
        # put_string(k)
        f = cmd.format(name=k['name'])
        downloads.append(f)

    downloads = '\n'.join(downloads)
    # put_string(s)
    text = text.format(downloads=downloads)
    try:
        with codecs.open(filename, 'w', 'UTF-8') as f:
            f.write(text)
        return True
    except:
        put_string("Generate %s file failed." % filename)