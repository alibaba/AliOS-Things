;******************************************************************************
;* TI ARM C/C++ Codegen                                      Unix v16.9.6.LTS *
;* Date/Time created: Thu Dec 28 17:45:45 2017                                *
;******************************************************************************
	.compiler_opts --abi=eabi --arm_vmrs_si_workaround=off --code_state=16 --embedded_constants=on --endian=little --float_support=vfplib --hll_source=on --object_format=elf --quiet --section_sizes=on --silicon_version=7M4 --symdebug:dwarf --symdebug:dwarf_version=3 --unaligned_access=on 
	.thumb

$C$DW$CU	.dwtag  DW_TAG_compile_unit
	.dwattr $C$DW$CU, DW_AT_name("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$CU, DW_AT_producer("TI TI ARM C/C++ Codegen Unix v16.9.6.LTS Copyright (c) 1996-2017 Texas Instruments Incorporated")
	.dwattr $C$DW$CU, DW_AT_TI_version(0x01)
	.dwattr $C$DW$CU, DW_AT_comp_dir("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/ccs/Release")

$C$DW$1	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$1, DW_AT_name("memset")
	.dwattr $C$DW$1, DW_AT_TI_symbol_name("memset")
	.dwattr $C$DW$1, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$1, DW_AT_declaration
	.dwattr $C$DW$1, DW_AT_external
	.dwattr $C$DW$1, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/string.h")
	.dwattr $C$DW$1, DW_AT_decl_line(0x71)
	.dwattr $C$DW$1, DW_AT_decl_column(0x16)
$C$DW$2	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$2, DW_AT_type(*$C$DW$T$3)

$C$DW$3	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$3, DW_AT_type(*$C$DW$T$10)

$C$DW$4	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$4, DW_AT_type(*$C$DW$T$573)

	.dwendtag $C$DW$1


$C$DW$5	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$5, DW_AT_name("sl_FsOpen")
	.dwattr $C$DW$5, DW_AT_TI_symbol_name("sl_FsOpen")
	.dwattr $C$DW$5, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$5, DW_AT_declaration
	.dwattr $C$DW$5, DW_AT_external
	.dwattr $C$DW$5, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$5, DW_AT_decl_line(0x167)
	.dwattr $C$DW$5, DW_AT_decl_column(0x06)
$C$DW$6	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$6, DW_AT_type(*$C$DW$T$592)

$C$DW$7	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$7, DW_AT_type(*$C$DW$T$600)

$C$DW$8	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$8, DW_AT_type(*$C$DW$T$630)

	.dwendtag $C$DW$5


$C$DW$9	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$9, DW_AT_name("sl_FsClose")
	.dwattr $C$DW$9, DW_AT_TI_symbol_name("sl_FsClose")
	.dwattr $C$DW$9, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$9, DW_AT_declaration
	.dwattr $C$DW$9, DW_AT_external
	.dwattr $C$DW$9, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$9, DW_AT_decl_line(0x18a)
	.dwattr $C$DW$9, DW_AT_decl_column(0x06)
$C$DW$10	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$10, DW_AT_type(*$C$DW$T$599)

$C$DW$11	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$11, DW_AT_type(*$C$DW$T$592)

$C$DW$12	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$12, DW_AT_type(*$C$DW$T$592)

$C$DW$13	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$13, DW_AT_type(*$C$DW$T$600)

	.dwendtag $C$DW$9


$C$DW$14	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$14, DW_AT_name("sl_FsWrite")
	.dwattr $C$DW$14, DW_AT_TI_symbol_name("sl_FsWrite")
	.dwattr $C$DW$14, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$14, DW_AT_declaration
	.dwattr $C$DW$14, DW_AT_external
	.dwattr $C$DW$14, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$14, DW_AT_decl_line(0x1ba)
	.dwattr $C$DW$14, DW_AT_decl_column(0x06)
$C$DW$15	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$15, DW_AT_type(*$C$DW$T$599)

$C$DW$16	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$16, DW_AT_type(*$C$DW$T$13)

$C$DW$17	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$17, DW_AT_type(*$C$DW$T$84)

$C$DW$18	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$18, DW_AT_type(*$C$DW$T$13)

	.dwendtag $C$DW$14


$C$DW$19	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$19, DW_AT_name("sl_Socket")
	.dwattr $C$DW$19, DW_AT_TI_symbol_name("sl_Socket")
	.dwattr $C$DW$19, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$19, DW_AT_declaration
	.dwattr $C$DW$19, DW_AT_external
	.dwattr $C$DW$19, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$19, DW_AT_decl_line(0x1e7)
	.dwattr $C$DW$19, DW_AT_decl_column(0x06)
$C$DW$20	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$20, DW_AT_type(*$C$DW$T$8)

$C$DW$21	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$21, DW_AT_type(*$C$DW$T$8)

$C$DW$22	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$22, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$19


$C$DW$23	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$23, DW_AT_name("sl_Close")
	.dwattr $C$DW$23, DW_AT_TI_symbol_name("sl_Close")
	.dwattr $C$DW$23, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$23, DW_AT_declaration
	.dwattr $C$DW$23, DW_AT_external
	.dwattr $C$DW$23, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$23, DW_AT_decl_line(0x1f9)
	.dwattr $C$DW$23, DW_AT_decl_column(0x06)
$C$DW$24	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$24, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$23


$C$DW$25	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$25, DW_AT_name("sl_Accept")
	.dwattr $C$DW$25, DW_AT_TI_symbol_name("sl_Accept")
	.dwattr $C$DW$25, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$25, DW_AT_declaration
	.dwattr $C$DW$25, DW_AT_external
	.dwattr $C$DW$25, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$25, DW_AT_decl_line(0x22f)
	.dwattr $C$DW$25, DW_AT_decl_column(0x06)
$C$DW$26	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$26, DW_AT_type(*$C$DW$T$8)

$C$DW$27	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$27, DW_AT_type(*$C$DW$T$567)

$C$DW$28	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$28, DW_AT_type(*$C$DW$T$608)

	.dwendtag $C$DW$25


$C$DW$29	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$29, DW_AT_name("sl_Bind")
	.dwattr $C$DW$29, DW_AT_TI_symbol_name("sl_Bind")
	.dwattr $C$DW$29, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$29, DW_AT_declaration
	.dwattr $C$DW$29, DW_AT_external
	.dwattr $C$DW$29, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$29, DW_AT_decl_line(0x24d)
	.dwattr $C$DW$29, DW_AT_decl_column(0x06)
$C$DW$30	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$30, DW_AT_type(*$C$DW$T$8)

$C$DW$31	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$31, DW_AT_type(*$C$DW$T$566)

$C$DW$32	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$32, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$29


$C$DW$33	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$33, DW_AT_name("sl_Listen")
	.dwattr $C$DW$33, DW_AT_TI_symbol_name("sl_Listen")
	.dwattr $C$DW$33, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$33, DW_AT_declaration
	.dwattr $C$DW$33, DW_AT_external
	.dwattr $C$DW$33, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$33, DW_AT_decl_line(0x264)
	.dwattr $C$DW$33, DW_AT_decl_column(0x06)
$C$DW$34	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$34, DW_AT_type(*$C$DW$T$8)

$C$DW$35	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$35, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$33


$C$DW$36	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$36, DW_AT_name("sl_Connect")
	.dwattr $C$DW$36, DW_AT_TI_symbol_name("sl_Connect")
	.dwattr $C$DW$36, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$36, DW_AT_declaration
	.dwattr $C$DW$36, DW_AT_external
	.dwattr $C$DW$36, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$36, DW_AT_decl_line(0x28e)
	.dwattr $C$DW$36, DW_AT_decl_column(0x06)
$C$DW$37	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$37, DW_AT_type(*$C$DW$T$8)

$C$DW$38	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$38, DW_AT_type(*$C$DW$T$566)

$C$DW$39	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$39, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$36


$C$DW$40	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$40, DW_AT_name("sl_Select")
	.dwattr $C$DW$40, DW_AT_TI_symbol_name("sl_Select")
	.dwattr $C$DW$40, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$40, DW_AT_declaration
	.dwattr $C$DW$40, DW_AT_external
	.dwattr $C$DW$40, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$40, DW_AT_decl_line(0x2cb)
	.dwattr $C$DW$40, DW_AT_decl_column(0x06)
$C$DW$41	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$41, DW_AT_type(*$C$DW$T$8)

$C$DW$42	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$42, DW_AT_type(*$C$DW$T$547)

$C$DW$43	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$43, DW_AT_type(*$C$DW$T$547)

$C$DW$44	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$44, DW_AT_type(*$C$DW$T$547)

$C$DW$45	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$45, DW_AT_type(*$C$DW$T$569)

	.dwendtag $C$DW$40


$C$DW$46	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$46, DW_AT_name("sl_SetSockOpt")
	.dwattr $C$DW$46, DW_AT_TI_symbol_name("sl_SetSockOpt")
	.dwattr $C$DW$46, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$46, DW_AT_declaration
	.dwattr $C$DW$46, DW_AT_external
	.dwattr $C$DW$46, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$46, DW_AT_decl_line(0x477)
	.dwattr $C$DW$46, DW_AT_decl_column(0x06)
$C$DW$47	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$47, DW_AT_type(*$C$DW$T$8)

$C$DW$48	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$48, DW_AT_type(*$C$DW$T$8)

$C$DW$49	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$49, DW_AT_type(*$C$DW$T$8)

$C$DW$50	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$50, DW_AT_type(*$C$DW$T$323)

$C$DW$51	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$51, DW_AT_type(*$C$DW$T$607)

	.dwendtag $C$DW$46


$C$DW$52	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$52, DW_AT_name("sl_GetSockOpt")
	.dwattr $C$DW$52, DW_AT_TI_symbol_name("sl_GetSockOpt")
	.dwattr $C$DW$52, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$52, DW_AT_declaration
	.dwattr $C$DW$52, DW_AT_external
	.dwattr $C$DW$52, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$52, DW_AT_decl_line(0x4a2)
	.dwattr $C$DW$52, DW_AT_decl_column(0x06)
$C$DW$53	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$53, DW_AT_type(*$C$DW$T$8)

$C$DW$54	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$54, DW_AT_type(*$C$DW$T$8)

$C$DW$55	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$55, DW_AT_type(*$C$DW$T$8)

$C$DW$56	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$56, DW_AT_type(*$C$DW$T$3)

$C$DW$57	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$57, DW_AT_type(*$C$DW$T$608)

	.dwendtag $C$DW$52


$C$DW$58	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$58, DW_AT_name("sl_Recv")
	.dwattr $C$DW$58, DW_AT_TI_symbol_name("sl_Recv")
	.dwattr $C$DW$58, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$58, DW_AT_declaration
	.dwattr $C$DW$58, DW_AT_external
	.dwattr $C$DW$58, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$58, DW_AT_decl_line(0x4e8)
	.dwattr $C$DW$58, DW_AT_decl_column(0x06)
$C$DW$59	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$59, DW_AT_type(*$C$DW$T$8)

$C$DW$60	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$60, DW_AT_type(*$C$DW$T$3)

$C$DW$61	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$61, DW_AT_type(*$C$DW$T$8)

$C$DW$62	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$62, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$58


$C$DW$63	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$63, DW_AT_name("sl_RecvFrom")
	.dwattr $C$DW$63, DW_AT_TI_symbol_name("sl_RecvFrom")
	.dwattr $C$DW$63, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$63, DW_AT_declaration
	.dwattr $C$DW$63, DW_AT_external
	.dwattr $C$DW$63, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$63, DW_AT_decl_line(0x524)
	.dwattr $C$DW$63, DW_AT_decl_column(0x06)
$C$DW$64	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$64, DW_AT_type(*$C$DW$T$8)

$C$DW$65	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$65, DW_AT_type(*$C$DW$T$3)

$C$DW$66	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$66, DW_AT_type(*$C$DW$T$8)

$C$DW$67	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$67, DW_AT_type(*$C$DW$T$8)

$C$DW$68	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$68, DW_AT_type(*$C$DW$T$567)

$C$DW$69	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$69, DW_AT_type(*$C$DW$T$608)

	.dwendtag $C$DW$63


$C$DW$70	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$70, DW_AT_name("sl_Send")
	.dwattr $C$DW$70, DW_AT_TI_symbol_name("sl_Send")
	.dwattr $C$DW$70, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$70, DW_AT_declaration
	.dwattr $C$DW$70, DW_AT_external
	.dwattr $C$DW$70, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$70, DW_AT_decl_line(0x555)
	.dwattr $C$DW$70, DW_AT_decl_column(0x06)
$C$DW$71	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$71, DW_AT_type(*$C$DW$T$8)

$C$DW$72	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$72, DW_AT_type(*$C$DW$T$323)

$C$DW$73	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$73, DW_AT_type(*$C$DW$T$8)

$C$DW$74	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$74, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$70


$C$DW$75	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$75, DW_AT_name("sl_SendTo")
	.dwattr $C$DW$75, DW_AT_TI_symbol_name("sl_SendTo")
	.dwattr $C$DW$75, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$75, DW_AT_declaration
	.dwattr $C$DW$75, DW_AT_external
	.dwattr $C$DW$75, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$75, DW_AT_decl_line(0x58a)
	.dwattr $C$DW$75, DW_AT_decl_column(0x06)
$C$DW$76	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$76, DW_AT_type(*$C$DW$T$8)

$C$DW$77	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$77, DW_AT_type(*$C$DW$T$323)

$C$DW$78	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$78, DW_AT_type(*$C$DW$T$8)

$C$DW$79	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$79, DW_AT_type(*$C$DW$T$8)

$C$DW$80	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$80, DW_AT_type(*$C$DW$T$566)

$C$DW$81	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$81, DW_AT_type(*$C$DW$T$607)

	.dwendtag $C$DW$75


$C$DW$82	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$82, DW_AT_name("sl_StartTLS")
	.dwattr $C$DW$82, DW_AT_TI_symbol_name("sl_StartTLS")
	.dwattr $C$DW$82, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$82, DW_AT_declaration
	.dwattr $C$DW$82, DW_AT_external
	.dwattr $C$DW$82, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$82, DW_AT_decl_line(0x59d)
	.dwattr $C$DW$82, DW_AT_decl_column(0x06)
$C$DW$83	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$83, DW_AT_type(*$C$DW$T$8)

	.dwendtag $C$DW$82


$C$DW$84	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$84, DW_AT_name("sl_NetAppDnsGetHostByName")
	.dwattr $C$DW$84, DW_AT_TI_symbol_name("sl_NetAppDnsGetHostByName")
	.dwattr $C$DW$84, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$84, DW_AT_declaration
	.dwattr $C$DW$84, DW_AT_external
	.dwattr $C$DW$84, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$84, DW_AT_decl_line(0x29a)
	.dwattr $C$DW$84, DW_AT_decl_column(0x06)
$C$DW$85	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$85, DW_AT_type(*$C$DW$T$130)

$C$DW$86	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$86, DW_AT_type(*$C$DW$T$629)

$C$DW$87	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$87, DW_AT_type(*$C$DW$T$630)

$C$DW$88	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$88, DW_AT_type(*$C$DW$T$591)

	.dwendtag $C$DW$84


$C$DW$89	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$89, DW_AT_name("sl_WlanGet")
	.dwattr $C$DW$89, DW_AT_TI_symbol_name("sl_WlanGet")
	.dwattr $C$DW$89, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$89, DW_AT_declaration
	.dwattr $C$DW$89, DW_AT_external
	.dwattr $C$DW$89, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$89, DW_AT_decl_line(0x8a2)
	.dwattr $C$DW$89, DW_AT_decl_column(0x06)
$C$DW$90	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$90, DW_AT_type(*$C$DW$T$629)

$C$DW$91	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$91, DW_AT_type(*$C$DW$T$633)

$C$DW$92	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$92, DW_AT_type(*$C$DW$T$633)

$C$DW$93	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$93, DW_AT_type(*$C$DW$T$84)

	.dwendtag $C$DW$89


$C$DW$94	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$94, DW_AT_name("sl_NetCfgGet")
	.dwattr $C$DW$94, DW_AT_TI_symbol_name("sl_NetCfgGet")
	.dwattr $C$DW$94, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$94, DW_AT_declaration
	.dwattr $C$DW$94, DW_AT_external
	.dwattr $C$DW$94, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$94, DW_AT_decl_line(0x283)
	.dwattr $C$DW$94, DW_AT_decl_column(0x06)
$C$DW$95	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$95, DW_AT_type(*$C$DW$T$629)

$C$DW$96	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$96, DW_AT_type(*$C$DW$T$633)

$C$DW$97	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$97, DW_AT_type(*$C$DW$T$633)

$C$DW$98	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$98, DW_AT_type(*$C$DW$T$84)

	.dwendtag $C$DW$94

	.global	SlNetIfConfigWifi
	.sect	".data:SlNetIfConfigWifi", RW
	.clink
	.align	4
	.elfsym	SlNetIfConfigWifi,SYM_SIZE(88)
SlNetIfConfigWifi:
	.bits	SlNetIfWifi_socket,32		; SlNetIfConfigWifi.sockCreate @ 0
	.bits	SlNetIfWifi_close,32		; SlNetIfConfigWifi.sockClose @ 32
	.bits	0,32			; SlNetIfConfigWifi.sockShutdown @ 64
	.bits	SlNetIfWifi_accept,32		; SlNetIfConfigWifi.sockAccept @ 96
	.bits	SlNetIfWifi_bind,32		; SlNetIfConfigWifi.sockBind @ 128
	.bits	SlNetIfWifi_listen,32		; SlNetIfConfigWifi.sockListen @ 160
	.bits	SlNetIfWifi_connect,32		; SlNetIfConfigWifi.sockConnect @ 192
	.bits	0,32			; SlNetIfConfigWifi.sockGetPeerName @ 224
	.bits	0,32			; SlNetIfConfigWifi.sockGetLocalName @ 256
	.bits	SlNetIfWifi_select,32		; SlNetIfConfigWifi.sockSelect @ 288
	.bits	SlNetIfWifi_setSockOpt,32		; SlNetIfConfigWifi.sockSetOpt @ 320
	.bits	SlNetIfWifi_getSockOpt,32		; SlNetIfConfigWifi.sockGetOpt @ 352
	.bits	SlNetIfWifi_recv,32		; SlNetIfConfigWifi.sockRecv @ 384
	.bits	SlNetIfWifi_recvFrom,32		; SlNetIfConfigWifi.sockRecvFrom @ 416
	.bits	SlNetIfWifi_send,32		; SlNetIfConfigWifi.sockSend @ 448
	.bits	SlNetIfWifi_sendTo,32		; SlNetIfConfigWifi.sockSendTo @ 480
	.bits	SlNetIfWifi_sockstartSec,32		; SlNetIfConfigWifi.sockstartSec @ 512
	.bits	SlNetIfWifi_getHostByName,32		; SlNetIfConfigWifi.utilGetHostByName @ 544
	.bits	SlNetIfWifi_getIPAddr,32		; SlNetIfConfigWifi.ifGetIPAddr @ 576
	.bits	SlNetIfWifi_getConnectionStatus,32		; SlNetIfConfigWifi.ifGetConnectionStatus @ 608
	.bits	SlNetIfWifi_loadSecObj,32		; SlNetIfConfigWifi.ifLoadSecObj @ 640
	.bits	0,32			; SlNetIfConfigWifi.ifCreateContext @ 672

$C$DW$99	.dwtag  DW_TAG_variable
	.dwattr $C$DW$99, DW_AT_name("SlNetIfConfigWifi")
	.dwattr $C$DW$99, DW_AT_TI_symbol_name("SlNetIfConfigWifi")
	.dwattr $C$DW$99, DW_AT_location[DW_OP_addr SlNetIfConfigWifi]
	.dwattr $C$DW$99, DW_AT_type(*$C$DW$T$355)
	.dwattr $C$DW$99, DW_AT_external
	.dwattr $C$DW$99, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$99, DW_AT_decl_line(0x40)
	.dwattr $C$DW$99, DW_AT_decl_column(0x12)

	.sect	".const:StartSecOptName"
	.clink
	.align	2
	.elfsym	StartSecOptName,SYM_SIZE(20)
StartSecOptName:
	.bits	30,16			; StartSecOptName[0] @ 0
	.bits	31,16			; StartSecOptName[1] @ 16
	.bits	32,16			; StartSecOptName[2] @ 32
	.bits	33,16			; StartSecOptName[3] @ 48
	.bits	25,16			; StartSecOptName[4] @ 64
	.bits	26,16			; StartSecOptName[5] @ 80
	.bits	40,16			; StartSecOptName[6] @ 96
	.bits	41,16			; StartSecOptName[7] @ 112
	.bits	42,16			; StartSecOptName[8] @ 128
	.bits	38,16			; StartSecOptName[9] @ 144

$C$DW$100	.dwtag  DW_TAG_variable
	.dwattr $C$DW$100, DW_AT_name("StartSecOptName")
	.dwattr $C$DW$100, DW_AT_TI_symbol_name("StartSecOptName")
	.dwattr $C$DW$100, DW_AT_type(*$C$DW$T$637)
	.dwattr $C$DW$100, DW_AT_location[DW_OP_addr StartSecOptName]
	.dwattr $C$DW$100, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$100, DW_AT_decl_line(0x5a)
	.dwattr $C$DW$100, DW_AT_decl_column(0x16)

	.sect	".const:socketType"
	.clink
	.align	2
	.elfsym	socketType,SYM_SIZE(16)
socketType:
	.bits	1,16			; socketType[0] @ 0
	.bits	2,16			; socketType[1] @ 16
	.bits	3,16			; socketType[2] @ 32
	.bits	4,16			; socketType[3] @ 48
	.bits	2,16			; socketType[4] @ 64
	.bits	3,16			; socketType[5] @ 80
	.bits	7,16			; socketType[6] @ 96
	.bits	8,16			; socketType[7] @ 112

$C$DW$101	.dwtag  DW_TAG_variable
	.dwattr $C$DW$101, DW_AT_name("socketType")
	.dwattr $C$DW$101, DW_AT_TI_symbol_name("socketType")
	.dwattr $C$DW$101, DW_AT_type(*$C$DW$T$638)
	.dwattr $C$DW$101, DW_AT_location[DW_OP_addr socketType]
	.dwattr $C$DW$101, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$101, DW_AT_decl_line(0x68)
	.dwattr $C$DW$101, DW_AT_decl_column(0x16)

;	/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armacpia -@/tmp/TI7JczJdM7L 
	.sect	".text:SlNetIfWifi_socket"
	.clink
	.thumbfunc SlNetIfWifi_socket
	.thumb
	.global	SlNetIfWifi_socket

$C$DW$102	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$102, DW_AT_name("SlNetIfWifi_socket")
	.dwattr $C$DW$102, DW_AT_low_pc(SlNetIfWifi_socket)
	.dwattr $C$DW$102, DW_AT_high_pc(0x00)
	.dwattr $C$DW$102, DW_AT_TI_symbol_name("SlNetIfWifi_socket")
	.dwattr $C$DW$102, DW_AT_external
	.dwattr $C$DW$102, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$102, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$102, DW_AT_TI_begin_line(0x7d)
	.dwattr $C$DW$102, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$102, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$102, DW_AT_decl_line(0x7d)
	.dwattr $C$DW$102, DW_AT_decl_column(0x09)
	.dwattr $C$DW$102, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 126,column 1,is_stmt,address SlNetIfWifi_socket,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_socket
$C$DW$103	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$103, DW_AT_name("ifContext")
	.dwattr $C$DW$103, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$103, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$103, DW_AT_location[DW_OP_reg0]

$C$DW$104	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$104, DW_AT_name("Domain")
	.dwattr $C$DW$104, DW_AT_TI_symbol_name("Domain")
	.dwattr $C$DW$104, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$104, DW_AT_location[DW_OP_reg1]

$C$DW$105	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$105, DW_AT_name("Type")
	.dwattr $C$DW$105, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$105, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$105, DW_AT_location[DW_OP_reg2]

$C$DW$106	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$106, DW_AT_name("Protocol")
	.dwattr $C$DW$106, DW_AT_TI_symbol_name("Protocol")
	.dwattr $C$DW$106, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$106, DW_AT_location[DW_OP_reg3]

$C$DW$107	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$107, DW_AT_name("sdContext")
	.dwattr $C$DW$107, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$107, DW_AT_type(*$C$DW$T$286)
	.dwattr $C$DW$107, DW_AT_location[DW_OP_breg13 16]

;----------------------------------------------------------------------
; 125 | int16_t SlNetIfWifi_socket(void *ifContext, int16_t Domain, int16_t Typ
;     | e, int16_t Protocol, void **sdContext)                                 
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_socket                                         *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 12 Auto + 4 Save = 16 byte                 *
;*****************************************************************************
SlNetIfWifi_socket:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A2, A3, A4, LR}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 3, -8
	.dwcfi	save_reg_to_mem, 2, -12
	.dwcfi	save_reg_to_mem, 1, -16
$C$DW$108	.dwtag  DW_TAG_variable
	.dwattr $C$DW$108, DW_AT_name("ifContext")
	.dwattr $C$DW$108, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$108, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$108, DW_AT_location[DW_OP_breg13 0]

$C$DW$109	.dwtag  DW_TAG_variable
	.dwattr $C$DW$109, DW_AT_name("Domain")
	.dwattr $C$DW$109, DW_AT_TI_symbol_name("Domain")
	.dwattr $C$DW$109, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$109, DW_AT_location[DW_OP_breg13 4]

$C$DW$110	.dwtag  DW_TAG_variable
	.dwattr $C$DW$110, DW_AT_name("Type")
	.dwattr $C$DW$110, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$110, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$110, DW_AT_location[DW_OP_breg13 6]

$C$DW$111	.dwtag  DW_TAG_variable
	.dwattr $C$DW$111, DW_AT_name("Protocol")
	.dwattr $C$DW$111, DW_AT_TI_symbol_name("Protocol")
	.dwattr $C$DW$111, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$111, DW_AT_location[DW_OP_breg13 8]

$C$DW$112	.dwtag  DW_TAG_variable
	.dwattr $C$DW$112, DW_AT_name("mappedSocketType")
	.dwattr $C$DW$112, DW_AT_TI_symbol_name("mappedSocketType")
	.dwattr $C$DW$112, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$112, DW_AT_location[DW_OP_breg13 10]

        STRH      A4, [SP, #8]          ; [DPU_3_PIPE] |126| 
        STRH      A3, [SP, #6]          ; [DPU_3_PIPE] |126| 
        STRH      A2, [SP, #4]          ; [DPU_3_PIPE] |126| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |126| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 128,column 30,is_stmt,isa 1
;----------------------------------------------------------------------
; 128 | int16_t mappedSocketType = socketType[Type - 1];                       
;----------------------------------------------------------------------
        LDR       A2, $C$CON1           ; [DPU_3_PIPE] |128| 
        LDRSH     A1, [SP, #6]          ; [DPU_3_PIPE] |128| 
        LSLS      A1, A1, #1            ; [DPU_3_PIPE] |128| 
        SUBS      A1, A1, #2            ; [DPU_3_PIPE] |128| 
        LDRH      A1, [A2, +A1]         ; [DPU_3_PIPE] |128| 
        STRH      A1, [SP, #10]         ; [DPU_3_PIPE] |128| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 129,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 129 | return (sl_Socket(Domain, mappedSocketType, Protocol));                
;----------------------------------------------------------------------
        LDRSH     A3, [SP, #8]          ; [DPU_3_PIPE] |129| 
        LDRSH     A1, [SP, #4]          ; [DPU_3_PIPE] |129| 
        LDRSH     A2, [SP, #10]         ; [DPU_3_PIPE] |129| 
$C$DW$113	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$113, DW_AT_low_pc(0x00)
	.dwattr $C$DW$113, DW_AT_name("sl_Socket")
	.dwattr $C$DW$113, DW_AT_TI_call

        BL        sl_Socket             ; [DPU_3_PIPE] |129| 
        ; CALL OCCURS {sl_Socket }       ; [] |129| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 130,column 1,is_stmt,isa 1
$C$DW$114	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$114, DW_AT_low_pc(0x00)
	.dwattr $C$DW$114, DW_AT_TI_return

        POP       {A2, A3, A4, PC}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$102, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$102, DW_AT_TI_end_line(0x82)
	.dwattr $C$DW$102, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$102

	.sect	".text:SlNetIfWifi_close"
	.clink
	.thumbfunc SlNetIfWifi_close
	.thumb
	.global	SlNetIfWifi_close

$C$DW$115	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$115, DW_AT_name("SlNetIfWifi_close")
	.dwattr $C$DW$115, DW_AT_low_pc(SlNetIfWifi_close)
	.dwattr $C$DW$115, DW_AT_high_pc(0x00)
	.dwattr $C$DW$115, DW_AT_TI_symbol_name("SlNetIfWifi_close")
	.dwattr $C$DW$115, DW_AT_external
	.dwattr $C$DW$115, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$115, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$115, DW_AT_TI_begin_line(0x8a)
	.dwattr $C$DW$115, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$115, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$115, DW_AT_decl_line(0x8a)
	.dwattr $C$DW$115, DW_AT_decl_column(0x09)
	.dwattr $C$DW$115, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 139,column 1,is_stmt,address SlNetIfWifi_close,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_close
$C$DW$116	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$116, DW_AT_name("sd")
	.dwattr $C$DW$116, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$116, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$116, DW_AT_location[DW_OP_reg0]

$C$DW$117	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$117, DW_AT_name("sdContext")
	.dwattr $C$DW$117, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$117, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$117, DW_AT_location[DW_OP_reg1]

;----------------------------------------------------------------------
; 138 | int32_t SlNetIfWifi_close(int16_t sd, void *sdContext)                 
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_close                                          *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 8 Auto + 4 Save = 12 byte                  *
;*****************************************************************************
SlNetIfWifi_close:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A2, A3, A4, LR}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 3, -8
	.dwcfi	save_reg_to_mem, 2, -12
	.dwcfi	save_reg_to_mem, 1, -16
$C$DW$118	.dwtag  DW_TAG_variable
	.dwattr $C$DW$118, DW_AT_name("sdContext")
	.dwattr $C$DW$118, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$118, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$118, DW_AT_location[DW_OP_breg13 0]

$C$DW$119	.dwtag  DW_TAG_variable
	.dwattr $C$DW$119, DW_AT_name("sd")
	.dwattr $C$DW$119, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$119, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$119, DW_AT_location[DW_OP_breg13 4]

        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |139| 
        STRH      A1, [SP, #4]          ; [DPU_3_PIPE] |139| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 141,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 141 | return sl_Close(sd);                                                   
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #4]          ; [DPU_3_PIPE] |141| 
$C$DW$120	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$120, DW_AT_low_pc(0x00)
	.dwattr $C$DW$120, DW_AT_name("sl_Close")
	.dwattr $C$DW$120, DW_AT_TI_call

        BL        sl_Close              ; [DPU_3_PIPE] |141| 
        ; CALL OCCURS {sl_Close }        ; [] |141| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 142,column 1,is_stmt,isa 1
$C$DW$121	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$121, DW_AT_low_pc(0x00)
	.dwattr $C$DW$121, DW_AT_TI_return

        POP       {A2, A3, A4, PC}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$115, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$115, DW_AT_TI_end_line(0x8e)
	.dwattr $C$DW$115, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$115

	.sect	".text:SlNetIfWifi_accept"
	.clink
	.thumbfunc SlNetIfWifi_accept
	.thumb
	.global	SlNetIfWifi_accept

$C$DW$122	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$122, DW_AT_name("SlNetIfWifi_accept")
	.dwattr $C$DW$122, DW_AT_low_pc(SlNetIfWifi_accept)
	.dwattr $C$DW$122, DW_AT_high_pc(0x00)
	.dwattr $C$DW$122, DW_AT_TI_symbol_name("SlNetIfWifi_accept")
	.dwattr $C$DW$122, DW_AT_external
	.dwattr $C$DW$122, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$122, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$122, DW_AT_TI_begin_line(0x96)
	.dwattr $C$DW$122, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$122, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$122, DW_AT_decl_line(0x96)
	.dwattr $C$DW$122, DW_AT_decl_column(0x09)
	.dwattr $C$DW$122, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 151,column 1,is_stmt,address SlNetIfWifi_accept,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_accept
$C$DW$123	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$123, DW_AT_name("sd")
	.dwattr $C$DW$123, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$123, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$123, DW_AT_location[DW_OP_reg0]

$C$DW$124	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$124, DW_AT_name("sdContext")
	.dwattr $C$DW$124, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$124, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$124, DW_AT_location[DW_OP_reg1]

$C$DW$125	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$125, DW_AT_name("addr")
	.dwattr $C$DW$125, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$125, DW_AT_type(*$C$DW$T$295)
	.dwattr $C$DW$125, DW_AT_location[DW_OP_reg2]

$C$DW$126	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$126, DW_AT_name("addrlen")
	.dwattr $C$DW$126, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$126, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$126, DW_AT_location[DW_OP_reg3]

$C$DW$127	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$127, DW_AT_name("flags")
	.dwattr $C$DW$127, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$127, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$127, DW_AT_location[DW_OP_breg13 24]

$C$DW$128	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$128, DW_AT_name("acceptedSdContext")
	.dwattr $C$DW$128, DW_AT_TI_symbol_name("acceptedSdContext")
	.dwattr $C$DW$128, DW_AT_type(*$C$DW$T$286)
	.dwattr $C$DW$128, DW_AT_location[DW_OP_breg13 28]

;----------------------------------------------------------------------
; 150 | int16_t SlNetIfWifi_accept(int16_t sd, void *sdContext, SlNetSock_Addr_
;     | t *addr, SlNetSocklen_t *addrlen, uint8_t flags, void **acceptedSdConte
;     | xt)                                                                    
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_accept                                         *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 16 Auto + 4 Save = 20 byte                 *
;*****************************************************************************
SlNetIfWifi_accept:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {LR}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
	.dwcfi	save_reg_to_mem, 14, -4
        SUB       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
$C$DW$129	.dwtag  DW_TAG_variable
	.dwattr $C$DW$129, DW_AT_name("sdContext")
	.dwattr $C$DW$129, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$129, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$129, DW_AT_location[DW_OP_breg13 0]

$C$DW$130	.dwtag  DW_TAG_variable
	.dwattr $C$DW$130, DW_AT_name("addr")
	.dwattr $C$DW$130, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$130, DW_AT_type(*$C$DW$T$295)
	.dwattr $C$DW$130, DW_AT_location[DW_OP_breg13 4]

$C$DW$131	.dwtag  DW_TAG_variable
	.dwattr $C$DW$131, DW_AT_name("addrlen")
	.dwattr $C$DW$131, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$131, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$131, DW_AT_location[DW_OP_breg13 8]

$C$DW$132	.dwtag  DW_TAG_variable
	.dwattr $C$DW$132, DW_AT_name("sd")
	.dwattr $C$DW$132, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$132, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$132, DW_AT_location[DW_OP_breg13 12]

        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |151| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |151| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |151| 
        STRH      A1, [SP, #12]         ; [DPU_3_PIPE] |151| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 152,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 152 | return sl_Accept(sd, (SlSockAddr_t *)addr, addrlen);                   
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #12]         ; [DPU_3_PIPE] |152| 
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |152| 
        LDR       A3, [SP, #8]          ; [DPU_3_PIPE] |152| 
$C$DW$133	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$133, DW_AT_low_pc(0x00)
	.dwattr $C$DW$133, DW_AT_name("sl_Accept")
	.dwattr $C$DW$133, DW_AT_TI_call

        BL        sl_Accept             ; [DPU_3_PIPE] |152| 
        ; CALL OCCURS {sl_Accept }       ; [] |152| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 153,column 1,is_stmt,isa 1
        ADD       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
$C$DW$134	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$134, DW_AT_low_pc(0x00)
	.dwattr $C$DW$134, DW_AT_TI_return

        POP       {PC}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$122, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$122, DW_AT_TI_end_line(0x99)
	.dwattr $C$DW$122, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$122

	.sect	".text:SlNetIfWifi_bind"
	.clink
	.thumbfunc SlNetIfWifi_bind
	.thumb
	.global	SlNetIfWifi_bind

$C$DW$135	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$135, DW_AT_name("SlNetIfWifi_bind")
	.dwattr $C$DW$135, DW_AT_low_pc(SlNetIfWifi_bind)
	.dwattr $C$DW$135, DW_AT_high_pc(0x00)
	.dwattr $C$DW$135, DW_AT_TI_symbol_name("SlNetIfWifi_bind")
	.dwattr $C$DW$135, DW_AT_external
	.dwattr $C$DW$135, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$135, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$135, DW_AT_TI_begin_line(0xa1)
	.dwattr $C$DW$135, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$135, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$135, DW_AT_decl_line(0xa1)
	.dwattr $C$DW$135, DW_AT_decl_column(0x09)
	.dwattr $C$DW$135, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 162,column 1,is_stmt,address SlNetIfWifi_bind,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_bind
$C$DW$136	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$136, DW_AT_name("sd")
	.dwattr $C$DW$136, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$136, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$136, DW_AT_location[DW_OP_reg0]

$C$DW$137	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$137, DW_AT_name("sdContext")
	.dwattr $C$DW$137, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$137, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$137, DW_AT_location[DW_OP_reg1]

$C$DW$138	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$138, DW_AT_name("addr")
	.dwattr $C$DW$138, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$138, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$138, DW_AT_location[DW_OP_reg2]

$C$DW$139	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$139, DW_AT_name("addrlen")
	.dwattr $C$DW$139, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$139, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$139, DW_AT_location[DW_OP_reg3]

;----------------------------------------------------------------------
; 161 | int32_t SlNetIfWifi_bind(int16_t sd, void *sdContext, const SlNetSock_A
;     | ddr_t *addr, int16_t addrlen)                                          
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_bind                                           *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 12 Auto + 4 Save = 16 byte                 *
;*****************************************************************************
SlNetIfWifi_bind:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A2, A3, A4, LR}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 3, -8
	.dwcfi	save_reg_to_mem, 2, -12
	.dwcfi	save_reg_to_mem, 1, -16
$C$DW$140	.dwtag  DW_TAG_variable
	.dwattr $C$DW$140, DW_AT_name("sdContext")
	.dwattr $C$DW$140, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$140, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$140, DW_AT_location[DW_OP_breg13 0]

$C$DW$141	.dwtag  DW_TAG_variable
	.dwattr $C$DW$141, DW_AT_name("addr")
	.dwattr $C$DW$141, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$141, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$141, DW_AT_location[DW_OP_breg13 4]

$C$DW$142	.dwtag  DW_TAG_variable
	.dwattr $C$DW$142, DW_AT_name("sd")
	.dwattr $C$DW$142, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$142, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$142, DW_AT_location[DW_OP_breg13 8]

$C$DW$143	.dwtag  DW_TAG_variable
	.dwattr $C$DW$143, DW_AT_name("addrlen")
	.dwattr $C$DW$143, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$143, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$143, DW_AT_location[DW_OP_breg13 10]

        STRH      A4, [SP, #10]         ; [DPU_3_PIPE] |162| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |162| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |162| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |162| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 163,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 163 | return sl_Bind(sd, (const SlSockAddr_t *)addr, addrlen);               
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #8]          ; [DPU_3_PIPE] |163| 
        LDRSH     A3, [SP, #10]         ; [DPU_3_PIPE] |163| 
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |163| 
$C$DW$144	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$144, DW_AT_low_pc(0x00)
	.dwattr $C$DW$144, DW_AT_name("sl_Bind")
	.dwattr $C$DW$144, DW_AT_TI_call

        BL        sl_Bind               ; [DPU_3_PIPE] |163| 
        ; CALL OCCURS {sl_Bind }         ; [] |163| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 164,column 1,is_stmt,isa 1
$C$DW$145	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$145, DW_AT_low_pc(0x00)
	.dwattr $C$DW$145, DW_AT_TI_return

        POP       {A2, A3, A4, PC}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$135, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$135, DW_AT_TI_end_line(0xa4)
	.dwattr $C$DW$135, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$135

	.sect	".text:SlNetIfWifi_listen"
	.clink
	.thumbfunc SlNetIfWifi_listen
	.thumb
	.global	SlNetIfWifi_listen

$C$DW$146	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$146, DW_AT_name("SlNetIfWifi_listen")
	.dwattr $C$DW$146, DW_AT_low_pc(SlNetIfWifi_listen)
	.dwattr $C$DW$146, DW_AT_high_pc(0x00)
	.dwattr $C$DW$146, DW_AT_TI_symbol_name("SlNetIfWifi_listen")
	.dwattr $C$DW$146, DW_AT_external
	.dwattr $C$DW$146, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$146, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$146, DW_AT_TI_begin_line(0xac)
	.dwattr $C$DW$146, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$146, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$146, DW_AT_decl_line(0xac)
	.dwattr $C$DW$146, DW_AT_decl_column(0x09)
	.dwattr $C$DW$146, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 173,column 1,is_stmt,address SlNetIfWifi_listen,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_listen
$C$DW$147	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$147, DW_AT_name("sd")
	.dwattr $C$DW$147, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$147, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$147, DW_AT_location[DW_OP_reg0]

$C$DW$148	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$148, DW_AT_name("sdContext")
	.dwattr $C$DW$148, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$148, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$148, DW_AT_location[DW_OP_reg1]

$C$DW$149	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$149, DW_AT_name("backlog")
	.dwattr $C$DW$149, DW_AT_TI_symbol_name("backlog")
	.dwattr $C$DW$149, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$149, DW_AT_location[DW_OP_reg2]

;----------------------------------------------------------------------
; 172 | int32_t SlNetIfWifi_listen(int16_t sd, void *sdContext, int16_t backlog
;     | )                                                                      
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_listen                                         *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 8 Auto + 4 Save = 12 byte                  *
;*****************************************************************************
SlNetIfWifi_listen:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A2, A3, A4, LR}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 3, -8
	.dwcfi	save_reg_to_mem, 2, -12
	.dwcfi	save_reg_to_mem, 1, -16
$C$DW$150	.dwtag  DW_TAG_variable
	.dwattr $C$DW$150, DW_AT_name("sdContext")
	.dwattr $C$DW$150, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$150, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$150, DW_AT_location[DW_OP_breg13 0]

$C$DW$151	.dwtag  DW_TAG_variable
	.dwattr $C$DW$151, DW_AT_name("sd")
	.dwattr $C$DW$151, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$151, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$151, DW_AT_location[DW_OP_breg13 4]

$C$DW$152	.dwtag  DW_TAG_variable
	.dwattr $C$DW$152, DW_AT_name("backlog")
	.dwattr $C$DW$152, DW_AT_TI_symbol_name("backlog")
	.dwattr $C$DW$152, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$152, DW_AT_location[DW_OP_breg13 6]

        STRH      A3, [SP, #6]          ; [DPU_3_PIPE] |173| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |173| 
        STRH      A1, [SP, #4]          ; [DPU_3_PIPE] |173| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 174,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 174 | return sl_Listen(sd, backlog);                                         
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #4]          ; [DPU_3_PIPE] |174| 
        LDRSH     A2, [SP, #6]          ; [DPU_3_PIPE] |174| 
$C$DW$153	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$153, DW_AT_low_pc(0x00)
	.dwattr $C$DW$153, DW_AT_name("sl_Listen")
	.dwattr $C$DW$153, DW_AT_TI_call

        BL        sl_Listen             ; [DPU_3_PIPE] |174| 
        ; CALL OCCURS {sl_Listen }       ; [] |174| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 175,column 1,is_stmt,isa 1
$C$DW$154	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$154, DW_AT_low_pc(0x00)
	.dwattr $C$DW$154, DW_AT_TI_return

        POP       {A2, A3, A4, PC}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$146, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$146, DW_AT_TI_end_line(0xaf)
	.dwattr $C$DW$146, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$146

	.sect	".text:SlNetIfWifi_connect"
	.clink
	.thumbfunc SlNetIfWifi_connect
	.thumb
	.global	SlNetIfWifi_connect

$C$DW$155	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$155, DW_AT_name("SlNetIfWifi_connect")
	.dwattr $C$DW$155, DW_AT_low_pc(SlNetIfWifi_connect)
	.dwattr $C$DW$155, DW_AT_high_pc(0x00)
	.dwattr $C$DW$155, DW_AT_TI_symbol_name("SlNetIfWifi_connect")
	.dwattr $C$DW$155, DW_AT_external
	.dwattr $C$DW$155, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$155, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$155, DW_AT_TI_begin_line(0xb7)
	.dwattr $C$DW$155, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$155, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$155, DW_AT_decl_line(0xb7)
	.dwattr $C$DW$155, DW_AT_decl_column(0x09)
	.dwattr $C$DW$155, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 184,column 1,is_stmt,address SlNetIfWifi_connect,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_connect
$C$DW$156	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$156, DW_AT_name("sd")
	.dwattr $C$DW$156, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$156, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$156, DW_AT_location[DW_OP_reg0]

$C$DW$157	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$157, DW_AT_name("sdContext")
	.dwattr $C$DW$157, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$157, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$157, DW_AT_location[DW_OP_reg1]

$C$DW$158	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$158, DW_AT_name("addr")
	.dwattr $C$DW$158, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$158, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$158, DW_AT_location[DW_OP_reg2]

$C$DW$159	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$159, DW_AT_name("addrlen")
	.dwattr $C$DW$159, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$159, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$159, DW_AT_location[DW_OP_reg3]

$C$DW$160	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$160, DW_AT_name("flags")
	.dwattr $C$DW$160, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$160, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$160, DW_AT_location[DW_OP_breg13 16]

;----------------------------------------------------------------------
; 183 | int32_t SlNetIfWifi_connect(int16_t sd, void *sdContext, const SlNetSoc
;     | k_Addr_t *addr, SlNetSocklen_t addrlen, uint8_t flags)                 
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_connect                                        *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 12 Auto + 4 Save = 16 byte                 *
;*****************************************************************************
SlNetIfWifi_connect:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A2, A3, A4, LR}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 3, -8
	.dwcfi	save_reg_to_mem, 2, -12
	.dwcfi	save_reg_to_mem, 1, -16
$C$DW$161	.dwtag  DW_TAG_variable
	.dwattr $C$DW$161, DW_AT_name("sdContext")
	.dwattr $C$DW$161, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$161, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$161, DW_AT_location[DW_OP_breg13 0]

$C$DW$162	.dwtag  DW_TAG_variable
	.dwattr $C$DW$162, DW_AT_name("addr")
	.dwattr $C$DW$162, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$162, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$162, DW_AT_location[DW_OP_breg13 4]

$C$DW$163	.dwtag  DW_TAG_variable
	.dwattr $C$DW$163, DW_AT_name("sd")
	.dwattr $C$DW$163, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$163, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$163, DW_AT_location[DW_OP_breg13 8]

$C$DW$164	.dwtag  DW_TAG_variable
	.dwattr $C$DW$164, DW_AT_name("addrlen")
	.dwattr $C$DW$164, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$164, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$164, DW_AT_location[DW_OP_breg13 10]

        STRH      A4, [SP, #10]         ; [DPU_3_PIPE] |184| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |184| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |184| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |184| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 185,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 185 | return sl_Connect(sd, (const SlSockAddr_t *)addr, addrlen);            
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #8]          ; [DPU_3_PIPE] |185| 
        LDRSH     A3, [SP, #10]         ; [DPU_3_PIPE] |185| 
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |185| 
$C$DW$165	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$165, DW_AT_low_pc(0x00)
	.dwattr $C$DW$165, DW_AT_name("sl_Connect")
	.dwattr $C$DW$165, DW_AT_TI_call

        BL        sl_Connect            ; [DPU_3_PIPE] |185| 
        ; CALL OCCURS {sl_Connect }      ; [] |185| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 186,column 1,is_stmt,isa 1
$C$DW$166	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$166, DW_AT_low_pc(0x00)
	.dwattr $C$DW$166, DW_AT_TI_return

        POP       {A2, A3, A4, PC}      ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$155, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$155, DW_AT_TI_end_line(0xba)
	.dwattr $C$DW$155, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$155

	.sect	".text:SlNetIfWifi_getSockName"
	.clink
	.thumbfunc SlNetIfWifi_getSockName
	.thumb
	.global	SlNetIfWifi_getSockName

$C$DW$167	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$167, DW_AT_name("SlNetIfWifi_getSockName")
	.dwattr $C$DW$167, DW_AT_low_pc(SlNetIfWifi_getSockName)
	.dwattr $C$DW$167, DW_AT_high_pc(0x00)
	.dwattr $C$DW$167, DW_AT_TI_symbol_name("SlNetIfWifi_getSockName")
	.dwattr $C$DW$167, DW_AT_external
	.dwattr $C$DW$167, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$167, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$167, DW_AT_TI_begin_line(0xc3)
	.dwattr $C$DW$167, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$167, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$167, DW_AT_decl_line(0xc3)
	.dwattr $C$DW$167, DW_AT_decl_column(0x09)
	.dwattr $C$DW$167, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 196,column 1,is_stmt,address SlNetIfWifi_getSockName,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_getSockName
$C$DW$168	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$168, DW_AT_name("sd")
	.dwattr $C$DW$168, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$168, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$168, DW_AT_location[DW_OP_reg0]

$C$DW$169	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$169, DW_AT_name("sdContext")
	.dwattr $C$DW$169, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$169, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$169, DW_AT_location[DW_OP_reg1]

$C$DW$170	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$170, DW_AT_name("addr")
	.dwattr $C$DW$170, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$170, DW_AT_type(*$C$DW$T$295)
	.dwattr $C$DW$170, DW_AT_location[DW_OP_reg2]

$C$DW$171	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$171, DW_AT_name("addrlen")
	.dwattr $C$DW$171, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$171, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$171, DW_AT_location[DW_OP_reg3]

;----------------------------------------------------------------------
; 195 | int32_t SlNetIfWifi_getSockName(int16_t sd, void *sdContext, SlNetSock_
;     | Addr_t *addr, SlNetSocklen_t *addrlen)                                 
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_getSockName                                    *
;*                                                                           *
;*   Regs Modified     : A1,SP                                               *
;*   Regs Used         : A1,A2,A3,A4,SP,LR                                   *
;*   Local Frame Size  : 0 Args + 16 Auto + 0 Save = 16 byte                 *
;*****************************************************************************
SlNetIfWifi_getSockName:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        SUB       SP, SP, #16           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
$C$DW$172	.dwtag  DW_TAG_variable
	.dwattr $C$DW$172, DW_AT_name("sdContext")
	.dwattr $C$DW$172, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$172, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$172, DW_AT_location[DW_OP_breg13 0]

$C$DW$173	.dwtag  DW_TAG_variable
	.dwattr $C$DW$173, DW_AT_name("addr")
	.dwattr $C$DW$173, DW_AT_TI_symbol_name("addr")
	.dwattr $C$DW$173, DW_AT_type(*$C$DW$T$295)
	.dwattr $C$DW$173, DW_AT_location[DW_OP_breg13 4]

$C$DW$174	.dwtag  DW_TAG_variable
	.dwattr $C$DW$174, DW_AT_name("addrlen")
	.dwattr $C$DW$174, DW_AT_TI_symbol_name("addrlen")
	.dwattr $C$DW$174, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$174, DW_AT_location[DW_OP_breg13 8]

$C$DW$175	.dwtag  DW_TAG_variable
	.dwattr $C$DW$175, DW_AT_name("sd")
	.dwattr $C$DW$175, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$175, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$175, DW_AT_location[DW_OP_breg13 12]

;----------------------------------------------------------------------
; 197 | // Not implemented in NWP                                              
;----------------------------------------------------------------------
        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |196| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |196| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |196| 
        STRH      A1, [SP, #12]         ; [DPU_3_PIPE] |196| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 198,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 198 | return SLNETERR_INVALPARAM;                                            
;----------------------------------------------------------------------
        LDR       A1, $C$CON2           ; [DPU_3_PIPE] |198| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 199,column 1,is_stmt,isa 1
        ADD       SP, SP, #16           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
$C$DW$176	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$176, DW_AT_low_pc(0x00)
	.dwattr $C$DW$176, DW_AT_TI_return

        BX        LR                    ; [DPU_3_PIPE] 
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$167, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$167, DW_AT_TI_end_line(0xc7)
	.dwattr $C$DW$167, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$167

	.sect	".text:SlNetIfWifi_select"
	.clink
	.thumbfunc SlNetIfWifi_select
	.thumb
	.global	SlNetIfWifi_select

$C$DW$177	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$177, DW_AT_name("SlNetIfWifi_select")
	.dwattr $C$DW$177, DW_AT_low_pc(SlNetIfWifi_select)
	.dwattr $C$DW$177, DW_AT_high_pc(0x00)
	.dwattr $C$DW$177, DW_AT_TI_symbol_name("SlNetIfWifi_select")
	.dwattr $C$DW$177, DW_AT_external
	.dwattr $C$DW$177, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$177, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$177, DW_AT_TI_begin_line(0xcf)
	.dwattr $C$DW$177, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$177, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$177, DW_AT_decl_line(0xcf)
	.dwattr $C$DW$177, DW_AT_decl_column(0x09)
	.dwattr $C$DW$177, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 208,column 1,is_stmt,address SlNetIfWifi_select,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_select
$C$DW$178	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$178, DW_AT_name("ifContext")
	.dwattr $C$DW$178, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$178, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$178, DW_AT_location[DW_OP_reg0]

$C$DW$179	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$179, DW_AT_name("nfds")
	.dwattr $C$DW$179, DW_AT_TI_symbol_name("nfds")
	.dwattr $C$DW$179, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$179, DW_AT_location[DW_OP_reg1]

$C$DW$180	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$180, DW_AT_name("readsds")
	.dwattr $C$DW$180, DW_AT_TI_symbol_name("readsds")
	.dwattr $C$DW$180, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$180, DW_AT_location[DW_OP_reg2]

$C$DW$181	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$181, DW_AT_name("writesds")
	.dwattr $C$DW$181, DW_AT_TI_symbol_name("writesds")
	.dwattr $C$DW$181, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$181, DW_AT_location[DW_OP_reg3]

$C$DW$182	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$182, DW_AT_name("exceptsds")
	.dwattr $C$DW$182, DW_AT_TI_symbol_name("exceptsds")
	.dwattr $C$DW$182, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$182, DW_AT_location[DW_OP_breg13 24]

$C$DW$183	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$183, DW_AT_name("timeout")
	.dwattr $C$DW$183, DW_AT_TI_symbol_name("timeout")
	.dwattr $C$DW$183, DW_AT_type(*$C$DW$T$311)
	.dwattr $C$DW$183, DW_AT_location[DW_OP_breg13 28]

;----------------------------------------------------------------------
; 207 | int32_t SlNetIfWifi_select(void *ifContext, int16_t nfds, SlNetSock_SdS
;     | et_t *readsds, SlNetSock_SdSet_t *writesds, SlNetSock_SdSet_t *exceptsd
;     | s, SlNetSock_Timeval_t *timeout)                                       
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_select                                         *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 4 Args + 16 Auto + 4 Save = 24 byte                 *
;*****************************************************************************
SlNetIfWifi_select:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {LR}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
	.dwcfi	save_reg_to_mem, 14, -4
        SUB       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
$C$DW$184	.dwtag  DW_TAG_variable
	.dwattr $C$DW$184, DW_AT_name("ifContext")
	.dwattr $C$DW$184, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$184, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$184, DW_AT_location[DW_OP_breg13 4]

$C$DW$185	.dwtag  DW_TAG_variable
	.dwattr $C$DW$185, DW_AT_name("readsds")
	.dwattr $C$DW$185, DW_AT_TI_symbol_name("readsds")
	.dwattr $C$DW$185, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$185, DW_AT_location[DW_OP_breg13 8]

$C$DW$186	.dwtag  DW_TAG_variable
	.dwattr $C$DW$186, DW_AT_name("writesds")
	.dwattr $C$DW$186, DW_AT_TI_symbol_name("writesds")
	.dwattr $C$DW$186, DW_AT_type(*$C$DW$T$309)
	.dwattr $C$DW$186, DW_AT_location[DW_OP_breg13 12]

$C$DW$187	.dwtag  DW_TAG_variable
	.dwattr $C$DW$187, DW_AT_name("nfds")
	.dwattr $C$DW$187, DW_AT_TI_symbol_name("nfds")
	.dwattr $C$DW$187, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$187, DW_AT_location[DW_OP_breg13 16]

        STR       A4, [SP, #12]         ; [DPU_3_PIPE] |208| 
        STR       A3, [SP, #8]          ; [DPU_3_PIPE] |208| 
        STRH      A2, [SP, #16]         ; [DPU_3_PIPE] |208| 
        STR       A1, [SP, #4]          ; [DPU_3_PIPE] |208| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 209,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 209 | return sl_Select(nfds, (SlFdSet_t *)readsds, (SlFdSet_t *)writesds, (Sl
;     | FdSet_t *)exceptsds, (struct SlTimeval_t *)timeout);                   
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |209| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |209| 
        LDR       A4, [SP, #24]         ; [DPU_3_PIPE] |209| 
        LDR       A2, [SP, #8]          ; [DPU_3_PIPE] |209| 
        LDR       A3, [SP, #12]         ; [DPU_3_PIPE] |209| 
        LDRSH     A1, [SP, #16]         ; [DPU_3_PIPE] |209| 
$C$DW$188	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$188, DW_AT_low_pc(0x00)
	.dwattr $C$DW$188, DW_AT_name("sl_Select")
	.dwattr $C$DW$188, DW_AT_TI_call

        BL        sl_Select             ; [DPU_3_PIPE] |209| 
        ; CALL OCCURS {sl_Select }       ; [] |209| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 210,column 1,is_stmt,isa 1
        ADD       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
$C$DW$189	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$189, DW_AT_low_pc(0x00)
	.dwattr $C$DW$189, DW_AT_TI_return

        POP       {PC}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$177, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$177, DW_AT_TI_end_line(0xd2)
	.dwattr $C$DW$177, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$177

	.sect	".text:SlNetIfWifi_setSockOpt"
	.clink
	.thumbfunc SlNetIfWifi_setSockOpt
	.thumb
	.global	SlNetIfWifi_setSockOpt

$C$DW$190	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$190, DW_AT_name("SlNetIfWifi_setSockOpt")
	.dwattr $C$DW$190, DW_AT_low_pc(SlNetIfWifi_setSockOpt)
	.dwattr $C$DW$190, DW_AT_high_pc(0x00)
	.dwattr $C$DW$190, DW_AT_TI_symbol_name("SlNetIfWifi_setSockOpt")
	.dwattr $C$DW$190, DW_AT_external
	.dwattr $C$DW$190, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$190, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$190, DW_AT_TI_begin_line(0xda)
	.dwattr $C$DW$190, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$190, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$190, DW_AT_decl_line(0xda)
	.dwattr $C$DW$190, DW_AT_decl_column(0x09)
	.dwattr $C$DW$190, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 219,column 1,is_stmt,address SlNetIfWifi_setSockOpt,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_setSockOpt
$C$DW$191	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$191, DW_AT_name("sd")
	.dwattr $C$DW$191, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$191, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$191, DW_AT_location[DW_OP_reg0]

$C$DW$192	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$192, DW_AT_name("sdContext")
	.dwattr $C$DW$192, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$192, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$192, DW_AT_location[DW_OP_reg1]

$C$DW$193	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$193, DW_AT_name("level")
	.dwattr $C$DW$193, DW_AT_TI_symbol_name("level")
	.dwattr $C$DW$193, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$193, DW_AT_location[DW_OP_reg2]

$C$DW$194	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$194, DW_AT_name("optname")
	.dwattr $C$DW$194, DW_AT_TI_symbol_name("optname")
	.dwattr $C$DW$194, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$194, DW_AT_location[DW_OP_reg3]

$C$DW$195	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$195, DW_AT_name("optval")
	.dwattr $C$DW$195, DW_AT_TI_symbol_name("optval")
	.dwattr $C$DW$195, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$195, DW_AT_location[DW_OP_breg13 24]

$C$DW$196	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$196, DW_AT_name("optlen")
	.dwattr $C$DW$196, DW_AT_TI_symbol_name("optlen")
	.dwattr $C$DW$196, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$196, DW_AT_location[DW_OP_breg13 28]

;----------------------------------------------------------------------
; 218 | int32_t SlNetIfWifi_setSockOpt(int16_t sd, void *sdContext, int16_t lev
;     | el, int16_t optname, void *optval, SlNetSocklen_t optlen)              
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_setSockOpt                                     *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 4 Args + 12 Auto + 8 Save = 24 byte                 *
;*****************************************************************************
SlNetIfWifi_setSockOpt:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A1, A2, A3, A4, V4, LR} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
	.dwcfi	save_reg_to_mem, 3, -12
	.dwcfi	save_reg_to_mem, 2, -16
	.dwcfi	save_reg_to_mem, 1, -20
	.dwcfi	save_reg_to_mem, 0, -24
        ADD       V4, SP, #24           ; [DPU_3_PIPE] 
$C$DW$197	.dwtag  DW_TAG_variable
	.dwattr $C$DW$197, DW_AT_name("sdContext")
	.dwattr $C$DW$197, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$197, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$197, DW_AT_location[DW_OP_breg13 4]

$C$DW$198	.dwtag  DW_TAG_variable
	.dwattr $C$DW$198, DW_AT_name("sd")
	.dwattr $C$DW$198, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$198, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$198, DW_AT_location[DW_OP_breg13 8]

$C$DW$199	.dwtag  DW_TAG_variable
	.dwattr $C$DW$199, DW_AT_name("level")
	.dwattr $C$DW$199, DW_AT_TI_symbol_name("level")
	.dwattr $C$DW$199, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$199, DW_AT_location[DW_OP_breg13 10]

$C$DW$200	.dwtag  DW_TAG_variable
	.dwattr $C$DW$200, DW_AT_name("optname")
	.dwattr $C$DW$200, DW_AT_TI_symbol_name("optname")
	.dwattr $C$DW$200, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$200, DW_AT_location[DW_OP_breg13 12]

        STRH      A4, [SP, #12]         ; [DPU_3_PIPE] |219| 
        STRH      A3, [SP, #10]         ; [DPU_3_PIPE] |219| 
        STR       A2, [SP, #4]          ; [DPU_3_PIPE] |219| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |219| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 220,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 220 | return sl_SetSockOpt(sd, level, optname, optval, optlen);              
;----------------------------------------------------------------------
        LDRH      A1, [V4, #4]          ; [DPU_3_PIPE] |220| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |220| 
        LDR       A4, [V4, #0]          ; [DPU_3_PIPE] |220| 
        LDRSH     A2, [SP, #10]         ; [DPU_3_PIPE] |220| 
        LDRSH     A3, [SP, #12]         ; [DPU_3_PIPE] |220| 
        LDRSH     A1, [SP, #8]          ; [DPU_3_PIPE] |220| 
$C$DW$201	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$201, DW_AT_low_pc(0x00)
	.dwattr $C$DW$201, DW_AT_name("sl_SetSockOpt")
	.dwattr $C$DW$201, DW_AT_TI_call

        BL        sl_SetSockOpt         ; [DPU_3_PIPE] |220| 
        ; CALL OCCURS {sl_SetSockOpt }   ; [] |220| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 221,column 1,is_stmt,isa 1
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] 
$C$DW$202	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$202, DW_AT_low_pc(0x00)
	.dwattr $C$DW$202, DW_AT_TI_return

        POP       {A1, A2, A3, A4, V4, PC} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
	.dwcfi	restore_reg, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$190, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$190, DW_AT_TI_end_line(0xdd)
	.dwattr $C$DW$190, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$190

	.sect	".text:SlNetIfWifi_getSockOpt"
	.clink
	.thumbfunc SlNetIfWifi_getSockOpt
	.thumb
	.global	SlNetIfWifi_getSockOpt

$C$DW$203	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$203, DW_AT_name("SlNetIfWifi_getSockOpt")
	.dwattr $C$DW$203, DW_AT_low_pc(SlNetIfWifi_getSockOpt)
	.dwattr $C$DW$203, DW_AT_high_pc(0x00)
	.dwattr $C$DW$203, DW_AT_TI_symbol_name("SlNetIfWifi_getSockOpt")
	.dwattr $C$DW$203, DW_AT_external
	.dwattr $C$DW$203, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$203, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$203, DW_AT_TI_begin_line(0xe5)
	.dwattr $C$DW$203, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$203, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$203, DW_AT_decl_line(0xe5)
	.dwattr $C$DW$203, DW_AT_decl_column(0x09)
	.dwattr $C$DW$203, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 230,column 1,is_stmt,address SlNetIfWifi_getSockOpt,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_getSockOpt
$C$DW$204	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$204, DW_AT_name("sd")
	.dwattr $C$DW$204, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$204, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$204, DW_AT_location[DW_OP_reg0]

$C$DW$205	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$205, DW_AT_name("sdContext")
	.dwattr $C$DW$205, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$205, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$205, DW_AT_location[DW_OP_reg1]

$C$DW$206	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$206, DW_AT_name("level")
	.dwattr $C$DW$206, DW_AT_TI_symbol_name("level")
	.dwattr $C$DW$206, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$206, DW_AT_location[DW_OP_reg2]

$C$DW$207	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$207, DW_AT_name("optname")
	.dwattr $C$DW$207, DW_AT_TI_symbol_name("optname")
	.dwattr $C$DW$207, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$207, DW_AT_location[DW_OP_reg3]

$C$DW$208	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$208, DW_AT_name("optval")
	.dwattr $C$DW$208, DW_AT_TI_symbol_name("optval")
	.dwattr $C$DW$208, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$208, DW_AT_location[DW_OP_breg13 24]

$C$DW$209	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$209, DW_AT_name("optlen")
	.dwattr $C$DW$209, DW_AT_TI_symbol_name("optlen")
	.dwattr $C$DW$209, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$209, DW_AT_location[DW_OP_breg13 28]

;----------------------------------------------------------------------
; 229 | int32_t SlNetIfWifi_getSockOpt(int16_t sd, void *sdContext, int16_t lev
;     | el, int16_t optname, void *optval, SlNetSocklen_t *optlen)             
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_getSockOpt                                     *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 4 Args + 12 Auto + 4 Save = 20 byte                 *
;*****************************************************************************
SlNetIfWifi_getSockOpt:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {LR}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
	.dwcfi	save_reg_to_mem, 14, -4
        SUB       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
$C$DW$210	.dwtag  DW_TAG_variable
	.dwattr $C$DW$210, DW_AT_name("sdContext")
	.dwattr $C$DW$210, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$210, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$210, DW_AT_location[DW_OP_breg13 4]

$C$DW$211	.dwtag  DW_TAG_variable
	.dwattr $C$DW$211, DW_AT_name("sd")
	.dwattr $C$DW$211, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$211, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$211, DW_AT_location[DW_OP_breg13 8]

$C$DW$212	.dwtag  DW_TAG_variable
	.dwattr $C$DW$212, DW_AT_name("level")
	.dwattr $C$DW$212, DW_AT_TI_symbol_name("level")
	.dwattr $C$DW$212, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$212, DW_AT_location[DW_OP_breg13 10]

$C$DW$213	.dwtag  DW_TAG_variable
	.dwattr $C$DW$213, DW_AT_name("optname")
	.dwattr $C$DW$213, DW_AT_TI_symbol_name("optname")
	.dwattr $C$DW$213, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$213, DW_AT_location[DW_OP_breg13 12]

        STRH      A4, [SP, #12]         ; [DPU_3_PIPE] |230| 
        STRH      A3, [SP, #10]         ; [DPU_3_PIPE] |230| 
        STR       A2, [SP, #4]          ; [DPU_3_PIPE] |230| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |230| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 231,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 231 | return sl_GetSockOpt(sd, level, optname, optval, optlen);              
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |231| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |231| 
        LDR       A4, [SP, #24]         ; [DPU_3_PIPE] |231| 
        LDRSH     A2, [SP, #10]         ; [DPU_3_PIPE] |231| 
        LDRSH     A3, [SP, #12]         ; [DPU_3_PIPE] |231| 
        LDRSH     A1, [SP, #8]          ; [DPU_3_PIPE] |231| 
$C$DW$214	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$214, DW_AT_low_pc(0x00)
	.dwattr $C$DW$214, DW_AT_name("sl_GetSockOpt")
	.dwattr $C$DW$214, DW_AT_TI_call

        BL        sl_GetSockOpt         ; [DPU_3_PIPE] |231| 
        ; CALL OCCURS {sl_GetSockOpt }   ; [] |231| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 232,column 1,is_stmt,isa 1
        ADD       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
$C$DW$215	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$215, DW_AT_low_pc(0x00)
	.dwattr $C$DW$215, DW_AT_TI_return

        POP       {PC}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$203, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$203, DW_AT_TI_end_line(0xe8)
	.dwattr $C$DW$203, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$203

	.sect	".text:SlNetIfWifi_recv"
	.clink
	.thumbfunc SlNetIfWifi_recv
	.thumb
	.global	SlNetIfWifi_recv

$C$DW$216	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$216, DW_AT_name("SlNetIfWifi_recv")
	.dwattr $C$DW$216, DW_AT_low_pc(SlNetIfWifi_recv)
	.dwattr $C$DW$216, DW_AT_high_pc(0x00)
	.dwattr $C$DW$216, DW_AT_TI_symbol_name("SlNetIfWifi_recv")
	.dwattr $C$DW$216, DW_AT_external
	.dwattr $C$DW$216, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$216, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$216, DW_AT_TI_begin_line(0xf0)
	.dwattr $C$DW$216, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$216, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$216, DW_AT_decl_line(0xf0)
	.dwattr $C$DW$216, DW_AT_decl_column(0x09)
	.dwattr $C$DW$216, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 241,column 1,is_stmt,address SlNetIfWifi_recv,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_recv
$C$DW$217	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$217, DW_AT_name("sd")
	.dwattr $C$DW$217, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$217, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$217, DW_AT_location[DW_OP_reg0]

$C$DW$218	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$218, DW_AT_name("sdContext")
	.dwattr $C$DW$218, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$218, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$218, DW_AT_location[DW_OP_reg1]

$C$DW$219	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$219, DW_AT_name("buf")
	.dwattr $C$DW$219, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$219, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$219, DW_AT_location[DW_OP_reg2]

$C$DW$220	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$220, DW_AT_name("len")
	.dwattr $C$DW$220, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$220, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$220, DW_AT_location[DW_OP_reg3]

$C$DW$221	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$221, DW_AT_name("flags")
	.dwattr $C$DW$221, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$221, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$221, DW_AT_location[DW_OP_breg13 24]

;----------------------------------------------------------------------
; 240 | int32_t SlNetIfWifi_recv(int16_t sd, void *sdContext, void *buf, uint32
;     | _t len, uint32_t flags)                                                
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_recv                                           *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 0 Args + 16 Auto + 8 Save = 24 byte                 *
;*****************************************************************************
SlNetIfWifi_recv:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A1, A2, A3, A4, V4, LR} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
	.dwcfi	save_reg_to_mem, 3, -12
	.dwcfi	save_reg_to_mem, 2, -16
	.dwcfi	save_reg_to_mem, 1, -20
	.dwcfi	save_reg_to_mem, 0, -24
        ADD       V4, SP, #24           ; [DPU_3_PIPE] 
$C$DW$222	.dwtag  DW_TAG_variable
	.dwattr $C$DW$222, DW_AT_name("sdContext")
	.dwattr $C$DW$222, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$222, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$222, DW_AT_location[DW_OP_breg13 0]

$C$DW$223	.dwtag  DW_TAG_variable
	.dwattr $C$DW$223, DW_AT_name("buf")
	.dwattr $C$DW$223, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$223, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$223, DW_AT_location[DW_OP_breg13 4]

$C$DW$224	.dwtag  DW_TAG_variable
	.dwattr $C$DW$224, DW_AT_name("len")
	.dwattr $C$DW$224, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$224, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$224, DW_AT_location[DW_OP_breg13 8]

$C$DW$225	.dwtag  DW_TAG_variable
	.dwattr $C$DW$225, DW_AT_name("sd")
	.dwattr $C$DW$225, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$225, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$225, DW_AT_location[DW_OP_breg13 12]

        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |241| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |241| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |241| 
        STRH      A1, [SP, #12]         ; [DPU_3_PIPE] |241| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 242,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 242 | DISABLE_SEC_BITS_FROM_INPUT_FLAGS(flags);                              
;----------------------------------------------------------------------
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |242| 
        BIC       A1, A1, #-16777216    ; [DPU_3_PIPE] |242| 
        STR       A1, [V4, #0]          ; [DPU_3_PIPE] |242| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 243,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 243 | return sl_Recv(sd, buf, len, flags);                                   
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #12]         ; [DPU_3_PIPE] |243| 
        LDRSH     A3, [SP, #8]          ; [DPU_3_PIPE] |243| 
        LDRSH     A4, [V4, #0]          ; [DPU_3_PIPE] |243| 
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |243| 
$C$DW$226	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$226, DW_AT_low_pc(0x00)
	.dwattr $C$DW$226, DW_AT_name("sl_Recv")
	.dwattr $C$DW$226, DW_AT_TI_call

        BL        sl_Recv               ; [DPU_3_PIPE] |243| 
        ; CALL OCCURS {sl_Recv }         ; [] |243| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 244,column 1,is_stmt,isa 1
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] 
$C$DW$227	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$227, DW_AT_low_pc(0x00)
	.dwattr $C$DW$227, DW_AT_TI_return

        POP       {A1, A2, A3, A4, V4, PC} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
	.dwcfi	restore_reg, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$216, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$216, DW_AT_TI_end_line(0xf4)
	.dwattr $C$DW$216, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$216

	.sect	".text:SlNetIfWifi_recvFrom"
	.clink
	.thumbfunc SlNetIfWifi_recvFrom
	.thumb
	.global	SlNetIfWifi_recvFrom

$C$DW$228	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$228, DW_AT_name("SlNetIfWifi_recvFrom")
	.dwattr $C$DW$228, DW_AT_low_pc(SlNetIfWifi_recvFrom)
	.dwattr $C$DW$228, DW_AT_high_pc(0x00)
	.dwattr $C$DW$228, DW_AT_TI_symbol_name("SlNetIfWifi_recvFrom")
	.dwattr $C$DW$228, DW_AT_external
	.dwattr $C$DW$228, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$228, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$228, DW_AT_TI_begin_line(0xfc)
	.dwattr $C$DW$228, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$228, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$228, DW_AT_decl_line(0xfc)
	.dwattr $C$DW$228, DW_AT_decl_column(0x09)
	.dwattr $C$DW$228, DW_AT_TI_max_frame_size(0x20)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 253,column 1,is_stmt,address SlNetIfWifi_recvFrom,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_recvFrom
$C$DW$229	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$229, DW_AT_name("sd")
	.dwattr $C$DW$229, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$229, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$229, DW_AT_location[DW_OP_reg0]

$C$DW$230	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$230, DW_AT_name("sdContext")
	.dwattr $C$DW$230, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$230, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$230, DW_AT_location[DW_OP_reg1]

$C$DW$231	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$231, DW_AT_name("buf")
	.dwattr $C$DW$231, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$231, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$231, DW_AT_location[DW_OP_reg2]

$C$DW$232	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$232, DW_AT_name("len")
	.dwattr $C$DW$232, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$232, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$232, DW_AT_location[DW_OP_reg3]

$C$DW$233	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$233, DW_AT_name("flags")
	.dwattr $C$DW$233, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$233, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$233, DW_AT_location[DW_OP_breg13 32]

$C$DW$234	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$234, DW_AT_name("from")
	.dwattr $C$DW$234, DW_AT_TI_symbol_name("from")
	.dwattr $C$DW$234, DW_AT_type(*$C$DW$T$295)
	.dwattr $C$DW$234, DW_AT_location[DW_OP_breg13 36]

$C$DW$235	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$235, DW_AT_name("fromlen")
	.dwattr $C$DW$235, DW_AT_TI_symbol_name("fromlen")
	.dwattr $C$DW$235, DW_AT_type(*$C$DW$T$297)
	.dwattr $C$DW$235, DW_AT_location[DW_OP_breg13 40]

;----------------------------------------------------------------------
; 252 | int32_t SlNetIfWifi_recvFrom(int16_t sd, void *sdContext, void *buf, ui
;     | nt32_t len, uint32_t flags, SlNetSock_Addr_t *from, SlNetSocklen_t *fro
;     | mlen)                                                                  
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_recvFrom                                       *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 8 Args + 16 Auto + 8 Save = 32 byte                 *
;*****************************************************************************
SlNetIfWifi_recvFrom:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {V4, LR}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
        ADD       V4, SP, #8            ; [DPU_3_PIPE] 
        SUB       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 32
$C$DW$236	.dwtag  DW_TAG_variable
	.dwattr $C$DW$236, DW_AT_name("sdContext")
	.dwattr $C$DW$236, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$236, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$236, DW_AT_location[DW_OP_breg13 8]

$C$DW$237	.dwtag  DW_TAG_variable
	.dwattr $C$DW$237, DW_AT_name("buf")
	.dwattr $C$DW$237, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$237, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$237, DW_AT_location[DW_OP_breg13 12]

$C$DW$238	.dwtag  DW_TAG_variable
	.dwattr $C$DW$238, DW_AT_name("len")
	.dwattr $C$DW$238, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$238, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$238, DW_AT_location[DW_OP_breg13 16]

$C$DW$239	.dwtag  DW_TAG_variable
	.dwattr $C$DW$239, DW_AT_name("sd")
	.dwattr $C$DW$239, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$239, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$239, DW_AT_location[DW_OP_breg13 20]

        STR       A4, [SP, #16]         ; [DPU_3_PIPE] |253| 
        STR       A3, [SP, #12]         ; [DPU_3_PIPE] |253| 
        STR       A2, [SP, #8]          ; [DPU_3_PIPE] |253| 
        STRH      A1, [SP, #20]         ; [DPU_3_PIPE] |253| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 254,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 254 | DISABLE_SEC_BITS_FROM_INPUT_FLAGS(flags);                              
;----------------------------------------------------------------------
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |254| 
        BIC       A1, A1, #-16777216    ; [DPU_3_PIPE] |254| 
        STR       A1, [V4, #0]          ; [DPU_3_PIPE] |254| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 255,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 255 | return sl_RecvFrom(sd, buf, len, flags, (SlSockAddr_t *)from, fromlen);
;----------------------------------------------------------------------
        LDR       A1, [V4, #4]          ; [DPU_3_PIPE] |255| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |255| 
        LDR       A1, [V4, #8]          ; [DPU_3_PIPE] |255| 
        STR       A1, [SP, #4]          ; [DPU_3_PIPE] |255| 
        LDRSH     A3, [SP, #16]         ; [DPU_3_PIPE] |255| 
        LDRSH     A4, [V4, #0]          ; [DPU_3_PIPE] |255| 
        LDR       A2, [SP, #12]         ; [DPU_3_PIPE] |255| 
        LDRSH     A1, [SP, #20]         ; [DPU_3_PIPE] |255| 
$C$DW$240	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$240, DW_AT_low_pc(0x00)
	.dwattr $C$DW$240, DW_AT_name("sl_RecvFrom")
	.dwattr $C$DW$240, DW_AT_TI_call

        BL        sl_RecvFrom           ; [DPU_3_PIPE] |255| 
        ; CALL OCCURS {sl_RecvFrom }     ; [] |255| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 256,column 1,is_stmt,isa 1
        ADD       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
$C$DW$241	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$241, DW_AT_low_pc(0x00)
	.dwattr $C$DW$241, DW_AT_TI_return

        POP       {V4, PC}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$228, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$228, DW_AT_TI_end_line(0x100)
	.dwattr $C$DW$228, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$228

	.sect	".text:SlNetIfWifi_send"
	.clink
	.thumbfunc SlNetIfWifi_send
	.thumb
	.global	SlNetIfWifi_send

$C$DW$242	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$242, DW_AT_name("SlNetIfWifi_send")
	.dwattr $C$DW$242, DW_AT_low_pc(SlNetIfWifi_send)
	.dwattr $C$DW$242, DW_AT_high_pc(0x00)
	.dwattr $C$DW$242, DW_AT_TI_symbol_name("SlNetIfWifi_send")
	.dwattr $C$DW$242, DW_AT_external
	.dwattr $C$DW$242, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$242, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$242, DW_AT_TI_begin_line(0x108)
	.dwattr $C$DW$242, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$242, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$242, DW_AT_decl_line(0x108)
	.dwattr $C$DW$242, DW_AT_decl_column(0x09)
	.dwattr $C$DW$242, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 265,column 1,is_stmt,address SlNetIfWifi_send,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_send
$C$DW$243	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$243, DW_AT_name("sd")
	.dwattr $C$DW$243, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$243, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$243, DW_AT_location[DW_OP_reg0]

$C$DW$244	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$244, DW_AT_name("sdContext")
	.dwattr $C$DW$244, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$244, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$244, DW_AT_location[DW_OP_reg1]

$C$DW$245	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$245, DW_AT_name("buf")
	.dwattr $C$DW$245, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$245, DW_AT_type(*$C$DW$T$323)
	.dwattr $C$DW$245, DW_AT_location[DW_OP_reg2]

$C$DW$246	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$246, DW_AT_name("len")
	.dwattr $C$DW$246, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$246, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$246, DW_AT_location[DW_OP_reg3]

$C$DW$247	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$247, DW_AT_name("flags")
	.dwattr $C$DW$247, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$247, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$247, DW_AT_location[DW_OP_breg13 24]

;----------------------------------------------------------------------
; 264 | int32_t SlNetIfWifi_send(int16_t sd, void *sdContext, const void *buf,
;     | uint32_t len, uint32_t flags)                                          
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_send                                           *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 0 Args + 16 Auto + 8 Save = 24 byte                 *
;*****************************************************************************
SlNetIfWifi_send:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {A1, A2, A3, A4, V4, LR} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
	.dwcfi	save_reg_to_mem, 3, -12
	.dwcfi	save_reg_to_mem, 2, -16
	.dwcfi	save_reg_to_mem, 1, -20
	.dwcfi	save_reg_to_mem, 0, -24
        ADD       V4, SP, #24           ; [DPU_3_PIPE] 
$C$DW$248	.dwtag  DW_TAG_variable
	.dwattr $C$DW$248, DW_AT_name("sdContext")
	.dwattr $C$DW$248, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$248, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$248, DW_AT_location[DW_OP_breg13 0]

$C$DW$249	.dwtag  DW_TAG_variable
	.dwattr $C$DW$249, DW_AT_name("buf")
	.dwattr $C$DW$249, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$249, DW_AT_type(*$C$DW$T$323)
	.dwattr $C$DW$249, DW_AT_location[DW_OP_breg13 4]

$C$DW$250	.dwtag  DW_TAG_variable
	.dwattr $C$DW$250, DW_AT_name("len")
	.dwattr $C$DW$250, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$250, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$250, DW_AT_location[DW_OP_breg13 8]

$C$DW$251	.dwtag  DW_TAG_variable
	.dwattr $C$DW$251, DW_AT_name("sd")
	.dwattr $C$DW$251, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$251, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$251, DW_AT_location[DW_OP_breg13 12]

        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |265| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |265| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |265| 
        STRH      A1, [SP, #12]         ; [DPU_3_PIPE] |265| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 266,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 266 | DISABLE_SEC_BITS_FROM_INPUT_FLAGS(flags);                              
;----------------------------------------------------------------------
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |266| 
        BIC       A1, A1, #-16777216    ; [DPU_3_PIPE] |266| 
        STR       A1, [V4, #0]          ; [DPU_3_PIPE] |266| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 267,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 267 | return sl_Send(sd, buf, len, flags);                                   
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #12]         ; [DPU_3_PIPE] |267| 
        LDRSH     A3, [SP, #8]          ; [DPU_3_PIPE] |267| 
        LDRSH     A4, [V4, #0]          ; [DPU_3_PIPE] |267| 
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |267| 
$C$DW$252	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$252, DW_AT_low_pc(0x00)
	.dwattr $C$DW$252, DW_AT_name("sl_Send")
	.dwattr $C$DW$252, DW_AT_TI_call

        BL        sl_Send               ; [DPU_3_PIPE] |267| 
        ; CALL OCCURS {sl_Send }         ; [] |267| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 268,column 1,is_stmt,isa 1
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] 
$C$DW$253	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$253, DW_AT_low_pc(0x00)
	.dwattr $C$DW$253, DW_AT_TI_return

        POP       {A1, A2, A3, A4, V4, PC} ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
	.dwcfi	restore_reg, 3
	.dwcfi	restore_reg, 2
	.dwcfi	restore_reg, 1
	.dwcfi	restore_reg, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$242, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$242, DW_AT_TI_end_line(0x10c)
	.dwattr $C$DW$242, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$242

	.sect	".text:SlNetIfWifi_sendTo"
	.clink
	.thumbfunc SlNetIfWifi_sendTo
	.thumb
	.global	SlNetIfWifi_sendTo

$C$DW$254	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$254, DW_AT_name("SlNetIfWifi_sendTo")
	.dwattr $C$DW$254, DW_AT_low_pc(SlNetIfWifi_sendTo)
	.dwattr $C$DW$254, DW_AT_high_pc(0x00)
	.dwattr $C$DW$254, DW_AT_TI_symbol_name("SlNetIfWifi_sendTo")
	.dwattr $C$DW$254, DW_AT_external
	.dwattr $C$DW$254, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$254, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$254, DW_AT_TI_begin_line(0x114)
	.dwattr $C$DW$254, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$254, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$254, DW_AT_decl_line(0x114)
	.dwattr $C$DW$254, DW_AT_decl_column(0x09)
	.dwattr $C$DW$254, DW_AT_TI_max_frame_size(0x20)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 277,column 1,is_stmt,address SlNetIfWifi_sendTo,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_sendTo
$C$DW$255	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$255, DW_AT_name("sd")
	.dwattr $C$DW$255, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$255, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$255, DW_AT_location[DW_OP_reg0]

$C$DW$256	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$256, DW_AT_name("sdContext")
	.dwattr $C$DW$256, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$256, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$256, DW_AT_location[DW_OP_reg1]

$C$DW$257	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$257, DW_AT_name("buf")
	.dwattr $C$DW$257, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$257, DW_AT_type(*$C$DW$T$323)
	.dwattr $C$DW$257, DW_AT_location[DW_OP_reg2]

$C$DW$258	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$258, DW_AT_name("len")
	.dwattr $C$DW$258, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$258, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$258, DW_AT_location[DW_OP_reg3]

$C$DW$259	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$259, DW_AT_name("flags")
	.dwattr $C$DW$259, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$259, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$259, DW_AT_location[DW_OP_breg13 32]

$C$DW$260	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$260, DW_AT_name("to")
	.dwattr $C$DW$260, DW_AT_TI_symbol_name("to")
	.dwattr $C$DW$260, DW_AT_type(*$C$DW$T$301)
	.dwattr $C$DW$260, DW_AT_location[DW_OP_breg13 36]

$C$DW$261	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$261, DW_AT_name("tolen")
	.dwattr $C$DW$261, DW_AT_TI_symbol_name("tolen")
	.dwattr $C$DW$261, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$261, DW_AT_location[DW_OP_breg13 40]

;----------------------------------------------------------------------
; 276 | int32_t SlNetIfWifi_sendTo(int16_t sd, void *sdContext, const void *buf
;     | , uint32_t len, uint32_t flags, const SlNetSock_Addr_t *to, SlNetSockle
;     | n_t tolen)                                                             
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_sendTo                                         *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 8 Args + 16 Auto + 8 Save = 32 byte                 *
;*****************************************************************************
SlNetIfWifi_sendTo:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {V4, LR}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
        ADD       V4, SP, #8            ; [DPU_3_PIPE] 
        SUB       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 32
$C$DW$262	.dwtag  DW_TAG_variable
	.dwattr $C$DW$262, DW_AT_name("sdContext")
	.dwattr $C$DW$262, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$262, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$262, DW_AT_location[DW_OP_breg13 8]

$C$DW$263	.dwtag  DW_TAG_variable
	.dwattr $C$DW$263, DW_AT_name("buf")
	.dwattr $C$DW$263, DW_AT_TI_symbol_name("buf")
	.dwattr $C$DW$263, DW_AT_type(*$C$DW$T$323)
	.dwattr $C$DW$263, DW_AT_location[DW_OP_breg13 12]

$C$DW$264	.dwtag  DW_TAG_variable
	.dwattr $C$DW$264, DW_AT_name("len")
	.dwattr $C$DW$264, DW_AT_TI_symbol_name("len")
	.dwattr $C$DW$264, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$264, DW_AT_location[DW_OP_breg13 16]

$C$DW$265	.dwtag  DW_TAG_variable
	.dwattr $C$DW$265, DW_AT_name("sd")
	.dwattr $C$DW$265, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$265, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$265, DW_AT_location[DW_OP_breg13 20]

        STR       A4, [SP, #16]         ; [DPU_3_PIPE] |277| 
        STR       A3, [SP, #12]         ; [DPU_3_PIPE] |277| 
        STR       A2, [SP, #8]          ; [DPU_3_PIPE] |277| 
        STRH      A1, [SP, #20]         ; [DPU_3_PIPE] |277| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 278,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 278 | DISABLE_SEC_BITS_FROM_INPUT_FLAGS(flags);                              
;----------------------------------------------------------------------
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |278| 
        BIC       A1, A1, #-16777216    ; [DPU_3_PIPE] |278| 
        STR       A1, [V4, #0]          ; [DPU_3_PIPE] |278| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 279,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 279 | return sl_SendTo(sd, buf, len, flags, (const SlSockAddr_t *)to, tolen);
;----------------------------------------------------------------------
        LDR       A1, [V4, #4]          ; [DPU_3_PIPE] |279| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |279| 
        LDRH      A1, [V4, #8]          ; [DPU_3_PIPE] |279| 
        STR       A1, [SP, #4]          ; [DPU_3_PIPE] |279| 
        LDRSH     A3, [SP, #16]         ; [DPU_3_PIPE] |279| 
        LDRSH     A4, [V4, #0]          ; [DPU_3_PIPE] |279| 
        LDR       A2, [SP, #12]         ; [DPU_3_PIPE] |279| 
        LDRSH     A1, [SP, #20]         ; [DPU_3_PIPE] |279| 
$C$DW$266	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$266, DW_AT_low_pc(0x00)
	.dwattr $C$DW$266, DW_AT_name("sl_SendTo")
	.dwattr $C$DW$266, DW_AT_TI_call

        BL        sl_SendTo             ; [DPU_3_PIPE] |279| 
        ; CALL OCCURS {sl_SendTo }       ; [] |279| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 280,column 1,is_stmt,isa 1
        ADD       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
$C$DW$267	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$267, DW_AT_low_pc(0x00)
	.dwattr $C$DW$267, DW_AT_TI_return

        POP       {V4, PC}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$254, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$254, DW_AT_TI_end_line(0x118)
	.dwattr $C$DW$254, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$254

	.sect	".text:SlNetIfWifi_sockstartSec"
	.clink
	.thumbfunc SlNetIfWifi_sockstartSec
	.thumb
	.global	SlNetIfWifi_sockstartSec

$C$DW$268	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$268, DW_AT_name("SlNetIfWifi_sockstartSec")
	.dwattr $C$DW$268, DW_AT_low_pc(SlNetIfWifi_sockstartSec)
	.dwattr $C$DW$268, DW_AT_high_pc(0x00)
	.dwattr $C$DW$268, DW_AT_TI_symbol_name("SlNetIfWifi_sockstartSec")
	.dwattr $C$DW$268, DW_AT_external
	.dwattr $C$DW$268, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$268, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$268, DW_AT_TI_begin_line(0x120)
	.dwattr $C$DW$268, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$268, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$268, DW_AT_decl_line(0x120)
	.dwattr $C$DW$268, DW_AT_decl_column(0x09)
	.dwattr $C$DW$268, DW_AT_TI_max_frame_size(0x20)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 289,column 1,is_stmt,address SlNetIfWifi_sockstartSec,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_sockstartSec
$C$DW$269	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$269, DW_AT_name("sd")
	.dwattr $C$DW$269, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$269, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$269, DW_AT_location[DW_OP_reg0]

$C$DW$270	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$270, DW_AT_name("sdContext")
	.dwattr $C$DW$270, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$270, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$270, DW_AT_location[DW_OP_reg1]

$C$DW$271	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$271, DW_AT_name("secAttrib")
	.dwattr $C$DW$271, DW_AT_TI_symbol_name("secAttrib")
	.dwattr $C$DW$271, DW_AT_type(*$C$DW$T$331)
	.dwattr $C$DW$271, DW_AT_location[DW_OP_reg2]

$C$DW$272	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$272, DW_AT_name("flags")
	.dwattr $C$DW$272, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$272, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$272, DW_AT_location[DW_OP_reg3]

;----------------------------------------------------------------------
; 288 | int32_t SlNetIfWifi_sockstartSec(int16_t sd, void *sdContext, SlNetSock
;     | SecAttrib_t *secAttrib, uint8_t flags)                                 
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_sockstartSec                                   *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V1,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V1,V9,SP,LR,SR                          *
;*   Local Frame Size  : 4 Args + 20 Auto + 8 Save = 32 byte                 *
;*****************************************************************************
SlNetIfWifi_sockstartSec:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {V1, LR}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 4, -8
        SUB       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 32
$C$DW$273	.dwtag  DW_TAG_variable
	.dwattr $C$DW$273, DW_AT_name("sdContext")
	.dwattr $C$DW$273, DW_AT_TI_symbol_name("sdContext")
	.dwattr $C$DW$273, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$273, DW_AT_location[DW_OP_breg13 4]

$C$DW$274	.dwtag  DW_TAG_variable
	.dwattr $C$DW$274, DW_AT_name("secAttrib")
	.dwattr $C$DW$274, DW_AT_TI_symbol_name("secAttrib")
	.dwattr $C$DW$274, DW_AT_type(*$C$DW$T$331)
	.dwattr $C$DW$274, DW_AT_location[DW_OP_breg13 8]

$C$DW$275	.dwtag  DW_TAG_variable
	.dwattr $C$DW$275, DW_AT_name("tempSecAttrib")
	.dwattr $C$DW$275, DW_AT_TI_symbol_name("tempSecAttrib")
	.dwattr $C$DW$275, DW_AT_type(*$C$DW$T$329)
	.dwattr $C$DW$275, DW_AT_location[DW_OP_breg13 12]

$C$DW$276	.dwtag  DW_TAG_variable
	.dwattr $C$DW$276, DW_AT_name("retVal")
	.dwattr $C$DW$276, DW_AT_TI_symbol_name("retVal")
	.dwattr $C$DW$276, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$276, DW_AT_location[DW_OP_breg13 16]

$C$DW$277	.dwtag  DW_TAG_variable
	.dwattr $C$DW$277, DW_AT_name("sd")
	.dwattr $C$DW$277, DW_AT_TI_symbol_name("sd")
	.dwattr $C$DW$277, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$277, DW_AT_location[DW_OP_breg13 20]

$C$DW$278	.dwtag  DW_TAG_variable
	.dwattr $C$DW$278, DW_AT_name("flags")
	.dwattr $C$DW$278, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$278, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$278, DW_AT_location[DW_OP_breg13 22]

        STRB      A4, [SP, #22]         ; [DPU_3_PIPE] |289| 
        STR       A3, [SP, #8]          ; [DPU_3_PIPE] |289| 
        STR       A2, [SP, #4]          ; [DPU_3_PIPE] |289| 
        STRH      A1, [SP, #20]         ; [DPU_3_PIPE] |289| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 290,column 46,is_stmt,isa 1
;----------------------------------------------------------------------
; 290 | SlNetSock_SecAttribNode_t *tempSecAttrib = *secAttrib;                 
;----------------------------------------------------------------------
        LDR       A1, [SP, #8]          ; [DPU_3_PIPE] |290| 
        LDR       A1, [A1, #0]          ; [DPU_3_PIPE] |290| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |290| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 291,column 46,is_stmt,isa 1
;----------------------------------------------------------------------
; 291 | int32_t                   retVal         = SLNETERR_RET_CODE_OK;       
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |291| 
        STR       A1, [SP, #16]         ; [DPU_3_PIPE] |291| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 293,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 293 | if ( 0 != (flags & SLNETSOCK_SEC_BIND_CONTEXT_ONLY)  )                 
;----------------------------------------------------------------------
        LDRB      A1, [SP, #22]         ; [DPU_3_PIPE] |293| 
        LSRS      A1, A1, #2            ; [DPU_3_PIPE] |293| 
        BCC       ||$C$L4||             ; [DPU_3_PIPE] |293| 
        ; BRANCHCC OCCURS {||$C$L4||}    ; [] |293| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 296,column 16,is_stmt,isa 1
;----------------------------------------------------------------------
; 296 | while (NULL != tempSecAttrib)                                          
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |296| 
        CBZ       A1, ||$C$L4||         ; [] 
        ; BRANCHCC OCCURS {||$C$L4||}    ; [] |296| 
;* --------------------------------------------------------------------------*
;*   BEGIN LOOP ||$C$L1||
;*
;*   Loop source line                : 296
;*   Loop closing brace source line  : 307
;*   Known Minimum Trip Count        : 1
;*   Known Maximum Trip Count        : 4294967295
;*   Known Max Trip Count Factor     : 1
;* --------------------------------------------------------------------------*
||$C$L1||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 298,column 13,is_stmt,isa 1
;----------------------------------------------------------------------
; 298 | if ( tempSecAttrib->attribName <= SLNETSOCK_SEC_ATTRIB_DISABLE_CERT_STO
;     | RE)                                                                    
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |298| 
        LDRB      A1, [A1, #0]          ; [DPU_3_PIPE] |298| 
        CMP       A1, #9                ; [DPU_3_PIPE] |298| 
        BGT       ||$C$L2||             ; [DPU_3_PIPE] |298| 
        ; BRANCHCC OCCURS {||$C$L2||}    ; [] |298| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 300,column 17,is_stmt,isa 1
;----------------------------------------------------------------------
; 300 | retVal = sl_SetSockOpt(sd, SL_SOL_SOCKET, StartSecOptName[tempSecAttrib
;     | ->attribName], tempSecAttrib->attribBuff, tempSecAttrib->attribBuffLen)
;     | ;                                                                      
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |300| 
        LDRH      A1, [A1, #8]          ; [DPU_3_PIPE] |300| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |300| 
        LDR       A2, [SP, #12]         ; [DPU_3_PIPE] |300| 
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |300| 
        LDR       V1, $C$CON3           ; [DPU_3_PIPE] |300| 
        LDRB      A3, [A1, #0]          ; [DPU_3_PIPE] |300| 
        LDR       A4, [A2, #4]          ; [DPU_3_PIPE] |300| 
        LDRSH     A1, [SP, #20]         ; [DPU_3_PIPE] |300| 
        LDRSH     A3, [V1, +A3, LSL #1] ; [DPU_3_PIPE] |300| 
        MOVS      A2, #1                ; [DPU_3_PIPE] |300| 
$C$DW$279	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$279, DW_AT_low_pc(0x00)
	.dwattr $C$DW$279, DW_AT_name("sl_SetSockOpt")
	.dwattr $C$DW$279, DW_AT_TI_call

        BL        sl_SetSockOpt         ; [DPU_3_PIPE] |300| 
        ; CALL OCCURS {sl_SetSockOpt }   ; [] |300| 
        STR       A1, [SP, #16]         ; [DPU_3_PIPE] |300| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 301,column 13,is_stmt,isa 1
;----------------------------------------------------------------------
; 302 | else                                                                   
;----------------------------------------------------------------------
        B         ||$C$L3||             ; [DPU_3_PIPE] |301| 
        ; BRANCH OCCURS {||$C$L3||}      ; [] |301| 
;* --------------------------------------------------------------------------*
||$C$L2||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 304,column 17,is_stmt,isa 1
;----------------------------------------------------------------------
; 304 | return SLNETERR_RET_CODE_INVALID_INPUT;                                
;----------------------------------------------------------------------
        LDR       A1, $C$CON4           ; [DPU_3_PIPE] |304| 
        B         ||$C$L6||             ; [DPU_3_PIPE] |304| 
        ; BRANCH OCCURS {||$C$L6||}      ; [] |304| 
;* --------------------------------------------------------------------------*
||$C$L3||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 306,column 13,is_stmt,isa 1
;----------------------------------------------------------------------
; 306 | tempSecAttrib = tempSecAttrib->next;                                   
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |306| 
        LDR       A1, [A1, #12]         ; [DPU_3_PIPE] |306| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |306| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 296,column 16,is_stmt,isa 1
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |296| 
        CMP       A1, #0                ; [DPU_3_PIPE] |296| 
        BNE       ||$C$L1||             ; [DPU_3_PIPE] |296| 
        ; BRANCHCC OCCURS {||$C$L1||}    ; [] |296| 
;* --------------------------------------------------------------------------*
||$C$L4||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 310,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 310 | if ( 0 != (flags & SLNETSOCK_SEC_START_SECURITY_SESSION_ONLY)  )       
;----------------------------------------------------------------------
        LDRB      A1, [SP, #22]         ; [DPU_3_PIPE] |310| 
        LSRS      A1, A1, #1            ; [DPU_3_PIPE] |310| 
        BCC       ||$C$L5||             ; [DPU_3_PIPE] |310| 
        ; BRANCHCC OCCURS {||$C$L5||}    ; [] |310| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 313,column 9,is_stmt,isa 1
;----------------------------------------------------------------------
; 313 | retVal = sl_StartTLS(sd);                                              
;----------------------------------------------------------------------
        LDRSH     A1, [SP, #20]         ; [DPU_3_PIPE] |313| 
$C$DW$280	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$280, DW_AT_low_pc(0x00)
	.dwattr $C$DW$280, DW_AT_name("sl_StartTLS")
	.dwattr $C$DW$280, DW_AT_TI_call

        BL        sl_StartTLS           ; [DPU_3_PIPE] |313| 
        ; CALL OCCURS {sl_StartTLS }     ; [] |313| 
        STR       A1, [SP, #16]         ; [DPU_3_PIPE] |313| 
;* --------------------------------------------------------------------------*
||$C$L5||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 316,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 316 | return retVal;                                                         
;----------------------------------------------------------------------
        LDR       A1, [SP, #16]         ; [DPU_3_PIPE] |316| 
;* --------------------------------------------------------------------------*
||$C$L6||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 317,column 1,is_stmt,isa 1
        ADD       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
$C$DW$281	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$281, DW_AT_low_pc(0x00)
	.dwattr $C$DW$281, DW_AT_TI_return

        POP       {V1, PC}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 4
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$268, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$268, DW_AT_TI_end_line(0x13d)
	.dwattr $C$DW$268, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$268

	.sect	".text:SlNetIfWifi_getHostByName"
	.clink
	.thumbfunc SlNetIfWifi_getHostByName
	.thumb
	.global	SlNetIfWifi_getHostByName

$C$DW$282	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$282, DW_AT_name("SlNetIfWifi_getHostByName")
	.dwattr $C$DW$282, DW_AT_low_pc(SlNetIfWifi_getHostByName)
	.dwattr $C$DW$282, DW_AT_high_pc(0x00)
	.dwattr $C$DW$282, DW_AT_TI_symbol_name("SlNetIfWifi_getHostByName")
	.dwattr $C$DW$282, DW_AT_external
	.dwattr $C$DW$282, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$282, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$282, DW_AT_TI_begin_line(0x146)
	.dwattr $C$DW$282, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$282, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$282, DW_AT_decl_line(0x146)
	.dwattr $C$DW$282, DW_AT_decl_column(0x09)
	.dwattr $C$DW$282, DW_AT_TI_max_frame_size(0x20)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 327,column 1,is_stmt,address SlNetIfWifi_getHostByName,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_getHostByName
$C$DW$283	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$283, DW_AT_name("ifContext")
	.dwattr $C$DW$283, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$283, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$283, DW_AT_location[DW_OP_reg0]

$C$DW$284	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$284, DW_AT_name("name")
	.dwattr $C$DW$284, DW_AT_TI_symbol_name("name")
	.dwattr $C$DW$284, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$284, DW_AT_location[DW_OP_reg1]

$C$DW$285	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$285, DW_AT_name("nameLen")
	.dwattr $C$DW$285, DW_AT_TI_symbol_name("nameLen")
	.dwattr $C$DW$285, DW_AT_type(*$C$DW$T$335)
	.dwattr $C$DW$285, DW_AT_location[DW_OP_reg2]

$C$DW$286	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$286, DW_AT_name("ipAddr")
	.dwattr $C$DW$286, DW_AT_TI_symbol_name("ipAddr")
	.dwattr $C$DW$286, DW_AT_type(*$C$DW$T$336)
	.dwattr $C$DW$286, DW_AT_location[DW_OP_reg3]

$C$DW$287	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$287, DW_AT_name("ipAddrLen")
	.dwattr $C$DW$287, DW_AT_TI_symbol_name("ipAddrLen")
	.dwattr $C$DW$287, DW_AT_type(*$C$DW$T$337)
	.dwattr $C$DW$287, DW_AT_location[DW_OP_breg13 32]

$C$DW$288	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$288, DW_AT_name("family")
	.dwattr $C$DW$288, DW_AT_TI_symbol_name("family")
	.dwattr $C$DW$288, DW_AT_type(*$C$DW$T$338)
	.dwattr $C$DW$288, DW_AT_location[DW_OP_breg13 36]

;----------------------------------------------------------------------
; 326 | int32_t SlNetIfWifi_getHostByName(void *ifContext, char *name, const ui
;     | nt16_t nameLen, uint32_t *ipAddr, uint16_t *ipAddrLen, const uint8_t fa
;     | mily)                                                                  
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_getHostByName                                  *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 0 Args + 20 Auto + 8 Save = 28 byte                 *
;*****************************************************************************
SlNetIfWifi_getHostByName:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {V4, LR}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
        ADD       V4, SP, #8            ; [DPU_3_PIPE] 
        SUB       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 32
$C$DW$289	.dwtag  DW_TAG_variable
	.dwattr $C$DW$289, DW_AT_name("ifContext")
	.dwattr $C$DW$289, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$289, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$289, DW_AT_location[DW_OP_breg13 0]

$C$DW$290	.dwtag  DW_TAG_variable
	.dwattr $C$DW$290, DW_AT_name("name")
	.dwattr $C$DW$290, DW_AT_TI_symbol_name("name")
	.dwattr $C$DW$290, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$290, DW_AT_location[DW_OP_breg13 4]

$C$DW$291	.dwtag  DW_TAG_variable
	.dwattr $C$DW$291, DW_AT_name("ipAddr")
	.dwattr $C$DW$291, DW_AT_TI_symbol_name("ipAddr")
	.dwattr $C$DW$291, DW_AT_type(*$C$DW$T$336)
	.dwattr $C$DW$291, DW_AT_location[DW_OP_breg13 8]

$C$DW$292	.dwtag  DW_TAG_variable
	.dwattr $C$DW$292, DW_AT_name("retVal")
	.dwattr $C$DW$292, DW_AT_TI_symbol_name("retVal")
	.dwattr $C$DW$292, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$292, DW_AT_location[DW_OP_breg13 12]

$C$DW$293	.dwtag  DW_TAG_variable
	.dwattr $C$DW$293, DW_AT_name("nameLen")
	.dwattr $C$DW$293, DW_AT_TI_symbol_name("nameLen")
	.dwattr $C$DW$293, DW_AT_type(*$C$DW$T$335)
	.dwattr $C$DW$293, DW_AT_location[DW_OP_breg13 16]

        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |327| 
        STRH      A3, [SP, #16]         ; [DPU_3_PIPE] |327| 
        STR       A2, [SP, #4]          ; [DPU_3_PIPE] |327| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |327| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 328,column 21,is_stmt,isa 1
;----------------------------------------------------------------------
; 328 | int32_t  retVal = SLNETERR_RET_CODE_OK;                                
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |328| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |328| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 333,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 333 | retVal = sl_NetAppDnsGetHostByName((signed char *)name, nameLen, (_u32
;     | *)ipAddr, family);                                                     
;----------------------------------------------------------------------
        LDRB      A4, [V4, #4]          ; [DPU_3_PIPE] |333| 
        LDRH      A2, [SP, #16]         ; [DPU_3_PIPE] |333| 
        LDR       A3, [SP, #8]          ; [DPU_3_PIPE] |333| 
        LDR       A1, [SP, #4]          ; [DPU_3_PIPE] |333| 
$C$DW$294	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$294, DW_AT_low_pc(0x00)
	.dwattr $C$DW$294, DW_AT_name("sl_NetAppDnsGetHostByName")
	.dwattr $C$DW$294, DW_AT_TI_call

        BL        sl_NetAppDnsGetHostByName ; [DPU_3_PIPE] |333| 
        ; CALL OCCURS {sl_NetAppDnsGetHostByName }  ; [] |333| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |333| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 335,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 335 | if (retVal == SLNETERR_RET_CODE_OK)                                    
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |335| 
        CBNZ      A1, ||$C$L7||         ; [] 
        ; BRANCHCC OCCURS {||$C$L7||}    ; [] |335| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 337,column 9,is_stmt,isa 1
;----------------------------------------------------------------------
; 337 | *ipAddrLen = 1;                                                        
;----------------------------------------------------------------------
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |337| 
        MOVS      A2, #1                ; [DPU_3_PIPE] |337| 
        STRH      A2, [A1, #0]          ; [DPU_3_PIPE] |337| 
;* --------------------------------------------------------------------------*
||$C$L7||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 340,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 340 | return retVal;                                                         
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |340| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 343,column 1,is_stmt,isa 1
        ADD       SP, SP, #24           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
$C$DW$295	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$295, DW_AT_low_pc(0x00)
	.dwattr $C$DW$295, DW_AT_TI_return

        POP       {V4, PC}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$282, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$282, DW_AT_TI_end_line(0x157)
	.dwattr $C$DW$282, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$282

	.sect	".text:SlNetIfWifi_getIPAddr"
	.clink
	.thumbfunc SlNetIfWifi_getIPAddr
	.thumb
	.global	SlNetIfWifi_getIPAddr

$C$DW$296	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$296, DW_AT_name("SlNetIfWifi_getIPAddr")
	.dwattr $C$DW$296, DW_AT_low_pc(SlNetIfWifi_getIPAddr)
	.dwattr $C$DW$296, DW_AT_high_pc(0x00)
	.dwattr $C$DW$296, DW_AT_TI_symbol_name("SlNetIfWifi_getIPAddr")
	.dwattr $C$DW$296, DW_AT_external
	.dwattr $C$DW$296, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$296, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$296, DW_AT_TI_begin_line(0x15f)
	.dwattr $C$DW$296, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$296, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$296, DW_AT_decl_line(0x15f)
	.dwattr $C$DW$296, DW_AT_decl_column(0x09)
	.dwattr $C$DW$296, DW_AT_TI_max_frame_size(0x18)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 352,column 1,is_stmt,address SlNetIfWifi_getIPAddr,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_getIPAddr
$C$DW$297	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$297, DW_AT_name("ifContext")
	.dwattr $C$DW$297, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$297, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$297, DW_AT_location[DW_OP_reg0]

$C$DW$298	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$298, DW_AT_name("addrType")
	.dwattr $C$DW$298, DW_AT_TI_symbol_name("addrType")
	.dwattr $C$DW$298, DW_AT_type(*$C$DW$T$342)
	.dwattr $C$DW$298, DW_AT_location[DW_OP_reg1]

$C$DW$299	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$299, DW_AT_name("addrConfig")
	.dwattr $C$DW$299, DW_AT_TI_symbol_name("addrConfig")
	.dwattr $C$DW$299, DW_AT_type(*$C$DW$T$337)
	.dwattr $C$DW$299, DW_AT_location[DW_OP_reg2]

$C$DW$300	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$300, DW_AT_name("ipAddr")
	.dwattr $C$DW$300, DW_AT_TI_symbol_name("ipAddr")
	.dwattr $C$DW$300, DW_AT_type(*$C$DW$T$336)
	.dwattr $C$DW$300, DW_AT_location[DW_OP_reg3]

;----------------------------------------------------------------------
; 351 | int32_t SlNetIfWifi_getIPAddr(void *ifContext, SlNetIfAddressType_e add
;     | rType, uint16_t *addrConfig, uint32_t *ipAddr)                         
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_getIPAddr                                      *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 16 Auto + 4 Save = 20 byte                 *
;*****************************************************************************
SlNetIfWifi_getIPAddr:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {LR}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
	.dwcfi	save_reg_to_mem, 14, -4
        SUB       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 24
$C$DW$301	.dwtag  DW_TAG_variable
	.dwattr $C$DW$301, DW_AT_name("ifContext")
	.dwattr $C$DW$301, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$301, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$301, DW_AT_location[DW_OP_breg13 0]

$C$DW$302	.dwtag  DW_TAG_variable
	.dwattr $C$DW$302, DW_AT_name("addrConfig")
	.dwattr $C$DW$302, DW_AT_TI_symbol_name("addrConfig")
	.dwattr $C$DW$302, DW_AT_type(*$C$DW$T$337)
	.dwattr $C$DW$302, DW_AT_location[DW_OP_breg13 4]

$C$DW$303	.dwtag  DW_TAG_variable
	.dwattr $C$DW$303, DW_AT_name("ipAddr")
	.dwattr $C$DW$303, DW_AT_TI_symbol_name("ipAddr")
	.dwattr $C$DW$303, DW_AT_type(*$C$DW$T$336)
	.dwattr $C$DW$303, DW_AT_location[DW_OP_breg13 8]

$C$DW$304	.dwtag  DW_TAG_variable
	.dwattr $C$DW$304, DW_AT_name("ipAddrLen")
	.dwattr $C$DW$304, DW_AT_TI_symbol_name("ipAddrLen")
	.dwattr $C$DW$304, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$304, DW_AT_location[DW_OP_breg13 12]

$C$DW$305	.dwtag  DW_TAG_variable
	.dwattr $C$DW$305, DW_AT_name("addrType")
	.dwattr $C$DW$305, DW_AT_TI_symbol_name("addrType")
	.dwattr $C$DW$305, DW_AT_type(*$C$DW$T$342)
	.dwattr $C$DW$305, DW_AT_location[DW_OP_breg13 14]

        STR       A4, [SP, #8]          ; [DPU_3_PIPE] |352| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |352| 
        STRB      A2, [SP, #14]         ; [DPU_3_PIPE] |352| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |352| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 353,column 24,is_stmt,isa 1
;----------------------------------------------------------------------
; 353 | uint16_t ipAddrLen = sizeof(ipAddr);                                   
;----------------------------------------------------------------------
        MOVS      A1, #4                ; [DPU_3_PIPE] |353| 
        STRH      A1, [SP, #12]         ; [DPU_3_PIPE] |353| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 354,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 354 | return sl_NetCfgGet(addrType, addrConfig, &ipAddrLen, (unsigned char *)
;     | ipAddr);                                                               
;----------------------------------------------------------------------
        LDR       A2, [SP, #4]          ; [DPU_3_PIPE] |354| 
        LDR       A4, [SP, #8]          ; [DPU_3_PIPE] |354| 
        LDRB      A1, [SP, #14]         ; [DPU_3_PIPE] |354| 
        ADD       A3, SP, #12           ; [DPU_3_PIPE] |354| 
$C$DW$306	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$306, DW_AT_low_pc(0x00)
	.dwattr $C$DW$306, DW_AT_name("sl_NetCfgGet")
	.dwattr $C$DW$306, DW_AT_TI_call

        BL        sl_NetCfgGet          ; [DPU_3_PIPE] |354| 
        ; CALL OCCURS {sl_NetCfgGet }    ; [] |354| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 355,column 1,is_stmt,isa 1
        ADD       SP, SP, #20           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
$C$DW$307	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$307, DW_AT_low_pc(0x00)
	.dwattr $C$DW$307, DW_AT_TI_return

        POP       {PC}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$296, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$296, DW_AT_TI_end_line(0x163)
	.dwattr $C$DW$296, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$296

	.sect	".text:SlNetIfWifi_getConnectionStatus"
	.clink
	.thumbfunc SlNetIfWifi_getConnectionStatus
	.thumb
	.global	SlNetIfWifi_getConnectionStatus

$C$DW$308	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$308, DW_AT_name("SlNetIfWifi_getConnectionStatus")
	.dwattr $C$DW$308, DW_AT_low_pc(SlNetIfWifi_getConnectionStatus)
	.dwattr $C$DW$308, DW_AT_high_pc(0x00)
	.dwattr $C$DW$308, DW_AT_TI_symbol_name("SlNetIfWifi_getConnectionStatus")
	.dwattr $C$DW$308, DW_AT_external
	.dwattr $C$DW$308, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$308, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$308, DW_AT_TI_begin_line(0x16b)
	.dwattr $C$DW$308, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$308, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$308, DW_AT_decl_line(0x16b)
	.dwattr $C$DW$308, DW_AT_decl_column(0x09)
	.dwattr $C$DW$308, DW_AT_TI_max_frame_size(0x60)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 364,column 1,is_stmt,address SlNetIfWifi_getConnectionStatus,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_getConnectionStatus
$C$DW$309	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$309, DW_AT_name("ifContext")
	.dwattr $C$DW$309, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$309, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$309, DW_AT_location[DW_OP_reg0]

;----------------------------------------------------------------------
; 363 | int32_t SlNetIfWifi_getConnectionStatus(void *ifContext)               
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_getConnectionStatus                            *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Regs Used         : A1,A2,A3,A4,V9,SP,LR,SR                             *
;*   Local Frame Size  : 0 Args + 92 Auto + 4 Save = 96 byte                 *
;*****************************************************************************
SlNetIfWifi_getConnectionStatus:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {LR}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
	.dwcfi	save_reg_to_mem, 14, -4
        SUB       SP, SP, #92           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 96
$C$DW$310	.dwtag  DW_TAG_variable
	.dwattr $C$DW$310, DW_AT_name("ifContext")
	.dwattr $C$DW$310, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$310, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$310, DW_AT_location[DW_OP_breg13 0]

$C$DW$311	.dwtag  DW_TAG_variable
	.dwattr $C$DW$311, DW_AT_name("retVal")
	.dwattr $C$DW$311, DW_AT_TI_symbol_name("retVal")
	.dwattr $C$DW$311, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$311, DW_AT_location[DW_OP_breg13 4]

$C$DW$312	.dwtag  DW_TAG_variable
	.dwattr $C$DW$312, DW_AT_name("Opt")
	.dwattr $C$DW$312, DW_AT_TI_symbol_name("Opt")
	.dwattr $C$DW$312, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$312, DW_AT_location[DW_OP_breg13 8]

$C$DW$313	.dwtag  DW_TAG_variable
	.dwattr $C$DW$313, DW_AT_name("Size")
	.dwattr $C$DW$313, DW_AT_TI_symbol_name("Size")
	.dwattr $C$DW$313, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$313, DW_AT_location[DW_OP_breg13 10]

$C$DW$314	.dwtag  DW_TAG_variable
	.dwattr $C$DW$314, DW_AT_name("connectionParams")
	.dwattr $C$DW$314, DW_AT_TI_symbol_name("connectionParams")
	.dwattr $C$DW$314, DW_AT_type(*$C$DW$T$449)
	.dwattr $C$DW$314, DW_AT_location[DW_OP_breg13 12]

;----------------------------------------------------------------------
; 365 | SlWlanConnStatusParam_t connectionParams;                              
;----------------------------------------------------------------------
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |364| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 366,column 36,is_stmt,isa 1
;----------------------------------------------------------------------
; 366 | uint16_t                Opt    = 0;                                    
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |366| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |366| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 367,column 36,is_stmt,isa 1
;----------------------------------------------------------------------
; 367 | int32_t                 retVal = 0;                                    
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |367| 
        STR       A1, [SP, #4]          ; [DPU_3_PIPE] |367| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 368,column 36,is_stmt,isa 1
;----------------------------------------------------------------------
; 368 | uint16_t                Size   = 0;                                    
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |368| 
        STRH      A1, [SP, #10]         ; [DPU_3_PIPE] |368| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 370,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 370 | memset(&connectionParams, 0, sizeof(SlWlanConnStatusParam_t));         
;----------------------------------------------------------------------
        MOVS      A2, #0                ; [DPU_3_PIPE] |370| 
        MOVS      A3, #80               ; [DPU_3_PIPE] |370| 
        ADD       A1, SP, #12           ; [DPU_3_PIPE] |370| 
$C$DW$315	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$315, DW_AT_low_pc(0x00)
	.dwattr $C$DW$315, DW_AT_name("memset")
	.dwattr $C$DW$315, DW_AT_TI_call

        BL        memset                ; [DPU_3_PIPE] |370| 
        ; CALL OCCURS {memset }          ; [] |370| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 372,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 372 | retVal = sl_WlanGet(SL_WLAN_CONNECTION_INFO, &Opt, &Size, (uint8_t *)&c
;     | onnectionParams);                                                      
;----------------------------------------------------------------------
        MOVS      A1, #5                ; [DPU_3_PIPE] |372| 
        ADD       A2, SP, #8            ; [DPU_3_PIPE] |372| 
        ADD       A3, SP, #10           ; [DPU_3_PIPE] |372| 
        ADD       A4, SP, #12           ; [DPU_3_PIPE] |372| 
$C$DW$316	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$316, DW_AT_low_pc(0x00)
	.dwattr $C$DW$316, DW_AT_name("sl_WlanGet")
	.dwattr $C$DW$316, DW_AT_TI_call

        BL        sl_WlanGet            ; [DPU_3_PIPE] |372| 
        ; CALL OCCURS {sl_WlanGet }      ; [] |372| 
        STR       A1, [SP, #4]          ; [DPU_3_PIPE] |372| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 375,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 375 | if (retVal < SLNETERR_RET_CODE_OK)                                     
;----------------------------------------------------------------------
        LDR       A1, [SP, #4]          ; [DPU_3_PIPE] |375| 
        CMP       A1, #0                ; [DPU_3_PIPE] |375| 
        BPL       ||$C$L8||             ; [DPU_3_PIPE] |375| 
        ; BRANCHCC OCCURS {||$C$L8||}    ; [] |375| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 378,column 9,is_stmt,isa 1
;----------------------------------------------------------------------
; 378 | return retVal;                                                         
;----------------------------------------------------------------------
        LDR       A1, [SP, #4]          ; [DPU_3_PIPE] |378| 
        B         ||$C$L9||             ; [DPU_3_PIPE] |378| 
        ; BRANCH OCCURS {||$C$L9||}      ; [] |378| 
;* --------------------------------------------------------------------------*
||$C$L8||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 380,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 380 | return connectionParams.ConnStatus;                                    
;----------------------------------------------------------------------
        LDRB      A1, [SP, #13]         ; [DPU_3_PIPE] |380| 
;* --------------------------------------------------------------------------*
||$C$L9||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 381,column 1,is_stmt,isa 1
        ADD       SP, SP, #92           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 4
$C$DW$317	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$317, DW_AT_low_pc(0x00)
	.dwattr $C$DW$317, DW_AT_TI_return

        POP       {PC}                  ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$308, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$308, DW_AT_TI_end_line(0x17d)
	.dwattr $C$DW$308, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$308

	.sect	".text:SlNetIfWifi_loadSecObj"
	.clink
	.thumbfunc SlNetIfWifi_loadSecObj
	.thumb
	.global	SlNetIfWifi_loadSecObj

$C$DW$318	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$318, DW_AT_name("SlNetIfWifi_loadSecObj")
	.dwattr $C$DW$318, DW_AT_low_pc(SlNetIfWifi_loadSecObj)
	.dwattr $C$DW$318, DW_AT_high_pc(0x00)
	.dwattr $C$DW$318, DW_AT_TI_symbol_name("SlNetIfWifi_loadSecObj")
	.dwattr $C$DW$318, DW_AT_external
	.dwattr $C$DW$318, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$318, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$318, DW_AT_TI_begin_line(0x185)
	.dwattr $C$DW$318, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$318, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$318, DW_AT_decl_line(0x185)
	.dwattr $C$DW$318, DW_AT_decl_column(0x09)
	.dwattr $C$DW$318, DW_AT_TI_max_frame_size(0x30)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 390,column 1,is_stmt,address SlNetIfWifi_loadSecObj,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_loadSecObj
$C$DW$319	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$319, DW_AT_name("ifContext")
	.dwattr $C$DW$319, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$319, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$319, DW_AT_location[DW_OP_reg0]

$C$DW$320	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$320, DW_AT_name("objType")
	.dwattr $C$DW$320, DW_AT_TI_symbol_name("objType")
	.dwattr $C$DW$320, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$320, DW_AT_location[DW_OP_reg1]

$C$DW$321	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$321, DW_AT_name("objName")
	.dwattr $C$DW$321, DW_AT_TI_symbol_name("objName")
	.dwattr $C$DW$321, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$321, DW_AT_location[DW_OP_reg2]

$C$DW$322	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$322, DW_AT_name("objNameLen")
	.dwattr $C$DW$322, DW_AT_TI_symbol_name("objNameLen")
	.dwattr $C$DW$322, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$322, DW_AT_location[DW_OP_reg3]

$C$DW$323	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$323, DW_AT_name("objBuff")
	.dwattr $C$DW$323, DW_AT_TI_symbol_name("objBuff")
	.dwattr $C$DW$323, DW_AT_type(*$C$DW$T$347)
	.dwattr $C$DW$323, DW_AT_location[DW_OP_breg13 48]

$C$DW$324	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$324, DW_AT_name("objBuffLen")
	.dwattr $C$DW$324, DW_AT_TI_symbol_name("objBuffLen")
	.dwattr $C$DW$324, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$324, DW_AT_location[DW_OP_breg13 52]

;----------------------------------------------------------------------
; 389 | int32_t SlNetIfWifi_loadSecObj(void *ifContext, uint16_t objType, char
;     | *objName, int16_t objNameLen, uint8_t *objBuff, int16_t objBuffLen)    
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_loadSecObj                                     *
;*                                                                           *
;*   Regs Modified     : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Regs Used         : A1,A2,A3,A4,V4,V9,SP,LR,SR                          *
;*   Local Frame Size  : 0 Args + 36 Auto + 8 Save = 44 byte                 *
;*****************************************************************************
SlNetIfWifi_loadSecObj:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        PUSH      {V4, LR}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
	.dwcfi	save_reg_to_mem, 14, -4
	.dwcfi	save_reg_to_mem, 7, -8
        ADD       V4, SP, #8            ; [DPU_3_PIPE] 
        SUB       SP, SP, #40           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 48
$C$DW$325	.dwtag  DW_TAG_variable
	.dwattr $C$DW$325, DW_AT_name("ifContext")
	.dwattr $C$DW$325, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$325, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$325, DW_AT_location[DW_OP_breg13 0]

$C$DW$326	.dwtag  DW_TAG_variable
	.dwattr $C$DW$326, DW_AT_name("objName")
	.dwattr $C$DW$326, DW_AT_TI_symbol_name("objName")
	.dwattr $C$DW$326, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$326, DW_AT_location[DW_OP_breg13 4]

$C$DW$327	.dwtag  DW_TAG_variable
	.dwattr $C$DW$327, DW_AT_name("deviceFileName")
	.dwattr $C$DW$327, DW_AT_TI_symbol_name("deviceFileName")
	.dwattr $C$DW$327, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$327, DW_AT_location[DW_OP_breg13 8]

$C$DW$328	.dwtag  DW_TAG_variable
	.dwattr $C$DW$328, DW_AT_name("DeviceFileHandle")
	.dwattr $C$DW$328, DW_AT_TI_symbol_name("DeviceFileHandle")
	.dwattr $C$DW$328, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$328, DW_AT_location[DW_OP_breg13 12]

$C$DW$329	.dwtag  DW_TAG_variable
	.dwattr $C$DW$329, DW_AT_name("retVal")
	.dwattr $C$DW$329, DW_AT_TI_symbol_name("retVal")
	.dwattr $C$DW$329, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$329, DW_AT_location[DW_OP_breg13 16]

$C$DW$330	.dwtag  DW_TAG_variable
	.dwattr $C$DW$330, DW_AT_name("Offset")
	.dwattr $C$DW$330, DW_AT_TI_symbol_name("Offset")
	.dwattr $C$DW$330, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$330, DW_AT_location[DW_OP_breg13 20]

$C$DW$331	.dwtag  DW_TAG_variable
	.dwattr $C$DW$331, DW_AT_name("MasterToken")
	.dwattr $C$DW$331, DW_AT_TI_symbol_name("MasterToken")
	.dwattr $C$DW$331, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$331, DW_AT_location[DW_OP_breg13 24]

$C$DW$332	.dwtag  DW_TAG_variable
	.dwattr $C$DW$332, DW_AT_name("OpenFlags")
	.dwattr $C$DW$332, DW_AT_TI_symbol_name("OpenFlags")
	.dwattr $C$DW$332, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$332, DW_AT_location[DW_OP_breg13 28]

$C$DW$333	.dwtag  DW_TAG_variable
	.dwattr $C$DW$333, DW_AT_name("objType")
	.dwattr $C$DW$333, DW_AT_TI_symbol_name("objType")
	.dwattr $C$DW$333, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$333, DW_AT_location[DW_OP_breg13 32]

$C$DW$334	.dwtag  DW_TAG_variable
	.dwattr $C$DW$334, DW_AT_name("objNameLen")
	.dwattr $C$DW$334, DW_AT_TI_symbol_name("objNameLen")
	.dwattr $C$DW$334, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$334, DW_AT_location[DW_OP_breg13 34]

        STRH      A4, [SP, #34]         ; [DPU_3_PIPE] |390| 
        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |390| 
        STRH      A2, [SP, #32]         ; [DPU_3_PIPE] |390| 
        STR       A1, [SP, #0]          ; [DPU_3_PIPE] |390| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 391,column 32,is_stmt,isa 1
;----------------------------------------------------------------------
; 391 | char     *deviceFileName   = objName;                                  
;----------------------------------------------------------------------
        LDR       A1, [SP, #4]          ; [DPU_3_PIPE] |391| 
        STR       A1, [SP, #8]          ; [DPU_3_PIPE] |391| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 392,column 32,is_stmt,isa 1
;----------------------------------------------------------------------
; 392 | int32_t   DeviceFileHandle = -1;                                       
; 393 | int32_t   retVal;                //negative retVal is an error         
;----------------------------------------------------------------------
        MOV       A1, #-1               ; [DPU_3_PIPE] |392| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |392| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 394,column 32,is_stmt,isa 1
;----------------------------------------------------------------------
; 394 | uint32_t  Offset           = 0;                                        
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |394| 
        STR       A1, [SP, #20]         ; [DPU_3_PIPE] |394| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 395,column 32,is_stmt,isa 1
;----------------------------------------------------------------------
; 395 | uint32_t  MasterToken      = 0;                                        
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |395| 
        STR       A1, [SP, #24]         ; [DPU_3_PIPE] |395| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 396,column 32,is_stmt,isa 1
;----------------------------------------------------------------------
; 396 | int32_t   OpenFlags        = 0;                                        
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |396| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |396| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 399,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 399 | if ( (NULL == objName) || (NULL == objBuff) )                          
;----------------------------------------------------------------------
        LDR       A1, [SP, #4]          ; [DPU_3_PIPE] |399| 
        CBZ       A1, ||$C$L10||        ; [] 
        ; BRANCHCC OCCURS {||$C$L10||}   ; [] |399| 
;* --------------------------------------------------------------------------*
        LDR       A1, [V4, #0]          ; [DPU_3_PIPE] |399| 
        CBNZ      A1, ||$C$L11||        ; [] 
        ; BRANCHCC OCCURS {||$C$L11||}   ; [] |399| 
;* --------------------------------------------------------------------------*
||$C$L10||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 402,column 9,is_stmt,isa 1
;----------------------------------------------------------------------
; 402 | return SLNETERR_RET_CODE_INVALID_INPUT;                                
;----------------------------------------------------------------------
        LDR       A1, $C$CON5           ; [DPU_3_PIPE] |402| 
        B         ||$C$L13||            ; [DPU_3_PIPE] |402| 
        ; BRANCH OCCURS {||$C$L13||}     ; [] |402| 
;* --------------------------------------------------------------------------*
||$C$L11||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 407,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 407 | OpenFlags = SL_FS_CREATE;                                              
;----------------------------------------------------------------------
        MOV       A1, #134217728        ; [DPU_3_PIPE] |407| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |407| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 408,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 408 | OpenFlags |= SL_FS_OVERWRITE;                                          
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |408| 
        ORR       A1, A1, #536870912    ; [DPU_3_PIPE] |408| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |408| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 409,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 409 | OpenFlags |= SL_FS_CREATE_SECURE;                                      
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |409| 
        ORR       A1, A1, #262144       ; [DPU_3_PIPE] |409| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |409| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 410,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 410 | OpenFlags |= SL_FS_CREATE_NOSIGNATURE;                                 
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |410| 
        ORR       A1, A1, #524288       ; [DPU_3_PIPE] |410| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |410| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 411,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 411 | OpenFlags |= SL_FS_CREATE_FAILSAFE;                                    
;----------------------------------------------------------------------
        LDR       A1, [SP, #28]         ; [DPU_3_PIPE] |411| 
        ORR       A1, A1, #131072       ; [DPU_3_PIPE] |411| 
        STR       A1, [SP, #28]         ; [DPU_3_PIPE] |411| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 414,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 414 | DeviceFileHandle =  sl_FsOpen((unsigned char *)deviceFileName, OpenFlag
;     | s | SL_FS_CREATE_MAX_SIZE( objBuffLen ), (unsigned long *)&MasterToken)
;     | ;                                                                      
;----------------------------------------------------------------------
        LDRSH     A1, [V4, #4]          ; [DPU_3_PIPE] |414| 
        ADDS      A1, A1, #255          ; [DPU_3_PIPE] |414| 
        UBFX      A2, A1, #8, #17       ; [DPU_3_PIPE] |414| 
        LDR       A3, [SP, #28]         ; [DPU_3_PIPE] |414| 
        LDR       A1, [SP, #8]          ; [DPU_3_PIPE] |414| 
        ORRS      A2, A2, A3            ; [DPU_3_PIPE] |414| 
        ADD       A3, SP, #24           ; [DPU_3_PIPE] |414| 
$C$DW$335	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$335, DW_AT_low_pc(0x00)
	.dwattr $C$DW$335, DW_AT_name("sl_FsOpen")
	.dwattr $C$DW$335, DW_AT_TI_call

        BL        sl_FsOpen             ; [DPU_3_PIPE] |414| 
        ; CALL OCCURS {sl_FsOpen }       ; [] |414| 
        STR       A1, [SP, #12]         ; [DPU_3_PIPE] |414| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 417,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 417 | if ( DeviceFileHandle < SLNETERR_RET_CODE_OK )                         
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |417| 
        CMP       A1, #0                ; [DPU_3_PIPE] |417| 
        BPL       ||$C$L12||            ; [DPU_3_PIPE] |417| 
        ; BRANCHCC OCCURS {||$C$L12||}   ; [] |417| 
;* --------------------------------------------------------------------------*
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 419,column 9,is_stmt,isa 1
;----------------------------------------------------------------------
; 419 | return DeviceFileHandle;                                               
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |419| 
        B         ||$C$L13||            ; [DPU_3_PIPE] |419| 
        ; BRANCH OCCURS {||$C$L13||}     ; [] |419| 
;* --------------------------------------------------------------------------*
||$C$L12||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 422,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 422 | Offset = 0;                                                            
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |422| 
        STR       A1, [SP, #20]         ; [DPU_3_PIPE] |422| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 424,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 424 | retVal = sl_FsWrite(DeviceFileHandle, Offset, (unsigned char *)objBuff,
;     |  objBuffLen);                                                          
;----------------------------------------------------------------------
        LDRSH     A4, [V4, #4]          ; [DPU_3_PIPE] |424| 
        LDR       A3, [V4, #0]          ; [DPU_3_PIPE] |424| 
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |424| 
        LDR       A2, [SP, #20]         ; [DPU_3_PIPE] |424| 
$C$DW$336	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$336, DW_AT_low_pc(0x00)
	.dwattr $C$DW$336, DW_AT_name("sl_FsWrite")
	.dwattr $C$DW$336, DW_AT_TI_call

        BL        sl_FsWrite            ; [DPU_3_PIPE] |424| 
        ; CALL OCCURS {sl_FsWrite }      ; [] |424| 
        STR       A1, [SP, #16]         ; [DPU_3_PIPE] |424| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 427,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 427 | retVal = sl_FsClose(DeviceFileHandle, NULL, NULL , 0);                 
;----------------------------------------------------------------------
        LDR       A1, [SP, #12]         ; [DPU_3_PIPE] |427| 
        MOVS      A2, #0                ; [DPU_3_PIPE] |427| 
        MOVS      A3, #0                ; [DPU_3_PIPE] |427| 
        MOVS      A4, #0                ; [DPU_3_PIPE] |427| 
$C$DW$337	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$337, DW_AT_low_pc(0x00)
	.dwattr $C$DW$337, DW_AT_name("sl_FsClose")
	.dwattr $C$DW$337, DW_AT_TI_call

        BL        sl_FsClose            ; [DPU_3_PIPE] |427| 
        ; CALL OCCURS {sl_FsClose }      ; [] |427| 
        STR       A1, [SP, #16]         ; [DPU_3_PIPE] |427| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 429,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 429 | return retVal;                                                         
;----------------------------------------------------------------------
        LDR       A1, [SP, #16]         ; [DPU_3_PIPE] |429| 
;* --------------------------------------------------------------------------*
||$C$L13||:    
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 430,column 1,is_stmt,isa 1
        ADD       SP, SP, #40           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 8
$C$DW$338	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$338, DW_AT_low_pc(0x00)
	.dwattr $C$DW$338, DW_AT_TI_return

        POP       {V4, PC}              ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
	.dwcfi	restore_reg, 7
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$318, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$318, DW_AT_TI_end_line(0x1ae)
	.dwattr $C$DW$318, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$318

	.sect	".text:SlNetIfWifi_CreateContext"
	.clink
	.thumbfunc SlNetIfWifi_CreateContext
	.thumb
	.global	SlNetIfWifi_CreateContext

$C$DW$339	.dwtag  DW_TAG_subprogram
	.dwattr $C$DW$339, DW_AT_name("SlNetIfWifi_CreateContext")
	.dwattr $C$DW$339, DW_AT_low_pc(SlNetIfWifi_CreateContext)
	.dwattr $C$DW$339, DW_AT_high_pc(0x00)
	.dwattr $C$DW$339, DW_AT_TI_symbol_name("SlNetIfWifi_CreateContext")
	.dwattr $C$DW$339, DW_AT_external
	.dwattr $C$DW$339, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$339, DW_AT_TI_begin_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$339, DW_AT_TI_begin_line(0x1b5)
	.dwattr $C$DW$339, DW_AT_TI_begin_column(0x09)
	.dwattr $C$DW$339, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$339, DW_AT_decl_line(0x1b5)
	.dwattr $C$DW$339, DW_AT_decl_column(0x09)
	.dwattr $C$DW$339, DW_AT_TI_max_frame_size(0x10)
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 438,column 1,is_stmt,address SlNetIfWifi_CreateContext,isa 1

	.dwfde $C$DW$CIE, SlNetIfWifi_CreateContext
$C$DW$340	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$340, DW_AT_name("ifID")
	.dwattr $C$DW$340, DW_AT_TI_symbol_name("ifID")
	.dwattr $C$DW$340, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$340, DW_AT_location[DW_OP_reg0]

$C$DW$341	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$341, DW_AT_name("ifName")
	.dwattr $C$DW$341, DW_AT_TI_symbol_name("ifName")
	.dwattr $C$DW$341, DW_AT_type(*$C$DW$T$351)
	.dwattr $C$DW$341, DW_AT_location[DW_OP_reg1]

$C$DW$342	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$342, DW_AT_name("context")
	.dwattr $C$DW$342, DW_AT_TI_symbol_name("context")
	.dwattr $C$DW$342, DW_AT_type(*$C$DW$T$286)
	.dwattr $C$DW$342, DW_AT_location[DW_OP_reg2]

;----------------------------------------------------------------------
; 437 | int32_t SlNetIfWifi_CreateContext(uint16_t ifID, const char *ifName, vo
;     | id **context)                                                          
;----------------------------------------------------------------------

;*****************************************************************************
;* FUNCTION NAME: SlNetIfWifi_CreateContext                                  *
;*                                                                           *
;*   Regs Modified     : A1,SP,SR                                            *
;*   Regs Used         : A1,A2,A3,SP,LR,SR                                   *
;*   Local Frame Size  : 0 Args + 12 Auto + 0 Save = 12 byte                 *
;*****************************************************************************
SlNetIfWifi_CreateContext:
;* --------------------------------------------------------------------------*
	.dwcfi	cfa_offset, 0
        SUB       SP, SP, #16           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 16
$C$DW$343	.dwtag  DW_TAG_variable
	.dwattr $C$DW$343, DW_AT_name("ifName")
	.dwattr $C$DW$343, DW_AT_TI_symbol_name("ifName")
	.dwattr $C$DW$343, DW_AT_type(*$C$DW$T$351)
	.dwattr $C$DW$343, DW_AT_location[DW_OP_breg13 0]

$C$DW$344	.dwtag  DW_TAG_variable
	.dwattr $C$DW$344, DW_AT_name("context")
	.dwattr $C$DW$344, DW_AT_TI_symbol_name("context")
	.dwattr $C$DW$344, DW_AT_type(*$C$DW$T$286)
	.dwattr $C$DW$344, DW_AT_location[DW_OP_breg13 4]

$C$DW$345	.dwtag  DW_TAG_variable
	.dwattr $C$DW$345, DW_AT_name("ifID")
	.dwattr $C$DW$345, DW_AT_TI_symbol_name("ifID")
	.dwattr $C$DW$345, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$345, DW_AT_location[DW_OP_breg13 8]

        STR       A3, [SP, #4]          ; [DPU_3_PIPE] |438| 
        STR       A2, [SP, #0]          ; [DPU_3_PIPE] |438| 
        STRH      A1, [SP, #8]          ; [DPU_3_PIPE] |438| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 439,column 5,is_stmt,isa 1
;----------------------------------------------------------------------
; 439 | return SLNETERR_RET_CODE_OK;                                           
;----------------------------------------------------------------------
        MOVS      A1, #0                ; [DPU_3_PIPE] |439| 
	.dwpsn	file "/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c",line 440,column 1,is_stmt,isa 1
        ADD       SP, SP, #16           ; [DPU_3_PIPE] 
	.dwcfi	cfa_offset, 0
$C$DW$346	.dwtag  DW_TAG_TI_branch
	.dwattr $C$DW$346, DW_AT_low_pc(0x00)
	.dwattr $C$DW$346, DW_AT_TI_return

        BX        LR                    ; [DPU_3_PIPE] 
        ; BRANCH OCCURS                  ; [] 
	.dwattr $C$DW$339, DW_AT_TI_end_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/slnetif/slnetifwifi.c")
	.dwattr $C$DW$339, DW_AT_TI_end_line(0x1b8)
	.dwattr $C$DW$339, DW_AT_TI_end_column(0x01)
	.dwendentry
	.dwendtag $C$DW$339

;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
	.sect	".text:SlNetIfWifi_socket"
	.align	4
||$C$CON1||:	.bits	socketType,32
;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
	.sect	".text:SlNetIfWifi_getSockName"
	.align	4
||$C$CON2||:	.bits	-2003,32
;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
	.sect	".text:SlNetIfWifi_sockstartSec"
	.align	4
||$C$CON3||:	.bits	StartSecOptName,32
	.align	4
||$C$CON4||:	.bits	-2006,32
;******************************************************************************
;* CONSTANT TABLE                                                             *
;******************************************************************************
	.sect	".text:SlNetIfWifi_loadSecObj"
	.align	4
||$C$CON5||:	.bits	-2006,32
;*****************************************************************************
;* UNDEFINED EXTERNAL REFERENCES                                             *
;*****************************************************************************
	.global	memset
	.global	sl_FsOpen
	.global	sl_FsClose
	.global	sl_FsWrite
	.global	sl_Socket
	.global	sl_Close
	.global	sl_Accept
	.global	sl_Bind
	.global	sl_Listen
	.global	sl_Connect
	.global	sl_Select
	.global	sl_SetSockOpt
	.global	sl_GetSockOpt
	.global	sl_Recv
	.global	sl_RecvFrom
	.global	sl_Send
	.global	sl_SendTo
	.global	sl_StartTLS
	.global	sl_NetAppDnsGetHostByName
	.global	sl_WlanGet
	.global	sl_NetCfgGet

;******************************************************************************
;* BUILD ATTRIBUTES                                                           *
;******************************************************************************
	.battr "aeabi", Tag_File, 1, Tag_ABI_PCS_wchar_t(2)
	.battr "aeabi", Tag_File, 1, Tag_ABI_FP_rounding(0)
	.battr "aeabi", Tag_File, 1, Tag_ABI_FP_denormal(0)
	.battr "aeabi", Tag_File, 1, Tag_ABI_FP_exceptions(0)
	.battr "aeabi", Tag_File, 1, Tag_ABI_FP_number_model(1)
	.battr "aeabi", Tag_File, 1, Tag_ABI_enum_size(1)
	.battr "aeabi", Tag_File, 1, Tag_ABI_optimization_goals(5)
	.battr "aeabi", Tag_File, 1, Tag_ABI_FP_optimization_goals(0)
	.battr "TI", Tag_File, 1, Tag_Bitfield_layout(2)
	.battr "aeabi", Tag_File, 1, Tag_ABI_VFP_args(3)
	.battr "TI", Tag_File, 1, Tag_FP_interface(1)

;******************************************************************************
;* TYPE INFORMATION                                                           *
;******************************************************************************

$C$DW$T$168	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$168, DW_AT_byte_size(0x01)
$C$DW$347	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$347, DW_AT_name("SL_DEVICE_SOURCE_OTHER")
	.dwattr $C$DW$347, DW_AT_const_value(0x00)
	.dwattr $C$DW$347, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$347, DW_AT_decl_line(0x5f)
	.dwattr $C$DW$347, DW_AT_decl_column(0x05)

$C$DW$348	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$348, DW_AT_name("SL_DEVICE_SOURCE_WLAN")
	.dwattr $C$DW$348, DW_AT_const_value(0x01)
	.dwattr $C$DW$348, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$348, DW_AT_decl_line(0x60)
	.dwattr $C$DW$348, DW_AT_decl_column(0x05)

$C$DW$349	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$349, DW_AT_name("SL_DEVICE_SOURCE_NETCFG")
	.dwattr $C$DW$349, DW_AT_const_value(0x02)
	.dwattr $C$DW$349, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$349, DW_AT_decl_line(0x61)
	.dwattr $C$DW$349, DW_AT_decl_column(0x05)

$C$DW$350	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$350, DW_AT_name("SL_DEVICE_SOURCE_NETAPP")
	.dwattr $C$DW$350, DW_AT_const_value(0x03)
	.dwattr $C$DW$350, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$350, DW_AT_decl_line(0x62)
	.dwattr $C$DW$350, DW_AT_decl_column(0x05)

$C$DW$351	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$351, DW_AT_name("SL_DEVICE_SOURCE_SECURITY")
	.dwattr $C$DW$351, DW_AT_const_value(0x04)
	.dwattr $C$DW$351, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$351, DW_AT_decl_line(0x63)
	.dwattr $C$DW$351, DW_AT_decl_column(0x05)

$C$DW$352	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$352, DW_AT_name("SL_DEVICE_SOURCE_LAST")
	.dwattr $C$DW$352, DW_AT_const_value(0xff)
	.dwattr $C$DW$352, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$352, DW_AT_decl_line(0x65)
	.dwattr $C$DW$352, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$168, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$168, DW_AT_decl_line(0x5e)
	.dwattr $C$DW$T$168, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$168

$C$DW$T$169	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$169, DW_AT_name("SlDeviceSource_e")
	.dwattr $C$DW$T$169, DW_AT_type(*$C$DW$T$168)
	.dwattr $C$DW$T$169, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$169, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$169, DW_AT_decl_line(0x66)
	.dwattr $C$DW$T$169, DW_AT_decl_column(0x02)


$C$DW$T$341	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$341, DW_AT_byte_size(0x01)
$C$DW$353	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$353, DW_AT_name("SLNETIF_IPV4_ADDR")
	.dwattr $C$DW$353, DW_AT_const_value(0x00)
	.dwattr $C$DW$353, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$353, DW_AT_decl_line(0x6f)
	.dwattr $C$DW$353, DW_AT_decl_column(0x05)

$C$DW$354	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$354, DW_AT_name("SLNETIF_IPV6_ADDR_LOCAL")
	.dwattr $C$DW$354, DW_AT_const_value(0x01)
	.dwattr $C$DW$354, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$354, DW_AT_decl_line(0x70)
	.dwattr $C$DW$354, DW_AT_decl_column(0x05)

$C$DW$355	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$355, DW_AT_name("SLNETIF_IPV6_ADDR_GLOBAL")
	.dwattr $C$DW$355, DW_AT_const_value(0x02)
	.dwattr $C$DW$355, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$355, DW_AT_decl_line(0x71)
	.dwattr $C$DW$355, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$341, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$341, DW_AT_decl_line(0x6e)
	.dwattr $C$DW$T$341, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$341

$C$DW$T$342	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$342, DW_AT_name("SlNetIfAddressType_e")
	.dwattr $C$DW$T$342, DW_AT_type(*$C$DW$T$341)
	.dwattr $C$DW$T$342, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$342, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$342, DW_AT_decl_line(0x72)
	.dwattr $C$DW$T$342, DW_AT_decl_column(0x03)


$C$DW$T$375	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$375, DW_AT_byte_size(0x01)
$C$DW$356	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$356, DW_AT_name("SLNETSOCK_SEC_ATTRIB_PRIVATE_KEY")
	.dwattr $C$DW$356, DW_AT_const_value(0x00)
	.dwattr $C$DW$356, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$356, DW_AT_decl_line(0x139)
	.dwattr $C$DW$356, DW_AT_decl_column(0x06)

$C$DW$357	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$357, DW_AT_name("SLNETSOCK_SEC_ATTRIB_LOCAL_CERT")
	.dwattr $C$DW$357, DW_AT_const_value(0x01)
	.dwattr $C$DW$357, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$357, DW_AT_decl_line(0x13a)
	.dwattr $C$DW$357, DW_AT_decl_column(0x06)

$C$DW$358	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$358, DW_AT_name("SLNETSOCK_SEC_ATTRIB_PEER_ROOT_CA")
	.dwattr $C$DW$358, DW_AT_const_value(0x02)
	.dwattr $C$DW$358, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$358, DW_AT_decl_line(0x13b)
	.dwattr $C$DW$358, DW_AT_decl_column(0x06)

$C$DW$359	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$359, DW_AT_name("SLNETSOCK_SEC_ATTRIB_DH_KEY")
	.dwattr $C$DW$359, DW_AT_const_value(0x03)
	.dwattr $C$DW$359, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$359, DW_AT_decl_line(0x13c)
	.dwattr $C$DW$359, DW_AT_decl_column(0x06)

$C$DW$360	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$360, DW_AT_name("SLNETSOCK_SEC_ATTRIB_METHOD")
	.dwattr $C$DW$360, DW_AT_const_value(0x04)
	.dwattr $C$DW$360, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$360, DW_AT_decl_line(0x13d)
	.dwattr $C$DW$360, DW_AT_decl_column(0x06)

$C$DW$361	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$361, DW_AT_name("SLNETSOCK_SEC_ATTRIB_CIPHERS")
	.dwattr $C$DW$361, DW_AT_const_value(0x05)
	.dwattr $C$DW$361, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$361, DW_AT_decl_line(0x13e)
	.dwattr $C$DW$361, DW_AT_decl_column(0x06)

$C$DW$362	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$362, DW_AT_name("SLNETSOCK_SEC_ATTRIB_ALPN")
	.dwattr $C$DW$362, DW_AT_const_value(0x06)
	.dwattr $C$DW$362, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$362, DW_AT_decl_line(0x13f)
	.dwattr $C$DW$362, DW_AT_decl_column(0x06)

$C$DW$363	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$363, DW_AT_name("SLNETSOCK_SEC_ATTRIB_EXT_CLIENT_CHLNG_RESP")
	.dwattr $C$DW$363, DW_AT_const_value(0x07)
	.dwattr $C$DW$363, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$363, DW_AT_decl_line(0x140)
	.dwattr $C$DW$363, DW_AT_decl_column(0x06)

$C$DW$364	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$364, DW_AT_name("SLNETSOCK_SEC_ATTRIB_DOMAIN_NAME")
	.dwattr $C$DW$364, DW_AT_const_value(0x08)
	.dwattr $C$DW$364, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$364, DW_AT_decl_line(0x141)
	.dwattr $C$DW$364, DW_AT_decl_column(0x06)

$C$DW$365	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$365, DW_AT_name("SLNETSOCK_SEC_ATTRIB_DISABLE_CERT_STORE")
	.dwattr $C$DW$365, DW_AT_const_value(0x09)
	.dwattr $C$DW$365, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$365, DW_AT_decl_line(0x153)
	.dwattr $C$DW$365, DW_AT_decl_column(0x06)

	.dwattr $C$DW$T$375, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$375, DW_AT_decl_line(0x138)
	.dwattr $C$DW$T$375, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$375

$C$DW$T$376	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$376, DW_AT_name("SlNetSockSecAttrib_e")
	.dwattr $C$DW$T$376, DW_AT_type(*$C$DW$T$375)
	.dwattr $C$DW$T$376, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$376, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$376, DW_AT_decl_line(0x154)
	.dwattr $C$DW$T$376, DW_AT_decl_column(0x03)


$C$DW$T$395	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$395, DW_AT_byte_size(0x01)
$C$DW$366	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$366, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_MIN")
	.dwattr $C$DW$366, DW_AT_const_value(0x01)
	.dwattr $C$DW$366, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$366, DW_AT_decl_line(0x129)
	.dwattr $C$DW$366, DW_AT_decl_column(0x05)

$C$DW$367	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$367, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_LOW")
	.dwattr $C$DW$367, DW_AT_const_value(0x02)
	.dwattr $C$DW$367, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$367, DW_AT_decl_line(0x12a)
	.dwattr $C$DW$367, DW_AT_decl_column(0x05)

$C$DW$368	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$368, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_DEFAULT")
	.dwattr $C$DW$368, DW_AT_const_value(0x03)
	.dwattr $C$DW$368, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$368, DW_AT_decl_line(0x12b)
	.dwattr $C$DW$368, DW_AT_decl_column(0x05)

$C$DW$369	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$369, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_MED")
	.dwattr $C$DW$369, DW_AT_const_value(0x04)
	.dwattr $C$DW$369, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$369, DW_AT_decl_line(0x12c)
	.dwattr $C$DW$369, DW_AT_decl_column(0x05)

$C$DW$370	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$370, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_HIGH")
	.dwattr $C$DW$370, DW_AT_const_value(0x05)
	.dwattr $C$DW$370, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$370, DW_AT_decl_line(0x12d)
	.dwattr $C$DW$370, DW_AT_decl_column(0x05)

$C$DW$371	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$371, DW_AT_name("SLNETSOCK_TX_INHIBIT_THRESHOLD_MAX")
	.dwattr $C$DW$371, DW_AT_const_value(0x06)
	.dwattr $C$DW$371, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$371, DW_AT_decl_line(0x12e)
	.dwattr $C$DW$371, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$395, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$395, DW_AT_decl_line(0x128)
	.dwattr $C$DW$T$395, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$395

$C$DW$T$396	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$396, DW_AT_name("SlNetSockTxInhibitThreshold_e")
	.dwattr $C$DW$T$396, DW_AT_type(*$C$DW$T$395)
	.dwattr $C$DW$T$396, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$396, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$396, DW_AT_decl_line(0x12f)
	.dwattr $C$DW$T$396, DW_AT_decl_column(0x03)


$C$DW$T$397	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$397, DW_AT_byte_size(0x01)
$C$DW$372	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$372, DW_AT_name("SLNETIF_STATE_DISABLE")
	.dwattr $C$DW$372, DW_AT_const_value(0x00)
	.dwattr $C$DW$372, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$372, DW_AT_decl_line(0x66)
	.dwattr $C$DW$372, DW_AT_decl_column(0x05)

$C$DW$373	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$373, DW_AT_name("SLNETIF_STATE_ENABLE")
	.dwattr $C$DW$373, DW_AT_const_value(0x01)
	.dwattr $C$DW$373, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$373, DW_AT_decl_line(0x67)
	.dwattr $C$DW$373, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$397, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$397, DW_AT_decl_line(0x65)
	.dwattr $C$DW$T$397, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$397

$C$DW$T$398	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$398, DW_AT_name("SlNetIfState_e")
	.dwattr $C$DW$T$398, DW_AT_type(*$C$DW$T$397)
	.dwattr $C$DW$T$398, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$398, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$398, DW_AT_decl_line(0x68)
	.dwattr $C$DW$T$398, DW_AT_decl_column(0x03)


$C$DW$T$399	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$399, DW_AT_byte_size(0x01)
$C$DW$374	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$374, DW_AT_name("EVENT_PROPAGATION_BLOCK")
	.dwattr $C$DW$374, DW_AT_const_value(0x00)
	.dwattr $C$DW$374, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$374, DW_AT_decl_line(0x188)
	.dwattr $C$DW$374, DW_AT_decl_column(0x05)

$C$DW$375	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$375, DW_AT_name("EVENT_PROPAGATION_CONTINUE")
	.dwattr $C$DW$375, DW_AT_const_value(0x01)
	.dwattr $C$DW$375, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$375, DW_AT_decl_line(0x189)
	.dwattr $C$DW$375, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$399, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$399, DW_AT_decl_line(0x187)
	.dwattr $C$DW$T$399, DW_AT_decl_column(0x0f)
	.dwendtag $C$DW$T$399

$C$DW$T$400	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$400, DW_AT_name("_SlEventPropogationStatus_e")
	.dwattr $C$DW$T$400, DW_AT_type(*$C$DW$T$399)
	.dwattr $C$DW$T$400, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$400, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$400, DW_AT_decl_line(0x18a)
	.dwattr $C$DW$T$400, DW_AT_decl_column(0x04)


$C$DW$T$401	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$401, DW_AT_byte_size(0x01)
$C$DW$376	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$376, DW_AT_name("SL_FS_TOKEN_MASTER")
	.dwattr $C$DW$376, DW_AT_const_value(0x00)
	.dwattr $C$DW$376, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$376, DW_AT_decl_line(0x88)
	.dwattr $C$DW$376, DW_AT_decl_column(0x05)

$C$DW$377	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$377, DW_AT_name("SL_FS_TOKEN_WRITE_READ")
	.dwattr $C$DW$377, DW_AT_const_value(0x01)
	.dwattr $C$DW$377, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$377, DW_AT_decl_line(0x89)
	.dwattr $C$DW$377, DW_AT_decl_column(0x05)

$C$DW$378	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$378, DW_AT_name("SL_FS_TOKEN_WRITE_ONLY")
	.dwattr $C$DW$378, DW_AT_const_value(0x02)
	.dwattr $C$DW$378, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$378, DW_AT_decl_line(0x8a)
	.dwattr $C$DW$378, DW_AT_decl_column(0x05)

$C$DW$379	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$379, DW_AT_name("SL_FS_TOKEN_READ_ONLY")
	.dwattr $C$DW$379, DW_AT_const_value(0x03)
	.dwattr $C$DW$379, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$379, DW_AT_decl_line(0x8b)
	.dwattr $C$DW$379, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$401, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$401, DW_AT_decl_line(0x87)
	.dwattr $C$DW$T$401, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$401

$C$DW$T$402	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$402, DW_AT_name("SlFsTokenId_e")
	.dwattr $C$DW$T$402, DW_AT_type(*$C$DW$T$401)
	.dwattr $C$DW$T$402, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$402, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$402, DW_AT_decl_line(0x8c)
	.dwattr $C$DW$T$402, DW_AT_decl_column(0x02)


$C$DW$T$403	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$403, DW_AT_byte_size(0x01)
$C$DW$380	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$380, DW_AT_name("SL_FS_CTL_RESTORE")
	.dwattr $C$DW$380, DW_AT_const_value(0x00)
	.dwattr $C$DW$380, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$380, DW_AT_decl_line(0x9f)
	.dwattr $C$DW$380, DW_AT_decl_column(0x03)

$C$DW$381	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$381, DW_AT_name("SL_FS_CTL_ROLLBACK")
	.dwattr $C$DW$381, DW_AT_const_value(0x01)
	.dwattr $C$DW$381, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$381, DW_AT_decl_line(0xa0)
	.dwattr $C$DW$381, DW_AT_decl_column(0x03)

$C$DW$382	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$382, DW_AT_name("SL_FS_CTL_COMMIT")
	.dwattr $C$DW$382, DW_AT_const_value(0x02)
	.dwattr $C$DW$382, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$382, DW_AT_decl_line(0xa1)
	.dwattr $C$DW$382, DW_AT_decl_column(0x03)

$C$DW$383	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$383, DW_AT_name("SL_FS_CTL_RENAME")
	.dwattr $C$DW$383, DW_AT_const_value(0x03)
	.dwattr $C$DW$383, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$383, DW_AT_decl_line(0xa2)
	.dwattr $C$DW$383, DW_AT_decl_column(0x03)

$C$DW$384	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$384, DW_AT_name("SL_FS_CTL_GET_STORAGE_INFO")
	.dwattr $C$DW$384, DW_AT_const_value(0x05)
	.dwattr $C$DW$384, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$384, DW_AT_decl_line(0xa3)
	.dwattr $C$DW$384, DW_AT_decl_column(0x03)

$C$DW$385	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$385, DW_AT_name("SL_FS_CTL_BUNDLE_ROLLBACK")
	.dwattr $C$DW$385, DW_AT_const_value(0x06)
	.dwattr $C$DW$385, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$385, DW_AT_decl_line(0xa4)
	.dwattr $C$DW$385, DW_AT_decl_column(0x03)

$C$DW$386	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$386, DW_AT_name("SL_FS_CTL_BUNDLE_COMMIT")
	.dwattr $C$DW$386, DW_AT_const_value(0x07)
	.dwattr $C$DW$386, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$386, DW_AT_decl_line(0xa5)
	.dwattr $C$DW$386, DW_AT_decl_column(0x03)

	.dwattr $C$DW$T$403, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$403, DW_AT_decl_line(0x9e)
	.dwattr $C$DW$T$403, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$403

$C$DW$T$404	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$404, DW_AT_name("SlFsCtl_e")
	.dwattr $C$DW$T$404, DW_AT_type(*$C$DW$T$403)
	.dwattr $C$DW$T$404, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$404, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$404, DW_AT_decl_line(0xa6)
	.dwattr $C$DW$T$404, DW_AT_decl_column(0x02)


$C$DW$T$405	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$405, DW_AT_byte_size(0x01)
$C$DW$387	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$387, DW_AT_name("SL_FS_BUNDLE_STATE_STOPPED")
	.dwattr $C$DW$387, DW_AT_const_value(0x00)
	.dwattr $C$DW$387, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$387, DW_AT_decl_line(0xaa)
	.dwattr $C$DW$387, DW_AT_decl_column(0x03)

$C$DW$388	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$388, DW_AT_name("SL_FS_BUNDLE_STATE_STARTED")
	.dwattr $C$DW$388, DW_AT_const_value(0x01)
	.dwattr $C$DW$388, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$388, DW_AT_decl_line(0xab)
	.dwattr $C$DW$388, DW_AT_decl_column(0x03)

$C$DW$389	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$389, DW_AT_name("SL_FS_BUNDLE_STATE_PENDING_COMMIT")
	.dwattr $C$DW$389, DW_AT_const_value(0x03)
	.dwattr $C$DW$389, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$389, DW_AT_decl_line(0xac)
	.dwattr $C$DW$389, DW_AT_decl_column(0x03)

	.dwattr $C$DW$T$405, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$405, DW_AT_decl_line(0xa9)
	.dwattr $C$DW$T$405, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$405

$C$DW$T$406	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$406, DW_AT_name("SlFsBundleState_e")
	.dwattr $C$DW$T$406, DW_AT_type(*$C$DW$T$405)
	.dwattr $C$DW$T$406, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$406, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$406, DW_AT_decl_line(0xad)
	.dwattr $C$DW$T$406, DW_AT_decl_column(0x02)


$C$DW$T$407	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$407, DW_AT_byte_size(0x01)
$C$DW$390	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$390, DW_AT_name("SL_FS_FACTORY_RET_TO_IMAGE")
	.dwattr $C$DW$390, DW_AT_const_value(0x00)
	.dwattr $C$DW$390, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$390, DW_AT_decl_line(0xc4)
	.dwattr $C$DW$390, DW_AT_decl_column(0x03)

$C$DW$391	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$391, DW_AT_name("SL_FS_FACTORY_RET_TO_DEFAULT")
	.dwattr $C$DW$391, DW_AT_const_value(0x02)
	.dwattr $C$DW$391, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$391, DW_AT_decl_line(0xc5)
	.dwattr $C$DW$391, DW_AT_decl_column(0x03)

	.dwattr $C$DW$T$407, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$407, DW_AT_decl_line(0xc3)
	.dwattr $C$DW$T$407, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$407

$C$DW$T$408	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$408, DW_AT_name("SlFsRetToFactoryOper_e")
	.dwattr $C$DW$T$408, DW_AT_type(*$C$DW$T$407)
	.dwattr $C$DW$T$408, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$408, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$408, DW_AT_decl_line(0xc6)
	.dwattr $C$DW$T$408, DW_AT_decl_column(0x02)


$C$DW$T$409	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$409, DW_AT_byte_size(0x01)
$C$DW$392	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$392, DW_AT_name("SL_FS_GET_FILE_ATTRIBUTES")
	.dwattr $C$DW$392, DW_AT_const_value(0x01)
	.dwattr $C$DW$392, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$392, DW_AT_decl_line(0x109)
	.dwattr $C$DW$392, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$409, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$409, DW_AT_decl_line(0x108)
	.dwattr $C$DW$T$409, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$409

$C$DW$T$410	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$410, DW_AT_name("SlFileListFlags_t")
	.dwattr $C$DW$T$410, DW_AT_type(*$C$DW$T$409)
	.dwattr $C$DW$T$410, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$410, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$410, DW_AT_decl_line(0x10a)
	.dwattr $C$DW$T$410, DW_AT_decl_column(0x02)


$C$DW$T$411	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$411, DW_AT_byte_size(0x01)
$C$DW$393	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$393, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_MIN")
	.dwattr $C$DW$393, DW_AT_const_value(0x01)
	.dwattr $C$DW$393, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$393, DW_AT_decl_line(0x95)
	.dwattr $C$DW$393, DW_AT_decl_column(0x05)

$C$DW$394	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$394, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_LOW")
	.dwattr $C$DW$394, DW_AT_const_value(0x02)
	.dwattr $C$DW$394, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$394, DW_AT_decl_line(0x96)
	.dwattr $C$DW$394, DW_AT_decl_column(0x05)

$C$DW$395	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$395, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_DEFAULT")
	.dwattr $C$DW$395, DW_AT_const_value(0x03)
	.dwattr $C$DW$395, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$395, DW_AT_decl_line(0x97)
	.dwattr $C$DW$395, DW_AT_decl_column(0x05)

$C$DW$396	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$396, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_MED")
	.dwattr $C$DW$396, DW_AT_const_value(0x04)
	.dwattr $C$DW$396, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$396, DW_AT_decl_line(0x98)
	.dwattr $C$DW$396, DW_AT_decl_column(0x05)

$C$DW$397	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$397, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_HIGH")
	.dwattr $C$DW$397, DW_AT_const_value(0x05)
	.dwattr $C$DW$397, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$397, DW_AT_decl_line(0x99)
	.dwattr $C$DW$397, DW_AT_decl_column(0x05)

$C$DW$398	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$398, DW_AT_name("SL_TX_INHIBIT_THRESHOLD_MAX")
	.dwattr $C$DW$398, DW_AT_const_value(0x06)
	.dwattr $C$DW$398, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$398, DW_AT_decl_line(0x9a)
	.dwattr $C$DW$398, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$411, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$411, DW_AT_decl_line(0x94)
	.dwattr $C$DW$T$411, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$411

$C$DW$T$412	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$412, DW_AT_name("SlTxInhibitThreshold_e")
	.dwattr $C$DW$T$412, DW_AT_type(*$C$DW$T$411)
	.dwattr $C$DW$T$412, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$412, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$412, DW_AT_decl_line(0x9b)
	.dwattr $C$DW$T$412, DW_AT_decl_column(0x03)


$C$DW$T$413	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$413, DW_AT_byte_size(0x01)
$C$DW$399	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$399, DW_AT_name("SL_BSD_SECURED_PRIVATE_KEY_IDX")
	.dwattr $C$DW$399, DW_AT_const_value(0x00)
	.dwattr $C$DW$399, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$399, DW_AT_decl_line(0x15f)
	.dwattr $C$DW$399, DW_AT_decl_column(0x03)

$C$DW$400	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$400, DW_AT_name("SL_BSD_SECURED_CERTIFICATE_IDX")
	.dwattr $C$DW$400, DW_AT_const_value(0x01)
	.dwattr $C$DW$400, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$400, DW_AT_decl_line(0x160)
	.dwattr $C$DW$400, DW_AT_decl_column(0x03)

$C$DW$401	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$401, DW_AT_name("SL_BSD_SECURED_CA_IDX")
	.dwattr $C$DW$401, DW_AT_const_value(0x02)
	.dwattr $C$DW$401, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$401, DW_AT_decl_line(0x161)
	.dwattr $C$DW$401, DW_AT_decl_column(0x03)

$C$DW$402	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$402, DW_AT_name("SL_BSD_SECURED_DH_IDX")
	.dwattr $C$DW$402, DW_AT_const_value(0x03)
	.dwattr $C$DW$402, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$402, DW_AT_decl_line(0x162)
	.dwattr $C$DW$402, DW_AT_decl_column(0x03)

	.dwattr $C$DW$T$413, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$413, DW_AT_decl_line(0x15e)
	.dwattr $C$DW$T$413, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$413

$C$DW$T$414	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$414, DW_AT_name("SlSockSecureSocketFilesIndex_e")
	.dwattr $C$DW$T$414, DW_AT_type(*$C$DW$T$413)
	.dwattr $C$DW$T$414, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$414, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$414, DW_AT_decl_line(0x163)
	.dwattr $C$DW$T$414, DW_AT_decl_column(0x02)


$C$DW$T$415	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$415, DW_AT_byte_size(0x01)
$C$DW$403	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$403, DW_AT_name("SL_NETAPP_EVENT_IPV4_ACQUIRED")
	.dwattr $C$DW$403, DW_AT_const_value(0x01)
	.dwattr $C$DW$403, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$403, DW_AT_decl_line(0x4a)
	.dwattr $C$DW$403, DW_AT_decl_column(0x05)

$C$DW$404	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$404, DW_AT_name("SL_NETAPP_EVENT_IPV6_ACQUIRED")
	.dwattr $C$DW$404, DW_AT_const_value(0x02)
	.dwattr $C$DW$404, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$404, DW_AT_decl_line(0x4b)
	.dwattr $C$DW$404, DW_AT_decl_column(0x05)

$C$DW$405	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$405, DW_AT_name("SL_NETAPP_EVENT_IP_COLLISION")
	.dwattr $C$DW$405, DW_AT_const_value(0x03)
	.dwattr $C$DW$405, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$405, DW_AT_decl_line(0x4c)
	.dwattr $C$DW$405, DW_AT_decl_column(0x05)

$C$DW$406	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$406, DW_AT_name("SL_NETAPP_EVENT_DHCPV4_LEASED")
	.dwattr $C$DW$406, DW_AT_const_value(0x04)
	.dwattr $C$DW$406, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$406, DW_AT_decl_line(0x4d)
	.dwattr $C$DW$406, DW_AT_decl_column(0x05)

$C$DW$407	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$407, DW_AT_name("SL_NETAPP_EVENT_DHCPV4_RELEASED")
	.dwattr $C$DW$407, DW_AT_const_value(0x05)
	.dwattr $C$DW$407, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$407, DW_AT_decl_line(0x4e)
	.dwattr $C$DW$407, DW_AT_decl_column(0x05)

$C$DW$408	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$408, DW_AT_name("SL_NETAPP_EVENT_HTTP_TOKEN_GET")
	.dwattr $C$DW$408, DW_AT_const_value(0x06)
	.dwattr $C$DW$408, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$408, DW_AT_decl_line(0x4f)
	.dwattr $C$DW$408, DW_AT_decl_column(0x05)

$C$DW$409	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$409, DW_AT_name("SL_NETAPP_EVENT_HTTP_TOKEN_POST")
	.dwattr $C$DW$409, DW_AT_const_value(0x07)
	.dwattr $C$DW$409, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$409, DW_AT_decl_line(0x50)
	.dwattr $C$DW$409, DW_AT_decl_column(0x05)

$C$DW$410	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$410, DW_AT_name("SL_NETAPP_EVENT_IPV4_LOST")
	.dwattr $C$DW$410, DW_AT_const_value(0x08)
	.dwattr $C$DW$410, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$410, DW_AT_decl_line(0x51)
	.dwattr $C$DW$410, DW_AT_decl_column(0x05)

$C$DW$411	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$411, DW_AT_name("SL_NETAPP_EVENT_DHCP_IPV4_ACQUIRE_TIMEOUT")
	.dwattr $C$DW$411, DW_AT_const_value(0x09)
	.dwattr $C$DW$411, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$411, DW_AT_decl_line(0x52)
	.dwattr $C$DW$411, DW_AT_decl_column(0x05)

$C$DW$412	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$412, DW_AT_name("SL_NETAPP_EVENT_IPV6_LOST")
	.dwattr $C$DW$412, DW_AT_const_value(0x0a)
	.dwattr $C$DW$412, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$412, DW_AT_decl_line(0x53)
	.dwattr $C$DW$412, DW_AT_decl_column(0x05)

$C$DW$413	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$413, DW_AT_name("SL_NETAPP_EVENT_MAX")
	.dwattr $C$DW$413, DW_AT_const_value(0x0b)
	.dwattr $C$DW$413, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$413, DW_AT_decl_line(0x54)
	.dwattr $C$DW$413, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$415, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$415, DW_AT_decl_line(0x49)
	.dwattr $C$DW$T$415, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$415

$C$DW$T$416	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$416, DW_AT_name("SlNetAppEventId_e")
	.dwattr $C$DW$T$416, DW_AT_type(*$C$DW$T$415)
	.dwattr $C$DW$T$416, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$416, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$416, DW_AT_decl_line(0x55)
	.dwattr $C$DW$T$416, DW_AT_decl_column(0x03)


$C$DW$T$417	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$417, DW_AT_byte_size(0x01)
$C$DW$414	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$414, DW_AT_name("SL_BSD_IPV6_ACQUIRED_TYPE_LOCAL")
	.dwattr $C$DW$414, DW_AT_const_value(0x01)
	.dwattr $C$DW$414, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$414, DW_AT_decl_line(0xd9)
	.dwattr $C$DW$414, DW_AT_decl_column(0x05)

$C$DW$415	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$415, DW_AT_name("SL_BSD_IPV6_ACQUIRED_TYPE_GLOBAL")
	.dwattr $C$DW$415, DW_AT_const_value(0x02)
	.dwattr $C$DW$415, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$415, DW_AT_decl_line(0xda)
	.dwattr $C$DW$415, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$417, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$417, DW_AT_decl_line(0xd8)
	.dwattr $C$DW$T$417, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$417

$C$DW$T$418	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$418, DW_AT_name("SlIpV6AcquiredAsyncType_e")
	.dwattr $C$DW$T$418, DW_AT_type(*$C$DW$T$417)
	.dwattr $C$DW$T$418, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$418, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$418, DW_AT_decl_line(0xdb)
	.dwattr $C$DW$T$418, DW_AT_decl_column(0x02)


$C$DW$T$419	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$419, DW_AT_byte_size(0x01)
$C$DW$416	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$416, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_STATUS")
	.dwattr $C$DW$416, DW_AT_const_value(0x00)
	.dwattr $C$DW$416, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$416, DW_AT_decl_line(0x170)
	.dwattr $C$DW$416, DW_AT_decl_column(0x05)

$C$DW$417	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$417, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_VERSION")
	.dwattr $C$DW$417, DW_AT_const_value(0x01)
	.dwattr $C$DW$417, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$417, DW_AT_decl_line(0x171)
	.dwattr $C$DW$417, DW_AT_decl_column(0x05)

$C$DW$418	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$418, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI")
	.dwattr $C$DW$418, DW_AT_const_value(0x02)
	.dwattr $C$DW$418, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$418, DW_AT_decl_line(0x172)
	.dwattr $C$DW$418, DW_AT_decl_column(0x05)

$C$DW$419	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$419, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING")
	.dwattr $C$DW$419, DW_AT_const_value(0x03)
	.dwattr $C$DW$419, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$419, DW_AT_decl_line(0x173)
	.dwattr $C$DW$419, DW_AT_decl_column(0x05)

$C$DW$420	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$420, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN")
	.dwattr $C$DW$420, DW_AT_const_value(0x04)
	.dwattr $C$DW$420, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$420, DW_AT_decl_line(0x174)
	.dwattr $C$DW$420, DW_AT_decl_column(0x05)

$C$DW$421	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$421, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE")
	.dwattr $C$DW$421, DW_AT_const_value(0x05)
	.dwattr $C$DW$421, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$421, DW_AT_decl_line(0x175)
	.dwattr $C$DW$421, DW_AT_decl_column(0x05)

$C$DW$422	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$422, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_LOCATION")
	.dwattr $C$DW$422, DW_AT_const_value(0x06)
	.dwattr $C$DW$422, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$422, DW_AT_decl_line(0x176)
	.dwattr $C$DW$422, DW_AT_decl_column(0x05)

$C$DW$423	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$423, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SERVER")
	.dwattr $C$DW$423, DW_AT_const_value(0x07)
	.dwattr $C$DW$423, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$423, DW_AT_decl_line(0x177)
	.dwattr $C$DW$423, DW_AT_decl_column(0x05)

$C$DW$424	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$424, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_USER_AGENT")
	.dwattr $C$DW$424, DW_AT_const_value(0x08)
	.dwattr $C$DW$424, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$424, DW_AT_decl_line(0x178)
	.dwattr $C$DW$424, DW_AT_decl_column(0x05)

$C$DW$425	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$425, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_COOKIE")
	.dwattr $C$DW$425, DW_AT_const_value(0x09)
	.dwattr $C$DW$425, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$425, DW_AT_decl_line(0x179)
	.dwattr $C$DW$425, DW_AT_decl_column(0x05)

$C$DW$426	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$426, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SET_COOKIE")
	.dwattr $C$DW$426, DW_AT_const_value(0x0a)
	.dwattr $C$DW$426, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$426, DW_AT_decl_line(0x17a)
	.dwattr $C$DW$426, DW_AT_decl_column(0x05)

$C$DW$427	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$427, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_UPGRADE")
	.dwattr $C$DW$427, DW_AT_const_value(0x0b)
	.dwattr $C$DW$427, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$427, DW_AT_decl_line(0x17b)
	.dwattr $C$DW$427, DW_AT_decl_column(0x05)

$C$DW$428	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$428, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REFERER")
	.dwattr $C$DW$428, DW_AT_const_value(0x0c)
	.dwattr $C$DW$428, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$428, DW_AT_decl_line(0x17c)
	.dwattr $C$DW$428, DW_AT_decl_column(0x05)

$C$DW$429	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$429, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ACCEPT")
	.dwattr $C$DW$429, DW_AT_const_value(0x0d)
	.dwattr $C$DW$429, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$429, DW_AT_decl_line(0x17d)
	.dwattr $C$DW$429, DW_AT_decl_column(0x05)

$C$DW$430	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$430, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_ENCODING")
	.dwattr $C$DW$430, DW_AT_const_value(0x0e)
	.dwattr $C$DW$430, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$430, DW_AT_decl_line(0x17e)
	.dwattr $C$DW$430, DW_AT_decl_column(0x05)

$C$DW$431	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$431, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_DISPOSITION")
	.dwattr $C$DW$431, DW_AT_const_value(0x0f)
	.dwattr $C$DW$431, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$431, DW_AT_decl_line(0x17f)
	.dwattr $C$DW$431, DW_AT_decl_column(0x05)

$C$DW$432	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$432, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONNECTION")
	.dwattr $C$DW$432, DW_AT_const_value(0x10)
	.dwattr $C$DW$432, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$432, DW_AT_decl_line(0x180)
	.dwattr $C$DW$432, DW_AT_decl_column(0x05)

$C$DW$433	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$433, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ETAG")
	.dwattr $C$DW$433, DW_AT_const_value(0x11)
	.dwattr $C$DW$433, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$433, DW_AT_decl_line(0x181)
	.dwattr $C$DW$433, DW_AT_decl_column(0x05)

$C$DW$434	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$434, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_DATE")
	.dwattr $C$DW$434, DW_AT_const_value(0x12)
	.dwattr $C$DW$434, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$434, DW_AT_decl_line(0x182)
	.dwattr $C$DW$434, DW_AT_decl_column(0x05)

$C$DW$435	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$435, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HEADER_HOST")
	.dwattr $C$DW$435, DW_AT_const_value(0x13)
	.dwattr $C$DW$435, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$435, DW_AT_decl_line(0x183)
	.dwattr $C$DW$435, DW_AT_decl_column(0x05)

$C$DW$436	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$436, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_ENCODING")
	.dwattr $C$DW$436, DW_AT_const_value(0x14)
	.dwattr $C$DW$436, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$436, DW_AT_decl_line(0x184)
	.dwattr $C$DW$436, DW_AT_decl_column(0x05)

$C$DW$437	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$437, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_LANGUAGE")
	.dwattr $C$DW$437, DW_AT_const_value(0x15)
	.dwattr $C$DW$437, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$437, DW_AT_decl_line(0x185)
	.dwattr $C$DW$437, DW_AT_decl_column(0x05)

$C$DW$438	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$438, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_CONTENT_LANGUAGE")
	.dwattr $C$DW$438, DW_AT_const_value(0x16)
	.dwattr $C$DW$438, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$438, DW_AT_decl_line(0x186)
	.dwattr $C$DW$438, DW_AT_decl_column(0x05)

$C$DW$439	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$439, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_ORIGIN")
	.dwattr $C$DW$439, DW_AT_const_value(0x17)
	.dwattr $C$DW$439, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$439, DW_AT_decl_line(0x187)
	.dwattr $C$DW$439, DW_AT_decl_column(0x05)

$C$DW$440	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$440, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_ORIGIN_CONTROL_ACCESS")
	.dwattr $C$DW$440, DW_AT_const_value(0x18)
	.dwattr $C$DW$440, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$440, DW_AT_decl_line(0x188)
	.dwattr $C$DW$440, DW_AT_decl_column(0x05)

$C$DW$441	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$441, DW_AT_name("SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_NONE")
	.dwattr $C$DW$441, DW_AT_const_value(0x19)
	.dwattr $C$DW$441, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$441, DW_AT_decl_line(0x189)
	.dwattr $C$DW$441, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$419, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$419, DW_AT_decl_line(0x16f)
	.dwattr $C$DW$T$419, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$419

$C$DW$T$420	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$420, DW_AT_name("SlNetAppMetadataHTTPTypes_e")
	.dwattr $C$DW$T$420, DW_AT_type(*$C$DW$T$419)
	.dwattr $C$DW$T$420, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$420, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$420, DW_AT_decl_line(0x18a)
	.dwattr $C$DW$T$420, DW_AT_decl_column(0x03)


$C$DW$T$421	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$421, DW_AT_byte_size(0x02)
$C$DW$442	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$442, DW_AT_name("SL_NETAPP_RESPONSE_NONE")
	.dwattr $C$DW$442, DW_AT_const_value(0x00)
	.dwattr $C$DW$442, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$442, DW_AT_decl_line(0x18e)
	.dwattr $C$DW$442, DW_AT_decl_column(0x05)

$C$DW$443	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$443, DW_AT_name("SL_NETAPP_RESPONSE_PENDING")
	.dwattr $C$DW$443, DW_AT_const_value(0x01)
	.dwattr $C$DW$443, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$443, DW_AT_decl_line(0x18f)
	.dwattr $C$DW$443, DW_AT_decl_column(0x05)

$C$DW$444	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$444, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_101_SWITCHING_PROTOCOLS")
	.dwattr $C$DW$444, DW_AT_const_value(0x65)
	.dwattr $C$DW$444, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$444, DW_AT_decl_line(0x191)
	.dwattr $C$DW$444, DW_AT_decl_column(0x05)

$C$DW$445	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$445, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_200_OK")
	.dwattr $C$DW$445, DW_AT_const_value(0xc8)
	.dwattr $C$DW$445, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$445, DW_AT_decl_line(0x192)
	.dwattr $C$DW$445, DW_AT_decl_column(0x05)

$C$DW$446	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$446, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_201_CREATED")
	.dwattr $C$DW$446, DW_AT_const_value(0xc9)
	.dwattr $C$DW$446, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$446, DW_AT_decl_line(0x193)
	.dwattr $C$DW$446, DW_AT_decl_column(0x05)

$C$DW$447	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$447, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_202_ACCEPTED")
	.dwattr $C$DW$447, DW_AT_const_value(0xca)
	.dwattr $C$DW$447, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$447, DW_AT_decl_line(0x194)
	.dwattr $C$DW$447, DW_AT_decl_column(0x05)

$C$DW$448	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$448, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_204_OK_NO_CONTENT")
	.dwattr $C$DW$448, DW_AT_const_value(0xcc)
	.dwattr $C$DW$448, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$448, DW_AT_decl_line(0x195)
	.dwattr $C$DW$448, DW_AT_decl_column(0x05)

$C$DW$449	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$449, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_301_MOVED_PERMANENTLY")
	.dwattr $C$DW$449, DW_AT_const_value(0x12d)
	.dwattr $C$DW$449, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$449, DW_AT_decl_line(0x196)
	.dwattr $C$DW$449, DW_AT_decl_column(0x05)

$C$DW$450	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$450, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_302_MOVED_TEMPORARILY")
	.dwattr $C$DW$450, DW_AT_const_value(0x12e)
	.dwattr $C$DW$450, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$450, DW_AT_decl_line(0x197)
	.dwattr $C$DW$450, DW_AT_decl_column(0x05)

$C$DW$451	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$451, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_303_SEE_OTHER")
	.dwattr $C$DW$451, DW_AT_const_value(0x12f)
	.dwattr $C$DW$451, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$451, DW_AT_decl_line(0x198)
	.dwattr $C$DW$451, DW_AT_decl_column(0x05)

$C$DW$452	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$452, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_304_NOT_MODIFIED")
	.dwattr $C$DW$452, DW_AT_const_value(0x130)
	.dwattr $C$DW$452, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$452, DW_AT_decl_line(0x199)
	.dwattr $C$DW$452, DW_AT_decl_column(0x05)

$C$DW$453	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$453, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_400_BAD_REQUEST")
	.dwattr $C$DW$453, DW_AT_const_value(0x190)
	.dwattr $C$DW$453, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$453, DW_AT_decl_line(0x19a)
	.dwattr $C$DW$453, DW_AT_decl_column(0x05)

$C$DW$454	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$454, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_403_FORBIDDEN")
	.dwattr $C$DW$454, DW_AT_const_value(0x193)
	.dwattr $C$DW$454, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$454, DW_AT_decl_line(0x19b)
	.dwattr $C$DW$454, DW_AT_decl_column(0x05)

$C$DW$455	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$455, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND")
	.dwattr $C$DW$455, DW_AT_const_value(0x194)
	.dwattr $C$DW$455, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$455, DW_AT_decl_line(0x19c)
	.dwattr $C$DW$455, DW_AT_decl_column(0x05)

$C$DW$456	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$456, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_405_METHOD_NOT_ALLOWED")
	.dwattr $C$DW$456, DW_AT_const_value(0x195)
	.dwattr $C$DW$456, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$456, DW_AT_decl_line(0x19d)
	.dwattr $C$DW$456, DW_AT_decl_column(0x05)

$C$DW$457	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$457, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_500_INTERNAL_SERVER_ERROR")
	.dwattr $C$DW$457, DW_AT_const_value(0x1f4)
	.dwattr $C$DW$457, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$457, DW_AT_decl_line(0x19e)
	.dwattr $C$DW$457, DW_AT_decl_column(0x05)

$C$DW$458	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$458, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_503_SERVICE_UNAVAILABLE")
	.dwattr $C$DW$458, DW_AT_const_value(0x1f7)
	.dwattr $C$DW$458, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$458, DW_AT_decl_line(0x19f)
	.dwattr $C$DW$458, DW_AT_decl_column(0x05)

$C$DW$459	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$459, DW_AT_name("SL_NETAPP_HTTP_RESPONSE_504_GATEWAY_TIMEOUT")
	.dwattr $C$DW$459, DW_AT_const_value(0x1f8)
	.dwattr $C$DW$459, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$459, DW_AT_decl_line(0x1a0)
	.dwattr $C$DW$459, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$421, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$421, DW_AT_decl_line(0x18d)
	.dwattr $C$DW$T$421, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$421

$C$DW$T$422	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$422, DW_AT_name("SlNetAppResponseCode_e")
	.dwattr $C$DW$T$422, DW_AT_type(*$C$DW$T$421)
	.dwattr $C$DW$T$422, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$422, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$422, DW_AT_decl_line(0x1a1)
	.dwattr $C$DW$T$422, DW_AT_decl_column(0x03)


$C$DW$T$423	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$423, DW_AT_byte_size(0x01)
$C$DW$460	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$460, DW_AT_name("SL_NETAPP_FULL_SERVICE_WITH_TEXT_IPV4_TYPE")
	.dwattr $C$DW$460, DW_AT_const_value(0x01)
	.dwattr $C$DW$460, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$460, DW_AT_decl_line(0x1ca)
	.dwattr $C$DW$460, DW_AT_decl_column(0x05)

$C$DW$461	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$461, DW_AT_name("SL_NETAPP_FULL_SERVICE_IPV4_TYPE")
	.dwattr $C$DW$461, DW_AT_const_value(0x02)
	.dwattr $C$DW$461, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$461, DW_AT_decl_line(0x1cb)
	.dwattr $C$DW$461, DW_AT_decl_column(0x05)

$C$DW$462	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$462, DW_AT_name("SL_NETAPP_SHORT_SERVICE_IPV4_TYPE")
	.dwattr $C$DW$462, DW_AT_const_value(0x03)
	.dwattr $C$DW$462, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$462, DW_AT_decl_line(0x1cc)
	.dwattr $C$DW$462, DW_AT_decl_column(0x05)

$C$DW$463	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$463, DW_AT_name("SL_NETAPP_FULL_SERVICE_WITH_TEXT_IPV6_TYPE")
	.dwattr $C$DW$463, DW_AT_const_value(0x04)
	.dwattr $C$DW$463, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$463, DW_AT_decl_line(0x1cd)
	.dwattr $C$DW$463, DW_AT_decl_column(0x05)

$C$DW$464	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$464, DW_AT_name("SL_NETAPP_FULL_SERVICE_IPV6_TYPE")
	.dwattr $C$DW$464, DW_AT_const_value(0x05)
	.dwattr $C$DW$464, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$464, DW_AT_decl_line(0x1ce)
	.dwattr $C$DW$464, DW_AT_decl_column(0x05)

$C$DW$465	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$465, DW_AT_name("SL_NETAPP_SHORT_SERVICE_IPV6_TYPE")
	.dwattr $C$DW$465, DW_AT_const_value(0x06)
	.dwattr $C$DW$465, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$465, DW_AT_decl_line(0x1cf)
	.dwattr $C$DW$465, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$423, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$423, DW_AT_decl_line(0x1c9)
	.dwattr $C$DW$T$423, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$423

$C$DW$T$424	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$424, DW_AT_name("SlNetAppGetServiceListType_e")
	.dwattr $C$DW$T$424, DW_AT_type(*$C$DW$T$423)
	.dwattr $C$DW$T$424, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$424, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$424, DW_AT_decl_line(0x1d0)
	.dwattr $C$DW$T$424, DW_AT_decl_column(0x03)


$C$DW$T$425	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$425, DW_AT_byte_size(0x01)
$C$DW$466	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$466, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_DEFAULT")
	.dwattr $C$DW$466, DW_AT_const_value(0x00)
	.dwattr $C$DW$466, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$466, DW_AT_decl_line(0x55)
	.dwattr $C$DW$466, DW_AT_decl_column(0x05)

$C$DW$467	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$467, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_PIN_USER")
	.dwattr $C$DW$467, DW_AT_const_value(0x01)
	.dwattr $C$DW$467, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$467, DW_AT_decl_line(0x56)
	.dwattr $C$DW$467, DW_AT_decl_column(0x05)

$C$DW$468	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$468, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_PIN_MACHINE")
	.dwattr $C$DW$468, DW_AT_const_value(0x02)
	.dwattr $C$DW$468, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$468, DW_AT_decl_line(0x57)
	.dwattr $C$DW$468, DW_AT_decl_column(0x05)

$C$DW$469	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$469, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_REKEY")
	.dwattr $C$DW$469, DW_AT_const_value(0x03)
	.dwattr $C$DW$469, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$469, DW_AT_decl_line(0x58)
	.dwattr $C$DW$469, DW_AT_decl_column(0x05)

$C$DW$470	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$470, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_PBC")
	.dwattr $C$DW$470, DW_AT_const_value(0x04)
	.dwattr $C$DW$470, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$470, DW_AT_decl_line(0x59)
	.dwattr $C$DW$470, DW_AT_decl_column(0x05)

$C$DW$471	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$471, DW_AT_name("SL_WLAN_P2P_WPS_METHOD_REGISTRAR")
	.dwattr $C$DW$471, DW_AT_const_value(0x05)
	.dwattr $C$DW$471, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$471, DW_AT_decl_line(0x5a)
	.dwattr $C$DW$471, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$425, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$425, DW_AT_decl_line(0x54)
	.dwattr $C$DW$T$425, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$425

$C$DW$T$426	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$426, DW_AT_name("SlWlanP2PWpsMethod_e")
	.dwattr $C$DW$T$426, DW_AT_type(*$C$DW$T$425)
	.dwattr $C$DW$T$426, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$426, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$426, DW_AT_decl_line(0x5b)
	.dwattr $C$DW$T$426, DW_AT_decl_column(0x03)


$C$DW$T$427	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$427, DW_AT_byte_size(0x01)
$C$DW$472	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$472, DW_AT_name("SL_WLAN_EVENT_CONNECT")
	.dwattr $C$DW$472, DW_AT_const_value(0x01)
	.dwattr $C$DW$472, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$472, DW_AT_decl_line(0x60)
	.dwattr $C$DW$472, DW_AT_decl_column(0x05)

$C$DW$473	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$473, DW_AT_name("SL_WLAN_EVENT_DISCONNECT")
	.dwattr $C$DW$473, DW_AT_const_value(0x02)
	.dwattr $C$DW$473, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$473, DW_AT_decl_line(0x61)
	.dwattr $C$DW$473, DW_AT_decl_column(0x05)

$C$DW$474	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$474, DW_AT_name("SL_WLAN_EVENT_STA_ADDED")
	.dwattr $C$DW$474, DW_AT_const_value(0x03)
	.dwattr $C$DW$474, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$474, DW_AT_decl_line(0x62)
	.dwattr $C$DW$474, DW_AT_decl_column(0x05)

$C$DW$475	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$475, DW_AT_name("SL_WLAN_EVENT_STA_REMOVED")
	.dwattr $C$DW$475, DW_AT_const_value(0x04)
	.dwattr $C$DW$475, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$475, DW_AT_decl_line(0x63)
	.dwattr $C$DW$475, DW_AT_decl_column(0x05)

$C$DW$476	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$476, DW_AT_name("SL_WLAN_EVENT_P2P_CONNECT")
	.dwattr $C$DW$476, DW_AT_const_value(0x05)
	.dwattr $C$DW$476, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$476, DW_AT_decl_line(0x65)
	.dwattr $C$DW$476, DW_AT_decl_column(0x05)

$C$DW$477	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$477, DW_AT_name("SL_WLAN_EVENT_P2P_DISCONNECT")
	.dwattr $C$DW$477, DW_AT_const_value(0x06)
	.dwattr $C$DW$477, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$477, DW_AT_decl_line(0x66)
	.dwattr $C$DW$477, DW_AT_decl_column(0x05)

$C$DW$478	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$478, DW_AT_name("SL_WLAN_EVENT_P2P_CLIENT_ADDED")
	.dwattr $C$DW$478, DW_AT_const_value(0x07)
	.dwattr $C$DW$478, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$478, DW_AT_decl_line(0x67)
	.dwattr $C$DW$478, DW_AT_decl_column(0x05)

$C$DW$479	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$479, DW_AT_name("SL_WLAN_EVENT_P2P_CLIENT_REMOVED")
	.dwattr $C$DW$479, DW_AT_const_value(0x08)
	.dwattr $C$DW$479, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$479, DW_AT_decl_line(0x68)
	.dwattr $C$DW$479, DW_AT_decl_column(0x05)

$C$DW$480	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$480, DW_AT_name("SL_WLAN_EVENT_P2P_DEVFOUND")
	.dwattr $C$DW$480, DW_AT_const_value(0x09)
	.dwattr $C$DW$480, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$480, DW_AT_decl_line(0x69)
	.dwattr $C$DW$480, DW_AT_decl_column(0x05)

$C$DW$481	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$481, DW_AT_name("SL_WLAN_EVENT_P2P_REQUEST")
	.dwattr $C$DW$481, DW_AT_const_value(0x0a)
	.dwattr $C$DW$481, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$481, DW_AT_decl_line(0x6a)
	.dwattr $C$DW$481, DW_AT_decl_column(0x05)

$C$DW$482	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$482, DW_AT_name("SL_WLAN_EVENT_P2P_CONNECTFAIL")
	.dwattr $C$DW$482, DW_AT_const_value(0x0b)
	.dwattr $C$DW$482, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$482, DW_AT_decl_line(0x6b)
	.dwattr $C$DW$482, DW_AT_decl_column(0x05)

$C$DW$483	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$483, DW_AT_name("SL_WLAN_EVENT_RXFILTER")
	.dwattr $C$DW$483, DW_AT_const_value(0x0c)
	.dwattr $C$DW$483, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$483, DW_AT_decl_line(0x6d)
	.dwattr $C$DW$483, DW_AT_decl_column(0x05)

$C$DW$484	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$484, DW_AT_name("SL_WLAN_EVENT_PROVISIONING_STATUS")
	.dwattr $C$DW$484, DW_AT_const_value(0x0d)
	.dwattr $C$DW$484, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$484, DW_AT_decl_line(0x6e)
	.dwattr $C$DW$484, DW_AT_decl_column(0x05)

$C$DW$485	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$485, DW_AT_name("SL_WLAN_EVENT_PROVISIONING_PROFILE_ADDED")
	.dwattr $C$DW$485, DW_AT_const_value(0x0e)
	.dwattr $C$DW$485, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$485, DW_AT_decl_line(0x6f)
	.dwattr $C$DW$485, DW_AT_decl_column(0x05)

$C$DW$486	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$486, DW_AT_name("SL_WLAN_EVENT_MAX")
	.dwattr $C$DW$486, DW_AT_const_value(0x0f)
	.dwattr $C$DW$486, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$486, DW_AT_decl_line(0x70)
	.dwattr $C$DW$486, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$427, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$427, DW_AT_decl_line(0x5f)
	.dwattr $C$DW$T$427, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$427

$C$DW$T$428	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$428, DW_AT_name("SlWlanEventId_e")
	.dwattr $C$DW$T$428, DW_AT_type(*$C$DW$T$427)
	.dwattr $C$DW$T$428, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$428, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$428, DW_AT_decl_line(0x72)
	.dwattr $C$DW$T$428, DW_AT_decl_column(0x03)


$C$DW$T$429	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$429, DW_AT_byte_size(0x01)
$C$DW$487	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$487, DW_AT_name("SL_WLAN_RATE_1M")
	.dwattr $C$DW$487, DW_AT_const_value(0x01)
	.dwattr $C$DW$487, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$487, DW_AT_decl_line(0x1a6)
	.dwattr $C$DW$487, DW_AT_decl_column(0x05)

$C$DW$488	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$488, DW_AT_name("SL_WLAN_RATE_2M")
	.dwattr $C$DW$488, DW_AT_const_value(0x02)
	.dwattr $C$DW$488, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$488, DW_AT_decl_line(0x1a7)
	.dwattr $C$DW$488, DW_AT_decl_column(0x05)

$C$DW$489	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$489, DW_AT_name("SL_WLAN_RATE_5_5M")
	.dwattr $C$DW$489, DW_AT_const_value(0x03)
	.dwattr $C$DW$489, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$489, DW_AT_decl_line(0x1a8)
	.dwattr $C$DW$489, DW_AT_decl_column(0x05)

$C$DW$490	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$490, DW_AT_name("SL_WLAN_RATE_11M")
	.dwattr $C$DW$490, DW_AT_const_value(0x04)
	.dwattr $C$DW$490, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$490, DW_AT_decl_line(0x1a9)
	.dwattr $C$DW$490, DW_AT_decl_column(0x05)

$C$DW$491	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$491, DW_AT_name("SL_WLAN_RATE_6M")
	.dwattr $C$DW$491, DW_AT_const_value(0x06)
	.dwattr $C$DW$491, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$491, DW_AT_decl_line(0x1aa)
	.dwattr $C$DW$491, DW_AT_decl_column(0x05)

$C$DW$492	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$492, DW_AT_name("SL_WLAN_RATE_9M")
	.dwattr $C$DW$492, DW_AT_const_value(0x07)
	.dwattr $C$DW$492, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$492, DW_AT_decl_line(0x1ab)
	.dwattr $C$DW$492, DW_AT_decl_column(0x05)

$C$DW$493	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$493, DW_AT_name("SL_WLAN_RATE_12M")
	.dwattr $C$DW$493, DW_AT_const_value(0x08)
	.dwattr $C$DW$493, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$493, DW_AT_decl_line(0x1ac)
	.dwattr $C$DW$493, DW_AT_decl_column(0x05)

$C$DW$494	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$494, DW_AT_name("SL_WLAN_RATE_18M")
	.dwattr $C$DW$494, DW_AT_const_value(0x09)
	.dwattr $C$DW$494, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$494, DW_AT_decl_line(0x1ad)
	.dwattr $C$DW$494, DW_AT_decl_column(0x05)

$C$DW$495	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$495, DW_AT_name("SL_WLAN_RATE_24M")
	.dwattr $C$DW$495, DW_AT_const_value(0x0a)
	.dwattr $C$DW$495, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$495, DW_AT_decl_line(0x1ae)
	.dwattr $C$DW$495, DW_AT_decl_column(0x05)

$C$DW$496	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$496, DW_AT_name("SL_WLAN_RATE_36M")
	.dwattr $C$DW$496, DW_AT_const_value(0x0b)
	.dwattr $C$DW$496, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$496, DW_AT_decl_line(0x1af)
	.dwattr $C$DW$496, DW_AT_decl_column(0x05)

$C$DW$497	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$497, DW_AT_name("SL_WLAN_RATE_48M")
	.dwattr $C$DW$497, DW_AT_const_value(0x0c)
	.dwattr $C$DW$497, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$497, DW_AT_decl_line(0x1b0)
	.dwattr $C$DW$497, DW_AT_decl_column(0x05)

$C$DW$498	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$498, DW_AT_name("SL_WLAN_RATE_54M")
	.dwattr $C$DW$498, DW_AT_const_value(0x0d)
	.dwattr $C$DW$498, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$498, DW_AT_decl_line(0x1b1)
	.dwattr $C$DW$498, DW_AT_decl_column(0x05)

$C$DW$499	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$499, DW_AT_name("SL_WLAN_RATE_MCS_0")
	.dwattr $C$DW$499, DW_AT_const_value(0x0e)
	.dwattr $C$DW$499, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$499, DW_AT_decl_line(0x1b2)
	.dwattr $C$DW$499, DW_AT_decl_column(0x05)

$C$DW$500	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$500, DW_AT_name("SL_WLAN_RATE_MCS_1")
	.dwattr $C$DW$500, DW_AT_const_value(0x0f)
	.dwattr $C$DW$500, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$500, DW_AT_decl_line(0x1b3)
	.dwattr $C$DW$500, DW_AT_decl_column(0x05)

$C$DW$501	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$501, DW_AT_name("SL_WLAN_RATE_MCS_2")
	.dwattr $C$DW$501, DW_AT_const_value(0x10)
	.dwattr $C$DW$501, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$501, DW_AT_decl_line(0x1b4)
	.dwattr $C$DW$501, DW_AT_decl_column(0x05)

$C$DW$502	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$502, DW_AT_name("SL_WLAN_RATE_MCS_3")
	.dwattr $C$DW$502, DW_AT_const_value(0x11)
	.dwattr $C$DW$502, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$502, DW_AT_decl_line(0x1b5)
	.dwattr $C$DW$502, DW_AT_decl_column(0x05)

$C$DW$503	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$503, DW_AT_name("SL_WLAN_RATE_MCS_4")
	.dwattr $C$DW$503, DW_AT_const_value(0x12)
	.dwattr $C$DW$503, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$503, DW_AT_decl_line(0x1b6)
	.dwattr $C$DW$503, DW_AT_decl_column(0x05)

$C$DW$504	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$504, DW_AT_name("SL_WLAN_RATE_MCS_5")
	.dwattr $C$DW$504, DW_AT_const_value(0x13)
	.dwattr $C$DW$504, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$504, DW_AT_decl_line(0x1b7)
	.dwattr $C$DW$504, DW_AT_decl_column(0x05)

$C$DW$505	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$505, DW_AT_name("SL_WLAN_RATE_MCS_6")
	.dwattr $C$DW$505, DW_AT_const_value(0x14)
	.dwattr $C$DW$505, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$505, DW_AT_decl_line(0x1b8)
	.dwattr $C$DW$505, DW_AT_decl_column(0x05)

$C$DW$506	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$506, DW_AT_name("SL_WLAN_RATE_MCS_7")
	.dwattr $C$DW$506, DW_AT_const_value(0x15)
	.dwattr $C$DW$506, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$506, DW_AT_decl_line(0x1b9)
	.dwattr $C$DW$506, DW_AT_decl_column(0x05)

$C$DW$507	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$507, DW_AT_name("SL_WLAN_MAX_NUM_RATES")
	.dwattr $C$DW$507, DW_AT_const_value(0xff)
	.dwattr $C$DW$507, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$507, DW_AT_decl_line(0x1ba)
	.dwattr $C$DW$507, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$429, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$429, DW_AT_decl_line(0x1a5)
	.dwattr $C$DW$T$429, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$429

$C$DW$T$430	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$430, DW_AT_name("SlWlanRateIndex_e")
	.dwattr $C$DW$T$430, DW_AT_type(*$C$DW$T$429)
	.dwattr $C$DW$T$430, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$430, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$430, DW_AT_decl_line(0x1bb)
	.dwattr $C$DW$T$430, DW_AT_decl_column(0x02)


$C$DW$T$431	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$431, DW_AT_byte_size(0x01)
$C$DW$508	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$508, DW_AT_name("SL_WLAN_DEV_PW_DEFAULT")
	.dwattr $C$DW$508, DW_AT_const_value(0x00)
	.dwattr $C$DW$508, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$508, DW_AT_decl_line(0x1bf)
	.dwattr $C$DW$508, DW_AT_decl_column(0x05)

$C$DW$509	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$509, DW_AT_name("SL_WLAN_DEV_PW_PIN_KEYPAD")
	.dwattr $C$DW$509, DW_AT_const_value(0x01)
	.dwattr $C$DW$509, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$509, DW_AT_decl_line(0x1c0)
	.dwattr $C$DW$509, DW_AT_decl_column(0x05)

$C$DW$510	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$510, DW_AT_name("SL_WLAN_DEV_PW_PUSH_BUTTON")
	.dwattr $C$DW$510, DW_AT_const_value(0x04)
	.dwattr $C$DW$510, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$510, DW_AT_decl_line(0x1c1)
	.dwattr $C$DW$510, DW_AT_decl_column(0x05)

$C$DW$511	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$511, DW_AT_name("SL_WLAN_DEV_PW_PIN_DISPLAY")
	.dwattr $C$DW$511, DW_AT_const_value(0x05)
	.dwattr $C$DW$511, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$511, DW_AT_decl_line(0x1c2)
	.dwattr $C$DW$511, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$431, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$431, DW_AT_decl_line(0x1be)
	.dwattr $C$DW$T$431, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$431

$C$DW$T$432	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$432, DW_AT_name("SlWlanP2pDevPwdMethod_e")
	.dwattr $C$DW$T$432, DW_AT_type(*$C$DW$T$431)
	.dwattr $C$DW$T$432, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$432, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$432, DW_AT_decl_line(0x1c3)
	.dwattr $C$DW$T$432, DW_AT_decl_column(0x03)


$C$DW$T$433	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$433, DW_AT_byte_size(0x01)
$C$DW$512	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$512, DW_AT_name("ROLE_STA")
	.dwattr $C$DW$512, DW_AT_const_value(0x00)
	.dwattr $C$DW$512, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$512, DW_AT_decl_line(0x1f5)
	.dwattr $C$DW$512, DW_AT_decl_column(0x05)

$C$DW$513	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$513, DW_AT_name("ROLE_RESERVED")
	.dwattr $C$DW$513, DW_AT_const_value(0x01)
	.dwattr $C$DW$513, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$513, DW_AT_decl_line(0x1f6)
	.dwattr $C$DW$513, DW_AT_decl_column(0x05)

$C$DW$514	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$514, DW_AT_name("ROLE_AP")
	.dwattr $C$DW$514, DW_AT_const_value(0x02)
	.dwattr $C$DW$514, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$514, DW_AT_decl_line(0x1f7)
	.dwattr $C$DW$514, DW_AT_decl_column(0x05)

$C$DW$515	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$515, DW_AT_name("ROLE_P2P")
	.dwattr $C$DW$515, DW_AT_const_value(0x03)
	.dwattr $C$DW$515, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$515, DW_AT_decl_line(0x1f8)
	.dwattr $C$DW$515, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$433, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$433, DW_AT_decl_line(0x1f4)
	.dwattr $C$DW$T$433, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$433

$C$DW$T$434	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$434, DW_AT_name("SlWlanMode_e")
	.dwattr $C$DW$T$434, DW_AT_type(*$C$DW$T$433)
	.dwattr $C$DW$T$434, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$434, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$434, DW_AT_decl_line(0x1f9)
	.dwattr $C$DW$T$434, DW_AT_decl_column(0x02)


$C$DW$T$435	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$435, DW_AT_byte_size(0x01)
$C$DW$516	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$516, DW_AT_name("SL_WLAN_DISCONNECTED")
	.dwattr $C$DW$516, DW_AT_const_value(0x00)
	.dwattr $C$DW$516, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$516, DW_AT_decl_line(0x2aa)
	.dwattr $C$DW$516, DW_AT_decl_column(0x05)

$C$DW$517	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$517, DW_AT_name("SL_WLAN_CONNECTED_STA")
	.dwattr $C$DW$517, DW_AT_const_value(0x01)
	.dwattr $C$DW$517, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$517, DW_AT_decl_line(0x2ab)
	.dwattr $C$DW$517, DW_AT_decl_column(0x05)

$C$DW$518	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$518, DW_AT_name("SL_WLAN_CONNECTED_P2PCL")
	.dwattr $C$DW$518, DW_AT_const_value(0x02)
	.dwattr $C$DW$518, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$518, DW_AT_decl_line(0x2ac)
	.dwattr $C$DW$518, DW_AT_decl_column(0x05)

$C$DW$519	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$519, DW_AT_name("SL_WLAN_CONNECTED_P2PGO")
	.dwattr $C$DW$519, DW_AT_const_value(0x03)
	.dwattr $C$DW$519, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$519, DW_AT_decl_line(0x2ad)
	.dwattr $C$DW$519, DW_AT_decl_column(0x05)

$C$DW$520	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$520, DW_AT_name("SL_WLAN_AP_CONNECTED_STATIONS")
	.dwattr $C$DW$520, DW_AT_const_value(0x04)
	.dwattr $C$DW$520, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$520, DW_AT_decl_line(0x2ae)
	.dwattr $C$DW$520, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$435, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$435, DW_AT_decl_line(0x2a9)
	.dwattr $C$DW$T$435, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$435

$C$DW$T$436	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$436, DW_AT_name("SlWlanConnStatusFlags_e")
	.dwattr $C$DW$T$436, DW_AT_type(*$C$DW$T$435)
	.dwattr $C$DW$T$436, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$436, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$436, DW_AT_decl_line(0x2af)
	.dwattr $C$DW$T$436, DW_AT_decl_column(0x02)


$C$DW$T$437	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$437, DW_AT_byte_size(0x01)
$C$DW$521	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$521, DW_AT_name("SL_DEVICE_EVENT_FATAL_DEVICE_ABORT")
	.dwattr $C$DW$521, DW_AT_const_value(0x01)
	.dwattr $C$DW$521, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$521, DW_AT_decl_line(0x4c)
	.dwattr $C$DW$521, DW_AT_decl_column(0x05)

$C$DW$522	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$522, DW_AT_name("SL_DEVICE_EVENT_FATAL_DRIVER_ABORT")
	.dwattr $C$DW$522, DW_AT_const_value(0x02)
	.dwattr $C$DW$522, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$522, DW_AT_decl_line(0x4d)
	.dwattr $C$DW$522, DW_AT_decl_column(0x05)

$C$DW$523	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$523, DW_AT_name("SL_DEVICE_EVENT_FATAL_SYNC_LOSS")
	.dwattr $C$DW$523, DW_AT_const_value(0x03)
	.dwattr $C$DW$523, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$523, DW_AT_decl_line(0x4e)
	.dwattr $C$DW$523, DW_AT_decl_column(0x05)

$C$DW$524	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$524, DW_AT_name("SL_DEVICE_EVENT_FATAL_NO_CMD_ACK")
	.dwattr $C$DW$524, DW_AT_const_value(0x04)
	.dwattr $C$DW$524, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$524, DW_AT_decl_line(0x4f)
	.dwattr $C$DW$524, DW_AT_decl_column(0x05)

$C$DW$525	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$525, DW_AT_name("SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT")
	.dwattr $C$DW$525, DW_AT_const_value(0x05)
	.dwattr $C$DW$525, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$525, DW_AT_decl_line(0x50)
	.dwattr $C$DW$525, DW_AT_decl_column(0x05)

$C$DW$526	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$526, DW_AT_name("SL_DEVICE_EVENT_RESET_REQUEST")
	.dwattr $C$DW$526, DW_AT_const_value(0x06)
	.dwattr $C$DW$526, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$526, DW_AT_decl_line(0x51)
	.dwattr $C$DW$526, DW_AT_decl_column(0x05)

$C$DW$527	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$527, DW_AT_name("SL_DEVICE_EVENT_ERROR")
	.dwattr $C$DW$527, DW_AT_const_value(0x07)
	.dwattr $C$DW$527, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$527, DW_AT_decl_line(0x52)
	.dwattr $C$DW$527, DW_AT_decl_column(0x05)

$C$DW$528	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$528, DW_AT_name("SL_DEVICE_EVENT_MAX")
	.dwattr $C$DW$528, DW_AT_const_value(0x08)
	.dwattr $C$DW$528, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$528, DW_AT_decl_line(0x53)
	.dwattr $C$DW$528, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$437, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$437, DW_AT_decl_line(0x4b)
	.dwattr $C$DW$T$437, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$437

$C$DW$T$438	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$438, DW_AT_name("SlDeviceEventId_e")
	.dwattr $C$DW$T$438, DW_AT_type(*$C$DW$T$437)
	.dwattr $C$DW$T$438, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$438, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$438, DW_AT_decl_line(0x55)
	.dwattr $C$DW$T$438, DW_AT_decl_column(0x03)


$C$DW$T$439	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$439, DW_AT_byte_size(0x01)
$C$DW$529	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$529, DW_AT_name("SL_DEVICE_RESET_REQUEST_CALLER_PROVISIONING")
	.dwattr $C$DW$529, DW_AT_const_value(0x00)
	.dwattr $C$DW$529, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$529, DW_AT_decl_line(0x78)
	.dwattr $C$DW$529, DW_AT_decl_column(0x05)

$C$DW$530	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$530, DW_AT_name("SL_DEVICE_RESET_REQUEST_CALLER_PROVISIONING_EXTERNAL_CONFIGURATION")
	.dwattr $C$DW$530, DW_AT_const_value(0x01)
	.dwattr $C$DW$530, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$530, DW_AT_decl_line(0x79)
	.dwattr $C$DW$530, DW_AT_decl_column(0x05)

$C$DW$531	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$531, DW_AT_name("SL_DEVICE_RESET_REQUEST_NUM_OF_CALLERS")
	.dwattr $C$DW$531, DW_AT_const_value(0x02)
	.dwattr $C$DW$531, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$531, DW_AT_decl_line(0x7a)
	.dwattr $C$DW$531, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$439, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$439, DW_AT_decl_line(0x77)
	.dwattr $C$DW$T$439, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$439

$C$DW$T$440	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$440, DW_AT_name("SlDeviceResetRequestCaller_e")
	.dwattr $C$DW$T$440, DW_AT_type(*$C$DW$T$439)
	.dwattr $C$DW$T$440, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$440, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$440, DW_AT_decl_line(0x7b)
	.dwattr $C$DW$T$440, DW_AT_decl_column(0x02)


$C$DW$T$441	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$441, DW_AT_byte_size(0x01)
$C$DW$532	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$532, DW_AT_name("SL_NETCFG_MAC_ADDRESS_SET")
	.dwattr $C$DW$532, DW_AT_const_value(0x01)
	.dwattr $C$DW$532, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$532, DW_AT_decl_line(0x6f)
	.dwattr $C$DW$532, DW_AT_decl_column(0x05)

$C$DW$533	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$533, DW_AT_name("SL_NETCFG_MAC_ADDRESS_GET")
	.dwattr $C$DW$533, DW_AT_const_value(0x02)
	.dwattr $C$DW$533, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$533, DW_AT_decl_line(0x70)
	.dwattr $C$DW$533, DW_AT_decl_column(0x05)

$C$DW$534	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$534, DW_AT_name("SL_NETCFG_AP_STATIONS_NUM_CONNECTED")
	.dwattr $C$DW$534, DW_AT_const_value(0x03)
	.dwattr $C$DW$534, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$534, DW_AT_decl_line(0x71)
	.dwattr $C$DW$534, DW_AT_decl_column(0x05)

$C$DW$535	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$535, DW_AT_name("SL_NETCFG_AP_STATIONS_INFO_LIST")
	.dwattr $C$DW$535, DW_AT_const_value(0x04)
	.dwattr $C$DW$535, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$535, DW_AT_decl_line(0x72)
	.dwattr $C$DW$535, DW_AT_decl_column(0x05)

$C$DW$536	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$536, DW_AT_name("SL_NETCFG_AP_STATION_DISCONNECT")
	.dwattr $C$DW$536, DW_AT_const_value(0x05)
	.dwattr $C$DW$536, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$536, DW_AT_decl_line(0x73)
	.dwattr $C$DW$536, DW_AT_decl_column(0x05)

$C$DW$537	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$537, DW_AT_name("SL_NETCFG_IF")
	.dwattr $C$DW$537, DW_AT_const_value(0x06)
	.dwattr $C$DW$537, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$537, DW_AT_decl_line(0x74)
	.dwattr $C$DW$537, DW_AT_decl_column(0x05)

$C$DW$538	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$538, DW_AT_name("SL_NETCFG_IPV4_STA_ADDR_MODE")
	.dwattr $C$DW$538, DW_AT_const_value(0x07)
	.dwattr $C$DW$538, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$538, DW_AT_decl_line(0x75)
	.dwattr $C$DW$538, DW_AT_decl_column(0x05)

$C$DW$539	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$539, DW_AT_name("SL_NETCFG_IPV4_AP_ADDR_MODE")
	.dwattr $C$DW$539, DW_AT_const_value(0x08)
	.dwattr $C$DW$539, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$539, DW_AT_decl_line(0x76)
	.dwattr $C$DW$539, DW_AT_decl_column(0x05)

$C$DW$540	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$540, DW_AT_name("SL_NETCFG_IPV6_ADDR_LOCAL")
	.dwattr $C$DW$540, DW_AT_const_value(0x09)
	.dwattr $C$DW$540, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$540, DW_AT_decl_line(0x77)
	.dwattr $C$DW$540, DW_AT_decl_column(0x05)

$C$DW$541	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$541, DW_AT_name("SL_NETCFG_IPV6_ADDR_GLOBAL")
	.dwattr $C$DW$541, DW_AT_const_value(0x0a)
	.dwattr $C$DW$541, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$541, DW_AT_decl_line(0x78)
	.dwattr $C$DW$541, DW_AT_decl_column(0x05)

$C$DW$542	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$542, DW_AT_name("SL_NETCFG_IPV4_DHCP_CLIENT")
	.dwattr $C$DW$542, DW_AT_const_value(0x0b)
	.dwattr $C$DW$542, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$542, DW_AT_decl_line(0x79)
	.dwattr $C$DW$542, DW_AT_decl_column(0x05)

$C$DW$543	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$543, DW_AT_name("SL_NETCFG_IPV4_DNS_CLIENT")
	.dwattr $C$DW$543, DW_AT_const_value(0x0c)
	.dwattr $C$DW$543, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$543, DW_AT_decl_line(0x7a)
	.dwattr $C$DW$543, DW_AT_decl_column(0x05)

$C$DW$544	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$544, DW_AT_name("MAX_SETTINGS")
	.dwattr $C$DW$544, DW_AT_const_value(0xff)
	.dwattr $C$DW$544, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$544, DW_AT_decl_line(0x7b)
	.dwattr $C$DW$544, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$441, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$441, DW_AT_decl_line(0x6e)
	.dwattr $C$DW$T$441, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$441

$C$DW$T$442	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$442, DW_AT_name("SlNetCfg_e")
	.dwattr $C$DW$T$442, DW_AT_type(*$C$DW$T$441)
	.dwattr $C$DW$T$442, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$442, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$442, DW_AT_decl_line(0x7c)
	.dwattr $C$DW$T$442, DW_AT_decl_column(0x02)


$C$DW$T$443	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$443, DW_AT_byte_size(0x01)
$C$DW$545	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$545, DW_AT_name("SL_NETCFG_DHCP_CLIENT_UNKNOWN")
	.dwattr $C$DW$545, DW_AT_const_value(0x00)
	.dwattr $C$DW$545, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$545, DW_AT_decl_line(0x93)
	.dwattr $C$DW$545, DW_AT_decl_column(0x05)

$C$DW$546	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$546, DW_AT_name("SL_NETCFG_DHCP_CLIENT_DISABLED")
	.dwattr $C$DW$546, DW_AT_const_value(0x01)
	.dwattr $C$DW$546, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$546, DW_AT_decl_line(0x94)
	.dwattr $C$DW$546, DW_AT_decl_column(0x05)

$C$DW$547	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$547, DW_AT_name("SL_NETCFG_DHCP_CLIENT_ENABLED")
	.dwattr $C$DW$547, DW_AT_const_value(0x02)
	.dwattr $C$DW$547, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$547, DW_AT_decl_line(0x95)
	.dwattr $C$DW$547, DW_AT_decl_column(0x05)

$C$DW$548	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$548, DW_AT_name("SL_NETCFG_DHCP_CLIENT_BOUND")
	.dwattr $C$DW$548, DW_AT_const_value(0x03)
	.dwattr $C$DW$548, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$548, DW_AT_decl_line(0x96)
	.dwattr $C$DW$548, DW_AT_decl_column(0x05)

$C$DW$549	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$549, DW_AT_name("SL_NETCFG_DHCP_CLIENT_RENEW")
	.dwattr $C$DW$549, DW_AT_const_value(0x04)
	.dwattr $C$DW$549, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$549, DW_AT_decl_line(0x97)
	.dwattr $C$DW$549, DW_AT_decl_column(0x05)

$C$DW$550	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$550, DW_AT_name("SL_NETCFG_DHCP_CLIENT_REBIND")
	.dwattr $C$DW$550, DW_AT_const_value(0x05)
	.dwattr $C$DW$550, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$550, DW_AT_decl_line(0x98)
	.dwattr $C$DW$550, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$443, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$443, DW_AT_decl_line(0x92)
	.dwattr $C$DW$T$443, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$443

$C$DW$T$444	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$444, DW_AT_name("SlNetCfgIpv4DhcpClientState_e")
	.dwattr $C$DW$T$444, DW_AT_type(*$C$DW$T$443)
	.dwattr $C$DW$T$444, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$444, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$444, DW_AT_decl_line(0x99)
	.dwattr $C$DW$T$444, DW_AT_decl_column(0x02)


$C$DW$T$445	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$445, DW_AT_byte_size(0x01)
$C$DW$551	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$551, DW_AT_name("SL_NETCFG_DHCP_OPT_DISABLE_LLA")
	.dwattr $C$DW$551, DW_AT_const_value(0x02)
	.dwattr $C$DW$551, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$551, DW_AT_decl_line(0x9e)
	.dwattr $C$DW$551, DW_AT_decl_column(0x05)

$C$DW$552	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$552, DW_AT_name("SL_NETCFG_DHCP_OPT_RELEASE_IP_BEFORE_DISCONNECT")
	.dwattr $C$DW$552, DW_AT_const_value(0x04)
	.dwattr $C$DW$552, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$552, DW_AT_decl_line(0x9f)
	.dwattr $C$DW$552, DW_AT_decl_column(0x05)

$C$DW$553	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$553, DW_AT_name("MAX_SL_NETCFG_DHCP_OPT")
	.dwattr $C$DW$553, DW_AT_const_value(0xff)
	.dwattr $C$DW$553, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$553, DW_AT_decl_line(0xa0)
	.dwattr $C$DW$553, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$445, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$445, DW_AT_decl_line(0x9d)
	.dwattr $C$DW$T$445, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$445

$C$DW$T$446	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$446, DW_AT_name("SlNetCfgDhcpOption_e")
	.dwattr $C$DW$T$446, DW_AT_type(*$C$DW$T$445)
	.dwattr $C$DW$T$446, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$446, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$446, DW_AT_decl_line(0xa1)
	.dwattr $C$DW$T$446, DW_AT_decl_column(0x03)


$C$DW$T$447	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$447, DW_AT_byte_size(0x01)
$C$DW$554	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$554, DW_AT_name("SL_EVENT_HDL_FATAL_ERROR")
	.dwattr $C$DW$554, DW_AT_const_value(0x00)
	.dwattr $C$DW$554, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$554, DW_AT_decl_line(0x3b)
	.dwattr $C$DW$554, DW_AT_decl_column(0x05)

$C$DW$555	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$555, DW_AT_name("SL_EVENT_HDL_DEVICE_GENERAL")
	.dwattr $C$DW$555, DW_AT_const_value(0x01)
	.dwattr $C$DW$555, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$555, DW_AT_decl_line(0x3c)
	.dwattr $C$DW$555, DW_AT_decl_column(0x05)

$C$DW$556	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$556, DW_AT_name("SL_EVENT_HDL_WLAN")
	.dwattr $C$DW$556, DW_AT_const_value(0x02)
	.dwattr $C$DW$556, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$556, DW_AT_decl_line(0x3d)
	.dwattr $C$DW$556, DW_AT_decl_column(0x05)

$C$DW$557	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$557, DW_AT_name("SL_EVENT_HDL_NETAPP")
	.dwattr $C$DW$557, DW_AT_const_value(0x03)
	.dwattr $C$DW$557, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$557, DW_AT_decl_line(0x3e)
	.dwattr $C$DW$557, DW_AT_decl_column(0x05)

$C$DW$558	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$558, DW_AT_name("SL_EVENT_HDL_SOCKET")
	.dwattr $C$DW$558, DW_AT_const_value(0x04)
	.dwattr $C$DW$558, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$558, DW_AT_decl_line(0x3f)
	.dwattr $C$DW$558, DW_AT_decl_column(0x05)

$C$DW$559	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$559, DW_AT_name("SL_EVENT_HDL_HTTP_SERVER")
	.dwattr $C$DW$559, DW_AT_const_value(0x05)
	.dwattr $C$DW$559, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$559, DW_AT_decl_line(0x40)
	.dwattr $C$DW$559, DW_AT_decl_column(0x05)

$C$DW$560	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$560, DW_AT_name("SL_EVENT_HDL_NETAPP_REQUEST")
	.dwattr $C$DW$560, DW_AT_const_value(0x06)
	.dwattr $C$DW$560, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$560, DW_AT_decl_line(0x41)
	.dwattr $C$DW$560, DW_AT_decl_column(0x05)

$C$DW$561	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$561, DW_AT_name("SL_EVENT_HDL_MEM_FREE")
	.dwattr $C$DW$561, DW_AT_const_value(0x07)
	.dwattr $C$DW$561, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$561, DW_AT_decl_line(0x42)
	.dwattr $C$DW$561, DW_AT_decl_column(0x05)

$C$DW$562	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$562, DW_AT_name("SL_EVENT_HDL_SOCKET_TRIGGER")
	.dwattr $C$DW$562, DW_AT_const_value(0x08)
	.dwattr $C$DW$562, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$562, DW_AT_decl_line(0x43)
	.dwattr $C$DW$562, DW_AT_decl_column(0x05)

$C$DW$563	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$563, DW_AT_name("SL_NUM_OF_EVENT_TYPES")
	.dwattr $C$DW$563, DW_AT_const_value(0x09)
	.dwattr $C$DW$563, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$563, DW_AT_decl_line(0x44)
	.dwattr $C$DW$563, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$447, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$T$447, DW_AT_decl_line(0x3a)
	.dwattr $C$DW$T$447, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$447

$C$DW$T$448	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$448, DW_AT_name("SlEventHandler_e")
	.dwattr $C$DW$T$448, DW_AT_type(*$C$DW$T$447)
	.dwattr $C$DW$T$448, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$448, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$T$448, DW_AT_decl_line(0x45)
	.dwattr $C$DW$T$448, DW_AT_decl_column(0x02)


$C$DW$T$21	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$21, DW_AT_byte_size(0x28)
$C$DW$564	.dwtag  DW_TAG_member
	.dwattr $C$DW$564, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$564, DW_AT_name("SsidLen")
	.dwattr $C$DW$564, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$564, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$564, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$564, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$564, DW_AT_decl_line(0x1fd)
	.dwattr $C$DW$564, DW_AT_decl_column(0x0d)

$C$DW$565	.dwtag  DW_TAG_member
	.dwattr $C$DW$565, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$565, DW_AT_name("SsidName")
	.dwattr $C$DW$565, DW_AT_TI_symbol_name("SsidName")
	.dwattr $C$DW$565, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$565, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$565, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$565, DW_AT_decl_line(0x1fe)
	.dwattr $C$DW$565, DW_AT_decl_column(0x0d)

$C$DW$566	.dwtag  DW_TAG_member
	.dwattr $C$DW$566, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$566, DW_AT_name("Bssid")
	.dwattr $C$DW$566, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$566, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$566, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$566, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$566, DW_AT_decl_line(0x1ff)
	.dwattr $C$DW$566, DW_AT_decl_column(0x0d)

$C$DW$567	.dwtag  DW_TAG_member
	.dwattr $C$DW$567, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$567, DW_AT_name("Padding")
	.dwattr $C$DW$567, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$567, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$567, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$567, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$567, DW_AT_decl_line(0x200)
	.dwattr $C$DW$567, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$21, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$21, DW_AT_decl_line(0x1fc)
	.dwattr $C$DW$T$21, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$21

$C$DW$T$197	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$197, DW_AT_name("SlWlanEventConnect_t")
	.dwattr $C$DW$T$197, DW_AT_type(*$C$DW$T$21)
	.dwattr $C$DW$T$197, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$197, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$197, DW_AT_decl_line(0x201)
	.dwattr $C$DW$T$197, DW_AT_decl_column(0x03)


$C$DW$T$23	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$23, DW_AT_byte_size(0x4c)
$C$DW$568	.dwtag  DW_TAG_member
	.dwattr $C$DW$568, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$568, DW_AT_name("SsidLen")
	.dwattr $C$DW$568, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$568, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$568, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$568, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$568, DW_AT_decl_line(0x214)
	.dwattr $C$DW$568, DW_AT_decl_column(0x0d)

$C$DW$569	.dwtag  DW_TAG_member
	.dwattr $C$DW$569, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$569, DW_AT_name("SsidName")
	.dwattr $C$DW$569, DW_AT_TI_symbol_name("SsidName")
	.dwattr $C$DW$569, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$569, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$569, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$569, DW_AT_decl_line(0x215)
	.dwattr $C$DW$569, DW_AT_decl_column(0x0d)

$C$DW$570	.dwtag  DW_TAG_member
	.dwattr $C$DW$570, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$570, DW_AT_name("Bssid")
	.dwattr $C$DW$570, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$570, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$570, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$570, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$570, DW_AT_decl_line(0x216)
	.dwattr $C$DW$570, DW_AT_decl_column(0x0d)

$C$DW$571	.dwtag  DW_TAG_member
	.dwattr $C$DW$571, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$571, DW_AT_name("Reserved")
	.dwattr $C$DW$571, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$571, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$571, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$571, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$571, DW_AT_decl_line(0x217)
	.dwattr $C$DW$571, DW_AT_decl_column(0x0d)

$C$DW$572	.dwtag  DW_TAG_member
	.dwattr $C$DW$572, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$572, DW_AT_name("GoDeviceNameLen")
	.dwattr $C$DW$572, DW_AT_TI_symbol_name("GoDeviceNameLen")
	.dwattr $C$DW$572, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$572, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$572, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$572, DW_AT_decl_line(0x218)
	.dwattr $C$DW$572, DW_AT_decl_column(0x0d)

$C$DW$573	.dwtag  DW_TAG_member
	.dwattr $C$DW$573, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$573, DW_AT_name("GoDeviceName")
	.dwattr $C$DW$573, DW_AT_TI_symbol_name("GoDeviceName")
	.dwattr $C$DW$573, DW_AT_data_member_location[DW_OP_plus_uconst 0x29]
	.dwattr $C$DW$573, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$573, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$573, DW_AT_decl_line(0x219)
	.dwattr $C$DW$573, DW_AT_decl_column(0x0d)

$C$DW$574	.dwtag  DW_TAG_member
	.dwattr $C$DW$574, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$574, DW_AT_name("Padding")
	.dwattr $C$DW$574, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$574, DW_AT_data_member_location[DW_OP_plus_uconst 0x49]
	.dwattr $C$DW$574, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$574, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$574, DW_AT_decl_line(0x21a)
	.dwattr $C$DW$574, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$23, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$23, DW_AT_decl_line(0x213)
	.dwattr $C$DW$T$23, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$23

$C$DW$T$198	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$198, DW_AT_name("SlWlanEventP2PConnect_t")
	.dwattr $C$DW$T$198, DW_AT_type(*$C$DW$T$23)
	.dwattr $C$DW$T$198, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$198, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$198, DW_AT_decl_line(0x21b)
	.dwattr $C$DW$T$198, DW_AT_decl_column(0x03)


$C$DW$T$25	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$25, DW_AT_byte_size(0x50)
$C$DW$575	.dwtag  DW_TAG_member
	.dwattr $C$DW$575, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$575, DW_AT_name("Mode")
	.dwattr $C$DW$575, DW_AT_TI_symbol_name("Mode")
	.dwattr $C$DW$575, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$575, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$575, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$575, DW_AT_decl_line(0x2b3)
	.dwattr $C$DW$575, DW_AT_decl_column(0x09)

$C$DW$576	.dwtag  DW_TAG_member
	.dwattr $C$DW$576, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$576, DW_AT_name("ConnStatus")
	.dwattr $C$DW$576, DW_AT_TI_symbol_name("ConnStatus")
	.dwattr $C$DW$576, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$576, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$576, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$576, DW_AT_decl_line(0x2b4)
	.dwattr $C$DW$576, DW_AT_decl_column(0x09)

$C$DW$577	.dwtag  DW_TAG_member
	.dwattr $C$DW$577, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$577, DW_AT_name("SecType")
	.dwattr $C$DW$577, DW_AT_TI_symbol_name("SecType")
	.dwattr $C$DW$577, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$577, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$577, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$577, DW_AT_decl_line(0x2b5)
	.dwattr $C$DW$577, DW_AT_decl_column(0x09)

$C$DW$578	.dwtag  DW_TAG_member
	.dwattr $C$DW$578, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$578, DW_AT_name("Reserved")
	.dwattr $C$DW$578, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$578, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$578, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$578, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$578, DW_AT_decl_line(0x2b6)
	.dwattr $C$DW$578, DW_AT_decl_column(0x09)

$C$DW$579	.dwtag  DW_TAG_member
	.dwattr $C$DW$579, DW_AT_type(*$C$DW$T$24)
	.dwattr $C$DW$579, DW_AT_name("ConnectionInfo")
	.dwattr $C$DW$579, DW_AT_TI_symbol_name("ConnectionInfo")
	.dwattr $C$DW$579, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$579, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$579, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$579, DW_AT_decl_line(0x2b7)
	.dwattr $C$DW$579, DW_AT_decl_column(0x1c)

	.dwattr $C$DW$T$25, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$25, DW_AT_decl_line(0x2b2)
	.dwattr $C$DW$T$25, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$25

$C$DW$T$449	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$449, DW_AT_name("SlWlanConnStatusParam_t")
	.dwattr $C$DW$T$449, DW_AT_type(*$C$DW$T$25)
	.dwattr $C$DW$T$449, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$449, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$449, DW_AT_decl_line(0x2b8)
	.dwattr $C$DW$T$449, DW_AT_decl_column(0x02)


$C$DW$T$29	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$29, DW_AT_byte_size(0x08)
$C$DW$580	.dwtag  DW_TAG_member
	.dwattr $C$DW$580, DW_AT_type(*$C$DW$T$28)
	.dwattr $C$DW$580, DW_AT_name("pEntry")
	.dwattr $C$DW$580, DW_AT_TI_symbol_name("pEntry")
	.dwattr $C$DW$580, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$580, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$580, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$580, DW_AT_decl_line(0x64)
	.dwattr $C$DW$580, DW_AT_decl_column(0x13)

$C$DW$581	.dwtag  DW_TAG_member
	.dwattr $C$DW$581, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$581, DW_AT_name("pValue")
	.dwattr $C$DW$581, DW_AT_TI_symbol_name("pValue")
	.dwattr $C$DW$581, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$581, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$581, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$581, DW_AT_decl_line(0x65)
	.dwattr $C$DW$581, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$29, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$T$29, DW_AT_decl_line(0x63)
	.dwattr $C$DW$T$29, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$29

$C$DW$T$451	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$451, DW_AT_name("tSimpleLinkSpawnMsg")
	.dwattr $C$DW$T$451, DW_AT_type(*$C$DW$T$29)
	.dwattr $C$DW$T$451, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$451, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$T$451, DW_AT_decl_line(0x66)
	.dwattr $C$DW$T$451, DW_AT_decl_column(0x02)


$C$DW$T$31	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$31, DW_AT_byte_size(0x24)
$C$DW$582	.dwtag  DW_TAG_member
	.dwattr $C$DW$582, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$582, DW_AT_name("Flags")
	.dwattr $C$DW$582, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$582, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$582, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$582, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$582, DW_AT_decl_line(0x90)
	.dwattr $C$DW$582, DW_AT_decl_column(0x0b)

$C$DW$583	.dwtag  DW_TAG_member
	.dwattr $C$DW$583, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$583, DW_AT_name("Len")
	.dwattr $C$DW$583, DW_AT_TI_symbol_name("Len")
	.dwattr $C$DW$583, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$583, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$583, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$583, DW_AT_decl_line(0x91)
	.dwattr $C$DW$583, DW_AT_decl_column(0x0b)

$C$DW$584	.dwtag  DW_TAG_member
	.dwattr $C$DW$584, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$584, DW_AT_name("MaxSize")
	.dwattr $C$DW$584, DW_AT_TI_symbol_name("MaxSize")
	.dwattr $C$DW$584, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$584, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$584, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$584, DW_AT_decl_line(0x92)
	.dwattr $C$DW$584, DW_AT_decl_column(0x0b)

$C$DW$585	.dwtag  DW_TAG_member
	.dwattr $C$DW$585, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$585, DW_AT_name("Token")
	.dwattr $C$DW$585, DW_AT_TI_symbol_name("Token")
	.dwattr $C$DW$585, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$585, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$585, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$585, DW_AT_decl_line(0x93)
	.dwattr $C$DW$585, DW_AT_decl_column(0x0b)

$C$DW$586	.dwtag  DW_TAG_member
	.dwattr $C$DW$586, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$586, DW_AT_name("StorageSize")
	.dwattr $C$DW$586, DW_AT_TI_symbol_name("StorageSize")
	.dwattr $C$DW$586, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$586, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$586, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$586, DW_AT_decl_line(0x94)
	.dwattr $C$DW$586, DW_AT_decl_column(0x0b)

$C$DW$587	.dwtag  DW_TAG_member
	.dwattr $C$DW$587, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$587, DW_AT_name("WriteCounter")
	.dwattr $C$DW$587, DW_AT_TI_symbol_name("WriteCounter")
	.dwattr $C$DW$587, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$587, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$587, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$587, DW_AT_decl_line(0x95)
	.dwattr $C$DW$587, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$31, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$31, DW_AT_decl_line(0x8f)
	.dwattr $C$DW$T$31, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$31

$C$DW$T$452	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$452, DW_AT_name("SlFsFileInfo_t")
	.dwattr $C$DW$T$452, DW_AT_type(*$C$DW$T$31)
	.dwattr $C$DW$T$452, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$452, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$452, DW_AT_decl_line(0x96)
	.dwattr $C$DW$T$452, DW_AT_decl_column(0x02)


$C$DW$T$32	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$32, DW_AT_byte_size(0x10)
$C$DW$588	.dwtag  DW_TAG_member
	.dwattr $C$DW$588, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$588, DW_AT_name("Key")
	.dwattr $C$DW$588, DW_AT_TI_symbol_name("Key")
	.dwattr $C$DW$588, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$588, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$588, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$588, DW_AT_decl_line(0xb1)
	.dwattr $C$DW$588, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$32, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$32, DW_AT_decl_line(0xb0)
	.dwattr $C$DW$T$32, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$32

$C$DW$T$453	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$453, DW_AT_name("SlFsKey_t")
	.dwattr $C$DW$T$453, DW_AT_type(*$C$DW$T$32)
	.dwattr $C$DW$T$453, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$453, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$453, DW_AT_decl_line(0xb2)
	.dwattr $C$DW$T$453, DW_AT_decl_column(0x02)


$C$DW$T$33	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$33, DW_AT_byte_size(0x01)
$C$DW$589	.dwtag  DW_TAG_member
	.dwattr $C$DW$589, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$589, DW_AT_name("Index")
	.dwattr $C$DW$589, DW_AT_TI_symbol_name("Index")
	.dwattr $C$DW$589, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$589, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$589, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$589, DW_AT_decl_line(0xb6)
	.dwattr $C$DW$589, DW_AT_decl_column(0x09)

	.dwattr $C$DW$T$33, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$33, DW_AT_decl_line(0xb5)
	.dwattr $C$DW$T$33, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$33

$C$DW$T$207	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$207, DW_AT_name("SlFsFileNameIndex_t")
	.dwattr $C$DW$T$207, DW_AT_type(*$C$DW$T$33)
	.dwattr $C$DW$T$207, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$207, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$207, DW_AT_decl_line(0xb7)
	.dwattr $C$DW$T$207, DW_AT_decl_column(0x02)


$C$DW$T$34	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$34, DW_AT_byte_size(0x04)
$C$DW$590	.dwtag  DW_TAG_member
	.dwattr $C$DW$590, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$590, DW_AT_name("Operation")
	.dwattr $C$DW$590, DW_AT_TI_symbol_name("Operation")
	.dwattr $C$DW$590, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$590, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$590, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$590, DW_AT_decl_line(0xca)
	.dwattr $C$DW$590, DW_AT_decl_column(0x08)

	.dwattr $C$DW$T$34, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$34, DW_AT_decl_line(0xc9)
	.dwattr $C$DW$T$34, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$34

$C$DW$T$454	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$454, DW_AT_name("SlFsRetToFactoryCommand_t")
	.dwattr $C$DW$T$454, DW_AT_type(*$C$DW$T$34)
	.dwattr $C$DW$T$454, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$454, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$454, DW_AT_decl_line(0xcb)
	.dwattr $C$DW$T$454, DW_AT_decl_column(0x02)


$C$DW$T$35	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$35, DW_AT_byte_size(0x04)
$C$DW$591	.dwtag  DW_TAG_member
	.dwattr $C$DW$591, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$591, DW_AT_name("IncludeFilters")
	.dwattr $C$DW$591, DW_AT_TI_symbol_name("IncludeFilters")
	.dwattr $C$DW$591, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$591, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$591, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$591, DW_AT_decl_line(0xd1)
	.dwattr $C$DW$591, DW_AT_decl_column(0x12)

	.dwattr $C$DW$T$35, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$35, DW_AT_decl_line(0xd0)
	.dwattr $C$DW$T$35, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$35

$C$DW$T$455	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$455, DW_AT_name("SlFsControl_t")
	.dwattr $C$DW$T$455, DW_AT_type(*$C$DW$T$35)
	.dwattr $C$DW$T$455, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$455, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$455, DW_AT_decl_line(0xd3)
	.dwattr $C$DW$T$455, DW_AT_decl_column(0x02)


$C$DW$T$37	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$37, DW_AT_byte_size(0x10)
$C$DW$592	.dwtag  DW_TAG_member
	.dwattr $C$DW$592, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$592, DW_AT_name("DeviceBlockSize")
	.dwattr $C$DW$592, DW_AT_TI_symbol_name("DeviceBlockSize")
	.dwattr $C$DW$592, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$592, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$592, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$592, DW_AT_decl_line(0xd7)
	.dwattr $C$DW$592, DW_AT_decl_column(0x08)

$C$DW$593	.dwtag  DW_TAG_member
	.dwattr $C$DW$593, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$593, DW_AT_name("DeviceBlocksCapacity")
	.dwattr $C$DW$593, DW_AT_TI_symbol_name("DeviceBlocksCapacity")
	.dwattr $C$DW$593, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$593, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$593, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$593, DW_AT_decl_line(0xd8)
	.dwattr $C$DW$593, DW_AT_decl_column(0x08)

$C$DW$594	.dwtag  DW_TAG_member
	.dwattr $C$DW$594, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$594, DW_AT_name("NumOfAllocatedBlocks")
	.dwattr $C$DW$594, DW_AT_TI_symbol_name("NumOfAllocatedBlocks")
	.dwattr $C$DW$594, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$594, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$594, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$594, DW_AT_decl_line(0xd9)
	.dwattr $C$DW$594, DW_AT_decl_column(0x08)

$C$DW$595	.dwtag  DW_TAG_member
	.dwattr $C$DW$595, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$595, DW_AT_name("NumOfReservedBlocks")
	.dwattr $C$DW$595, DW_AT_TI_symbol_name("NumOfReservedBlocks")
	.dwattr $C$DW$595, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$595, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$595, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$595, DW_AT_decl_line(0xda)
	.dwattr $C$DW$595, DW_AT_decl_column(0x08)

$C$DW$596	.dwtag  DW_TAG_member
	.dwattr $C$DW$596, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$596, DW_AT_name("NumOfReservedBlocksForSystemfiles")
	.dwattr $C$DW$596, DW_AT_TI_symbol_name("NumOfReservedBlocksForSystemfiles")
	.dwattr $C$DW$596, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$596, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$596, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$596, DW_AT_decl_line(0xdb)
	.dwattr $C$DW$596, DW_AT_decl_column(0x08)

$C$DW$597	.dwtag  DW_TAG_member
	.dwattr $C$DW$597, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$597, DW_AT_name("LargestAllocatedGapInBlocks")
	.dwattr $C$DW$597, DW_AT_TI_symbol_name("LargestAllocatedGapInBlocks")
	.dwattr $C$DW$597, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$597, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$597, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$597, DW_AT_decl_line(0xdc)
	.dwattr $C$DW$597, DW_AT_decl_column(0x08)

$C$DW$598	.dwtag  DW_TAG_member
	.dwattr $C$DW$598, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$598, DW_AT_name("NumOfAvailableBlocksForUserFiles")
	.dwattr $C$DW$598, DW_AT_TI_symbol_name("NumOfAvailableBlocksForUserFiles")
	.dwattr $C$DW$598, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$598, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$598, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$598, DW_AT_decl_line(0xdd)
	.dwattr $C$DW$598, DW_AT_decl_column(0x08)

$C$DW$599	.dwtag  DW_TAG_member
	.dwattr $C$DW$599, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$599, DW_AT_name("Padding")
	.dwattr $C$DW$599, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$599, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr $C$DW$599, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$599, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$599, DW_AT_decl_line(0xde)
	.dwattr $C$DW$599, DW_AT_decl_column(0x08)

	.dwattr $C$DW$T$37, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$37, DW_AT_decl_line(0xd6)
	.dwattr $C$DW$T$37, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$37

$C$DW$T$39	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$39, DW_AT_name("SlFsControlDeviceUsage_t")
	.dwattr $C$DW$T$39, DW_AT_type(*$C$DW$T$37)
	.dwattr $C$DW$T$39, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$39, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$39, DW_AT_decl_line(0xdf)
	.dwattr $C$DW$T$39, DW_AT_decl_column(0x03)


$C$DW$T$38	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$38, DW_AT_byte_size(0x14)
$C$DW$600	.dwtag  DW_TAG_member
	.dwattr $C$DW$600, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$600, DW_AT_name("MaxFsFiles")
	.dwattr $C$DW$600, DW_AT_TI_symbol_name("MaxFsFiles")
	.dwattr $C$DW$600, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$600, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$600, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$600, DW_AT_decl_line(0xe3)
	.dwattr $C$DW$600, DW_AT_decl_column(0x08)

$C$DW$601	.dwtag  DW_TAG_member
	.dwattr $C$DW$601, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$601, DW_AT_name("IsDevlopmentFormatType")
	.dwattr $C$DW$601, DW_AT_TI_symbol_name("IsDevlopmentFormatType")
	.dwattr $C$DW$601, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$601, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$601, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$601, DW_AT_decl_line(0xe4)
	.dwattr $C$DW$601, DW_AT_decl_column(0x08)

$C$DW$602	.dwtag  DW_TAG_member
	.dwattr $C$DW$602, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$602, DW_AT_name("Bundlestate")
	.dwattr $C$DW$602, DW_AT_TI_symbol_name("Bundlestate")
	.dwattr $C$DW$602, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$602, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$602, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$602, DW_AT_decl_line(0xe5)
	.dwattr $C$DW$602, DW_AT_decl_column(0x08)

$C$DW$603	.dwtag  DW_TAG_member
	.dwattr $C$DW$603, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$603, DW_AT_name("Reserved")
	.dwattr $C$DW$603, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$603, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$603, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$603, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$603, DW_AT_decl_line(0xe6)
	.dwattr $C$DW$603, DW_AT_decl_column(0x08)

$C$DW$604	.dwtag  DW_TAG_member
	.dwattr $C$DW$604, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$604, DW_AT_name("MaxFsFilesReservedForSysFiles")
	.dwattr $C$DW$604, DW_AT_TI_symbol_name("MaxFsFilesReservedForSysFiles")
	.dwattr $C$DW$604, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$604, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$604, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$604, DW_AT_decl_line(0xe7)
	.dwattr $C$DW$604, DW_AT_decl_column(0x08)

$C$DW$605	.dwtag  DW_TAG_member
	.dwattr $C$DW$605, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$605, DW_AT_name("ActualNumOfUserFiles")
	.dwattr $C$DW$605, DW_AT_TI_symbol_name("ActualNumOfUserFiles")
	.dwattr $C$DW$605, DW_AT_data_member_location[DW_OP_plus_uconst 0x5]
	.dwattr $C$DW$605, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$605, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$605, DW_AT_decl_line(0xe8)
	.dwattr $C$DW$605, DW_AT_decl_column(0x08)

$C$DW$606	.dwtag  DW_TAG_member
	.dwattr $C$DW$606, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$606, DW_AT_name("ActualNumOfSysFiles")
	.dwattr $C$DW$606, DW_AT_TI_symbol_name("ActualNumOfSysFiles")
	.dwattr $C$DW$606, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$606, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$606, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$606, DW_AT_decl_line(0xe9)
	.dwattr $C$DW$606, DW_AT_decl_column(0x08)

$C$DW$607	.dwtag  DW_TAG_member
	.dwattr $C$DW$607, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$607, DW_AT_name("Padding")
	.dwattr $C$DW$607, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$607, DW_AT_data_member_location[DW_OP_plus_uconst 0x7]
	.dwattr $C$DW$607, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$607, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$607, DW_AT_decl_line(0xea)
	.dwattr $C$DW$607, DW_AT_decl_column(0x08)

$C$DW$608	.dwtag  DW_TAG_member
	.dwattr $C$DW$608, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$608, DW_AT_name("NumOfAlerts")
	.dwattr $C$DW$608, DW_AT_TI_symbol_name("NumOfAlerts")
	.dwattr $C$DW$608, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$608, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$608, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$608, DW_AT_decl_line(0xeb)
	.dwattr $C$DW$608, DW_AT_decl_column(0x08)

$C$DW$609	.dwtag  DW_TAG_member
	.dwattr $C$DW$609, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$609, DW_AT_name("NumOfAlertsThreshold")
	.dwattr $C$DW$609, DW_AT_TI_symbol_name("NumOfAlertsThreshold")
	.dwattr $C$DW$609, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$609, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$609, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$609, DW_AT_decl_line(0xec)
	.dwattr $C$DW$609, DW_AT_decl_column(0x08)

$C$DW$610	.dwtag  DW_TAG_member
	.dwattr $C$DW$610, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$610, DW_AT_name("FATWriteCounter")
	.dwattr $C$DW$610, DW_AT_TI_symbol_name("FATWriteCounter")
	.dwattr $C$DW$610, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$610, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$610, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$610, DW_AT_decl_line(0xed)
	.dwattr $C$DW$610, DW_AT_decl_column(0x08)

$C$DW$611	.dwtag  DW_TAG_member
	.dwattr $C$DW$611, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$611, DW_AT_name("Padding2")
	.dwattr $C$DW$611, DW_AT_TI_symbol_name("Padding2")
	.dwattr $C$DW$611, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$611, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$611, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$611, DW_AT_decl_line(0xee)
	.dwattr $C$DW$611, DW_AT_decl_column(0x08)

	.dwattr $C$DW$T$38, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$38, DW_AT_decl_line(0xe2)
	.dwattr $C$DW$T$38, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$38

$C$DW$T$40	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$40, DW_AT_name("SlFsControlFilesUsage_t")
	.dwattr $C$DW$T$40, DW_AT_type(*$C$DW$T$38)
	.dwattr $C$DW$T$40, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$40, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$40, DW_AT_decl_line(0xef)
	.dwattr $C$DW$T$40, DW_AT_decl_column(0x02)


$C$DW$T$41	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$41, DW_AT_byte_size(0x24)
$C$DW$612	.dwtag  DW_TAG_member
	.dwattr $C$DW$612, DW_AT_type(*$C$DW$T$39)
	.dwattr $C$DW$612, DW_AT_name("DeviceUsage")
	.dwattr $C$DW$612, DW_AT_TI_symbol_name("DeviceUsage")
	.dwattr $C$DW$612, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$612, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$612, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$612, DW_AT_decl_line(0xf4)
	.dwattr $C$DW$612, DW_AT_decl_column(0x1e)

$C$DW$613	.dwtag  DW_TAG_member
	.dwattr $C$DW$613, DW_AT_type(*$C$DW$T$40)
	.dwattr $C$DW$613, DW_AT_name("FilesUsage")
	.dwattr $C$DW$613, DW_AT_TI_symbol_name("FilesUsage")
	.dwattr $C$DW$613, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$613, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$613, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$613, DW_AT_decl_line(0xf5)
	.dwattr $C$DW$613, DW_AT_decl_column(0x1e)

	.dwattr $C$DW$T$41, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$41, DW_AT_decl_line(0xf3)
	.dwattr $C$DW$T$41, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$41

$C$DW$T$456	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$456, DW_AT_name("SlFsControlGetStorageInfoResponse_t")
	.dwattr $C$DW$T$456, DW_AT_type(*$C$DW$T$41)
	.dwattr $C$DW$T$456, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$456, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$456, DW_AT_decl_line(0xf6)
	.dwattr $C$DW$T$456, DW_AT_decl_column(0x03)


$C$DW$T$42	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$42, DW_AT_byte_size(0x0c)
$C$DW$614	.dwtag  DW_TAG_member
	.dwattr $C$DW$614, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$614, DW_AT_name("IncludeFilters")
	.dwattr $C$DW$614, DW_AT_TI_symbol_name("IncludeFilters")
	.dwattr $C$DW$614, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$614, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$614, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$614, DW_AT_decl_line(0xfa)
	.dwattr $C$DW$614, DW_AT_decl_column(0x0b)

$C$DW$615	.dwtag  DW_TAG_member
	.dwattr $C$DW$615, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$615, DW_AT_name("OpenedForWriteCnt")
	.dwattr $C$DW$615, DW_AT_TI_symbol_name("OpenedForWriteCnt")
	.dwattr $C$DW$615, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$615, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$615, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$615, DW_AT_decl_line(0xfb)
	.dwattr $C$DW$615, DW_AT_decl_column(0x0b)

$C$DW$616	.dwtag  DW_TAG_member
	.dwattr $C$DW$616, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$616, DW_AT_name("OpeneForReadCnt")
	.dwattr $C$DW$616, DW_AT_TI_symbol_name("OpeneForReadCnt")
	.dwattr $C$DW$616, DW_AT_data_member_location[DW_OP_plus_uconst 0x5]
	.dwattr $C$DW$616, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$616, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$616, DW_AT_decl_line(0xfc)
	.dwattr $C$DW$616, DW_AT_decl_column(0x0b)

$C$DW$617	.dwtag  DW_TAG_member
	.dwattr $C$DW$617, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$617, DW_AT_name("ClosedFilesCnt")
	.dwattr $C$DW$617, DW_AT_TI_symbol_name("ClosedFilesCnt")
	.dwattr $C$DW$617, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$617, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$617, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$617, DW_AT_decl_line(0xfd)
	.dwattr $C$DW$617, DW_AT_decl_column(0x0b)

$C$DW$618	.dwtag  DW_TAG_member
	.dwattr $C$DW$618, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$618, DW_AT_name("OpenedForWriteCntWithValidFailSafeImage")
	.dwattr $C$DW$618, DW_AT_TI_symbol_name("OpenedForWriteCntWithValidFailSafeImage")
	.dwattr $C$DW$618, DW_AT_data_member_location[DW_OP_plus_uconst 0x7]
	.dwattr $C$DW$618, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$618, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$618, DW_AT_decl_line(0xfe)
	.dwattr $C$DW$618, DW_AT_decl_column(0x0b)

$C$DW$619	.dwtag  DW_TAG_member
	.dwattr $C$DW$619, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$619, DW_AT_name("OpeneForReadCntWithValidFailSafeImage")
	.dwattr $C$DW$619, DW_AT_TI_symbol_name("OpeneForReadCntWithValidFailSafeImage")
	.dwattr $C$DW$619, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$619, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$619, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$619, DW_AT_decl_line(0xff)
	.dwattr $C$DW$619, DW_AT_decl_column(0x0b)

$C$DW$620	.dwtag  DW_TAG_member
	.dwattr $C$DW$620, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$620, DW_AT_name("ClosedFilesCntWithValidFailSafeImage")
	.dwattr $C$DW$620, DW_AT_TI_symbol_name("ClosedFilesCntWithValidFailSafeImage")
	.dwattr $C$DW$620, DW_AT_data_member_location[DW_OP_plus_uconst 0x9]
	.dwattr $C$DW$620, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$620, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$620, DW_AT_decl_line(0x100)
	.dwattr $C$DW$620, DW_AT_decl_column(0x0b)

$C$DW$621	.dwtag  DW_TAG_member
	.dwattr $C$DW$621, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$621, DW_AT_name("padding")
	.dwattr $C$DW$621, DW_AT_TI_symbol_name("padding")
	.dwattr $C$DW$621, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$621, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$621, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$621, DW_AT_decl_line(0x101)
	.dwattr $C$DW$621, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$42, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$42, DW_AT_decl_line(0xf9)
	.dwattr $C$DW$T$42, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$42

$C$DW$T$457	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$457, DW_AT_name("SlFsControlGetCountersResponse_t")
	.dwattr $C$DW$T$457, DW_AT_type(*$C$DW$T$42)
	.dwattr $C$DW$T$457, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$457, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$457, DW_AT_decl_line(0x102)
	.dwattr $C$DW$T$457, DW_AT_decl_column(0x03)


$C$DW$T$43	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$43, DW_AT_byte_size(0x0c)
$C$DW$622	.dwtag  DW_TAG_member
	.dwattr $C$DW$622, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$622, DW_AT_name("FileMaxSize")
	.dwattr $C$DW$622, DW_AT_TI_symbol_name("FileMaxSize")
	.dwattr $C$DW$622, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$622, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$622, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$622, DW_AT_decl_line(0x111)
	.dwattr $C$DW$622, DW_AT_decl_column(0x0a)

$C$DW$623	.dwtag  DW_TAG_member
	.dwattr $C$DW$623, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$623, DW_AT_name("Properties")
	.dwattr $C$DW$623, DW_AT_TI_symbol_name("Properties")
	.dwattr $C$DW$623, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$623, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$623, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$623, DW_AT_decl_line(0x112)
	.dwattr $C$DW$623, DW_AT_decl_column(0x0a)

$C$DW$624	.dwtag  DW_TAG_member
	.dwattr $C$DW$624, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$624, DW_AT_name("FileAllocatedBlocks")
	.dwattr $C$DW$624, DW_AT_TI_symbol_name("FileAllocatedBlocks")
	.dwattr $C$DW$624, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$624, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$624, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$624, DW_AT_decl_line(0x113)
	.dwattr $C$DW$624, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$43, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$43, DW_AT_decl_line(0x110)
	.dwattr $C$DW$T$43, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$43

$C$DW$T$458	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$458, DW_AT_name("SlFileAttributes_t")
	.dwattr $C$DW$T$458, DW_AT_type(*$C$DW$T$43)
	.dwattr $C$DW$T$458, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$458, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$458, DW_AT_decl_line(0x114)
	.dwattr $C$DW$T$458, DW_AT_decl_column(0x02)


$C$DW$T$44	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$44, DW_AT_byte_size(0x04)
$C$DW$625	.dwtag  DW_TAG_member
	.dwattr $C$DW$625, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$625, DW_AT_name("KeepaliveEnabled")
	.dwattr $C$DW$625, DW_AT_TI_symbol_name("KeepaliveEnabled")
	.dwattr $C$DW$625, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$625, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$625, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$625, DW_AT_decl_line(0xf6)
	.dwattr $C$DW$625, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$44, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$44, DW_AT_decl_line(0xf5)
	.dwattr $C$DW$T$44, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$44

$C$DW$T$459	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$459, DW_AT_name("SlSockKeepalive_t")
	.dwattr $C$DW$T$459, DW_AT_type(*$C$DW$T$44)
	.dwattr $C$DW$T$459, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$459, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$459, DW_AT_decl_line(0xf7)
	.dwattr $C$DW$T$459, DW_AT_decl_column(0x02)


$C$DW$T$45	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$45, DW_AT_byte_size(0x04)
$C$DW$626	.dwtag  DW_TAG_member
	.dwattr $C$DW$626, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$626, DW_AT_name("ReuseaddrEnabled")
	.dwattr $C$DW$626, DW_AT_TI_symbol_name("ReuseaddrEnabled")
	.dwattr $C$DW$626, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$626, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$626, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$626, DW_AT_decl_line(0xfb)
	.dwattr $C$DW$626, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$45, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$45, DW_AT_decl_line(0xfa)
	.dwattr $C$DW$T$45, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$45

$C$DW$T$460	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$460, DW_AT_name("SlSockReuseaddr_t")
	.dwattr $C$DW$T$460, DW_AT_type(*$C$DW$T$45)
	.dwattr $C$DW$T$460, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$460, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$460, DW_AT_decl_line(0xfc)
	.dwattr $C$DW$T$460, DW_AT_decl_column(0x02)


$C$DW$T$46	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$46, DW_AT_byte_size(0x04)
$C$DW$627	.dwtag  DW_TAG_member
	.dwattr $C$DW$627, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$627, DW_AT_name("RxIpNoBoundaryEnabled")
	.dwattr $C$DW$627, DW_AT_TI_symbol_name("RxIpNoBoundaryEnabled")
	.dwattr $C$DW$627, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$627, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$627, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$627, DW_AT_decl_line(0x100)
	.dwattr $C$DW$627, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$46, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$46, DW_AT_decl_line(0xff)
	.dwattr $C$DW$T$46, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$46

$C$DW$T$461	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$461, DW_AT_name("SlSockRxNoIpBoundary_t")
	.dwattr $C$DW$T$461, DW_AT_type(*$C$DW$T$46)
	.dwattr $C$DW$T$461, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$461, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$461, DW_AT_decl_line(0x101)
	.dwattr $C$DW$T$461, DW_AT_decl_column(0x03)


$C$DW$T$47	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$47, DW_AT_byte_size(0x04)
$C$DW$628	.dwtag  DW_TAG_member
	.dwattr $C$DW$628, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$628, DW_AT_name("WinSize")
	.dwattr $C$DW$628, DW_AT_TI_symbol_name("WinSize")
	.dwattr $C$DW$628, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$628, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$628, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$628, DW_AT_decl_line(0x105)
	.dwattr $C$DW$628, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$47, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$47, DW_AT_decl_line(0x104)
	.dwattr $C$DW$T$47, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$47

$C$DW$T$462	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$462, DW_AT_name("SlSockWinsize_t")
	.dwattr $C$DW$T$462, DW_AT_type(*$C$DW$T$47)
	.dwattr $C$DW$T$462, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$462, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$462, DW_AT_decl_line(0x106)
	.dwattr $C$DW$T$462, DW_AT_decl_column(0x02)


$C$DW$T$48	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$48, DW_AT_byte_size(0x04)
$C$DW$629	.dwtag  DW_TAG_member
	.dwattr $C$DW$629, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$629, DW_AT_name("NonBlockingEnabled")
	.dwattr $C$DW$629, DW_AT_TI_symbol_name("NonBlockingEnabled")
	.dwattr $C$DW$629, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$629, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$629, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$629, DW_AT_decl_line(0x10a)
	.dwattr $C$DW$629, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$48, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$48, DW_AT_decl_line(0x109)
	.dwattr $C$DW$T$48, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$48

$C$DW$T$463	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$463, DW_AT_name("SlSockNonblocking_t")
	.dwattr $C$DW$T$463, DW_AT_type(*$C$DW$T$48)
	.dwattr $C$DW$T$463, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$463, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$463, DW_AT_decl_line(0x10b)
	.dwattr $C$DW$T$463, DW_AT_decl_column(0x02)


$C$DW$T$50	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$50, DW_AT_byte_size(0x84)
$C$DW$630	.dwtag  DW_TAG_member
	.dwattr $C$DW$630, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$630, DW_AT_name("Sd")
	.dwattr $C$DW$630, DW_AT_TI_symbol_name("Sd")
	.dwattr $C$DW$630, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$630, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$630, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$630, DW_AT_decl_line(0x10f)
	.dwattr $C$DW$630, DW_AT_decl_column(0x0b)

$C$DW$631	.dwtag  DW_TAG_member
	.dwattr $C$DW$631, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$631, DW_AT_name("Type")
	.dwattr $C$DW$631, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$631, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$631, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$631, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$631, DW_AT_decl_line(0x110)
	.dwattr $C$DW$631, DW_AT_decl_column(0x0b)

$C$DW$632	.dwtag  DW_TAG_member
	.dwattr $C$DW$632, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$632, DW_AT_name("Val")
	.dwattr $C$DW$632, DW_AT_TI_symbol_name("Val")
	.dwattr $C$DW$632, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$632, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$632, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$632, DW_AT_decl_line(0x111)
	.dwattr $C$DW$632, DW_AT_decl_column(0x0b)

$C$DW$633	.dwtag  DW_TAG_member
	.dwattr $C$DW$633, DW_AT_type(*$C$DW$T$49)
	.dwattr $C$DW$633, DW_AT_name("pExtraInfo")
	.dwattr $C$DW$633, DW_AT_TI_symbol_name("pExtraInfo")
	.dwattr $C$DW$633, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$633, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$633, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$633, DW_AT_decl_line(0x112)
	.dwattr $C$DW$633, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$50, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$50, DW_AT_decl_line(0x10e)
	.dwattr $C$DW$T$50, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$50

$C$DW$T$211	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$211, DW_AT_name("SlSocketAsyncEvent_t")
	.dwattr $C$DW$T$211, DW_AT_type(*$C$DW$T$50)
	.dwattr $C$DW$T$211, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$211, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$211, DW_AT_decl_line(0x113)
	.dwattr $C$DW$T$211, DW_AT_decl_column(0x03)


$C$DW$T$51	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$51, DW_AT_byte_size(0x04)
$C$DW$634	.dwtag  DW_TAG_member
	.dwattr $C$DW$634, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$634, DW_AT_name("Status")
	.dwattr $C$DW$634, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$634, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$634, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$634, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$634, DW_AT_decl_line(0x117)
	.dwattr $C$DW$634, DW_AT_decl_column(0x10)

$C$DW$635	.dwtag  DW_TAG_member
	.dwattr $C$DW$635, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$635, DW_AT_name("Sd")
	.dwattr $C$DW$635, DW_AT_TI_symbol_name("Sd")
	.dwattr $C$DW$635, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$635, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$635, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$635, DW_AT_decl_line(0x118)
	.dwattr $C$DW$635, DW_AT_decl_column(0x10)

$C$DW$636	.dwtag  DW_TAG_member
	.dwattr $C$DW$636, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$636, DW_AT_name("Padding")
	.dwattr $C$DW$636, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$636, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$636, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$636, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$636, DW_AT_decl_line(0x119)
	.dwattr $C$DW$636, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$51, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$51, DW_AT_decl_line(0x116)
	.dwattr $C$DW$T$51, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$51

$C$DW$T$210	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$210, DW_AT_name("SlSockTxFailEventData_t")
	.dwattr $C$DW$T$210, DW_AT_type(*$C$DW$T$51)
	.dwattr $C$DW$T$210, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$210, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$210, DW_AT_decl_line(0x11a)
	.dwattr $C$DW$T$210, DW_AT_decl_column(0x03)


$C$DW$T$53	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$53, DW_AT_byte_size(0x88)
$C$DW$637	.dwtag  DW_TAG_member
	.dwattr $C$DW$637, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$637, DW_AT_name("Event")
	.dwattr $C$DW$637, DW_AT_TI_symbol_name("Event")
	.dwattr $C$DW$637, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$637, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$637, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$637, DW_AT_decl_line(0x126)
	.dwattr $C$DW$637, DW_AT_decl_column(0x1c)

$C$DW$638	.dwtag  DW_TAG_member
	.dwattr $C$DW$638, DW_AT_type(*$C$DW$T$52)
	.dwattr $C$DW$638, DW_AT_name("SocketAsyncEvent")
	.dwattr $C$DW$638, DW_AT_TI_symbol_name("SocketAsyncEvent")
	.dwattr $C$DW$638, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$638, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$638, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$638, DW_AT_decl_line(0x127)
	.dwattr $C$DW$638, DW_AT_decl_column(0x1c)

	.dwattr $C$DW$T$53, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$53, DW_AT_decl_line(0x125)
	.dwattr $C$DW$T$53, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$53

$C$DW$T$464	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$464, DW_AT_name("SlSockEvent_t")
	.dwattr $C$DW$T$464, DW_AT_type(*$C$DW$T$53)
	.dwattr $C$DW$T$464, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$464, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$464, DW_AT_decl_line(0x128)
	.dwattr $C$DW$T$464, DW_AT_decl_column(0x03)


$C$DW$T$54	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$54, DW_AT_byte_size(0x08)
$C$DW$639	.dwtag  DW_TAG_member
	.dwattr $C$DW$639, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$639, DW_AT_name("Event")
	.dwattr $C$DW$639, DW_AT_TI_symbol_name("Event")
	.dwattr $C$DW$639, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$639, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$639, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$639, DW_AT_decl_line(0x12c)
	.dwattr $C$DW$639, DW_AT_decl_column(0x11)

$C$DW$640	.dwtag  DW_TAG_member
	.dwattr $C$DW$640, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$640, DW_AT_name("EventData")
	.dwattr $C$DW$640, DW_AT_TI_symbol_name("EventData")
	.dwattr $C$DW$640, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$640, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$640, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$640, DW_AT_decl_line(0x12d)
	.dwattr $C$DW$640, DW_AT_decl_column(0x11)

	.dwattr $C$DW$T$54, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$54, DW_AT_decl_line(0x12b)
	.dwattr $C$DW$T$54, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$54

$C$DW$T$465	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$465, DW_AT_name("SlSockTriggerEvent_t")
	.dwattr $C$DW$T$465, DW_AT_type(*$C$DW$T$54)
	.dwattr $C$DW$T$465, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$465, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$465, DW_AT_decl_line(0x12e)
	.dwattr $C$DW$T$465, DW_AT_decl_column(0x03)


$C$DW$T$55	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$55, DW_AT_byte_size(0x04)
$C$DW$641	.dwtag  DW_TAG_member
	.dwattr $C$DW$641, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$641, DW_AT_name("SecureALPN")
	.dwattr $C$DW$641, DW_AT_TI_symbol_name("SecureALPN")
	.dwattr $C$DW$641, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$641, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$641, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$641, DW_AT_decl_line(0x133)
	.dwattr $C$DW$641, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$55, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$55, DW_AT_decl_line(0x132)
	.dwattr $C$DW$T$55, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$55

$C$DW$T$466	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$466, DW_AT_name("SlSockSecureALPN_t")
	.dwattr $C$DW$T$466, DW_AT_type(*$C$DW$T$55)
	.dwattr $C$DW$T$466, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$466, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$466, DW_AT_decl_line(0x134)
	.dwattr $C$DW$T$466, DW_AT_decl_column(0x03)


$C$DW$T$56	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$56, DW_AT_byte_size(0x04)
$C$DW$642	.dwtag  DW_TAG_member
	.dwattr $C$DW$642, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$642, DW_AT_name("SecureMask")
	.dwattr $C$DW$642, DW_AT_TI_symbol_name("SecureMask")
	.dwattr $C$DW$642, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$642, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$642, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$642, DW_AT_decl_line(0x138)
	.dwattr $C$DW$642, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$56, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$56, DW_AT_decl_line(0x137)
	.dwattr $C$DW$T$56, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$56

$C$DW$T$467	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$467, DW_AT_name("SlSockSecureMask_t")
	.dwattr $C$DW$T$467, DW_AT_type(*$C$DW$T$56)
	.dwattr $C$DW$T$467, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$467, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$467, DW_AT_decl_line(0x139)
	.dwattr $C$DW$T$467, DW_AT_decl_column(0x03)


$C$DW$T$57	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$57, DW_AT_byte_size(0x01)
$C$DW$643	.dwtag  DW_TAG_member
	.dwattr $C$DW$643, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$643, DW_AT_name("SecureMethod")
	.dwattr $C$DW$643, DW_AT_TI_symbol_name("SecureMethod")
	.dwattr $C$DW$643, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$643, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$643, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$643, DW_AT_decl_line(0x13d)
	.dwattr $C$DW$643, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$57, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$57, DW_AT_decl_line(0x13c)
	.dwattr $C$DW$T$57, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$57

$C$DW$T$468	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$468, DW_AT_name("SlSockSecureMethod_t")
	.dwattr $C$DW$T$468, DW_AT_type(*$C$DW$T$57)
	.dwattr $C$DW$T$468, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$468, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$468, DW_AT_decl_line(0x13e)
	.dwattr $C$DW$T$468, DW_AT_decl_column(0x03)


$C$DW$T$61	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$61, DW_AT_byte_size(0x38)
$C$DW$644	.dwtag  DW_TAG_member
	.dwattr $C$DW$644, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$644, DW_AT_name("SubjectNameXoredSha1")
	.dwattr $C$DW$644, DW_AT_TI_symbol_name("SubjectNameXoredSha1")
	.dwattr $C$DW$644, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$644, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$644, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$644, DW_AT_decl_line(0x142)
	.dwattr $C$DW$644, DW_AT_decl_column(0x0a)

$C$DW$645	.dwtag  DW_TAG_member
	.dwattr $C$DW$645, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$645, DW_AT_name("IssuerNameXoredSha1")
	.dwattr $C$DW$645, DW_AT_TI_symbol_name("IssuerNameXoredSha1")
	.dwattr $C$DW$645, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$645, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$645, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$645, DW_AT_decl_line(0x143)
	.dwattr $C$DW$645, DW_AT_decl_column(0x0a)

$C$DW$646	.dwtag  DW_TAG_member
	.dwattr $C$DW$646, DW_AT_type(*$C$DW$T$58)
	.dwattr $C$DW$646, DW_AT_name("FromDate")
	.dwattr $C$DW$646, DW_AT_TI_symbol_name("FromDate")
	.dwattr $C$DW$646, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$646, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$646, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$646, DW_AT_decl_line(0x144)
	.dwattr $C$DW$646, DW_AT_decl_column(0x0a)

$C$DW$647	.dwtag  DW_TAG_member
	.dwattr $C$DW$647, DW_AT_type(*$C$DW$T$58)
	.dwattr $C$DW$647, DW_AT_name("ToDate")
	.dwattr $C$DW$647, DW_AT_TI_symbol_name("ToDate")
	.dwattr $C$DW$647, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$647, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$647, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$647, DW_AT_decl_line(0x145)
	.dwattr $C$DW$647, DW_AT_decl_column(0x0a)

$C$DW$648	.dwtag  DW_TAG_member
	.dwattr $C$DW$648, DW_AT_type(*$C$DW$T$59)
	.dwattr $C$DW$648, DW_AT_name("SubjectName")
	.dwattr $C$DW$648, DW_AT_TI_symbol_name("SubjectName")
	.dwattr $C$DW$648, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$648, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$648, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$648, DW_AT_decl_line(0x146)
	.dwattr $C$DW$648, DW_AT_decl_column(0x0a)

$C$DW$649	.dwtag  DW_TAG_member
	.dwattr $C$DW$649, DW_AT_type(*$C$DW$T$59)
	.dwattr $C$DW$649, DW_AT_name("IssuerName")
	.dwattr $C$DW$649, DW_AT_TI_symbol_name("IssuerName")
	.dwattr $C$DW$649, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$649, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$649, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$649, DW_AT_decl_line(0x147)
	.dwattr $C$DW$649, DW_AT_decl_column(0x0a)

$C$DW$650	.dwtag  DW_TAG_member
	.dwattr $C$DW$650, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$650, DW_AT_name("SubjectNameLen")
	.dwattr $C$DW$650, DW_AT_TI_symbol_name("SubjectNameLen")
	.dwattr $C$DW$650, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$650, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$650, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$650, DW_AT_decl_line(0x148)
	.dwattr $C$DW$650, DW_AT_decl_column(0x0a)

$C$DW$651	.dwtag  DW_TAG_member
	.dwattr $C$DW$651, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$651, DW_AT_name("IssuerNameLen")
	.dwattr $C$DW$651, DW_AT_TI_symbol_name("IssuerNameLen")
	.dwattr $C$DW$651, DW_AT_data_member_location[DW_OP_plus_uconst 0x35]
	.dwattr $C$DW$651, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$651, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$651, DW_AT_decl_line(0x149)
	.dwattr $C$DW$651, DW_AT_decl_column(0x0a)

$C$DW$652	.dwtag  DW_TAG_member
	.dwattr $C$DW$652, DW_AT_type(*$C$DW$T$60)
	.dwattr $C$DW$652, DW_AT_name("Padding")
	.dwattr $C$DW$652, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$652, DW_AT_data_member_location[DW_OP_plus_uconst 0x36]
	.dwattr $C$DW$652, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$652, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$652, DW_AT_decl_line(0x14a)
	.dwattr $C$DW$652, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$61, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$61, DW_AT_decl_line(0x141)
	.dwattr $C$DW$T$61, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$61

$C$DW$T$62	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$62, DW_AT_name("SlSockSSLCertInfo_t")
	.dwattr $C$DW$T$62, DW_AT_type(*$C$DW$T$61)
	.dwattr $C$DW$T$62, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$62, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$62, DW_AT_decl_line(0x14b)
	.dwattr $C$DW$T$62, DW_AT_decl_column(0x03)


$C$DW$T$63	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$63, DW_AT_byte_size(0x48)
$C$DW$653	.dwtag  DW_TAG_member
	.dwattr $C$DW$653, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$653, DW_AT_name("SecureVersion")
	.dwattr $C$DW$653, DW_AT_TI_symbol_name("SecureVersion")
	.dwattr $C$DW$653, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$653, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$653, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$653, DW_AT_decl_line(0x150)
	.dwattr $C$DW$653, DW_AT_decl_column(0x0e)

$C$DW$654	.dwtag  DW_TAG_member
	.dwattr $C$DW$654, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$654, DW_AT_name("SecureCipherSuit")
	.dwattr $C$DW$654, DW_AT_TI_symbol_name("SecureCipherSuit")
	.dwattr $C$DW$654, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$654, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$654, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$654, DW_AT_decl_line(0x151)
	.dwattr $C$DW$654, DW_AT_decl_column(0x0e)

$C$DW$655	.dwtag  DW_TAG_member
	.dwattr $C$DW$655, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$655, DW_AT_name("SecureIsPeerValidated")
	.dwattr $C$DW$655, DW_AT_TI_symbol_name("SecureIsPeerValidated")
	.dwattr $C$DW$655, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$655, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$655, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$655, DW_AT_decl_line(0x152)
	.dwattr $C$DW$655, DW_AT_decl_column(0x0e)

$C$DW$656	.dwtag  DW_TAG_member
	.dwattr $C$DW$656, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$656, DW_AT_name("SecureALPNChosenProtocol")
	.dwattr $C$DW$656, DW_AT_TI_symbol_name("SecureALPNChosenProtocol")
	.dwattr $C$DW$656, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$656, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$656, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$656, DW_AT_decl_line(0x153)
	.dwattr $C$DW$656, DW_AT_decl_column(0x0e)

$C$DW$657	.dwtag  DW_TAG_member
	.dwattr $C$DW$657, DW_AT_type(*$C$DW$T$62)
	.dwattr $C$DW$657, DW_AT_name("SecurePeerCertinfo")
	.dwattr $C$DW$657, DW_AT_TI_symbol_name("SecurePeerCertinfo")
	.dwattr $C$DW$657, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$657, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$657, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$657, DW_AT_decl_line(0x15a)
	.dwattr $C$DW$657, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$63, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$63, DW_AT_decl_line(0x14f)
	.dwattr $C$DW$T$63, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$63

$C$DW$T$469	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$469, DW_AT_name("SlSockSSLConnectionParams_t")
	.dwattr $C$DW$T$469, DW_AT_type(*$C$DW$T$63)
	.dwattr $C$DW$T$469, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$469, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$469, DW_AT_decl_line(0x15b)
	.dwattr $C$DW$T$469, DW_AT_decl_column(0x03)


$C$DW$T$65	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$65, DW_AT_byte_size(0x08)
$C$DW$658	.dwtag  DW_TAG_member
	.dwattr $C$DW$658, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$658, DW_AT_name("imr_multiaddr")
	.dwattr $C$DW$658, DW_AT_TI_symbol_name("imr_multiaddr")
	.dwattr $C$DW$658, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$658, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$658, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$658, DW_AT_decl_line(0x167)
	.dwattr $C$DW$658, DW_AT_decl_column(0x12)

$C$DW$659	.dwtag  DW_TAG_member
	.dwattr $C$DW$659, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$659, DW_AT_name("imr_interface")
	.dwattr $C$DW$659, DW_AT_TI_symbol_name("imr_interface")
	.dwattr $C$DW$659, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$659, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$659, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$659, DW_AT_decl_line(0x168)
	.dwattr $C$DW$659, DW_AT_decl_column(0x12)

	.dwattr $C$DW$T$65, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$65, DW_AT_decl_line(0x166)
	.dwattr $C$DW$T$65, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$65

$C$DW$T$470	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$470, DW_AT_name("SlSockIpMreq_t")
	.dwattr $C$DW$T$470, DW_AT_type(*$C$DW$T$65)
	.dwattr $C$DW$T$470, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$470, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$470, DW_AT_decl_line(0x169)
	.dwattr $C$DW$T$470, DW_AT_decl_column(0x02)


$C$DW$T$67	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$67, DW_AT_byte_size(0x14)
$C$DW$660	.dwtag  DW_TAG_member
	.dwattr $C$DW$660, DW_AT_type(*$C$DW$T$66)
	.dwattr $C$DW$660, DW_AT_name("ipv6mr_multiaddr")
	.dwattr $C$DW$660, DW_AT_TI_symbol_name("ipv6mr_multiaddr")
	.dwattr $C$DW$660, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$660, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$660, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$660, DW_AT_decl_line(0x16c)
	.dwattr $C$DW$660, DW_AT_decl_column(0x11)

$C$DW$661	.dwtag  DW_TAG_member
	.dwattr $C$DW$661, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$661, DW_AT_name("ipv6mr_interface")
	.dwattr $C$DW$661, DW_AT_TI_symbol_name("ipv6mr_interface")
	.dwattr $C$DW$661, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$661, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$661, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$661, DW_AT_decl_line(0x16d)
	.dwattr $C$DW$661, DW_AT_decl_column(0x11)

	.dwattr $C$DW$T$67, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$67, DW_AT_decl_line(0x16b)
	.dwattr $C$DW$T$67, DW_AT_decl_column(0x0f)
	.dwendtag $C$DW$T$67

$C$DW$T$471	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$471, DW_AT_name("SlSockIpV6Mreq_t")
	.dwattr $C$DW$T$471, DW_AT_type(*$C$DW$T$67)
	.dwattr $C$DW$T$471, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$471, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$471, DW_AT_decl_line(0x16e)
	.dwattr $C$DW$T$471, DW_AT_decl_column(0x02)


$C$DW$T$68	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$68, DW_AT_byte_size(0x08)
$C$DW$662	.dwtag  DW_TAG_member
	.dwattr $C$DW$662, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$662, DW_AT_name("l_onoff")
	.dwattr $C$DW$662, DW_AT_TI_symbol_name("l_onoff")
	.dwattr $C$DW$662, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$662, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$662, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$662, DW_AT_decl_line(0x172)
	.dwattr $C$DW$662, DW_AT_decl_column(0x0a)

$C$DW$663	.dwtag  DW_TAG_member
	.dwattr $C$DW$663, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$663, DW_AT_name("l_linger")
	.dwattr $C$DW$663, DW_AT_TI_symbol_name("l_linger")
	.dwattr $C$DW$663, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$663, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$663, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$663, DW_AT_decl_line(0x173)
	.dwattr $C$DW$663, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$68, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$68, DW_AT_decl_line(0x171)
	.dwattr $C$DW$T$68, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$68

$C$DW$T$472	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$472, DW_AT_name("SlSocklinger_t")
	.dwattr $C$DW$T$472, DW_AT_type(*$C$DW$T$68)
	.dwattr $C$DW$T$472, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$472, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$472, DW_AT_decl_line(0x174)
	.dwattr $C$DW$T$472, DW_AT_decl_column(0x02)


$C$DW$T$70	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$70, DW_AT_byte_size(0x04)
$C$DW$664	.dwtag  DW_TAG_member
	.dwattr $C$DW$664, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$664, DW_AT_name("SecureFiles")
	.dwattr $C$DW$664, DW_AT_TI_symbol_name("SecureFiles")
	.dwattr $C$DW$664, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$664, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$664, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$664, DW_AT_decl_line(0x19e)
	.dwattr $C$DW$664, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$70, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$70, DW_AT_decl_line(0x19d)
	.dwattr $C$DW$T$70, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$70

$C$DW$T$473	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$473, DW_AT_name("SlSockSecureFiles_t")
	.dwattr $C$DW$T$473, DW_AT_type(*$C$DW$T$70)
	.dwattr $C$DW$T$473, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$473, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$473, DW_AT_decl_line(0x19f)
	.dwattr $C$DW$T$473, DW_AT_decl_column(0x02)


$C$DW$T$71	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$71, DW_AT_byte_size(0x08)
$C$DW$665	.dwtag  DW_TAG_member
	.dwattr $C$DW$665, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$665, DW_AT_name("Rate")
	.dwattr $C$DW$665, DW_AT_TI_symbol_name("Rate")
	.dwattr $C$DW$665, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$665, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$665, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$665, DW_AT_decl_line(0x1a8)
	.dwattr $C$DW$665, DW_AT_decl_column(0x0b)

$C$DW$666	.dwtag  DW_TAG_member
	.dwattr $C$DW$666, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$666, DW_AT_name("Channel")
	.dwattr $C$DW$666, DW_AT_TI_symbol_name("Channel")
	.dwattr $C$DW$666, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$666, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$666, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$666, DW_AT_decl_line(0x1a9)
	.dwattr $C$DW$666, DW_AT_decl_column(0x0b)

$C$DW$667	.dwtag  DW_TAG_member
	.dwattr $C$DW$667, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$667, DW_AT_name("Rssi")
	.dwattr $C$DW$667, DW_AT_TI_symbol_name("Rssi")
	.dwattr $C$DW$667, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$667, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$667, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$667, DW_AT_decl_line(0x1aa)
	.dwattr $C$DW$667, DW_AT_decl_column(0x0b)

$C$DW$668	.dwtag  DW_TAG_member
	.dwattr $C$DW$668, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$668, DW_AT_name("Padding")
	.dwattr $C$DW$668, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$668, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$668, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$668, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$668, DW_AT_decl_line(0x1ab)
	.dwattr $C$DW$668, DW_AT_decl_column(0x0b)

$C$DW$669	.dwtag  DW_TAG_member
	.dwattr $C$DW$669, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$669, DW_AT_name("Timestamp")
	.dwattr $C$DW$669, DW_AT_TI_symbol_name("Timestamp")
	.dwattr $C$DW$669, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$669, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$669, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$669, DW_AT_decl_line(0x1ac)
	.dwattr $C$DW$669, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$71, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$71, DW_AT_decl_line(0x1a7)
	.dwattr $C$DW$T$71, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$71

$C$DW$T$474	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$474, DW_AT_name("SlTransceiverRxOverHead_t")
	.dwattr $C$DW$T$474, DW_AT_type(*$C$DW$T$71)
	.dwattr $C$DW$T$474, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$474, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$474, DW_AT_decl_line(0x1ad)
	.dwattr $C$DW$T$474, DW_AT_decl_column(0x02)


$C$DW$T$72	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$72, DW_AT_byte_size(0x0c)
$C$DW$670	.dwtag  DW_TAG_member
	.dwattr $C$DW$670, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$670, DW_AT_name("Ip")
	.dwattr $C$DW$670, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$670, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$670, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$670, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$670, DW_AT_decl_line(0xd2)
	.dwattr $C$DW$670, DW_AT_decl_column(0x0a)

$C$DW$671	.dwtag  DW_TAG_member
	.dwattr $C$DW$671, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$671, DW_AT_name("Gateway")
	.dwattr $C$DW$671, DW_AT_TI_symbol_name("Gateway")
	.dwattr $C$DW$671, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$671, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$671, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$671, DW_AT_decl_line(0xd3)
	.dwattr $C$DW$671, DW_AT_decl_column(0x0a)

$C$DW$672	.dwtag  DW_TAG_member
	.dwattr $C$DW$672, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$672, DW_AT_name("Dns")
	.dwattr $C$DW$672, DW_AT_TI_symbol_name("Dns")
	.dwattr $C$DW$672, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$672, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$672, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$672, DW_AT_decl_line(0xd4)
	.dwattr $C$DW$672, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$72, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$72, DW_AT_decl_line(0xd1)
	.dwattr $C$DW$T$72, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$72

$C$DW$T$213	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$213, DW_AT_name("SlIpV4AcquiredAsync_t")
	.dwattr $C$DW$T$213, DW_AT_type(*$C$DW$T$72)
	.dwattr $C$DW$T$213, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$213, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$213, DW_AT_decl_line(0xd5)
	.dwattr $C$DW$T$213, DW_AT_decl_column(0x02)


$C$DW$T$73	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$73, DW_AT_byte_size(0x20)
$C$DW$673	.dwtag  DW_TAG_member
	.dwattr $C$DW$673, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$673, DW_AT_name("Ip")
	.dwattr $C$DW$673, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$673, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$673, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$673, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$673, DW_AT_decl_line(0xdf)
	.dwattr $C$DW$673, DW_AT_decl_column(0x0a)

$C$DW$674	.dwtag  DW_TAG_member
	.dwattr $C$DW$674, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$674, DW_AT_name("Dns")
	.dwattr $C$DW$674, DW_AT_TI_symbol_name("Dns")
	.dwattr $C$DW$674, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$674, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$674, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$674, DW_AT_decl_line(0xe0)
	.dwattr $C$DW$674, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$73, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$73, DW_AT_decl_line(0xde)
	.dwattr $C$DW$T$73, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$73

$C$DW$T$214	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$214, DW_AT_name("SlIpV6AcquiredAsync_t")
	.dwattr $C$DW$T$214, DW_AT_type(*$C$DW$T$73)
	.dwattr $C$DW$T$214, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$214, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$214, DW_AT_decl_line(0xe1)
	.dwattr $C$DW$T$214, DW_AT_decl_column(0x02)


$C$DW$T$74	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$74, DW_AT_byte_size(0x10)
$C$DW$675	.dwtag  DW_TAG_member
	.dwattr $C$DW$675, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$675, DW_AT_name("IpAddress")
	.dwattr $C$DW$675, DW_AT_TI_symbol_name("IpAddress")
	.dwattr $C$DW$675, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$675, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$675, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$675, DW_AT_decl_line(0xe5)
	.dwattr $C$DW$675, DW_AT_decl_column(0x0d)

$C$DW$676	.dwtag  DW_TAG_member
	.dwattr $C$DW$676, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$676, DW_AT_name("LeaseTime")
	.dwattr $C$DW$676, DW_AT_TI_symbol_name("LeaseTime")
	.dwattr $C$DW$676, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$676, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$676, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$676, DW_AT_decl_line(0xe6)
	.dwattr $C$DW$676, DW_AT_decl_column(0x0d)

$C$DW$677	.dwtag  DW_TAG_member
	.dwattr $C$DW$677, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$677, DW_AT_name("Mac")
	.dwattr $C$DW$677, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$677, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$677, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$677, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$677, DW_AT_decl_line(0xe7)
	.dwattr $C$DW$677, DW_AT_decl_column(0x0d)

$C$DW$678	.dwtag  DW_TAG_member
	.dwattr $C$DW$678, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$678, DW_AT_name("Padding")
	.dwattr $C$DW$678, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$678, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr $C$DW$678, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$678, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$678, DW_AT_decl_line(0xe8)
	.dwattr $C$DW$678, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$74, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$74, DW_AT_decl_line(0xe4)
	.dwattr $C$DW$T$74, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$74

$C$DW$T$215	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$215, DW_AT_name("SlIpLeasedAsync_t")
	.dwattr $C$DW$T$215, DW_AT_type(*$C$DW$T$74)
	.dwattr $C$DW$T$215, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$215, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$215, DW_AT_decl_line(0xe9)
	.dwattr $C$DW$T$215, DW_AT_decl_column(0x02)


$C$DW$T$75	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$75, DW_AT_byte_size(0x0c)
$C$DW$679	.dwtag  DW_TAG_member
	.dwattr $C$DW$679, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$679, DW_AT_name("IpAddress")
	.dwattr $C$DW$679, DW_AT_TI_symbol_name("IpAddress")
	.dwattr $C$DW$679, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$679, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$679, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$679, DW_AT_decl_line(0xed)
	.dwattr $C$DW$679, DW_AT_decl_column(0x0d)

$C$DW$680	.dwtag  DW_TAG_member
	.dwattr $C$DW$680, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$680, DW_AT_name("Mac")
	.dwattr $C$DW$680, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$680, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$680, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$680, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$680, DW_AT_decl_line(0xee)
	.dwattr $C$DW$680, DW_AT_decl_column(0x0d)

$C$DW$681	.dwtag  DW_TAG_member
	.dwattr $C$DW$681, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$681, DW_AT_name("Reason")
	.dwattr $C$DW$681, DW_AT_TI_symbol_name("Reason")
	.dwattr $C$DW$681, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$681, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$681, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$681, DW_AT_decl_line(0xef)
	.dwattr $C$DW$681, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$75, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$75, DW_AT_decl_line(0xec)
	.dwattr $C$DW$T$75, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$75

$C$DW$T$216	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$216, DW_AT_name("SlIpReleasedAsync_t")
	.dwattr $C$DW$T$216, DW_AT_type(*$C$DW$T$75)
	.dwattr $C$DW$T$216, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$216, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$216, DW_AT_decl_line(0xf0)
	.dwattr $C$DW$T$216, DW_AT_decl_column(0x02)


$C$DW$T$76	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$76, DW_AT_byte_size(0x10)
$C$DW$682	.dwtag  DW_TAG_member
	.dwattr $C$DW$682, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$682, DW_AT_name("IpAddress")
	.dwattr $C$DW$682, DW_AT_TI_symbol_name("IpAddress")
	.dwattr $C$DW$682, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$682, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$682, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$682, DW_AT_decl_line(0xf4)
	.dwattr $C$DW$682, DW_AT_decl_column(0x0d)

$C$DW$683	.dwtag  DW_TAG_member
	.dwattr $C$DW$683, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$683, DW_AT_name("DhcpMac")
	.dwattr $C$DW$683, DW_AT_TI_symbol_name("DhcpMac")
	.dwattr $C$DW$683, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$683, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$683, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$683, DW_AT_decl_line(0xf5)
	.dwattr $C$DW$683, DW_AT_decl_column(0x0d)

$C$DW$684	.dwtag  DW_TAG_member
	.dwattr $C$DW$684, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$684, DW_AT_name("ConflictMac")
	.dwattr $C$DW$684, DW_AT_TI_symbol_name("ConflictMac")
	.dwattr $C$DW$684, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$684, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$684, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$684, DW_AT_decl_line(0xf6)
	.dwattr $C$DW$684, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$76, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$76, DW_AT_decl_line(0xf3)
	.dwattr $C$DW$T$76, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$76

$C$DW$T$219	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$219, DW_AT_name("SlIpCollisionAsync_t")
	.dwattr $C$DW$T$219, DW_AT_type(*$C$DW$T$76)
	.dwattr $C$DW$T$219, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$219, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$219, DW_AT_decl_line(0xf7)
	.dwattr $C$DW$T$219, DW_AT_decl_column(0x02)


$C$DW$T$77	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$77, DW_AT_byte_size(0x04)
$C$DW$685	.dwtag  DW_TAG_member
	.dwattr $C$DW$685, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$685, DW_AT_name("Status")
	.dwattr $C$DW$685, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$685, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$685, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$685, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$685, DW_AT_decl_line(0xfb)
	.dwattr $C$DW$685, DW_AT_decl_column(0x0b)

$C$DW$686	.dwtag  DW_TAG_member
	.dwattr $C$DW$686, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$686, DW_AT_name("Padding")
	.dwattr $C$DW$686, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$686, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$686, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$686, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$686, DW_AT_decl_line(0xfc)
	.dwattr $C$DW$686, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$77, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$77, DW_AT_decl_line(0xfa)
	.dwattr $C$DW$T$77, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$77

$C$DW$T$217	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$217, DW_AT_name("SlIpV4Lost_t")
	.dwattr $C$DW$T$217, DW_AT_type(*$C$DW$T$77)
	.dwattr $C$DW$T$217, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$217, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$217, DW_AT_decl_line(0xfd)
	.dwattr $C$DW$T$217, DW_AT_decl_column(0x02)


$C$DW$T$78	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$78, DW_AT_byte_size(0x10)
$C$DW$687	.dwtag  DW_TAG_member
	.dwattr $C$DW$687, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$687, DW_AT_name("IpLost")
	.dwattr $C$DW$687, DW_AT_TI_symbol_name("IpLost")
	.dwattr $C$DW$687, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$687, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$687, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$687, DW_AT_decl_line(0x101)
	.dwattr $C$DW$687, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$78, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$78, DW_AT_decl_line(0x100)
	.dwattr $C$DW$T$78, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$78

$C$DW$T$220	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$220, DW_AT_name("SlIpV6Lost_t")
	.dwattr $C$DW$T$220, DW_AT_type(*$C$DW$T$78)
	.dwattr $C$DW$T$220, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$220, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$220, DW_AT_decl_line(0x102)
	.dwattr $C$DW$T$220, DW_AT_decl_column(0x02)


$C$DW$T$79	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$79, DW_AT_byte_size(0x04)
$C$DW$688	.dwtag  DW_TAG_member
	.dwattr $C$DW$688, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$688, DW_AT_name("Status")
	.dwattr $C$DW$688, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$688, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$688, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$688, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$688, DW_AT_decl_line(0x106)
	.dwattr $C$DW$688, DW_AT_decl_column(0x0b)

$C$DW$689	.dwtag  DW_TAG_member
	.dwattr $C$DW$689, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$689, DW_AT_name("Padding")
	.dwattr $C$DW$689, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$689, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$689, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$689, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$689, DW_AT_decl_line(0x107)
	.dwattr $C$DW$689, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$79, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$79, DW_AT_decl_line(0x105)
	.dwattr $C$DW$T$79, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$79

$C$DW$T$218	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$218, DW_AT_name("SlDhcpIpAcquireTimeout_t")
	.dwattr $C$DW$T$218, DW_AT_type(*$C$DW$T$79)
	.dwattr $C$DW$T$218, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$218, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$218, DW_AT_decl_line(0x108)
	.dwattr $C$DW$T$218, DW_AT_decl_column(0x02)


$C$DW$T$81	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$81, DW_AT_byte_size(0x24)
$C$DW$690	.dwtag  DW_TAG_member
	.dwattr $C$DW$690, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$690, DW_AT_name("Id")
	.dwattr $C$DW$690, DW_AT_TI_symbol_name("Id")
	.dwattr $C$DW$690, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$690, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$690, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$690, DW_AT_decl_line(0x119)
	.dwattr $C$DW$690, DW_AT_decl_column(0x1e)

$C$DW$691	.dwtag  DW_TAG_member
	.dwattr $C$DW$691, DW_AT_type(*$C$DW$T$80)
	.dwattr $C$DW$691, DW_AT_name("Data")
	.dwattr $C$DW$691, DW_AT_TI_symbol_name("Data")
	.dwattr $C$DW$691, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$691, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$691, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$691, DW_AT_decl_line(0x11a)
	.dwattr $C$DW$691, DW_AT_decl_column(0x1e)

	.dwattr $C$DW$T$81, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$81, DW_AT_decl_line(0x118)
	.dwattr $C$DW$T$81, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$81

$C$DW$T$475	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$475, DW_AT_name("SlNetAppEvent_t")
	.dwattr $C$DW$T$475, DW_AT_type(*$C$DW$T$81)
	.dwattr $C$DW$T$475, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$475, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$475, DW_AT_decl_line(0x11b)
	.dwattr $C$DW$T$475, DW_AT_decl_column(0x02)


$C$DW$T$82	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$82, DW_AT_byte_size(0x14)
$C$DW$692	.dwtag  DW_TAG_member
	.dwattr $C$DW$692, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$692, DW_AT_name("PacketsSent")
	.dwattr $C$DW$692, DW_AT_TI_symbol_name("PacketsSent")
	.dwattr $C$DW$692, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$692, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$692, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$692, DW_AT_decl_line(0x11f)
	.dwattr $C$DW$692, DW_AT_decl_column(0x0d)

$C$DW$693	.dwtag  DW_TAG_member
	.dwattr $C$DW$693, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$693, DW_AT_name("PacketsReceived")
	.dwattr $C$DW$693, DW_AT_TI_symbol_name("PacketsReceived")
	.dwattr $C$DW$693, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$693, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$693, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$693, DW_AT_decl_line(0x120)
	.dwattr $C$DW$693, DW_AT_decl_column(0x0d)

$C$DW$694	.dwtag  DW_TAG_member
	.dwattr $C$DW$694, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$694, DW_AT_name("MinRoundTime")
	.dwattr $C$DW$694, DW_AT_TI_symbol_name("MinRoundTime")
	.dwattr $C$DW$694, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$694, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$694, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$694, DW_AT_decl_line(0x121)
	.dwattr $C$DW$694, DW_AT_decl_column(0x0d)

$C$DW$695	.dwtag  DW_TAG_member
	.dwattr $C$DW$695, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$695, DW_AT_name("MaxRoundTime")
	.dwattr $C$DW$695, DW_AT_TI_symbol_name("MaxRoundTime")
	.dwattr $C$DW$695, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$695, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$695, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$695, DW_AT_decl_line(0x122)
	.dwattr $C$DW$695, DW_AT_decl_column(0x0d)

$C$DW$696	.dwtag  DW_TAG_member
	.dwattr $C$DW$696, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$696, DW_AT_name("AvgRoundTime")
	.dwattr $C$DW$696, DW_AT_TI_symbol_name("AvgRoundTime")
	.dwattr $C$DW$696, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$696, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$696, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$696, DW_AT_decl_line(0x123)
	.dwattr $C$DW$696, DW_AT_decl_column(0x0d)

$C$DW$697	.dwtag  DW_TAG_member
	.dwattr $C$DW$697, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$697, DW_AT_name("TestTime")
	.dwattr $C$DW$697, DW_AT_TI_symbol_name("TestTime")
	.dwattr $C$DW$697, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$697, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$697, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$697, DW_AT_decl_line(0x124)
	.dwattr $C$DW$697, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$82, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$82, DW_AT_decl_line(0x11e)
	.dwattr $C$DW$T$82, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$82

$C$DW$T$476	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$476, DW_AT_name("SlNetAppPingReport_t")
	.dwattr $C$DW$T$476, DW_AT_type(*$C$DW$T$82)
	.dwattr $C$DW$T$476, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$476, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$476, DW_AT_decl_line(0x125)
	.dwattr $C$DW$T$476, DW_AT_decl_column(0x02)

$C$DW$T$477	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$477, DW_AT_type(*$C$DW$T$476)
	.dwattr $C$DW$T$477, DW_AT_address_class(0x20)


$C$DW$T$83	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$83, DW_AT_byte_size(0x20)
$C$DW$698	.dwtag  DW_TAG_member
	.dwattr $C$DW$698, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$698, DW_AT_name("PingIntervalTime")
	.dwattr $C$DW$698, DW_AT_TI_symbol_name("PingIntervalTime")
	.dwattr $C$DW$698, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$698, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$698, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$698, DW_AT_decl_line(0x129)
	.dwattr $C$DW$698, DW_AT_decl_column(0x0d)

$C$DW$699	.dwtag  DW_TAG_member
	.dwattr $C$DW$699, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$699, DW_AT_name("PingSize")
	.dwattr $C$DW$699, DW_AT_TI_symbol_name("PingSize")
	.dwattr $C$DW$699, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$699, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$699, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$699, DW_AT_decl_line(0x12a)
	.dwattr $C$DW$699, DW_AT_decl_column(0x0d)

$C$DW$700	.dwtag  DW_TAG_member
	.dwattr $C$DW$700, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$700, DW_AT_name("PingRequestTimeout")
	.dwattr $C$DW$700, DW_AT_TI_symbol_name("PingRequestTimeout")
	.dwattr $C$DW$700, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$700, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$700, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$700, DW_AT_decl_line(0x12b)
	.dwattr $C$DW$700, DW_AT_decl_column(0x0d)

$C$DW$701	.dwtag  DW_TAG_member
	.dwattr $C$DW$701, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$701, DW_AT_name("TotalNumberOfAttempts")
	.dwattr $C$DW$701, DW_AT_TI_symbol_name("TotalNumberOfAttempts")
	.dwattr $C$DW$701, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$701, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$701, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$701, DW_AT_decl_line(0x12c)
	.dwattr $C$DW$701, DW_AT_decl_column(0x0d)

$C$DW$702	.dwtag  DW_TAG_member
	.dwattr $C$DW$702, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$702, DW_AT_name("Flags")
	.dwattr $C$DW$702, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$702, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$702, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$702, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$702, DW_AT_decl_line(0x12d)
	.dwattr $C$DW$702, DW_AT_decl_column(0x0d)

$C$DW$703	.dwtag  DW_TAG_member
	.dwattr $C$DW$703, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$703, DW_AT_name("Ip")
	.dwattr $C$DW$703, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$703, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$703, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$703, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$703, DW_AT_decl_line(0x12e)
	.dwattr $C$DW$703, DW_AT_decl_column(0x0d)

$C$DW$704	.dwtag  DW_TAG_member
	.dwattr $C$DW$704, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$704, DW_AT_name("Ip1OrPadding")
	.dwattr $C$DW$704, DW_AT_TI_symbol_name("Ip1OrPadding")
	.dwattr $C$DW$704, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$704, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$704, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$704, DW_AT_decl_line(0x12f)
	.dwattr $C$DW$704, DW_AT_decl_column(0x0d)

$C$DW$705	.dwtag  DW_TAG_member
	.dwattr $C$DW$705, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$705, DW_AT_name("Ip2OrPadding")
	.dwattr $C$DW$705, DW_AT_TI_symbol_name("Ip2OrPadding")
	.dwattr $C$DW$705, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$705, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$705, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$705, DW_AT_decl_line(0x130)
	.dwattr $C$DW$705, DW_AT_decl_column(0x0d)

$C$DW$706	.dwtag  DW_TAG_member
	.dwattr $C$DW$706, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$706, DW_AT_name("Ip3OrPadding")
	.dwattr $C$DW$706, DW_AT_TI_symbol_name("Ip3OrPadding")
	.dwattr $C$DW$706, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$706, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$706, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$706, DW_AT_decl_line(0x131)
	.dwattr $C$DW$706, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$83, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$83, DW_AT_decl_line(0x128)
	.dwattr $C$DW$T$83, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$83

$C$DW$T$478	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$478, DW_AT_name("SlNetAppPingCommand_t")
	.dwattr $C$DW$T$478, DW_AT_type(*$C$DW$T$83)
	.dwattr $C$DW$T$478, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$478, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$478, DW_AT_decl_line(0x132)
	.dwattr $C$DW$T$478, DW_AT_decl_column(0x02)


$C$DW$T$85	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$85, DW_AT_byte_size(0x08)
$C$DW$707	.dwtag  DW_TAG_member
	.dwattr $C$DW$707, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$707, DW_AT_name("Len")
	.dwattr $C$DW$707, DW_AT_TI_symbol_name("Len")
	.dwattr $C$DW$707, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$707, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$707, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$707, DW_AT_decl_line(0x136)
	.dwattr $C$DW$707, DW_AT_decl_column(0x0d)

$C$DW$708	.dwtag  DW_TAG_member
	.dwattr $C$DW$708, DW_AT_type(*$C$DW$T$84)
	.dwattr $C$DW$708, DW_AT_name("pData")
	.dwattr $C$DW$708, DW_AT_TI_symbol_name("pData")
	.dwattr $C$DW$708, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$708, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$708, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$708, DW_AT_decl_line(0x137)
	.dwattr $C$DW$708, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$85, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$85, DW_AT_decl_line(0x135)
	.dwattr $C$DW$T$85, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$85

$C$DW$T$87	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$87, DW_AT_name("SlNetAppHttpServerString_t")
	.dwattr $C$DW$T$87, DW_AT_type(*$C$DW$T$85)
	.dwattr $C$DW$T$87, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$87, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$87, DW_AT_decl_line(0x138)
	.dwattr $C$DW$T$87, DW_AT_decl_column(0x03)


$C$DW$T$86	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$86, DW_AT_byte_size(0x0c)
$C$DW$709	.dwtag  DW_TAG_member
	.dwattr $C$DW$709, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$709, DW_AT_name("ValueLen")
	.dwattr $C$DW$709, DW_AT_TI_symbol_name("ValueLen")
	.dwattr $C$DW$709, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$709, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$709, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$709, DW_AT_decl_line(0x13c)
	.dwattr $C$DW$709, DW_AT_decl_column(0x0d)

$C$DW$710	.dwtag  DW_TAG_member
	.dwattr $C$DW$710, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$710, DW_AT_name("NameLen")
	.dwattr $C$DW$710, DW_AT_TI_symbol_name("NameLen")
	.dwattr $C$DW$710, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$710, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$710, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$710, DW_AT_decl_line(0x13d)
	.dwattr $C$DW$710, DW_AT_decl_column(0x0d)

$C$DW$711	.dwtag  DW_TAG_member
	.dwattr $C$DW$711, DW_AT_type(*$C$DW$T$84)
	.dwattr $C$DW$711, DW_AT_name("pTokenValue")
	.dwattr $C$DW$711, DW_AT_TI_symbol_name("pTokenValue")
	.dwattr $C$DW$711, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$711, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$711, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$711, DW_AT_decl_line(0x13e)
	.dwattr $C$DW$711, DW_AT_decl_column(0x0e)

$C$DW$712	.dwtag  DW_TAG_member
	.dwattr $C$DW$712, DW_AT_type(*$C$DW$T$84)
	.dwattr $C$DW$712, DW_AT_name("pTokenName")
	.dwattr $C$DW$712, DW_AT_TI_symbol_name("pTokenName")
	.dwattr $C$DW$712, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$712, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$712, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$712, DW_AT_decl_line(0x13f)
	.dwattr $C$DW$712, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$86, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$86, DW_AT_decl_line(0x13b)
	.dwattr $C$DW$T$86, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$86

$C$DW$T$479	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$479, DW_AT_name("SlNetAppHttpServerData_t")
	.dwattr $C$DW$T$479, DW_AT_type(*$C$DW$T$86)
	.dwattr $C$DW$T$479, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$479, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$479, DW_AT_decl_line(0x140)
	.dwattr $C$DW$T$479, DW_AT_decl_column(0x03)


$C$DW$T$88	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$88, DW_AT_byte_size(0x18)
$C$DW$713	.dwtag  DW_TAG_member
	.dwattr $C$DW$713, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$713, DW_AT_name("Action")
	.dwattr $C$DW$713, DW_AT_TI_symbol_name("Action")
	.dwattr $C$DW$713, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$713, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$713, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$713, DW_AT_decl_line(0x144)
	.dwattr $C$DW$713, DW_AT_decl_column(0x20)

$C$DW$714	.dwtag  DW_TAG_member
	.dwattr $C$DW$714, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$714, DW_AT_name("TokenName")
	.dwattr $C$DW$714, DW_AT_TI_symbol_name("TokenName")
	.dwattr $C$DW$714, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$714, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$714, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$714, DW_AT_decl_line(0x145)
	.dwattr $C$DW$714, DW_AT_decl_column(0x20)

$C$DW$715	.dwtag  DW_TAG_member
	.dwattr $C$DW$715, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$715, DW_AT_name("TokenValue")
	.dwattr $C$DW$715, DW_AT_TI_symbol_name("TokenValue")
	.dwattr $C$DW$715, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$715, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$715, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$715, DW_AT_decl_line(0x146)
	.dwattr $C$DW$715, DW_AT_decl_column(0x20)

	.dwattr $C$DW$T$88, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$88, DW_AT_decl_line(0x143)
	.dwattr $C$DW$T$88, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$88

$C$DW$T$222	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$222, DW_AT_name("SlNetAppHttpServerPostData_t")
	.dwattr $C$DW$T$222, DW_AT_type(*$C$DW$T$88)
	.dwattr $C$DW$T$222, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$222, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$222, DW_AT_decl_line(0x147)
	.dwattr $C$DW$T$222, DW_AT_decl_column(0x02)


$C$DW$T$90	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$90, DW_AT_byte_size(0x1c)
$C$DW$716	.dwtag  DW_TAG_member
	.dwattr $C$DW$716, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$716, DW_AT_name("Event")
	.dwattr $C$DW$716, DW_AT_TI_symbol_name("Event")
	.dwattr $C$DW$716, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$716, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$716, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$716, DW_AT_decl_line(0x156)
	.dwattr $C$DW$716, DW_AT_decl_column(0x23)

$C$DW$717	.dwtag  DW_TAG_member
	.dwattr $C$DW$717, DW_AT_type(*$C$DW$T$89)
	.dwattr $C$DW$717, DW_AT_name("EventData")
	.dwattr $C$DW$717, DW_AT_TI_symbol_name("EventData")
	.dwattr $C$DW$717, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$717, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$717, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$717, DW_AT_decl_line(0x157)
	.dwattr $C$DW$717, DW_AT_decl_column(0x23)

	.dwattr $C$DW$T$90, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$90, DW_AT_decl_line(0x155)
	.dwattr $C$DW$T$90, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$90

$C$DW$T$480	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$480, DW_AT_name("SlNetAppHttpServerEvent_t")
	.dwattr $C$DW$T$480, DW_AT_type(*$C$DW$T$90)
	.dwattr $C$DW$T$480, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$480, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$480, DW_AT_decl_line(0x158)
	.dwattr $C$DW$T$480, DW_AT_decl_column(0x02)


$C$DW$T$92	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$92, DW_AT_byte_size(0x0c)
$C$DW$718	.dwtag  DW_TAG_member
	.dwattr $C$DW$718, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$718, DW_AT_name("Response")
	.dwattr $C$DW$718, DW_AT_TI_symbol_name("Response")
	.dwattr $C$DW$718, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$718, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$718, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$718, DW_AT_decl_line(0x15c)
	.dwattr $C$DW$718, DW_AT_decl_column(0x26)

$C$DW$719	.dwtag  DW_TAG_member
	.dwattr $C$DW$719, DW_AT_type(*$C$DW$T$91)
	.dwattr $C$DW$719, DW_AT_name("ResponseData")
	.dwattr $C$DW$719, DW_AT_TI_symbol_name("ResponseData")
	.dwattr $C$DW$719, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$719, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$719, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$719, DW_AT_decl_line(0x15d)
	.dwattr $C$DW$719, DW_AT_decl_column(0x26)

	.dwattr $C$DW$T$92, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$92, DW_AT_decl_line(0x15b)
	.dwattr $C$DW$T$92, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$92

$C$DW$T$481	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$481, DW_AT_name("SlNetAppHttpServerResponse_t")
	.dwattr $C$DW$T$481, DW_AT_type(*$C$DW$T$92)
	.dwattr $C$DW$T$481, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$481, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$481, DW_AT_decl_line(0x15e)
	.dwattr $C$DW$T$481, DW_AT_decl_column(0x02)


$C$DW$T$93	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$93, DW_AT_byte_size(0x14)
$C$DW$720	.dwtag  DW_TAG_member
	.dwattr $C$DW$720, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$720, DW_AT_name("MetadataLen")
	.dwattr $C$DW$720, DW_AT_TI_symbol_name("MetadataLen")
	.dwattr $C$DW$720, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$720, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$720, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$720, DW_AT_decl_line(0x1ab)
	.dwattr $C$DW$720, DW_AT_decl_column(0x0a)

$C$DW$721	.dwtag  DW_TAG_member
	.dwattr $C$DW$721, DW_AT_type(*$C$DW$T$84)
	.dwattr $C$DW$721, DW_AT_name("pMetadata")
	.dwattr $C$DW$721, DW_AT_TI_symbol_name("pMetadata")
	.dwattr $C$DW$721, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$721, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$721, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$721, DW_AT_decl_line(0x1ac)
	.dwattr $C$DW$721, DW_AT_decl_column(0x0a)

$C$DW$722	.dwtag  DW_TAG_member
	.dwattr $C$DW$722, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$722, DW_AT_name("PayloadLen")
	.dwattr $C$DW$722, DW_AT_TI_symbol_name("PayloadLen")
	.dwattr $C$DW$722, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$722, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$722, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$722, DW_AT_decl_line(0x1ad)
	.dwattr $C$DW$722, DW_AT_decl_column(0x0a)

$C$DW$723	.dwtag  DW_TAG_member
	.dwattr $C$DW$723, DW_AT_type(*$C$DW$T$84)
	.dwattr $C$DW$723, DW_AT_name("pPayload")
	.dwattr $C$DW$723, DW_AT_TI_symbol_name("pPayload")
	.dwattr $C$DW$723, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$723, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$723, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$723, DW_AT_decl_line(0x1ae)
	.dwattr $C$DW$723, DW_AT_decl_column(0x0a)

$C$DW$724	.dwtag  DW_TAG_member
	.dwattr $C$DW$724, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$724, DW_AT_name("Flags")
	.dwattr $C$DW$724, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$724, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$724, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$724, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$724, DW_AT_decl_line(0x1af)
	.dwattr $C$DW$724, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$93, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$93, DW_AT_decl_line(0x1aa)
	.dwattr $C$DW$T$93, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$93

$C$DW$T$94	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$94, DW_AT_name("SlNetAppData_t")
	.dwattr $C$DW$T$94, DW_AT_type(*$C$DW$T$93)
	.dwattr $C$DW$T$94, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$94, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$94, DW_AT_decl_line(0x1b0)
	.dwattr $C$DW$T$94, DW_AT_decl_column(0x03)


$C$DW$T$95	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$95, DW_AT_byte_size(0x18)
$C$DW$725	.dwtag  DW_TAG_member
	.dwattr $C$DW$725, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$725, DW_AT_name("AppId")
	.dwattr $C$DW$725, DW_AT_TI_symbol_name("AppId")
	.dwattr $C$DW$725, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$725, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$725, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$725, DW_AT_decl_line(0x1b4)
	.dwattr $C$DW$725, DW_AT_decl_column(0x0d)

$C$DW$726	.dwtag  DW_TAG_member
	.dwattr $C$DW$726, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$726, DW_AT_name("Type")
	.dwattr $C$DW$726, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$726, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$726, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$726, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$726, DW_AT_decl_line(0x1b5)
	.dwattr $C$DW$726, DW_AT_decl_column(0x0d)

$C$DW$727	.dwtag  DW_TAG_member
	.dwattr $C$DW$727, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$727, DW_AT_name("Handle")
	.dwattr $C$DW$727, DW_AT_TI_symbol_name("Handle")
	.dwattr $C$DW$727, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$727, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$727, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$727, DW_AT_decl_line(0x1b6)
	.dwattr $C$DW$727, DW_AT_decl_column(0x0d)

$C$DW$728	.dwtag  DW_TAG_member
	.dwattr $C$DW$728, DW_AT_type(*$C$DW$T$94)
	.dwattr $C$DW$728, DW_AT_name("requestData")
	.dwattr $C$DW$728, DW_AT_TI_symbol_name("requestData")
	.dwattr $C$DW$728, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$728, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$728, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$728, DW_AT_decl_line(0x1b7)
	.dwattr $C$DW$728, DW_AT_decl_column(0x15)

	.dwattr $C$DW$T$95, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$95, DW_AT_decl_line(0x1b3)
	.dwattr $C$DW$T$95, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$95

$C$DW$T$482	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$482, DW_AT_name("SlNetAppRequest_t")
	.dwattr $C$DW$T$482, DW_AT_type(*$C$DW$T$95)
	.dwattr $C$DW$T$482, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$482, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$482, DW_AT_decl_line(0x1b8)
	.dwattr $C$DW$T$482, DW_AT_decl_column(0x03)


$C$DW$T$96	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$96, DW_AT_byte_size(0x18)
$C$DW$729	.dwtag  DW_TAG_member
	.dwattr $C$DW$729, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$729, DW_AT_name("Status")
	.dwattr $C$DW$729, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$729, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$729, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$729, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$729, DW_AT_decl_line(0x1bc)
	.dwattr $C$DW$729, DW_AT_decl_column(0x15)

$C$DW$730	.dwtag  DW_TAG_member
	.dwattr $C$DW$730, DW_AT_type(*$C$DW$T$94)
	.dwattr $C$DW$730, DW_AT_name("ResponseData")
	.dwattr $C$DW$730, DW_AT_TI_symbol_name("ResponseData")
	.dwattr $C$DW$730, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$730, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$730, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$730, DW_AT_decl_line(0x1bd)
	.dwattr $C$DW$730, DW_AT_decl_column(0x15)

	.dwattr $C$DW$T$96, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$96, DW_AT_decl_line(0x1bb)
	.dwattr $C$DW$T$96, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$96

$C$DW$T$483	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$483, DW_AT_name("SlNetAppResponse_t")
	.dwattr $C$DW$T$483, DW_AT_type(*$C$DW$T$96)
	.dwattr $C$DW$T$483, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$483, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$483, DW_AT_decl_line(0x1be)
	.dwattr $C$DW$T$483, DW_AT_decl_column(0x03)


$C$DW$T$97	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$97, DW_AT_byte_size(0x0c)
$C$DW$731	.dwtag  DW_TAG_member
	.dwattr $C$DW$731, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$731, DW_AT_name("lease_time")
	.dwattr $C$DW$731, DW_AT_TI_symbol_name("lease_time")
	.dwattr $C$DW$731, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$731, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$731, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$731, DW_AT_decl_line(0x1c2)
	.dwattr $C$DW$731, DW_AT_decl_column(0x0c)

$C$DW$732	.dwtag  DW_TAG_member
	.dwattr $C$DW$732, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$732, DW_AT_name("ipv4_addr_start")
	.dwattr $C$DW$732, DW_AT_TI_symbol_name("ipv4_addr_start")
	.dwattr $C$DW$732, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$732, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$732, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$732, DW_AT_decl_line(0x1c3)
	.dwattr $C$DW$732, DW_AT_decl_column(0x0c)

$C$DW$733	.dwtag  DW_TAG_member
	.dwattr $C$DW$733, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$733, DW_AT_name("ipv4_addr_last")
	.dwattr $C$DW$733, DW_AT_TI_symbol_name("ipv4_addr_last")
	.dwattr $C$DW$733, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$733, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$733, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$733, DW_AT_decl_line(0x1c4)
	.dwattr $C$DW$733, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$97, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$97, DW_AT_decl_line(0x1c1)
	.dwattr $C$DW$T$97, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$97

$C$DW$T$484	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$484, DW_AT_name("SlNetAppDhcpServerBasicOpt_t")
	.dwattr $C$DW$T$484, DW_AT_type(*$C$DW$T$97)
	.dwattr $C$DW$T$484, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$484, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$484, DW_AT_decl_line(0x1c5)
	.dwattr $C$DW$T$484, DW_AT_decl_column(0x02)


$C$DW$T$98	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$98, DW_AT_byte_size(0x08)
$C$DW$734	.dwtag  DW_TAG_member
	.dwattr $C$DW$734, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$734, DW_AT_name("service_ipv4")
	.dwattr $C$DW$734, DW_AT_TI_symbol_name("service_ipv4")
	.dwattr $C$DW$734, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$734, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$734, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$734, DW_AT_decl_line(0x1d4)
	.dwattr $C$DW$734, DW_AT_decl_column(0x0c)

$C$DW$735	.dwtag  DW_TAG_member
	.dwattr $C$DW$735, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$735, DW_AT_name("service_port")
	.dwattr $C$DW$735, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$735, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$735, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$735, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$735, DW_AT_decl_line(0x1d5)
	.dwattr $C$DW$735, DW_AT_decl_column(0x0c)

$C$DW$736	.dwtag  DW_TAG_member
	.dwattr $C$DW$736, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$736, DW_AT_name("Reserved")
	.dwattr $C$DW$736, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$736, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$736, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$736, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$736, DW_AT_decl_line(0x1d6)
	.dwattr $C$DW$736, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$98, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$98, DW_AT_decl_line(0x1d3)
	.dwattr $C$DW$T$98, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$98

$C$DW$T$485	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$485, DW_AT_name("SlNetAppGetShortServiceIpv4List_t")
	.dwattr $C$DW$T$485, DW_AT_type(*$C$DW$T$98)
	.dwattr $C$DW$T$485, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$485, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$485, DW_AT_decl_line(0x1d7)
	.dwattr $C$DW$T$485, DW_AT_decl_column(0x02)


$C$DW$T$101	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$101, DW_AT_byte_size(0x84)
$C$DW$737	.dwtag  DW_TAG_member
	.dwattr $C$DW$737, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$737, DW_AT_name("service_ipv4")
	.dwattr $C$DW$737, DW_AT_TI_symbol_name("service_ipv4")
	.dwattr $C$DW$737, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$737, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$737, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$737, DW_AT_decl_line(0x1db)
	.dwattr $C$DW$737, DW_AT_decl_column(0x0c)

$C$DW$738	.dwtag  DW_TAG_member
	.dwattr $C$DW$738, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$738, DW_AT_name("service_port")
	.dwattr $C$DW$738, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$738, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$738, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$738, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$738, DW_AT_decl_line(0x1dc)
	.dwattr $C$DW$738, DW_AT_decl_column(0x0c)

$C$DW$739	.dwtag  DW_TAG_member
	.dwattr $C$DW$739, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$739, DW_AT_name("Reserved")
	.dwattr $C$DW$739, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$739, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$739, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$739, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$739, DW_AT_decl_line(0x1dd)
	.dwattr $C$DW$739, DW_AT_decl_column(0x0c)

$C$DW$740	.dwtag  DW_TAG_member
	.dwattr $C$DW$740, DW_AT_type(*$C$DW$T$99)
	.dwattr $C$DW$740, DW_AT_name("service_name")
	.dwattr $C$DW$740, DW_AT_TI_symbol_name("service_name")
	.dwattr $C$DW$740, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$740, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$740, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$740, DW_AT_decl_line(0x1de)
	.dwattr $C$DW$740, DW_AT_decl_column(0x0c)

$C$DW$741	.dwtag  DW_TAG_member
	.dwattr $C$DW$741, DW_AT_type(*$C$DW$T$100)
	.dwattr $C$DW$741, DW_AT_name("service_host")
	.dwattr $C$DW$741, DW_AT_TI_symbol_name("service_host")
	.dwattr $C$DW$741, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$741, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$741, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$741, DW_AT_decl_line(0x1df)
	.dwattr $C$DW$741, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$101, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$101, DW_AT_decl_line(0x1da)
	.dwattr $C$DW$T$101, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$101

$C$DW$T$486	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$486, DW_AT_name("SlNetAppGetFullServiceIpv4List_t")
	.dwattr $C$DW$T$486, DW_AT_type(*$C$DW$T$101)
	.dwattr $C$DW$T$486, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$486, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$486, DW_AT_decl_line(0x1e0)
	.dwattr $C$DW$T$486, DW_AT_decl_column(0x02)


$C$DW$T$103	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$103, DW_AT_byte_size(0x184)
$C$DW$742	.dwtag  DW_TAG_member
	.dwattr $C$DW$742, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$742, DW_AT_name("service_ipv4")
	.dwattr $C$DW$742, DW_AT_TI_symbol_name("service_ipv4")
	.dwattr $C$DW$742, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$742, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$742, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$742, DW_AT_decl_line(0x1e4)
	.dwattr $C$DW$742, DW_AT_decl_column(0x0d)

$C$DW$743	.dwtag  DW_TAG_member
	.dwattr $C$DW$743, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$743, DW_AT_name("service_port")
	.dwattr $C$DW$743, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$743, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$743, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$743, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$743, DW_AT_decl_line(0x1e5)
	.dwattr $C$DW$743, DW_AT_decl_column(0x0d)

$C$DW$744	.dwtag  DW_TAG_member
	.dwattr $C$DW$744, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$744, DW_AT_name("Reserved")
	.dwattr $C$DW$744, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$744, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$744, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$744, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$744, DW_AT_decl_line(0x1e6)
	.dwattr $C$DW$744, DW_AT_decl_column(0x0d)

$C$DW$745	.dwtag  DW_TAG_member
	.dwattr $C$DW$745, DW_AT_type(*$C$DW$T$99)
	.dwattr $C$DW$745, DW_AT_name("service_name")
	.dwattr $C$DW$745, DW_AT_TI_symbol_name("service_name")
	.dwattr $C$DW$745, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$745, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$745, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$745, DW_AT_decl_line(0x1e7)
	.dwattr $C$DW$745, DW_AT_decl_column(0x0d)

$C$DW$746	.dwtag  DW_TAG_member
	.dwattr $C$DW$746, DW_AT_type(*$C$DW$T$100)
	.dwattr $C$DW$746, DW_AT_name("service_host")
	.dwattr $C$DW$746, DW_AT_TI_symbol_name("service_host")
	.dwattr $C$DW$746, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$746, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$746, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$746, DW_AT_decl_line(0x1e8)
	.dwattr $C$DW$746, DW_AT_decl_column(0x0d)

$C$DW$747	.dwtag  DW_TAG_member
	.dwattr $C$DW$747, DW_AT_type(*$C$DW$T$102)
	.dwattr $C$DW$747, DW_AT_name("service_text")
	.dwattr $C$DW$747, DW_AT_TI_symbol_name("service_text")
	.dwattr $C$DW$747, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$747, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$747, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$747, DW_AT_decl_line(0x1e9)
	.dwattr $C$DW$747, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$103, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$103, DW_AT_decl_line(0x1e3)
	.dwattr $C$DW$T$103, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$103

$C$DW$T$487	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$487, DW_AT_name("SlNetAppGetFullServiceWithTextIpv4List_t")
	.dwattr $C$DW$T$487, DW_AT_type(*$C$DW$T$103)
	.dwattr $C$DW$T$487, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$487, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$487, DW_AT_decl_line(0x1ea)
	.dwattr $C$DW$T$487, DW_AT_decl_column(0x02)


$C$DW$T$104	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$104, DW_AT_byte_size(0x14)
$C$DW$748	.dwtag  DW_TAG_member
	.dwattr $C$DW$748, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$748, DW_AT_name("service_ipv6")
	.dwattr $C$DW$748, DW_AT_TI_symbol_name("service_ipv6")
	.dwattr $C$DW$748, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$748, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$748, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$748, DW_AT_decl_line(0x1ef)
	.dwattr $C$DW$748, DW_AT_decl_column(0x0c)

$C$DW$749	.dwtag  DW_TAG_member
	.dwattr $C$DW$749, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$749, DW_AT_name("service_port")
	.dwattr $C$DW$749, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$749, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$749, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$749, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$749, DW_AT_decl_line(0x1f0)
	.dwattr $C$DW$749, DW_AT_decl_column(0x0c)

$C$DW$750	.dwtag  DW_TAG_member
	.dwattr $C$DW$750, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$750, DW_AT_name("Reserved")
	.dwattr $C$DW$750, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$750, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$750, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$750, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$750, DW_AT_decl_line(0x1f1)
	.dwattr $C$DW$750, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$104, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$104, DW_AT_decl_line(0x1ee)
	.dwattr $C$DW$T$104, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$104

$C$DW$T$488	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$488, DW_AT_name("SlNetAppGetShortServiceIpv6List_t")
	.dwattr $C$DW$T$488, DW_AT_type(*$C$DW$T$104)
	.dwattr $C$DW$T$488, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$488, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$488, DW_AT_decl_line(0x1f2)
	.dwattr $C$DW$T$488, DW_AT_decl_column(0x02)


$C$DW$T$105	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$105, DW_AT_byte_size(0x90)
$C$DW$751	.dwtag  DW_TAG_member
	.dwattr $C$DW$751, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$751, DW_AT_name("service_ipv6")
	.dwattr $C$DW$751, DW_AT_TI_symbol_name("service_ipv6")
	.dwattr $C$DW$751, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$751, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$751, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$751, DW_AT_decl_line(0x1f6)
	.dwattr $C$DW$751, DW_AT_decl_column(0x0c)

$C$DW$752	.dwtag  DW_TAG_member
	.dwattr $C$DW$752, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$752, DW_AT_name("service_port")
	.dwattr $C$DW$752, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$752, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$752, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$752, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$752, DW_AT_decl_line(0x1f7)
	.dwattr $C$DW$752, DW_AT_decl_column(0x0c)

$C$DW$753	.dwtag  DW_TAG_member
	.dwattr $C$DW$753, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$753, DW_AT_name("Reserved")
	.dwattr $C$DW$753, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$753, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$753, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$753, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$753, DW_AT_decl_line(0x1f8)
	.dwattr $C$DW$753, DW_AT_decl_column(0x0c)

$C$DW$754	.dwtag  DW_TAG_member
	.dwattr $C$DW$754, DW_AT_type(*$C$DW$T$99)
	.dwattr $C$DW$754, DW_AT_name("service_name")
	.dwattr $C$DW$754, DW_AT_TI_symbol_name("service_name")
	.dwattr $C$DW$754, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$754, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$754, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$754, DW_AT_decl_line(0x1f9)
	.dwattr $C$DW$754, DW_AT_decl_column(0x0c)

$C$DW$755	.dwtag  DW_TAG_member
	.dwattr $C$DW$755, DW_AT_type(*$C$DW$T$100)
	.dwattr $C$DW$755, DW_AT_name("service_host")
	.dwattr $C$DW$755, DW_AT_TI_symbol_name("service_host")
	.dwattr $C$DW$755, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$755, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$755, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$755, DW_AT_decl_line(0x1fa)
	.dwattr $C$DW$755, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$105, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$105, DW_AT_decl_line(0x1f5)
	.dwattr $C$DW$T$105, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$105

$C$DW$T$489	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$489, DW_AT_name("SlNetAppGetFullServiceIpv6List_t")
	.dwattr $C$DW$T$489, DW_AT_type(*$C$DW$T$105)
	.dwattr $C$DW$T$489, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$489, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$489, DW_AT_decl_line(0x1fb)
	.dwattr $C$DW$T$489, DW_AT_decl_column(0x02)


$C$DW$T$106	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$106, DW_AT_byte_size(0x190)
$C$DW$756	.dwtag  DW_TAG_member
	.dwattr $C$DW$756, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$756, DW_AT_name("service_ipv6")
	.dwattr $C$DW$756, DW_AT_TI_symbol_name("service_ipv6")
	.dwattr $C$DW$756, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$756, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$756, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$756, DW_AT_decl_line(0x1ff)
	.dwattr $C$DW$756, DW_AT_decl_column(0x0d)

$C$DW$757	.dwtag  DW_TAG_member
	.dwattr $C$DW$757, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$757, DW_AT_name("service_port")
	.dwattr $C$DW$757, DW_AT_TI_symbol_name("service_port")
	.dwattr $C$DW$757, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$757, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$757, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$757, DW_AT_decl_line(0x200)
	.dwattr $C$DW$757, DW_AT_decl_column(0x0d)

$C$DW$758	.dwtag  DW_TAG_member
	.dwattr $C$DW$758, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$758, DW_AT_name("Reserved")
	.dwattr $C$DW$758, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$758, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$758, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$758, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$758, DW_AT_decl_line(0x201)
	.dwattr $C$DW$758, DW_AT_decl_column(0x0d)

$C$DW$759	.dwtag  DW_TAG_member
	.dwattr $C$DW$759, DW_AT_type(*$C$DW$T$99)
	.dwattr $C$DW$759, DW_AT_name("service_name")
	.dwattr $C$DW$759, DW_AT_TI_symbol_name("service_name")
	.dwattr $C$DW$759, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$759, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$759, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$759, DW_AT_decl_line(0x202)
	.dwattr $C$DW$759, DW_AT_decl_column(0x0d)

$C$DW$760	.dwtag  DW_TAG_member
	.dwattr $C$DW$760, DW_AT_type(*$C$DW$T$100)
	.dwattr $C$DW$760, DW_AT_name("service_host")
	.dwattr $C$DW$760, DW_AT_TI_symbol_name("service_host")
	.dwattr $C$DW$760, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$760, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$760, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$760, DW_AT_decl_line(0x203)
	.dwattr $C$DW$760, DW_AT_decl_column(0x0d)

$C$DW$761	.dwtag  DW_TAG_member
	.dwattr $C$DW$761, DW_AT_type(*$C$DW$T$102)
	.dwattr $C$DW$761, DW_AT_name("service_text")
	.dwattr $C$DW$761, DW_AT_TI_symbol_name("service_text")
	.dwattr $C$DW$761, DW_AT_data_member_location[DW_OP_plus_uconst 0x90]
	.dwattr $C$DW$761, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$761, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$761, DW_AT_decl_line(0x204)
	.dwattr $C$DW$761, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$106, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$106, DW_AT_decl_line(0x1fe)
	.dwattr $C$DW$T$106, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$106

$C$DW$T$490	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$490, DW_AT_name("SlNetAppGetFullServiceWithTextIpv6List_t")
	.dwattr $C$DW$T$490, DW_AT_type(*$C$DW$T$106)
	.dwattr $C$DW$T$490, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$490, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$490, DW_AT_decl_line(0x205)
	.dwattr $C$DW$T$490, DW_AT_decl_column(0x02)


$C$DW$T$107	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$107, DW_AT_byte_size(0x18)
$C$DW$762	.dwtag  DW_TAG_member
	.dwattr $C$DW$762, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$762, DW_AT_name("t")
	.dwattr $C$DW$762, DW_AT_TI_symbol_name("t")
	.dwattr $C$DW$762, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$762, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$762, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$762, DW_AT_decl_line(0x217)
	.dwattr $C$DW$762, DW_AT_decl_column(0x0d)

$C$DW$763	.dwtag  DW_TAG_member
	.dwattr $C$DW$763, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$763, DW_AT_name("p")
	.dwattr $C$DW$763, DW_AT_TI_symbol_name("p")
	.dwattr $C$DW$763, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$763, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$763, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$763, DW_AT_decl_line(0x218)
	.dwattr $C$DW$763, DW_AT_decl_column(0x0d)

$C$DW$764	.dwtag  DW_TAG_member
	.dwattr $C$DW$764, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$764, DW_AT_name("k")
	.dwattr $C$DW$764, DW_AT_TI_symbol_name("k")
	.dwattr $C$DW$764, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$764, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$764, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$764, DW_AT_decl_line(0x219)
	.dwattr $C$DW$764, DW_AT_decl_column(0x0d)

$C$DW$765	.dwtag  DW_TAG_member
	.dwattr $C$DW$765, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$765, DW_AT_name("RetransInterval")
	.dwattr $C$DW$765, DW_AT_TI_symbol_name("RetransInterval")
	.dwattr $C$DW$765, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$765, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$765, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$765, DW_AT_decl_line(0x21a)
	.dwattr $C$DW$765, DW_AT_decl_column(0x0d)

$C$DW$766	.dwtag  DW_TAG_member
	.dwattr $C$DW$766, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$766, DW_AT_name("Maxinterval")
	.dwattr $C$DW$766, DW_AT_TI_symbol_name("Maxinterval")
	.dwattr $C$DW$766, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$766, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$766, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$766, DW_AT_decl_line(0x21b)
	.dwattr $C$DW$766, DW_AT_decl_column(0x0d)

$C$DW$767	.dwtag  DW_TAG_member
	.dwattr $C$DW$767, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$767, DW_AT_name("max_time")
	.dwattr $C$DW$767, DW_AT_TI_symbol_name("max_time")
	.dwattr $C$DW$767, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$767, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$767, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$767, DW_AT_decl_line(0x21c)
	.dwattr $C$DW$767, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$107, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$107, DW_AT_decl_line(0x208)
	.dwattr $C$DW$T$107, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$107

$C$DW$T$491	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$491, DW_AT_name("SlNetAppServiceAdvertiseTimingParameters_t")
	.dwattr $C$DW$T$491, DW_AT_type(*$C$DW$T$107)
	.dwattr $C$DW$T$491, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$491, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$491, DW_AT_decl_line(0x21d)
	.dwattr $C$DW$T$491, DW_AT_decl_column(0x02)


$C$DW$T$108	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$108, DW_AT_byte_size(0x04)
$C$DW$768	.dwtag  DW_TAG_member
	.dwattr $C$DW$768, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$768, DW_AT_name("MaxResponseTime")
	.dwattr $C$DW$768, DW_AT_TI_symbol_name("MaxResponseTime")
	.dwattr $C$DW$768, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$768, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$768, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$768, DW_AT_decl_line(0x221)
	.dwattr $C$DW$768, DW_AT_decl_column(0x0c)

$C$DW$769	.dwtag  DW_TAG_member
	.dwattr $C$DW$769, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$769, DW_AT_name("NumOfRetries")
	.dwattr $C$DW$769, DW_AT_TI_symbol_name("NumOfRetries")
	.dwattr $C$DW$769, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$769, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$769, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$769, DW_AT_decl_line(0x222)
	.dwattr $C$DW$769, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$108, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$108, DW_AT_decl_line(0x220)
	.dwattr $C$DW$T$108, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$108

$C$DW$T$492	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$492, DW_AT_name("SlNetAppDnsClientTime_t")
	.dwattr $C$DW$T$492, DW_AT_type(*$C$DW$T$108)
	.dwattr $C$DW$T$492, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$492, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$492, DW_AT_decl_line(0x223)
	.dwattr $C$DW$T$492, DW_AT_decl_column(0x02)


$C$DW$T$109	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$109, DW_AT_byte_size(0x4c)
$C$DW$770	.dwtag  DW_TAG_member
	.dwattr $C$DW$770, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$770, DW_AT_name("Status")
	.dwattr $C$DW$770, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$770, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$770, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$770, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$770, DW_AT_decl_line(0x1c7)
	.dwattr $C$DW$770, DW_AT_decl_column(0x0d)

$C$DW$771	.dwtag  DW_TAG_member
	.dwattr $C$DW$771, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$771, DW_AT_name("SsidLen")
	.dwattr $C$DW$771, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$771, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$771, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$771, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$771, DW_AT_decl_line(0x1c8)
	.dwattr $C$DW$771, DW_AT_decl_column(0x0d)

$C$DW$772	.dwtag  DW_TAG_member
	.dwattr $C$DW$772, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$772, DW_AT_name("Ssid")
	.dwattr $C$DW$772, DW_AT_TI_symbol_name("Ssid")
	.dwattr $C$DW$772, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$772, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$772, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$772, DW_AT_decl_line(0x1c9)
	.dwattr $C$DW$772, DW_AT_decl_column(0x0d)

$C$DW$773	.dwtag  DW_TAG_member
	.dwattr $C$DW$773, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$773, DW_AT_name("PrivateTokenLen")
	.dwattr $C$DW$773, DW_AT_TI_symbol_name("PrivateTokenLen")
	.dwattr $C$DW$773, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$773, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$773, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$773, DW_AT_decl_line(0x1ca)
	.dwattr $C$DW$773, DW_AT_decl_column(0x0d)

$C$DW$774	.dwtag  DW_TAG_member
	.dwattr $C$DW$774, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$774, DW_AT_name("PrivateToken")
	.dwattr $C$DW$774, DW_AT_TI_symbol_name("PrivateToken")
	.dwattr $C$DW$774, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$774, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$774, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$774, DW_AT_decl_line(0x1cb)
	.dwattr $C$DW$774, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$109, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$109, DW_AT_decl_line(0x1c6)
	.dwattr $C$DW$T$109, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$109

$C$DW$T$493	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$493, DW_AT_name("SlWlanSmartConfigStartAsyncResponse_t")
	.dwattr $C$DW$T$493, DW_AT_type(*$C$DW$T$109)
	.dwattr $C$DW$T$493, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$493, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$493, DW_AT_decl_line(0x1cc)
	.dwattr $C$DW$T$493, DW_AT_decl_column(0x02)


$C$DW$T$110	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$110, DW_AT_byte_size(0x04)
$C$DW$775	.dwtag  DW_TAG_member
	.dwattr $C$DW$775, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$775, DW_AT_name("Status")
	.dwattr $C$DW$775, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$775, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$775, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$775, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$775, DW_AT_decl_line(0x1d0)
	.dwattr $C$DW$775, DW_AT_decl_column(0x0d)

$C$DW$776	.dwtag  DW_TAG_member
	.dwattr $C$DW$776, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$776, DW_AT_name("Padding")
	.dwattr $C$DW$776, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$776, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$776, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$776, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$776, DW_AT_decl_line(0x1d1)
	.dwattr $C$DW$776, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$110, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$110, DW_AT_decl_line(0x1cf)
	.dwattr $C$DW$T$110, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$110

$C$DW$T$494	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$494, DW_AT_name("SlWlanSmartConfigStopAsyncResponse_t")
	.dwattr $C$DW$T$494, DW_AT_type(*$C$DW$T$110)
	.dwattr $C$DW$T$494, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$494, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$494, DW_AT_decl_line(0x1d2)
	.dwattr $C$DW$T$494, DW_AT_decl_column(0x02)


$C$DW$T$111	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$111, DW_AT_byte_size(0x04)
$C$DW$777	.dwtag  DW_TAG_member
	.dwattr $C$DW$777, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$777, DW_AT_name("Status")
	.dwattr $C$DW$777, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$777, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$777, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$777, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$777, DW_AT_decl_line(0x1d6)
	.dwattr $C$DW$777, DW_AT_decl_column(0x0d)

$C$DW$778	.dwtag  DW_TAG_member
	.dwattr $C$DW$778, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$778, DW_AT_name("Padding")
	.dwattr $C$DW$778, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$778, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$778, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$778, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$778, DW_AT_decl_line(0x1d7)
	.dwattr $C$DW$778, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$111, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$111, DW_AT_decl_line(0x1d5)
	.dwattr $C$DW$T$111, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$111

$C$DW$T$495	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$495, DW_AT_name("SlWlanConnFailureAsyncResponse_t")
	.dwattr $C$DW$T$495, DW_AT_type(*$C$DW$T$111)
	.dwattr $C$DW$T$495, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$495, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$495, DW_AT_decl_line(0x1d8)
	.dwattr $C$DW$T$495, DW_AT_decl_column(0x02)


$C$DW$T$112	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$112, DW_AT_byte_size(0x04)
$C$DW$779	.dwtag  DW_TAG_member
	.dwattr $C$DW$779, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$779, DW_AT_name("Status")
	.dwattr $C$DW$779, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$779, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$779, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$779, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$779, DW_AT_decl_line(0x1dc)
	.dwattr $C$DW$779, DW_AT_decl_column(0x0d)

$C$DW$780	.dwtag  DW_TAG_member
	.dwattr $C$DW$780, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$780, DW_AT_name("Padding")
	.dwattr $C$DW$780, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$780, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$780, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$780, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$780, DW_AT_decl_line(0x1dd)
	.dwattr $C$DW$780, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$112, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$112, DW_AT_decl_line(0x1db)
	.dwattr $C$DW$T$112, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$112

$C$DW$T$496	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$496, DW_AT_name("SlWlanProvisioningStatusAsyncResponse_t")
	.dwattr $C$DW$T$496, DW_AT_type(*$C$DW$T$112)
	.dwattr $C$DW$T$496, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$496, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$496, DW_AT_decl_line(0x1de)
	.dwattr $C$DW$T$496, DW_AT_decl_column(0x02)


$C$DW$T$114	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$114, DW_AT_byte_size(0x09)
$C$DW$781	.dwtag  DW_TAG_member
	.dwattr $C$DW$781, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$781, DW_AT_name("Type")
	.dwattr $C$DW$781, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$781, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$781, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$781, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$781, DW_AT_decl_line(0x1ef)
	.dwattr $C$DW$781, DW_AT_decl_column(0x0d)

$C$DW$782	.dwtag  DW_TAG_member
	.dwattr $C$DW$782, DW_AT_type(*$C$DW$T$113)
	.dwattr $C$DW$782, DW_AT_name("UserActionIdBitmap")
	.dwattr $C$DW$782, DW_AT_TI_symbol_name("UserActionIdBitmap")
	.dwattr $C$DW$782, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$782, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$782, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$782, DW_AT_decl_line(0x1f0)
	.dwattr $C$DW$782, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$114, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$114, DW_AT_decl_line(0x1ee)
	.dwattr $C$DW$T$114, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$114

$C$DW$T$234	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$234, DW_AT_name("SlWlanEventRxFilterInfo_t")
	.dwattr $C$DW$T$234, DW_AT_type(*$C$DW$T$114)
	.dwattr $C$DW$T$234, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$234, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$234, DW_AT_decl_line(0x1f1)
	.dwattr $C$DW$T$234, DW_AT_decl_column(0x02)


$C$DW$T$115	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$115, DW_AT_byte_size(0x28)
$C$DW$783	.dwtag  DW_TAG_member
	.dwattr $C$DW$783, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$783, DW_AT_name("SsidLen")
	.dwattr $C$DW$783, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$783, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$783, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$783, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$783, DW_AT_decl_line(0x205)
	.dwattr $C$DW$783, DW_AT_decl_column(0x0d)

$C$DW$784	.dwtag  DW_TAG_member
	.dwattr $C$DW$784, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$784, DW_AT_name("SsidName")
	.dwattr $C$DW$784, DW_AT_TI_symbol_name("SsidName")
	.dwattr $C$DW$784, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$784, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$784, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$784, DW_AT_decl_line(0x206)
	.dwattr $C$DW$784, DW_AT_decl_column(0x0d)

$C$DW$785	.dwtag  DW_TAG_member
	.dwattr $C$DW$785, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$785, DW_AT_name("Bssid")
	.dwattr $C$DW$785, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$785, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$785, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$785, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$785, DW_AT_decl_line(0x207)
	.dwattr $C$DW$785, DW_AT_decl_column(0x0d)

$C$DW$786	.dwtag  DW_TAG_member
	.dwattr $C$DW$786, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$786, DW_AT_name("ReasonCode")
	.dwattr $C$DW$786, DW_AT_TI_symbol_name("ReasonCode")
	.dwattr $C$DW$786, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$786, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$786, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$786, DW_AT_decl_line(0x208)
	.dwattr $C$DW$786, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$115, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$115, DW_AT_decl_line(0x204)
	.dwattr $C$DW$T$115, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$115

$C$DW$T$225	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$225, DW_AT_name("SlWlanEventDisconnect_t")
	.dwattr $C$DW$T$225, DW_AT_type(*$C$DW$T$115)
	.dwattr $C$DW$T$225, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$225, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$225, DW_AT_decl_line(0x209)
	.dwattr $C$DW$T$225, DW_AT_decl_column(0x03)


$C$DW$T$116	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$116, DW_AT_byte_size(0x08)
$C$DW$787	.dwtag  DW_TAG_member
	.dwattr $C$DW$787, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$787, DW_AT_name("Mac")
	.dwattr $C$DW$787, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$787, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$787, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$787, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$787, DW_AT_decl_line(0x20d)
	.dwattr $C$DW$787, DW_AT_decl_column(0x0b)

$C$DW$788	.dwtag  DW_TAG_member
	.dwattr $C$DW$788, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$788, DW_AT_name("Padding")
	.dwattr $C$DW$788, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$788, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$788, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$788, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$788, DW_AT_decl_line(0x20e)
	.dwattr $C$DW$788, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$116, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$116, DW_AT_decl_line(0x20c)
	.dwattr $C$DW$T$116, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$116

$C$DW$T$226	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$226, DW_AT_name("SlWlanEventSTAAdded_t")
	.dwattr $C$DW$T$226, DW_AT_type(*$C$DW$T$116)
	.dwattr $C$DW$T$226, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$226, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$226, DW_AT_decl_line(0x20f)
	.dwattr $C$DW$T$226, DW_AT_decl_column(0x02)

$C$DW$T$227	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$227, DW_AT_name("SlWlanEventSTARemoved_t")
	.dwattr $C$DW$T$227, DW_AT_type(*$C$DW$T$116)
	.dwattr $C$DW$T$227, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$227, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$227, DW_AT_decl_line(0x20f)
	.dwattr $C$DW$T$227, DW_AT_decl_column(0x19)


$C$DW$T$117	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$117, DW_AT_byte_size(0x4c)
$C$DW$789	.dwtag  DW_TAG_member
	.dwattr $C$DW$789, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$789, DW_AT_name("SsidLen")
	.dwattr $C$DW$789, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$789, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$789, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$789, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$789, DW_AT_decl_line(0x21f)
	.dwattr $C$DW$789, DW_AT_decl_column(0x0d)

$C$DW$790	.dwtag  DW_TAG_member
	.dwattr $C$DW$790, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$790, DW_AT_name("SsidName")
	.dwattr $C$DW$790, DW_AT_TI_symbol_name("SsidName")
	.dwattr $C$DW$790, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$790, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$790, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$790, DW_AT_decl_line(0x220)
	.dwattr $C$DW$790, DW_AT_decl_column(0x0d)

$C$DW$791	.dwtag  DW_TAG_member
	.dwattr $C$DW$791, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$791, DW_AT_name("Bssid")
	.dwattr $C$DW$791, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$791, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$791, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$791, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$791, DW_AT_decl_line(0x221)
	.dwattr $C$DW$791, DW_AT_decl_column(0x0d)

$C$DW$792	.dwtag  DW_TAG_member
	.dwattr $C$DW$792, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$792, DW_AT_name("ReasonCode")
	.dwattr $C$DW$792, DW_AT_TI_symbol_name("ReasonCode")
	.dwattr $C$DW$792, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$792, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$792, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$792, DW_AT_decl_line(0x222)
	.dwattr $C$DW$792, DW_AT_decl_column(0x0d)

$C$DW$793	.dwtag  DW_TAG_member
	.dwattr $C$DW$793, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$793, DW_AT_name("GoDeviceNameLen")
	.dwattr $C$DW$793, DW_AT_TI_symbol_name("GoDeviceNameLen")
	.dwattr $C$DW$793, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$793, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$793, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$793, DW_AT_decl_line(0x223)
	.dwattr $C$DW$793, DW_AT_decl_column(0x0d)

$C$DW$794	.dwtag  DW_TAG_member
	.dwattr $C$DW$794, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$794, DW_AT_name("GoDeviceName")
	.dwattr $C$DW$794, DW_AT_TI_symbol_name("GoDeviceName")
	.dwattr $C$DW$794, DW_AT_data_member_location[DW_OP_plus_uconst 0x29]
	.dwattr $C$DW$794, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$794, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$794, DW_AT_decl_line(0x224)
	.dwattr $C$DW$794, DW_AT_decl_column(0x0d)

$C$DW$795	.dwtag  DW_TAG_member
	.dwattr $C$DW$795, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$795, DW_AT_name("Padding")
	.dwattr $C$DW$795, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$795, DW_AT_data_member_location[DW_OP_plus_uconst 0x49]
	.dwattr $C$DW$795, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$795, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$795, DW_AT_decl_line(0x225)
	.dwattr $C$DW$795, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$117, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$117, DW_AT_decl_line(0x21e)
	.dwattr $C$DW$T$117, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$117

$C$DW$T$228	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$228, DW_AT_name("SlWlanEventP2PDisconnect_t")
	.dwattr $C$DW$T$228, DW_AT_type(*$C$DW$T$117)
	.dwattr $C$DW$T$228, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$228, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$228, DW_AT_decl_line(0x226)
	.dwattr $C$DW$T$228, DW_AT_decl_column(0x03)


$C$DW$T$118	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$118, DW_AT_byte_size(0x48)
$C$DW$796	.dwtag  DW_TAG_member
	.dwattr $C$DW$796, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$796, DW_AT_name("Mac")
	.dwattr $C$DW$796, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$796, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$796, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$796, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$796, DW_AT_decl_line(0x22a)
	.dwattr $C$DW$796, DW_AT_decl_column(0x0d)

$C$DW$797	.dwtag  DW_TAG_member
	.dwattr $C$DW$797, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$797, DW_AT_name("ClDeviceNameLen")
	.dwattr $C$DW$797, DW_AT_TI_symbol_name("ClDeviceNameLen")
	.dwattr $C$DW$797, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$797, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$797, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$797, DW_AT_decl_line(0x22b)
	.dwattr $C$DW$797, DW_AT_decl_column(0x0d)

$C$DW$798	.dwtag  DW_TAG_member
	.dwattr $C$DW$798, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$798, DW_AT_name("ClDeviceName")
	.dwattr $C$DW$798, DW_AT_TI_symbol_name("ClDeviceName")
	.dwattr $C$DW$798, DW_AT_data_member_location[DW_OP_plus_uconst 0x7]
	.dwattr $C$DW$798, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$798, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$798, DW_AT_decl_line(0x22c)
	.dwattr $C$DW$798, DW_AT_decl_column(0x0d)

$C$DW$799	.dwtag  DW_TAG_member
	.dwattr $C$DW$799, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$799, DW_AT_name("OwnSsidLen")
	.dwattr $C$DW$799, DW_AT_TI_symbol_name("OwnSsidLen")
	.dwattr $C$DW$799, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$799, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$799, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$799, DW_AT_decl_line(0x22d)
	.dwattr $C$DW$799, DW_AT_decl_column(0x0d)

$C$DW$800	.dwtag  DW_TAG_member
	.dwattr $C$DW$800, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$800, DW_AT_name("OwnSsid")
	.dwattr $C$DW$800, DW_AT_TI_symbol_name("OwnSsid")
	.dwattr $C$DW$800, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$800, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$800, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$800, DW_AT_decl_line(0x22e)
	.dwattr $C$DW$800, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$118, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$118, DW_AT_decl_line(0x229)
	.dwattr $C$DW$T$118, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$118

$C$DW$T$229	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$229, DW_AT_name("SlWlanEventP2PClientAdded_t")
	.dwattr $C$DW$T$229, DW_AT_type(*$C$DW$T$118)
	.dwattr $C$DW$T$229, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$229, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$229, DW_AT_decl_line(0x22f)
	.dwattr $C$DW$T$229, DW_AT_decl_column(0x02)

$C$DW$T$230	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$230, DW_AT_name("SlWlanEventP2PClientRemoved_t")
	.dwattr $C$DW$T$230, DW_AT_type(*$C$DW$T$118)
	.dwattr $C$DW$T$230, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$230, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$230, DW_AT_decl_line(0x22f)
	.dwattr $C$DW$T$230, DW_AT_decl_column(0x1f)


$C$DW$T$119	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$119, DW_AT_byte_size(0x28)
$C$DW$801	.dwtag  DW_TAG_member
	.dwattr $C$DW$801, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$801, DW_AT_name("GoDeviceNameLen")
	.dwattr $C$DW$801, DW_AT_TI_symbol_name("GoDeviceNameLen")
	.dwattr $C$DW$801, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$801, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$801, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$801, DW_AT_decl_line(0x233)
	.dwattr $C$DW$801, DW_AT_decl_column(0x0d)

$C$DW$802	.dwtag  DW_TAG_member
	.dwattr $C$DW$802, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$802, DW_AT_name("GoDeviceName")
	.dwattr $C$DW$802, DW_AT_TI_symbol_name("GoDeviceName")
	.dwattr $C$DW$802, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$802, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$802, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$802, DW_AT_decl_line(0x234)
	.dwattr $C$DW$802, DW_AT_decl_column(0x0d)

$C$DW$803	.dwtag  DW_TAG_member
	.dwattr $C$DW$803, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$803, DW_AT_name("Mac")
	.dwattr $C$DW$803, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$803, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$803, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$803, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$803, DW_AT_decl_line(0x235)
	.dwattr $C$DW$803, DW_AT_decl_column(0x0d)

$C$DW$804	.dwtag  DW_TAG_member
	.dwattr $C$DW$804, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$804, DW_AT_name("WpsMethod")
	.dwattr $C$DW$804, DW_AT_TI_symbol_name("WpsMethod")
	.dwattr $C$DW$804, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$804, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$804, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$804, DW_AT_decl_line(0x236)
	.dwattr $C$DW$804, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$119, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$119, DW_AT_decl_line(0x232)
	.dwattr $C$DW$T$119, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$119

$C$DW$T$231	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$231, DW_AT_name("SlWlanEventP2PDevFound_t")
	.dwattr $C$DW$T$231, DW_AT_type(*$C$DW$T$119)
	.dwattr $C$DW$T$231, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$231, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$231, DW_AT_decl_line(0x237)
	.dwattr $C$DW$T$231, DW_AT_decl_column(0x02)

$C$DW$T$232	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$232, DW_AT_name("SlWlanEventP2PRequest_t")
	.dwattr $C$DW$T$232, DW_AT_type(*$C$DW$T$119)
	.dwattr $C$DW$T$232, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$232, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$232, DW_AT_decl_line(0x237)
	.dwattr $C$DW$T$232, DW_AT_decl_column(0x1c)


$C$DW$T$120	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$120, DW_AT_byte_size(0x04)
$C$DW$805	.dwtag  DW_TAG_member
	.dwattr $C$DW$805, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$805, DW_AT_name("Status")
	.dwattr $C$DW$805, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$805, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$805, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$805, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$805, DW_AT_decl_line(0x23c)
	.dwattr $C$DW$805, DW_AT_decl_column(0x0d)

$C$DW$806	.dwtag  DW_TAG_member
	.dwattr $C$DW$806, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$806, DW_AT_name("Padding")
	.dwattr $C$DW$806, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$806, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$806, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$806, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$806, DW_AT_decl_line(0x23d)
	.dwattr $C$DW$806, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$120, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$120, DW_AT_decl_line(0x23b)
	.dwattr $C$DW$T$120, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$120

$C$DW$T$233	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$233, DW_AT_name("SlWlanEventP2PConnectFail_t")
	.dwattr $C$DW$T$233, DW_AT_type(*$C$DW$T$120)
	.dwattr $C$DW$T$233, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$233, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$233, DW_AT_decl_line(0x23e)
	.dwattr $C$DW$T$233, DW_AT_decl_column(0x02)


$C$DW$T$121	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$121, DW_AT_byte_size(0x28)
$C$DW$807	.dwtag  DW_TAG_member
	.dwattr $C$DW$807, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$807, DW_AT_name("ProvisioningStatus")
	.dwattr $C$DW$807, DW_AT_TI_symbol_name("ProvisioningStatus")
	.dwattr $C$DW$807, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$807, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$807, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$807, DW_AT_decl_line(0x242)
	.dwattr $C$DW$807, DW_AT_decl_column(0x0a)

$C$DW$808	.dwtag  DW_TAG_member
	.dwattr $C$DW$808, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$808, DW_AT_name("Role")
	.dwattr $C$DW$808, DW_AT_TI_symbol_name("Role")
	.dwattr $C$DW$808, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$808, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$808, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$808, DW_AT_decl_line(0x243)
	.dwattr $C$DW$808, DW_AT_decl_column(0x0a)

$C$DW$809	.dwtag  DW_TAG_member
	.dwattr $C$DW$809, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$809, DW_AT_name("WlanStatus")
	.dwattr $C$DW$809, DW_AT_TI_symbol_name("WlanStatus")
	.dwattr $C$DW$809, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$809, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$809, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$809, DW_AT_decl_line(0x244)
	.dwattr $C$DW$809, DW_AT_decl_column(0x0a)

$C$DW$810	.dwtag  DW_TAG_member
	.dwattr $C$DW$810, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$810, DW_AT_name("Ssidlen")
	.dwattr $C$DW$810, DW_AT_TI_symbol_name("Ssidlen")
	.dwattr $C$DW$810, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$810, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$810, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$810, DW_AT_decl_line(0x245)
	.dwattr $C$DW$810, DW_AT_decl_column(0x0a)

$C$DW$811	.dwtag  DW_TAG_member
	.dwattr $C$DW$811, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$811, DW_AT_name("Ssid")
	.dwattr $C$DW$811, DW_AT_TI_symbol_name("Ssid")
	.dwattr $C$DW$811, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$811, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$811, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$811, DW_AT_decl_line(0x246)
	.dwattr $C$DW$811, DW_AT_decl_column(0x0a)

$C$DW$812	.dwtag  DW_TAG_member
	.dwattr $C$DW$812, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$812, DW_AT_name("Reserved")
	.dwattr $C$DW$812, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$812, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$812, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$812, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$812, DW_AT_decl_line(0x247)
	.dwattr $C$DW$812, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$121, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$121, DW_AT_decl_line(0x241)
	.dwattr $C$DW$T$121, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$121

$C$DW$T$235	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$235, DW_AT_name("SlWlanEventProvisioningStatus_t")
	.dwattr $C$DW$T$235, DW_AT_type(*$C$DW$T$121)
	.dwattr $C$DW$T$235, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$235, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$235, DW_AT_decl_line(0x248)
	.dwattr $C$DW$T$235, DW_AT_decl_column(0x02)


$C$DW$T$122	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$122, DW_AT_byte_size(0x4c)
$C$DW$813	.dwtag  DW_TAG_member
	.dwattr $C$DW$813, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$813, DW_AT_name("Status")
	.dwattr $C$DW$813, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$813, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$813, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$813, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$813, DW_AT_decl_line(0x24c)
	.dwattr $C$DW$813, DW_AT_decl_column(0x0d)

$C$DW$814	.dwtag  DW_TAG_member
	.dwattr $C$DW$814, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$814, DW_AT_name("SsidLen")
	.dwattr $C$DW$814, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$814, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$814, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$814, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$814, DW_AT_decl_line(0x24d)
	.dwattr $C$DW$814, DW_AT_decl_column(0x0d)

$C$DW$815	.dwtag  DW_TAG_member
	.dwattr $C$DW$815, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$815, DW_AT_name("Ssid")
	.dwattr $C$DW$815, DW_AT_TI_symbol_name("Ssid")
	.dwattr $C$DW$815, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$815, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$815, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$815, DW_AT_decl_line(0x24e)
	.dwattr $C$DW$815, DW_AT_decl_column(0x0d)

$C$DW$816	.dwtag  DW_TAG_member
	.dwattr $C$DW$816, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$816, DW_AT_name("ReservedLen")
	.dwattr $C$DW$816, DW_AT_TI_symbol_name("ReservedLen")
	.dwattr $C$DW$816, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$816, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$816, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$816, DW_AT_decl_line(0x24f)
	.dwattr $C$DW$816, DW_AT_decl_column(0x0d)

$C$DW$817	.dwtag  DW_TAG_member
	.dwattr $C$DW$817, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$817, DW_AT_name("Reserved")
	.dwattr $C$DW$817, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$817, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$817, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$817, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$817, DW_AT_decl_line(0x250)
	.dwattr $C$DW$817, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$122, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$122, DW_AT_decl_line(0x24b)
	.dwattr $C$DW$T$122, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$122

$C$DW$T$236	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$236, DW_AT_name("SlWlanEventProvisioningProfileAdded_t")
	.dwattr $C$DW$T$236, DW_AT_type(*$C$DW$T$122)
	.dwattr $C$DW$T$236, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$236, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$236, DW_AT_decl_line(0x251)
	.dwattr $C$DW$T$236, DW_AT_decl_column(0x03)


$C$DW$T$124	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$124, DW_AT_byte_size(0x50)
$C$DW$818	.dwtag  DW_TAG_member
	.dwattr $C$DW$818, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$818, DW_AT_name("Id")
	.dwattr $C$DW$818, DW_AT_TI_symbol_name("Id")
	.dwattr $C$DW$818, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$818, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$818, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$818, DW_AT_decl_line(0x268)
	.dwattr $C$DW$818, DW_AT_decl_column(0x19)

$C$DW$819	.dwtag  DW_TAG_member
	.dwattr $C$DW$819, DW_AT_type(*$C$DW$T$123)
	.dwattr $C$DW$819, DW_AT_name("Data")
	.dwattr $C$DW$819, DW_AT_TI_symbol_name("Data")
	.dwattr $C$DW$819, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$819, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$819, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$819, DW_AT_decl_line(0x269)
	.dwattr $C$DW$819, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$124, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$124, DW_AT_decl_line(0x267)
	.dwattr $C$DW$T$124, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$124

$C$DW$T$497	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$497, DW_AT_name("SlWlanEvent_t")
	.dwattr $C$DW$T$497, DW_AT_type(*$C$DW$T$124)
	.dwattr $C$DW$T$497, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$497, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$497, DW_AT_decl_line(0x26a)
	.dwattr $C$DW$T$497, DW_AT_decl_column(0x03)


$C$DW$T$127	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$127, DW_AT_byte_size(0x4c)
$C$DW$820	.dwtag  DW_TAG_member
	.dwattr $C$DW$820, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$820, DW_AT_name("ReceivedValidPacketsNumber")
	.dwattr $C$DW$820, DW_AT_TI_symbol_name("ReceivedValidPacketsNumber")
	.dwattr $C$DW$820, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$820, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$820, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$820, DW_AT_decl_line(0x26e)
	.dwattr $C$DW$820, DW_AT_decl_column(0x0b)

$C$DW$821	.dwtag  DW_TAG_member
	.dwattr $C$DW$821, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$821, DW_AT_name("ReceivedFcsErrorPacketsNumber")
	.dwattr $C$DW$821, DW_AT_TI_symbol_name("ReceivedFcsErrorPacketsNumber")
	.dwattr $C$DW$821, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$821, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$821, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$821, DW_AT_decl_line(0x26f)
	.dwattr $C$DW$821, DW_AT_decl_column(0x0b)

$C$DW$822	.dwtag  DW_TAG_member
	.dwattr $C$DW$822, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$822, DW_AT_name("ReceivedAddressMismatchPacketsNumber")
	.dwattr $C$DW$822, DW_AT_TI_symbol_name("ReceivedAddressMismatchPacketsNumber")
	.dwattr $C$DW$822, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$822, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$822, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$822, DW_AT_decl_line(0x270)
	.dwattr $C$DW$822, DW_AT_decl_column(0x0b)

$C$DW$823	.dwtag  DW_TAG_member
	.dwattr $C$DW$823, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$823, DW_AT_name("AvarageDataCtrlRssi")
	.dwattr $C$DW$823, DW_AT_TI_symbol_name("AvarageDataCtrlRssi")
	.dwattr $C$DW$823, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$823, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$823, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$823, DW_AT_decl_line(0x271)
	.dwattr $C$DW$823, DW_AT_decl_column(0x0b)

$C$DW$824	.dwtag  DW_TAG_member
	.dwattr $C$DW$824, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$824, DW_AT_name("AvarageMgMntRssi")
	.dwattr $C$DW$824, DW_AT_TI_symbol_name("AvarageMgMntRssi")
	.dwattr $C$DW$824, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr $C$DW$824, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$824, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$824, DW_AT_decl_line(0x272)
	.dwattr $C$DW$824, DW_AT_decl_column(0x0b)

$C$DW$825	.dwtag  DW_TAG_member
	.dwattr $C$DW$825, DW_AT_type(*$C$DW$T$125)
	.dwattr $C$DW$825, DW_AT_name("RateHistogram")
	.dwattr $C$DW$825, DW_AT_TI_symbol_name("RateHistogram")
	.dwattr $C$DW$825, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$825, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$825, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$825, DW_AT_decl_line(0x273)
	.dwattr $C$DW$825, DW_AT_decl_column(0x0b)

$C$DW$826	.dwtag  DW_TAG_member
	.dwattr $C$DW$826, DW_AT_type(*$C$DW$T$126)
	.dwattr $C$DW$826, DW_AT_name("RssiHistogram")
	.dwattr $C$DW$826, DW_AT_TI_symbol_name("RssiHistogram")
	.dwattr $C$DW$826, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$826, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$826, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$826, DW_AT_decl_line(0x274)
	.dwattr $C$DW$826, DW_AT_decl_column(0x0b)

$C$DW$827	.dwtag  DW_TAG_member
	.dwattr $C$DW$827, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$827, DW_AT_name("StartTimeStamp")
	.dwattr $C$DW$827, DW_AT_TI_symbol_name("StartTimeStamp")
	.dwattr $C$DW$827, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$827, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$827, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$827, DW_AT_decl_line(0x275)
	.dwattr $C$DW$827, DW_AT_decl_column(0x0b)

$C$DW$828	.dwtag  DW_TAG_member
	.dwattr $C$DW$828, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$828, DW_AT_name("GetTimeStamp")
	.dwattr $C$DW$828, DW_AT_TI_symbol_name("GetTimeStamp")
	.dwattr $C$DW$828, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$828, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$828, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$828, DW_AT_decl_line(0x276)
	.dwattr $C$DW$828, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$127, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$127, DW_AT_decl_line(0x26d)
	.dwattr $C$DW$T$127, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$127

$C$DW$T$498	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$498, DW_AT_name("SlWlanGetRxStatResponse_t")
	.dwattr $C$DW$T$498, DW_AT_type(*$C$DW$T$127)
	.dwattr $C$DW$T$498, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$498, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$498, DW_AT_decl_line(0x277)
	.dwattr $C$DW$T$498, DW_AT_decl_column(0x02)


$C$DW$T$129	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$129, DW_AT_byte_size(0x2c)
$C$DW$829	.dwtag  DW_TAG_member
	.dwattr $C$DW$829, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$829, DW_AT_name("Ssid")
	.dwattr $C$DW$829, DW_AT_TI_symbol_name("Ssid")
	.dwattr $C$DW$829, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$829, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$829, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$829, DW_AT_decl_line(0x27b)
	.dwattr $C$DW$829, DW_AT_decl_column(0x09)

$C$DW$830	.dwtag  DW_TAG_member
	.dwattr $C$DW$830, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$830, DW_AT_name("Bssid")
	.dwattr $C$DW$830, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$830, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$830, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$830, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$830, DW_AT_decl_line(0x27c)
	.dwattr $C$DW$830, DW_AT_decl_column(0x09)

$C$DW$831	.dwtag  DW_TAG_member
	.dwattr $C$DW$831, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$831, DW_AT_name("SsidLen")
	.dwattr $C$DW$831, DW_AT_TI_symbol_name("SsidLen")
	.dwattr $C$DW$831, DW_AT_data_member_location[DW_OP_plus_uconst 0x26]
	.dwattr $C$DW$831, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$831, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$831, DW_AT_decl_line(0x27d)
	.dwattr $C$DW$831, DW_AT_decl_column(0x09)

$C$DW$832	.dwtag  DW_TAG_member
	.dwattr $C$DW$832, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$832, DW_AT_name("Rssi")
	.dwattr $C$DW$832, DW_AT_TI_symbol_name("Rssi")
	.dwattr $C$DW$832, DW_AT_data_member_location[DW_OP_plus_uconst 0x27]
	.dwattr $C$DW$832, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$832, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$832, DW_AT_decl_line(0x27e)
	.dwattr $C$DW$832, DW_AT_decl_column(0x09)

$C$DW$833	.dwtag  DW_TAG_member
	.dwattr $C$DW$833, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$833, DW_AT_name("SecurityInfo")
	.dwattr $C$DW$833, DW_AT_TI_symbol_name("SecurityInfo")
	.dwattr $C$DW$833, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$833, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$833, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$833, DW_AT_decl_line(0x27f)
	.dwattr $C$DW$833, DW_AT_decl_column(0x0a)

$C$DW$834	.dwtag  DW_TAG_member
	.dwattr $C$DW$834, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$834, DW_AT_name("Channel")
	.dwattr $C$DW$834, DW_AT_TI_symbol_name("Channel")
	.dwattr $C$DW$834, DW_AT_data_member_location[DW_OP_plus_uconst 0x2a]
	.dwattr $C$DW$834, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$834, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$834, DW_AT_decl_line(0x280)
	.dwattr $C$DW$834, DW_AT_decl_column(0x09)

$C$DW$835	.dwtag  DW_TAG_member
	.dwattr $C$DW$835, DW_AT_type(*$C$DW$T$128)
	.dwattr $C$DW$835, DW_AT_name("Reserved")
	.dwattr $C$DW$835, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$835, DW_AT_data_member_location[DW_OP_plus_uconst 0x2b]
	.dwattr $C$DW$835, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$835, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$835, DW_AT_decl_line(0x281)
	.dwattr $C$DW$835, DW_AT_decl_column(0x09)

	.dwattr $C$DW$T$129, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$129, DW_AT_decl_line(0x27a)
	.dwattr $C$DW$T$129, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$129

$C$DW$T$499	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$499, DW_AT_name("SlWlanNetworkEntry_t")
	.dwattr $C$DW$T$499, DW_AT_type(*$C$DW$T$129)
	.dwattr $C$DW$T$499, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$499, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$499, DW_AT_decl_line(0x282)
	.dwattr $C$DW$T$499, DW_AT_decl_column(0x02)


$C$DW$T$131	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$131, DW_AT_byte_size(0x0c)
$C$DW$836	.dwtag  DW_TAG_member
	.dwattr $C$DW$836, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$836, DW_AT_name("Type")
	.dwattr $C$DW$836, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$836, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$836, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$836, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$836, DW_AT_decl_line(0x286)
	.dwattr $C$DW$836, DW_AT_decl_column(0x0b)

$C$DW$837	.dwtag  DW_TAG_member
	.dwattr $C$DW$837, DW_AT_type(*$C$DW$T$130)
	.dwattr $C$DW$837, DW_AT_name("Key")
	.dwattr $C$DW$837, DW_AT_TI_symbol_name("Key")
	.dwattr $C$DW$837, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$837, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$837, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$837, DW_AT_decl_line(0x287)
	.dwattr $C$DW$837, DW_AT_decl_column(0x0b)

$C$DW$838	.dwtag  DW_TAG_member
	.dwattr $C$DW$838, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$838, DW_AT_name("KeyLen")
	.dwattr $C$DW$838, DW_AT_TI_symbol_name("KeyLen")
	.dwattr $C$DW$838, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$838, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$838, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$838, DW_AT_decl_line(0x288)
	.dwattr $C$DW$838, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$131, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$131, DW_AT_decl_line(0x285)
	.dwattr $C$DW$T$131, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$131

$C$DW$T$500	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$500, DW_AT_name("SlWlanSecParams_t")
	.dwattr $C$DW$T$500, DW_AT_type(*$C$DW$T$131)
	.dwattr $C$DW$T$500, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$500, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$500, DW_AT_decl_line(0x289)
	.dwattr $C$DW$T$500, DW_AT_decl_column(0x02)


$C$DW$T$132	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$132, DW_AT_byte_size(0x14)
$C$DW$839	.dwtag  DW_TAG_member
	.dwattr $C$DW$839, DW_AT_type(*$C$DW$T$130)
	.dwattr $C$DW$839, DW_AT_name("User")
	.dwattr $C$DW$839, DW_AT_TI_symbol_name("User")
	.dwattr $C$DW$839, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$839, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$839, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$839, DW_AT_decl_line(0x28d)
	.dwattr $C$DW$839, DW_AT_decl_column(0x0b)

$C$DW$840	.dwtag  DW_TAG_member
	.dwattr $C$DW$840, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$840, DW_AT_name("UserLen")
	.dwattr $C$DW$840, DW_AT_TI_symbol_name("UserLen")
	.dwattr $C$DW$840, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$840, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$840, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$840, DW_AT_decl_line(0x28e)
	.dwattr $C$DW$840, DW_AT_decl_column(0x0b)

$C$DW$841	.dwtag  DW_TAG_member
	.dwattr $C$DW$841, DW_AT_type(*$C$DW$T$130)
	.dwattr $C$DW$841, DW_AT_name("AnonUser")
	.dwattr $C$DW$841, DW_AT_TI_symbol_name("AnonUser")
	.dwattr $C$DW$841, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$841, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$841, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$841, DW_AT_decl_line(0x28f)
	.dwattr $C$DW$841, DW_AT_decl_column(0x0b)

$C$DW$842	.dwtag  DW_TAG_member
	.dwattr $C$DW$842, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$842, DW_AT_name("AnonUserLen")
	.dwattr $C$DW$842, DW_AT_TI_symbol_name("AnonUserLen")
	.dwattr $C$DW$842, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$842, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$842, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$842, DW_AT_decl_line(0x290)
	.dwattr $C$DW$842, DW_AT_decl_column(0x0b)

$C$DW$843	.dwtag  DW_TAG_member
	.dwattr $C$DW$843, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$843, DW_AT_name("CertIndex")
	.dwattr $C$DW$843, DW_AT_TI_symbol_name("CertIndex")
	.dwattr $C$DW$843, DW_AT_data_member_location[DW_OP_plus_uconst 0xd]
	.dwattr $C$DW$843, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$843, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$843, DW_AT_decl_line(0x291)
	.dwattr $C$DW$843, DW_AT_decl_column(0x0b)

$C$DW$844	.dwtag  DW_TAG_member
	.dwattr $C$DW$844, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$844, DW_AT_name("EapMethod")
	.dwattr $C$DW$844, DW_AT_TI_symbol_name("EapMethod")
	.dwattr $C$DW$844, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$844, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$844, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$844, DW_AT_decl_line(0x292)
	.dwattr $C$DW$844, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$132, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$132, DW_AT_decl_line(0x28c)
	.dwattr $C$DW$T$132, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$132

$C$DW$T$501	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$501, DW_AT_name("SlWlanSecParamsExt_t")
	.dwattr $C$DW$T$501, DW_AT_type(*$C$DW$T$132)
	.dwattr $C$DW$T$501, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$501, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$501, DW_AT_decl_line(0x293)
	.dwattr $C$DW$T$501, DW_AT_decl_column(0x02)


$C$DW$T$134	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$134, DW_AT_byte_size(0x88)
$C$DW$845	.dwtag  DW_TAG_member
	.dwattr $C$DW$845, DW_AT_type(*$C$DW$T$133)
	.dwattr $C$DW$845, DW_AT_name("User")
	.dwattr $C$DW$845, DW_AT_TI_symbol_name("User")
	.dwattr $C$DW$845, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$845, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$845, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$845, DW_AT_decl_line(0x297)
	.dwattr $C$DW$845, DW_AT_decl_column(0x0b)

$C$DW$846	.dwtag  DW_TAG_member
	.dwattr $C$DW$846, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$846, DW_AT_name("UserLen")
	.dwattr $C$DW$846, DW_AT_TI_symbol_name("UserLen")
	.dwattr $C$DW$846, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$846, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$846, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$846, DW_AT_decl_line(0x298)
	.dwattr $C$DW$846, DW_AT_decl_column(0x0b)

$C$DW$847	.dwtag  DW_TAG_member
	.dwattr $C$DW$847, DW_AT_type(*$C$DW$T$133)
	.dwattr $C$DW$847, DW_AT_name("AnonUser")
	.dwattr $C$DW$847, DW_AT_TI_symbol_name("AnonUser")
	.dwattr $C$DW$847, DW_AT_data_member_location[DW_OP_plus_uconst 0x41]
	.dwattr $C$DW$847, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$847, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$847, DW_AT_decl_line(0x299)
	.dwattr $C$DW$847, DW_AT_decl_column(0x0b)

$C$DW$848	.dwtag  DW_TAG_member
	.dwattr $C$DW$848, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$848, DW_AT_name("AnonUserLen")
	.dwattr $C$DW$848, DW_AT_TI_symbol_name("AnonUserLen")
	.dwattr $C$DW$848, DW_AT_data_member_location[DW_OP_plus_uconst 0x81]
	.dwattr $C$DW$848, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$848, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$848, DW_AT_decl_line(0x29a)
	.dwattr $C$DW$848, DW_AT_decl_column(0x0b)

$C$DW$849	.dwtag  DW_TAG_member
	.dwattr $C$DW$849, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$849, DW_AT_name("CertIndex")
	.dwattr $C$DW$849, DW_AT_TI_symbol_name("CertIndex")
	.dwattr $C$DW$849, DW_AT_data_member_location[DW_OP_plus_uconst 0x82]
	.dwattr $C$DW$849, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$849, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$849, DW_AT_decl_line(0x29b)
	.dwattr $C$DW$849, DW_AT_decl_column(0x0b)

$C$DW$850	.dwtag  DW_TAG_member
	.dwattr $C$DW$850, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$850, DW_AT_name("EapMethod")
	.dwattr $C$DW$850, DW_AT_TI_symbol_name("EapMethod")
	.dwattr $C$DW$850, DW_AT_data_member_location[DW_OP_plus_uconst 0x84]
	.dwattr $C$DW$850, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$850, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$850, DW_AT_decl_line(0x29c)
	.dwattr $C$DW$850, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$134, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$134, DW_AT_decl_line(0x296)
	.dwattr $C$DW$T$134, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$134

$C$DW$T$502	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$502, DW_AT_name("SlWlanGetSecParamsExt_t")
	.dwattr $C$DW$T$502, DW_AT_type(*$C$DW$T$134)
	.dwattr $C$DW$T$502, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$502, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$502, DW_AT_decl_line(0x29d)
	.dwattr $C$DW$T$502, DW_AT_decl_column(0x02)


$C$DW$T$135	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$135, DW_AT_byte_size(0x08)
$C$DW$851	.dwtag  DW_TAG_member
	.dwattr $C$DW$851, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$851, DW_AT_name("ChannelsMask")
	.dwattr $C$DW$851, DW_AT_TI_symbol_name("ChannelsMask")
	.dwattr $C$DW$851, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$851, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$851, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$851, DW_AT_decl_line(0x2bc)
	.dwattr $C$DW$851, DW_AT_decl_column(0x0c)

$C$DW$852	.dwtag  DW_TAG_member
	.dwattr $C$DW$852, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$852, DW_AT_name("RssiThreshold")
	.dwattr $C$DW$852, DW_AT_TI_symbol_name("RssiThreshold")
	.dwattr $C$DW$852, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$852, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$852, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$852, DW_AT_decl_line(0x2bd)
	.dwattr $C$DW$852, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$135, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$135, DW_AT_decl_line(0x2bb)
	.dwattr $C$DW$T$135, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$135

$C$DW$T$503	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$503, DW_AT_name("SlWlanScanParamCommand_t")
	.dwattr $C$DW$T$503, DW_AT_type(*$C$DW$T$135)
	.dwattr $C$DW$T$503, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$503, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$503, DW_AT_decl_line(0x2be)
	.dwattr $C$DW$T$503, DW_AT_decl_column(0x02)


$C$DW$T$137	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$137, DW_AT_byte_size(0x102)
$C$DW$853	.dwtag  DW_TAG_member
	.dwattr $C$DW$853, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$853, DW_AT_name("Id")
	.dwattr $C$DW$853, DW_AT_TI_symbol_name("Id")
	.dwattr $C$DW$853, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$853, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$853, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$853, DW_AT_decl_line(0x2c2)
	.dwattr $C$DW$853, DW_AT_decl_column(0x0b)

$C$DW$854	.dwtag  DW_TAG_member
	.dwattr $C$DW$854, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$854, DW_AT_name("Oui")
	.dwattr $C$DW$854, DW_AT_TI_symbol_name("Oui")
	.dwattr $C$DW$854, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$854, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$854, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$854, DW_AT_decl_line(0x2c3)
	.dwattr $C$DW$854, DW_AT_decl_column(0x0b)

$C$DW$855	.dwtag  DW_TAG_member
	.dwattr $C$DW$855, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$855, DW_AT_name("Length")
	.dwattr $C$DW$855, DW_AT_TI_symbol_name("Length")
	.dwattr $C$DW$855, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$855, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$855, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$855, DW_AT_decl_line(0x2c4)
	.dwattr $C$DW$855, DW_AT_decl_column(0x0b)

$C$DW$856	.dwtag  DW_TAG_member
	.dwattr $C$DW$856, DW_AT_type(*$C$DW$T$136)
	.dwattr $C$DW$856, DW_AT_name("Data")
	.dwattr $C$DW$856, DW_AT_TI_symbol_name("Data")
	.dwattr $C$DW$856, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$856, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$856, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$856, DW_AT_decl_line(0x2c5)
	.dwattr $C$DW$856, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$137, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$137, DW_AT_decl_line(0x2c1)
	.dwattr $C$DW$T$137, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$137

$C$DW$T$138	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$138, DW_AT_name("SlWlanInfoElement_t")
	.dwattr $C$DW$T$138, DW_AT_type(*$C$DW$T$137)
	.dwattr $C$DW$T$138, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$138, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$138, DW_AT_decl_line(0x2c6)
	.dwattr $C$DW$T$138, DW_AT_decl_column(0x03)


$C$DW$T$139	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$139, DW_AT_byte_size(0x104)
$C$DW$857	.dwtag  DW_TAG_member
	.dwattr $C$DW$857, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$857, DW_AT_name("Index")
	.dwattr $C$DW$857, DW_AT_TI_symbol_name("Index")
	.dwattr $C$DW$857, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$857, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$857, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$857, DW_AT_decl_line(0x2ca)
	.dwattr $C$DW$857, DW_AT_decl_column(0x12)

$C$DW$858	.dwtag  DW_TAG_member
	.dwattr $C$DW$858, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$858, DW_AT_name("Role")
	.dwattr $C$DW$858, DW_AT_TI_symbol_name("Role")
	.dwattr $C$DW$858, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$858, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$858, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$858, DW_AT_decl_line(0x2cb)
	.dwattr $C$DW$858, DW_AT_decl_column(0x12)

$C$DW$859	.dwtag  DW_TAG_member
	.dwattr $C$DW$859, DW_AT_type(*$C$DW$T$138)
	.dwattr $C$DW$859, DW_AT_name("IE")
	.dwattr $C$DW$859, DW_AT_TI_symbol_name("IE")
	.dwattr $C$DW$859, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$859, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$859, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$859, DW_AT_decl_line(0x2cc)
	.dwattr $C$DW$859, DW_AT_decl_column(0x1b)

	.dwattr $C$DW$T$139, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$139, DW_AT_decl_line(0x2c9)
	.dwattr $C$DW$T$139, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$139

$C$DW$T$504	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$504, DW_AT_name("SlWlanSetInfoElement_t")
	.dwattr $C$DW$T$504, DW_AT_type(*$C$DW$T$139)
	.dwattr $C$DW$T$504, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$504, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$504, DW_AT_decl_line(0x2cd)
	.dwattr $C$DW$T$504, DW_AT_decl_column(0x03)


$C$DW$T$140	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$140, DW_AT_byte_size(0x08)
$C$DW$860	.dwtag  DW_TAG_member
	.dwattr $C$DW$860, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$860, DW_AT_name("Reserved")
	.dwattr $C$DW$860, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$860, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$860, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$860, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$860, DW_AT_decl_line(0x2d1)
	.dwattr $C$DW$860, DW_AT_decl_column(0x11)

$C$DW$861	.dwtag  DW_TAG_member
	.dwattr $C$DW$861, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$861, DW_AT_name("Reserved2")
	.dwattr $C$DW$861, DW_AT_TI_symbol_name("Reserved2")
	.dwattr $C$DW$861, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$861, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$861, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$861, DW_AT_decl_line(0x2d2)
	.dwattr $C$DW$861, DW_AT_decl_column(0x11)

$C$DW$862	.dwtag  DW_TAG_member
	.dwattr $C$DW$862, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$862, DW_AT_name("MaxSleepTimeMs")
	.dwattr $C$DW$862, DW_AT_TI_symbol_name("MaxSleepTimeMs")
	.dwattr $C$DW$862, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$862, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$862, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$862, DW_AT_decl_line(0x2d3)
	.dwattr $C$DW$862, DW_AT_decl_column(0x11)

$C$DW$863	.dwtag  DW_TAG_member
	.dwattr $C$DW$863, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$863, DW_AT_name("Reserved3")
	.dwattr $C$DW$863, DW_AT_TI_symbol_name("Reserved3")
	.dwattr $C$DW$863, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$863, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$863, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$863, DW_AT_decl_line(0x2d4)
	.dwattr $C$DW$863, DW_AT_decl_column(0x11)

	.dwattr $C$DW$T$140, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$140, DW_AT_decl_line(0x2d0)
	.dwattr $C$DW$T$140, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$140

$C$DW$T$505	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$505, DW_AT_name("SlWlanPmPolicyParams_t")
	.dwattr $C$DW$T$505, DW_AT_type(*$C$DW$T$140)
	.dwattr $C$DW$T$505, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$505, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$505, DW_AT_decl_line(0x2d5)
	.dwattr $C$DW$T$505, DW_AT_decl_column(0x03)


$C$DW$T$142	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$142, DW_AT_byte_size(0x14)
$C$DW$864	.dwtag  DW_TAG_member
	.dwattr $C$DW$864, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$864, DW_AT_name("Offset")
	.dwattr $C$DW$864, DW_AT_TI_symbol_name("Offset")
	.dwattr $C$DW$864, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$864, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$864, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$864, DW_AT_decl_line(0x2ed)
	.dwattr $C$DW$864, DW_AT_decl_column(0x0d)

$C$DW$865	.dwtag  DW_TAG_member
	.dwattr $C$DW$865, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$865, DW_AT_name("Length")
	.dwattr $C$DW$865, DW_AT_TI_symbol_name("Length")
	.dwattr $C$DW$865, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$865, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$865, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$865, DW_AT_decl_line(0x2ee)
	.dwattr $C$DW$865, DW_AT_decl_column(0x0d)

$C$DW$866	.dwtag  DW_TAG_member
	.dwattr $C$DW$866, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$866, DW_AT_name("Reserved")
	.dwattr $C$DW$866, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$866, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$866, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$866, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$866, DW_AT_decl_line(0x2ef)
	.dwattr $C$DW$866, DW_AT_decl_column(0x0d)

$C$DW$867	.dwtag  DW_TAG_member
	.dwattr $C$DW$867, DW_AT_type(*$C$DW$T$141)
	.dwattr $C$DW$867, DW_AT_name("Value")
	.dwattr $C$DW$867, DW_AT_TI_symbol_name("Value")
	.dwattr $C$DW$867, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$867, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$867, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$867, DW_AT_decl_line(0x2f0)
	.dwattr $C$DW$867, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$142, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$142, DW_AT_decl_line(0x2ec)
	.dwattr $C$DW$T$142, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$142

$C$DW$T$238	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$238, DW_AT_name("SlWlanRxFilterPatternArg_t")
	.dwattr $C$DW$T$238, DW_AT_type(*$C$DW$T$142)
	.dwattr $C$DW$T$238, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$238, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$238, DW_AT_decl_line(0x2f1)
	.dwattr $C$DW$T$238, DW_AT_decl_column(0x02)


$C$DW$T$144	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$144, DW_AT_byte_size(0x34)
$C$DW$868	.dwtag  DW_TAG_member
	.dwattr $C$DW$868, DW_AT_type(*$C$DW$T$143)
	.dwattr $C$DW$868, DW_AT_name("Value")
	.dwattr $C$DW$868, DW_AT_TI_symbol_name("Value")
	.dwattr $C$DW$868, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$868, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$868, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$868, DW_AT_decl_line(0x35f)
	.dwattr $C$DW$868, DW_AT_decl_column(0x31)

$C$DW$869	.dwtag  DW_TAG_member
	.dwattr $C$DW$869, DW_AT_type(*$C$DW$T$141)
	.dwattr $C$DW$869, DW_AT_name("Mask")
	.dwattr $C$DW$869, DW_AT_TI_symbol_name("Mask")
	.dwattr $C$DW$869, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$869, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$869, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$869, DW_AT_decl_line(0x360)
	.dwattr $C$DW$869, DW_AT_decl_column(0x31)

	.dwattr $C$DW$T$144, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$144, DW_AT_decl_line(0x35e)
	.dwattr $C$DW$T$144, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$144

$C$DW$T$145	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$145, DW_AT_name("SlWlanRxFilterRuleHeaderArgs_t")
	.dwattr $C$DW$T$145, DW_AT_type(*$C$DW$T$144)
	.dwattr $C$DW$T$145, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$145, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$145, DW_AT_decl_line(0x362)
	.dwattr $C$DW$T$145, DW_AT_decl_column(0x02)


$C$DW$T$148	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$148, DW_AT_byte_size(0x38)
$C$DW$870	.dwtag  DW_TAG_member
	.dwattr $C$DW$870, DW_AT_type(*$C$DW$T$145)
	.dwattr $C$DW$870, DW_AT_name("Args")
	.dwattr $C$DW$870, DW_AT_TI_symbol_name("Args")
	.dwattr $C$DW$870, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$870, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$870, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$870, DW_AT_decl_line(0x368)
	.dwattr $C$DW$870, DW_AT_decl_column(0x31)

$C$DW$871	.dwtag  DW_TAG_member
	.dwattr $C$DW$871, DW_AT_type(*$C$DW$T$146)
	.dwattr $C$DW$871, DW_AT_name("Field")
	.dwattr $C$DW$871, DW_AT_TI_symbol_name("Field")
	.dwattr $C$DW$871, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$871, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$871, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$871, DW_AT_decl_line(0x369)
	.dwattr $C$DW$871, DW_AT_decl_column(0x31)

$C$DW$872	.dwtag  DW_TAG_member
	.dwattr $C$DW$872, DW_AT_type(*$C$DW$T$147)
	.dwattr $C$DW$872, DW_AT_name("CompareFunc")
	.dwattr $C$DW$872, DW_AT_TI_symbol_name("CompareFunc")
	.dwattr $C$DW$872, DW_AT_data_member_location[DW_OP_plus_uconst 0x35]
	.dwattr $C$DW$872, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$872, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$872, DW_AT_decl_line(0x36a)
	.dwattr $C$DW$872, DW_AT_decl_column(0x31)

$C$DW$873	.dwtag  DW_TAG_member
	.dwattr $C$DW$873, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$873, DW_AT_name("Padding")
	.dwattr $C$DW$873, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$873, DW_AT_data_member_location[DW_OP_plus_uconst 0x36]
	.dwattr $C$DW$873, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$873, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$873, DW_AT_decl_line(0x36b)
	.dwattr $C$DW$873, DW_AT_decl_column(0x31)

	.dwattr $C$DW$T$148, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$148, DW_AT_decl_line(0x367)
	.dwattr $C$DW$T$148, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$148

$C$DW$T$248	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$248, DW_AT_name("SlWlanRxFilterRuleHeader_t")
	.dwattr $C$DW$T$248, DW_AT_type(*$C$DW$T$148)
	.dwattr $C$DW$T$248, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$248, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$248, DW_AT_decl_line(0x36c)
	.dwattr $C$DW$T$248, DW_AT_decl_column(0x02)


$C$DW$T$152	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$152, DW_AT_byte_size(0x04)
$C$DW$874	.dwtag  DW_TAG_member
	.dwattr $C$DW$874, DW_AT_type(*$C$DW$T$149)
	.dwattr $C$DW$874, DW_AT_name("Operator")
	.dwattr $C$DW$874, DW_AT_TI_symbol_name("Operator")
	.dwattr $C$DW$874, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$874, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$874, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$874, DW_AT_decl_line(0x37a)
	.dwattr $C$DW$874, DW_AT_decl_column(0x31)

$C$DW$875	.dwtag  DW_TAG_member
	.dwattr $C$DW$875, DW_AT_type(*$C$DW$T$151)
	.dwattr $C$DW$875, DW_AT_name("CombinationFilterId")
	.dwattr $C$DW$875, DW_AT_TI_symbol_name("CombinationFilterId")
	.dwattr $C$DW$875, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$875, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$875, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$875, DW_AT_decl_line(0x37b)
	.dwattr $C$DW$875, DW_AT_decl_column(0x31)

$C$DW$876	.dwtag  DW_TAG_member
	.dwattr $C$DW$876, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$876, DW_AT_name("Padding")
	.dwattr $C$DW$876, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$876, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$876, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$876, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$876, DW_AT_decl_line(0x37c)
	.dwattr $C$DW$876, DW_AT_decl_column(0x31)

	.dwattr $C$DW$T$152, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$152, DW_AT_decl_line(0x379)
	.dwattr $C$DW$T$152, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$152

$C$DW$T$249	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$249, DW_AT_name("SlWlanRxFilterRuleCombination_t")
	.dwattr $C$DW$T$249, DW_AT_type(*$C$DW$T$152)
	.dwattr $C$DW$T$249, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$249, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$249, DW_AT_decl_line(0x37d)
	.dwattr $C$DW$T$249, DW_AT_decl_column(0x02)


$C$DW$T$157	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$157, DW_AT_byte_size(0x0c)
$C$DW$877	.dwtag  DW_TAG_member
	.dwattr $C$DW$877, DW_AT_type(*$C$DW$T$150)
	.dwattr $C$DW$877, DW_AT_name("ParentFilterID")
	.dwattr $C$DW$877, DW_AT_TI_symbol_name("ParentFilterID")
	.dwattr $C$DW$877, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$877, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$877, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$877, DW_AT_decl_line(0x3af)
	.dwattr $C$DW$877, DW_AT_decl_column(0x2d)

$C$DW$878	.dwtag  DW_TAG_member
	.dwattr $C$DW$878, DW_AT_type(*$C$DW$T$153)
	.dwattr $C$DW$878, DW_AT_name("Counter")
	.dwattr $C$DW$878, DW_AT_TI_symbol_name("Counter")
	.dwattr $C$DW$878, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$878, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$878, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$878, DW_AT_decl_line(0x3b0)
	.dwattr $C$DW$878, DW_AT_decl_column(0x2d)

$C$DW$879	.dwtag  DW_TAG_member
	.dwattr $C$DW$879, DW_AT_type(*$C$DW$T$154)
	.dwattr $C$DW$879, DW_AT_name("ConnectionState")
	.dwattr $C$DW$879, DW_AT_TI_symbol_name("ConnectionState")
	.dwattr $C$DW$879, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$879, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$879, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$879, DW_AT_decl_line(0x3b1)
	.dwattr $C$DW$879, DW_AT_decl_column(0x2d)

$C$DW$880	.dwtag  DW_TAG_member
	.dwattr $C$DW$880, DW_AT_type(*$C$DW$T$155)
	.dwattr $C$DW$880, DW_AT_name("Role")
	.dwattr $C$DW$880, DW_AT_TI_symbol_name("Role")
	.dwattr $C$DW$880, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$880, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$880, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$880, DW_AT_decl_line(0x3b2)
	.dwattr $C$DW$880, DW_AT_decl_column(0x2d)

$C$DW$881	.dwtag  DW_TAG_member
	.dwattr $C$DW$881, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$881, DW_AT_name("CounterVal")
	.dwattr $C$DW$881, DW_AT_TI_symbol_name("CounterVal")
	.dwattr $C$DW$881, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$881, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$881, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$881, DW_AT_decl_line(0x3b3)
	.dwattr $C$DW$881, DW_AT_decl_column(0x2d)

$C$DW$882	.dwtag  DW_TAG_member
	.dwattr $C$DW$882, DW_AT_type(*$C$DW$T$156)
	.dwattr $C$DW$882, DW_AT_name("CompareFunction")
	.dwattr $C$DW$882, DW_AT_TI_symbol_name("CompareFunction")
	.dwattr $C$DW$882, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$882, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$882, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$882, DW_AT_decl_line(0x3b4)
	.dwattr $C$DW$882, DW_AT_decl_column(0x2d)

$C$DW$883	.dwtag  DW_TAG_member
	.dwattr $C$DW$883, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$883, DW_AT_name("Padding")
	.dwattr $C$DW$883, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$883, DW_AT_data_member_location[DW_OP_plus_uconst 0x9]
	.dwattr $C$DW$883, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$883, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$883, DW_AT_decl_line(0x3b5)
	.dwattr $C$DW$883, DW_AT_decl_column(0x2d)

	.dwattr $C$DW$T$157, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$157, DW_AT_decl_line(0x3ae)
	.dwattr $C$DW$T$157, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$157

$C$DW$T$506	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$506, DW_AT_name("SlWlanRxFilterTrigger_t")
	.dwattr $C$DW$T$506, DW_AT_type(*$C$DW$T$157)
	.dwattr $C$DW$T$506, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$506, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$506, DW_AT_decl_line(0x3b6)
	.dwattr $C$DW$T$506, DW_AT_decl_column(0x03)


$C$DW$T$159	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$159, DW_AT_byte_size(0x08)
$C$DW$884	.dwtag  DW_TAG_member
	.dwattr $C$DW$884, DW_AT_type(*$C$DW$T$158)
	.dwattr $C$DW$884, DW_AT_name("Type")
	.dwattr $C$DW$884, DW_AT_TI_symbol_name("Type")
	.dwattr $C$DW$884, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$884, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$884, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$884, DW_AT_decl_line(0x3c8)
	.dwattr $C$DW$884, DW_AT_decl_column(0x20)

$C$DW$885	.dwtag  DW_TAG_member
	.dwattr $C$DW$885, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$885, DW_AT_name("Counter")
	.dwattr $C$DW$885, DW_AT_TI_symbol_name("Counter")
	.dwattr $C$DW$885, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$885, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$885, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$885, DW_AT_decl_line(0x3c9)
	.dwattr $C$DW$885, DW_AT_decl_column(0x0c)

$C$DW$886	.dwtag  DW_TAG_member
	.dwattr $C$DW$886, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$886, DW_AT_name("Reserved")
	.dwattr $C$DW$886, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$886, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$886, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$886, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$886, DW_AT_decl_line(0x3ca)
	.dwattr $C$DW$886, DW_AT_decl_column(0x0c)

$C$DW$887	.dwtag  DW_TAG_member
	.dwattr $C$DW$887, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$887, DW_AT_name("UserId")
	.dwattr $C$DW$887, DW_AT_TI_symbol_name("UserId")
	.dwattr $C$DW$887, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$887, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$887, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$887, DW_AT_decl_line(0x3cb)
	.dwattr $C$DW$887, DW_AT_decl_column(0x0c)

$C$DW$888	.dwtag  DW_TAG_member
	.dwattr $C$DW$888, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$888, DW_AT_name("Padding")
	.dwattr $C$DW$888, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$888, DW_AT_data_member_location[DW_OP_plus_uconst 0x5]
	.dwattr $C$DW$888, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$888, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$888, DW_AT_decl_line(0x3cc)
	.dwattr $C$DW$888, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$159, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$159, DW_AT_decl_line(0x3c7)
	.dwattr $C$DW$T$159, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$159

$C$DW$T$507	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$507, DW_AT_name("SlWlanRxFilterAction_t")
	.dwattr $C$DW$T$507, DW_AT_type(*$C$DW$T$159)
	.dwattr $C$DW$T$507, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$507, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$507, DW_AT_decl_line(0x3ce)
	.dwattr $C$DW$T$507, DW_AT_decl_column(0x03)


$C$DW$T$161	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$161, DW_AT_byte_size(0x14)
$C$DW$889	.dwtag  DW_TAG_member
	.dwattr $C$DW$889, DW_AT_type(*$C$DW$T$160)
	.dwattr $C$DW$889, DW_AT_name("FilterBitmap")
	.dwattr $C$DW$889, DW_AT_TI_symbol_name("FilterBitmap")
	.dwattr $C$DW$889, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$889, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$889, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$889, DW_AT_decl_line(0x3d3)
	.dwattr $C$DW$889, DW_AT_decl_column(0x1c)

$C$DW$890	.dwtag  DW_TAG_member
	.dwattr $C$DW$890, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$890, DW_AT_name("Padding")
	.dwattr $C$DW$890, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$890, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$890, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$890, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$890, DW_AT_decl_line(0x3d4)
	.dwattr $C$DW$890, DW_AT_decl_column(0x09)

	.dwattr $C$DW$T$161, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$161, DW_AT_decl_line(0x3d2)
	.dwattr $C$DW$T$161, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$161

$C$DW$T$508	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$508, DW_AT_name("SlWlanRxFilterOperationCommandBuff_t")
	.dwattr $C$DW$T$508, DW_AT_type(*$C$DW$T$161)
	.dwattr $C$DW$T$508, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$508, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$508, DW_AT_decl_line(0x3d6)
	.dwattr $C$DW$T$508, DW_AT_decl_column(0x03)


$C$DW$T$162	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$162, DW_AT_byte_size(0x38)
$C$DW$891	.dwtag  DW_TAG_member
	.dwattr $C$DW$891, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$891, DW_AT_name("FilterId")
	.dwattr $C$DW$891, DW_AT_TI_symbol_name("FilterId")
	.dwattr $C$DW$891, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$891, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$891, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$891, DW_AT_decl_line(0x3db)
	.dwattr $C$DW$891, DW_AT_decl_column(0x0a)

$C$DW$892	.dwtag  DW_TAG_member
	.dwattr $C$DW$892, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$892, DW_AT_name("BinaryOrAscii")
	.dwattr $C$DW$892, DW_AT_TI_symbol_name("BinaryOrAscii")
	.dwattr $C$DW$892, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$892, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$892, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$892, DW_AT_decl_line(0x3dc)
	.dwattr $C$DW$892, DW_AT_decl_column(0x0a)

$C$DW$893	.dwtag  DW_TAG_member
	.dwattr $C$DW$893, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$893, DW_AT_name("Padding")
	.dwattr $C$DW$893, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$893, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$893, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$893, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$893, DW_AT_decl_line(0x3dd)
	.dwattr $C$DW$893, DW_AT_decl_column(0x09)

$C$DW$894	.dwtag  DW_TAG_member
	.dwattr $C$DW$894, DW_AT_type(*$C$DW$T$145)
	.dwattr $C$DW$894, DW_AT_name("Args")
	.dwattr $C$DW$894, DW_AT_TI_symbol_name("Args")
	.dwattr $C$DW$894, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$894, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$894, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$894, DW_AT_decl_line(0x3de)
	.dwattr $C$DW$894, DW_AT_decl_column(0x24)

	.dwattr $C$DW$T$162, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$162, DW_AT_decl_line(0x3da)
	.dwattr $C$DW$T$162, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$162

$C$DW$T$509	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$509, DW_AT_name("SlWlanRxFilterUpdateArgsCommandBuff_t")
	.dwattr $C$DW$T$509, DW_AT_type(*$C$DW$T$162)
	.dwattr $C$DW$T$509, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$509, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$509, DW_AT_decl_line(0x3e1)
	.dwattr $C$DW$T$509, DW_AT_decl_column(0x03)


$C$DW$T$163	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$163, DW_AT_byte_size(0x10)
$C$DW$895	.dwtag  DW_TAG_member
	.dwattr $C$DW$895, DW_AT_type(*$C$DW$T$160)
	.dwattr $C$DW$895, DW_AT_name("FilterIdMask")
	.dwattr $C$DW$895, DW_AT_TI_symbol_name("FilterIdMask")
	.dwattr $C$DW$895, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$895, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$895, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$895, DW_AT_decl_line(0x3e6)
	.dwattr $C$DW$895, DW_AT_decl_column(0x1c)

	.dwattr $C$DW$T$163, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$163, DW_AT_decl_line(0x3e5)
	.dwattr $C$DW$T$163, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$163

$C$DW$T$510	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$510, DW_AT_name("SlWlanRxFilterRetrieveStateBuff_t")
	.dwattr $C$DW$T$510, DW_AT_type(*$C$DW$T$163)
	.dwattr $C$DW$T$510, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$510, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$510, DW_AT_decl_line(0x3e8)
	.dwattr $C$DW$T$510, DW_AT_decl_column(0x02)


$C$DW$T$165	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$165, DW_AT_byte_size(0x04)
$C$DW$896	.dwtag  DW_TAG_member
	.dwattr $C$DW$896, DW_AT_type(*$C$DW$T$164)
	.dwattr $C$DW$896, DW_AT_name("FilterBitmap")
	.dwattr $C$DW$896, DW_AT_TI_symbol_name("FilterBitmap")
	.dwattr $C$DW$896, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$896, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$896, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$896, DW_AT_decl_line(0x3ed)
	.dwattr $C$DW$896, DW_AT_decl_column(0x25)

	.dwattr $C$DW$T$165, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$165, DW_AT_decl_line(0x3ec)
	.dwattr $C$DW$T$165, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$165

$C$DW$T$511	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$511, DW_AT_name("SlWlanRxFilterSysFiltersSetStateBuff_t")
	.dwattr $C$DW$T$511, DW_AT_type(*$C$DW$T$165)
	.dwattr $C$DW$T$511, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$511, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$511, DW_AT_decl_line(0x3ef)
	.dwattr $C$DW$T$511, DW_AT_decl_column(0x03)


$C$DW$T$166	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$166, DW_AT_byte_size(0x04)
$C$DW$897	.dwtag  DW_TAG_member
	.dwattr $C$DW$897, DW_AT_type(*$C$DW$T$164)
	.dwattr $C$DW$897, DW_AT_name("FilterBitmap")
	.dwattr $C$DW$897, DW_AT_TI_symbol_name("FilterBitmap")
	.dwattr $C$DW$897, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$897, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$897, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$897, DW_AT_decl_line(0x3f4)
	.dwattr $C$DW$897, DW_AT_decl_column(0x25)

	.dwattr $C$DW$T$166, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$166, DW_AT_decl_line(0x3f3)
	.dwattr $C$DW$T$166, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$166

$C$DW$T$512	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$512, DW_AT_name("SlWlanRxFilterSysFiltersRetrieveStateBuff_t")
	.dwattr $C$DW$T$512, DW_AT_type(*$C$DW$T$166)
	.dwattr $C$DW$T$512, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$512, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$512, DW_AT_decl_line(0x3f6)
	.dwattr $C$DW$T$512, DW_AT_decl_column(0x03)


$C$DW$T$167	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$167, DW_AT_byte_size(0x04)
$C$DW$898	.dwtag  DW_TAG_member
	.dwattr $C$DW$898, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$898, DW_AT_name("Status")
	.dwattr $C$DW$898, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$898, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$898, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$898, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$898, DW_AT_decl_line(0x59)
	.dwattr $C$DW$898, DW_AT_decl_column(0x0d)

$C$DW$899	.dwtag  DW_TAG_member
	.dwattr $C$DW$899, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$899, DW_AT_name("Caller")
	.dwattr $C$DW$899, DW_AT_TI_symbol_name("Caller")
	.dwattr $C$DW$899, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$899, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$899, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$899, DW_AT_decl_line(0x5a)
	.dwattr $C$DW$899, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$167, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$167, DW_AT_decl_line(0x58)
	.dwattr $C$DW$T$167, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$167

$C$DW$T$251	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$251, DW_AT_name("SlDeviceEventResetRequest_t")
	.dwattr $C$DW$T$251, DW_AT_type(*$C$DW$T$167)
	.dwattr $C$DW$T$251, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$251, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$251, DW_AT_decl_line(0x5b)
	.dwattr $C$DW$T$251, DW_AT_decl_column(0x02)


$C$DW$T$170	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$170, DW_AT_byte_size(0x04)
$C$DW$900	.dwtag  DW_TAG_member
	.dwattr $C$DW$900, DW_AT_type(*$C$DW$T$169)
	.dwattr $C$DW$900, DW_AT_name("Source")
	.dwattr $C$DW$900, DW_AT_TI_symbol_name("Source")
	.dwattr $C$DW$900, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$900, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$900, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$900, DW_AT_decl_line(0x6a)
	.dwattr $C$DW$900, DW_AT_decl_column(0x19)

$C$DW$901	.dwtag  DW_TAG_member
	.dwattr $C$DW$901, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$901, DW_AT_name("Code")
	.dwattr $C$DW$901, DW_AT_TI_symbol_name("Code")
	.dwattr $C$DW$901, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$901, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$901, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$901, DW_AT_decl_line(0x6b)
	.dwattr $C$DW$901, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$170, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$170, DW_AT_decl_line(0x69)
	.dwattr $C$DW$T$170, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$170

$C$DW$T$252	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$252, DW_AT_name("SlDeviceEventError_t")
	.dwattr $C$DW$T$252, DW_AT_type(*$C$DW$T$170)
	.dwattr $C$DW$T$252, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$252, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$252, DW_AT_decl_line(0x6c)
	.dwattr $C$DW$T$252, DW_AT_decl_column(0x02)


$C$DW$T$172	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$172, DW_AT_byte_size(0x08)
$C$DW$902	.dwtag  DW_TAG_member
	.dwattr $C$DW$902, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$902, DW_AT_name("Id")
	.dwattr $C$DW$902, DW_AT_TI_symbol_name("Id")
	.dwattr $C$DW$902, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$902, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$902, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$902, DW_AT_decl_line(0x7f)
	.dwattr $C$DW$902, DW_AT_decl_column(0x19)

$C$DW$903	.dwtag  DW_TAG_member
	.dwattr $C$DW$903, DW_AT_type(*$C$DW$T$171)
	.dwattr $C$DW$903, DW_AT_name("Data")
	.dwattr $C$DW$903, DW_AT_TI_symbol_name("Data")
	.dwattr $C$DW$903, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$903, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$903, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$903, DW_AT_decl_line(0x80)
	.dwattr $C$DW$903, DW_AT_decl_column(0x1c)

	.dwattr $C$DW$T$172, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$172, DW_AT_decl_line(0x7e)
	.dwattr $C$DW$T$172, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$172

$C$DW$T$513	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$513, DW_AT_name("SlDeviceEvent_t")
	.dwattr $C$DW$T$513, DW_AT_type(*$C$DW$T$172)
	.dwattr $C$DW$T$513, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$513, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$513, DW_AT_decl_line(0x81)
	.dwattr $C$DW$T$513, DW_AT_decl_column(0x02)


$C$DW$T$173	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$173, DW_AT_byte_size(0x08)
$C$DW$904	.dwtag  DW_TAG_member
	.dwattr $C$DW$904, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$904, DW_AT_name("Code")
	.dwattr $C$DW$904, DW_AT_TI_symbol_name("Code")
	.dwattr $C$DW$904, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$904, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$904, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$904, DW_AT_decl_line(0x8d)
	.dwattr $C$DW$904, DW_AT_decl_column(0x19)

$C$DW$905	.dwtag  DW_TAG_member
	.dwattr $C$DW$905, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$905, DW_AT_name("Value")
	.dwattr $C$DW$905, DW_AT_TI_symbol_name("Value")
	.dwattr $C$DW$905, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$905, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$905, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$905, DW_AT_decl_line(0x8e)
	.dwattr $C$DW$905, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$173, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$173, DW_AT_decl_line(0x8c)
	.dwattr $C$DW$T$173, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$173

$C$DW$T$254	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$254, DW_AT_name("SlDeviceFatalDeviceAssert_t")
	.dwattr $C$DW$T$254, DW_AT_type(*$C$DW$T$173)
	.dwattr $C$DW$T$254, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$254, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$254, DW_AT_decl_line(0x8f)
	.dwattr $C$DW$T$254, DW_AT_decl_column(0x03)


$C$DW$T$174	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$174, DW_AT_byte_size(0x04)
$C$DW$906	.dwtag  DW_TAG_member
	.dwattr $C$DW$906, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$906, DW_AT_name("Code")
	.dwattr $C$DW$906, DW_AT_TI_symbol_name("Code")
	.dwattr $C$DW$906, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$906, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$906, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$906, DW_AT_decl_line(0x94)
	.dwattr $C$DW$906, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$174, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$174, DW_AT_decl_line(0x93)
	.dwattr $C$DW$T$174, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$174

$C$DW$T$256	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$256, DW_AT_name("SlDeviceFatalCmdTimeout_t")
	.dwattr $C$DW$T$256, DW_AT_type(*$C$DW$T$174)
	.dwattr $C$DW$T$256, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$256, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$256, DW_AT_decl_line(0x95)
	.dwattr $C$DW$T$256, DW_AT_decl_column(0x1c)

$C$DW$T$255	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$255, DW_AT_name("SlDeviceFatalNoCmdAck_t")
	.dwattr $C$DW$T$255, DW_AT_type(*$C$DW$T$174)
	.dwattr $C$DW$T$255, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$255, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$255, DW_AT_decl_line(0x95)
	.dwattr $C$DW$T$255, DW_AT_decl_column(0x03)


$C$DW$T$176	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$176, DW_AT_byte_size(0x0c)
$C$DW$907	.dwtag  DW_TAG_member
	.dwattr $C$DW$907, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$907, DW_AT_name("Id")
	.dwattr $C$DW$907, DW_AT_TI_symbol_name("Id")
	.dwattr $C$DW$907, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$907, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$907, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$907, DW_AT_decl_line(0xa2)
	.dwattr $C$DW$907, DW_AT_decl_column(0x1d)

$C$DW$908	.dwtag  DW_TAG_member
	.dwattr $C$DW$908, DW_AT_type(*$C$DW$T$175)
	.dwattr $C$DW$908, DW_AT_name("Data")
	.dwattr $C$DW$908, DW_AT_TI_symbol_name("Data")
	.dwattr $C$DW$908, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$908, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$908, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$908, DW_AT_decl_line(0xa3)
	.dwattr $C$DW$908, DW_AT_decl_column(0x20)

	.dwattr $C$DW$T$176, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$176, DW_AT_decl_line(0xa1)
	.dwattr $C$DW$T$176, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$176

$C$DW$T$514	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$514, DW_AT_name("SlDeviceFatal_t")
	.dwattr $C$DW$T$514, DW_AT_type(*$C$DW$T$176)
	.dwattr $C$DW$T$514, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$514, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$514, DW_AT_decl_line(0xa4)
	.dwattr $C$DW$T$514, DW_AT_decl_column(0x02)


$C$DW$T$177	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$177, DW_AT_byte_size(0x14)
$C$DW$909	.dwtag  DW_TAG_member
	.dwattr $C$DW$909, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$909, DW_AT_name("ChipId")
	.dwattr $C$DW$909, DW_AT_TI_symbol_name("ChipId")
	.dwattr $C$DW$909, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$909, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$909, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$909, DW_AT_decl_line(0x102)
	.dwattr $C$DW$909, DW_AT_decl_column(0x19)

$C$DW$910	.dwtag  DW_TAG_member
	.dwattr $C$DW$910, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$910, DW_AT_name("FwVersion")
	.dwattr $C$DW$910, DW_AT_TI_symbol_name("FwVersion")
	.dwattr $C$DW$910, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$910, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$910, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$910, DW_AT_decl_line(0x103)
	.dwattr $C$DW$910, DW_AT_decl_column(0x19)

$C$DW$911	.dwtag  DW_TAG_member
	.dwattr $C$DW$911, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$911, DW_AT_name("PhyVersion")
	.dwattr $C$DW$911, DW_AT_TI_symbol_name("PhyVersion")
	.dwattr $C$DW$911, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$911, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$911, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$911, DW_AT_decl_line(0x104)
	.dwattr $C$DW$911, DW_AT_decl_column(0x19)

$C$DW$912	.dwtag  DW_TAG_member
	.dwattr $C$DW$912, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$912, DW_AT_name("NwpVersion")
	.dwattr $C$DW$912, DW_AT_TI_symbol_name("NwpVersion")
	.dwattr $C$DW$912, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$912, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$912, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$912, DW_AT_decl_line(0x105)
	.dwattr $C$DW$912, DW_AT_decl_column(0x19)

$C$DW$913	.dwtag  DW_TAG_member
	.dwattr $C$DW$913, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$913, DW_AT_name("RomVersion")
	.dwattr $C$DW$913, DW_AT_TI_symbol_name("RomVersion")
	.dwattr $C$DW$913, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$913, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$913, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$913, DW_AT_decl_line(0x106)
	.dwattr $C$DW$913, DW_AT_decl_column(0x19)

$C$DW$914	.dwtag  DW_TAG_member
	.dwattr $C$DW$914, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$914, DW_AT_name("Padding")
	.dwattr $C$DW$914, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$914, DW_AT_data_member_location[DW_OP_plus_uconst 0x12]
	.dwattr $C$DW$914, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$914, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$914, DW_AT_decl_line(0x107)
	.dwattr $C$DW$914, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$177, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$177, DW_AT_decl_line(0x101)
	.dwattr $C$DW$T$177, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$177

$C$DW$T$515	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$515, DW_AT_name("SlDeviceVersion_t")
	.dwattr $C$DW$T$515, DW_AT_type(*$C$DW$T$177)
	.dwattr $C$DW$T$515, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$515, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$515, DW_AT_decl_line(0x108)
	.dwattr $C$DW$T$515, DW_AT_decl_column(0x02)


$C$DW$T$179	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$179, DW_AT_byte_size(0x2c)
$C$DW$915	.dwtag  DW_TAG_member
	.dwattr $C$DW$915, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$915, DW_AT_name("tm_sec")
	.dwattr $C$DW$915, DW_AT_TI_symbol_name("tm_sec")
	.dwattr $C$DW$915, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$915, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$915, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$915, DW_AT_decl_line(0x10e)
	.dwattr $C$DW$915, DW_AT_decl_column(0x19)

$C$DW$916	.dwtag  DW_TAG_member
	.dwattr $C$DW$916, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$916, DW_AT_name("tm_min")
	.dwattr $C$DW$916, DW_AT_TI_symbol_name("tm_min")
	.dwattr $C$DW$916, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$916, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$916, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$916, DW_AT_decl_line(0x10f)
	.dwattr $C$DW$916, DW_AT_decl_column(0x19)

$C$DW$917	.dwtag  DW_TAG_member
	.dwattr $C$DW$917, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$917, DW_AT_name("tm_hour")
	.dwattr $C$DW$917, DW_AT_TI_symbol_name("tm_hour")
	.dwattr $C$DW$917, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$917, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$917, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$917, DW_AT_decl_line(0x110)
	.dwattr $C$DW$917, DW_AT_decl_column(0x19)

$C$DW$918	.dwtag  DW_TAG_member
	.dwattr $C$DW$918, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$918, DW_AT_name("tm_day")
	.dwattr $C$DW$918, DW_AT_TI_symbol_name("tm_day")
	.dwattr $C$DW$918, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$918, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$918, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$918, DW_AT_decl_line(0x112)
	.dwattr $C$DW$918, DW_AT_decl_column(0x19)

$C$DW$919	.dwtag  DW_TAG_member
	.dwattr $C$DW$919, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$919, DW_AT_name("tm_mon")
	.dwattr $C$DW$919, DW_AT_TI_symbol_name("tm_mon")
	.dwattr $C$DW$919, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$919, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$919, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$919, DW_AT_decl_line(0x113)
	.dwattr $C$DW$919, DW_AT_decl_column(0x19)

$C$DW$920	.dwtag  DW_TAG_member
	.dwattr $C$DW$920, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$920, DW_AT_name("tm_year")
	.dwattr $C$DW$920, DW_AT_TI_symbol_name("tm_year")
	.dwattr $C$DW$920, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$920, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$920, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$920, DW_AT_decl_line(0x114)
	.dwattr $C$DW$920, DW_AT_decl_column(0x19)

$C$DW$921	.dwtag  DW_TAG_member
	.dwattr $C$DW$921, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$921, DW_AT_name("tm_week_day")
	.dwattr $C$DW$921, DW_AT_TI_symbol_name("tm_week_day")
	.dwattr $C$DW$921, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$921, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$921, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$921, DW_AT_decl_line(0x115)
	.dwattr $C$DW$921, DW_AT_decl_column(0x19)

$C$DW$922	.dwtag  DW_TAG_member
	.dwattr $C$DW$922, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$922, DW_AT_name("tm_year_day")
	.dwattr $C$DW$922, DW_AT_TI_symbol_name("tm_year_day")
	.dwattr $C$DW$922, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$922, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$922, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$922, DW_AT_decl_line(0x116)
	.dwattr $C$DW$922, DW_AT_decl_column(0x19)

$C$DW$923	.dwtag  DW_TAG_member
	.dwattr $C$DW$923, DW_AT_type(*$C$DW$T$178)
	.dwattr $C$DW$923, DW_AT_name("reserved")
	.dwattr $C$DW$923, DW_AT_TI_symbol_name("reserved")
	.dwattr $C$DW$923, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$923, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$923, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$923, DW_AT_decl_line(0x117)
	.dwattr $C$DW$923, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$179, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$179, DW_AT_decl_line(0x10c)
	.dwattr $C$DW$T$179, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$179

$C$DW$T$516	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$516, DW_AT_name("SlDateTime_t")
	.dwattr $C$DW$T$516, DW_AT_type(*$C$DW$T$179)
	.dwattr $C$DW$T$516, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$516, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$516, DW_AT_decl_line(0x118)
	.dwattr $C$DW$T$516, DW_AT_decl_column(0x02)


$C$DW$T$180	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$180, DW_AT_byte_size(0x08)
$C$DW$924	.dwtag  DW_TAG_member
	.dwattr $C$DW$924, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$924, DW_AT_name("ChipId")
	.dwattr $C$DW$924, DW_AT_TI_symbol_name("ChipId")
	.dwattr $C$DW$924, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$924, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$924, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$924, DW_AT_decl_line(0x120)
	.dwattr $C$DW$924, DW_AT_decl_column(0x0d)

$C$DW$925	.dwtag  DW_TAG_member
	.dwattr $C$DW$925, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$925, DW_AT_name("MoreData")
	.dwattr $C$DW$925, DW_AT_TI_symbol_name("MoreData")
	.dwattr $C$DW$925, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$925, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$925, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$925, DW_AT_decl_line(0x121)
	.dwattr $C$DW$925, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$180, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$180, DW_AT_decl_line(0x11f)
	.dwattr $C$DW$T$180, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$180

$C$DW$T$517	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$517, DW_AT_name("SlDeviceInitInfo_t")
	.dwattr $C$DW$T$517, DW_AT_type(*$C$DW$T$180)
	.dwattr $C$DW$T$517, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$517, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$517, DW_AT_decl_line(0x122)
	.dwattr $C$DW$T$517, DW_AT_decl_column(0x02)

$C$DW$T$518	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$518, DW_AT_type(*$C$DW$T$517)
	.dwattr $C$DW$T$518, DW_AT_address_class(0x20)


$C$DW$T$181	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$181, DW_AT_byte_size(0x04)
$C$DW$926	.dwtag  DW_TAG_member
	.dwattr $C$DW$926, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$926, DW_AT_name("DnsSecondServerAddr")
	.dwattr $C$DW$926, DW_AT_TI_symbol_name("DnsSecondServerAddr")
	.dwattr $C$DW$926, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$926, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$926, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$926, DW_AT_decl_line(0x80)
	.dwattr $C$DW$926, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$181, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$181, DW_AT_decl_line(0x7f)
	.dwattr $C$DW$T$181, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$181

$C$DW$T$519	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$519, DW_AT_name("SlNetCfgIpV4DnsClientArgs_t")
	.dwattr $C$DW$T$519, DW_AT_type(*$C$DW$T$181)
	.dwattr $C$DW$T$519, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$519, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$519, DW_AT_decl_line(0x81)
	.dwattr $C$DW$T$519, DW_AT_decl_column(0x02)


$C$DW$T$183	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$183, DW_AT_byte_size(0x24)
$C$DW$927	.dwtag  DW_TAG_member
	.dwattr $C$DW$927, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$927, DW_AT_name("Ip")
	.dwattr $C$DW$927, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$927, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$927, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$927, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$927, DW_AT_decl_line(0x86)
	.dwattr $C$DW$927, DW_AT_decl_column(0x0b)

$C$DW$928	.dwtag  DW_TAG_member
	.dwattr $C$DW$928, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$928, DW_AT_name("Gateway")
	.dwattr $C$DW$928, DW_AT_TI_symbol_name("Gateway")
	.dwattr $C$DW$928, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$928, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$928, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$928, DW_AT_decl_line(0x87)
	.dwattr $C$DW$928, DW_AT_decl_column(0x0b)

$C$DW$929	.dwtag  DW_TAG_member
	.dwattr $C$DW$929, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$929, DW_AT_name("Mask")
	.dwattr $C$DW$929, DW_AT_TI_symbol_name("Mask")
	.dwattr $C$DW$929, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$929, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$929, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$929, DW_AT_decl_line(0x88)
	.dwattr $C$DW$929, DW_AT_decl_column(0x0b)

$C$DW$930	.dwtag  DW_TAG_member
	.dwattr $C$DW$930, DW_AT_type(*$C$DW$T$182)
	.dwattr $C$DW$930, DW_AT_name("Dns")
	.dwattr $C$DW$930, DW_AT_TI_symbol_name("Dns")
	.dwattr $C$DW$930, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$930, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$930, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$930, DW_AT_decl_line(0x89)
	.dwattr $C$DW$930, DW_AT_decl_column(0x0b)

$C$DW$931	.dwtag  DW_TAG_member
	.dwattr $C$DW$931, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$931, DW_AT_name("DhcpServer")
	.dwattr $C$DW$931, DW_AT_TI_symbol_name("DhcpServer")
	.dwattr $C$DW$931, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$931, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$931, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$931, DW_AT_decl_line(0x8a)
	.dwattr $C$DW$931, DW_AT_decl_column(0x0b)

$C$DW$932	.dwtag  DW_TAG_member
	.dwattr $C$DW$932, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$932, DW_AT_name("LeaseTime")
	.dwattr $C$DW$932, DW_AT_TI_symbol_name("LeaseTime")
	.dwattr $C$DW$932, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$932, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$932, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$932, DW_AT_decl_line(0x8b)
	.dwattr $C$DW$932, DW_AT_decl_column(0x0b)

$C$DW$933	.dwtag  DW_TAG_member
	.dwattr $C$DW$933, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$933, DW_AT_name("TimeToRenew")
	.dwattr $C$DW$933, DW_AT_TI_symbol_name("TimeToRenew")
	.dwattr $C$DW$933, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$933, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$933, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$933, DW_AT_decl_line(0x8c)
	.dwattr $C$DW$933, DW_AT_decl_column(0x0b)

$C$DW$934	.dwtag  DW_TAG_member
	.dwattr $C$DW$934, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$934, DW_AT_name("DhcpState")
	.dwattr $C$DW$934, DW_AT_TI_symbol_name("DhcpState")
	.dwattr $C$DW$934, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$934, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$934, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$934, DW_AT_decl_line(0x8d)
	.dwattr $C$DW$934, DW_AT_decl_column(0x0b)

$C$DW$935	.dwtag  DW_TAG_member
	.dwattr $C$DW$935, DW_AT_type(*$C$DW$T$22)
	.dwattr $C$DW$935, DW_AT_name("Reserved")
	.dwattr $C$DW$935, DW_AT_TI_symbol_name("Reserved")
	.dwattr $C$DW$935, DW_AT_data_member_location[DW_OP_plus_uconst 0x21]
	.dwattr $C$DW$935, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$935, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$935, DW_AT_decl_line(0x8e)
	.dwattr $C$DW$935, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$183, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$183, DW_AT_decl_line(0x85)
	.dwattr $C$DW$T$183, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$183

$C$DW$T$520	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$520, DW_AT_name("SlNetCfgIpv4DhcpClient_t")
	.dwattr $C$DW$T$520, DW_AT_type(*$C$DW$T$183)
	.dwattr $C$DW$T$520, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$520, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$520, DW_AT_decl_line(0x8f)
	.dwattr $C$DW$T$520, DW_AT_decl_column(0x03)


$C$DW$T$184	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$184, DW_AT_byte_size(0x10)
$C$DW$936	.dwtag  DW_TAG_member
	.dwattr $C$DW$936, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$936, DW_AT_name("Ip")
	.dwattr $C$DW$936, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$936, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$936, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$936, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$936, DW_AT_decl_line(0xa5)
	.dwattr $C$DW$936, DW_AT_decl_column(0x0b)

$C$DW$937	.dwtag  DW_TAG_member
	.dwattr $C$DW$937, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$937, DW_AT_name("IpMask")
	.dwattr $C$DW$937, DW_AT_TI_symbol_name("IpMask")
	.dwattr $C$DW$937, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$937, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$937, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$937, DW_AT_decl_line(0xa6)
	.dwattr $C$DW$937, DW_AT_decl_column(0x0b)

$C$DW$938	.dwtag  DW_TAG_member
	.dwattr $C$DW$938, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$938, DW_AT_name("IpGateway")
	.dwattr $C$DW$938, DW_AT_TI_symbol_name("IpGateway")
	.dwattr $C$DW$938, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$938, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$938, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$938, DW_AT_decl_line(0xa7)
	.dwattr $C$DW$938, DW_AT_decl_column(0x0b)

$C$DW$939	.dwtag  DW_TAG_member
	.dwattr $C$DW$939, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$939, DW_AT_name("IpDnsServer")
	.dwattr $C$DW$939, DW_AT_TI_symbol_name("IpDnsServer")
	.dwattr $C$DW$939, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$939, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$939, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$939, DW_AT_decl_line(0xa8)
	.dwattr $C$DW$939, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$184, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$184, DW_AT_decl_line(0xa4)
	.dwattr $C$DW$T$184, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$184

$C$DW$T$521	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$521, DW_AT_name("SlNetCfgIpV4Args_t")
	.dwattr $C$DW$T$521, DW_AT_type(*$C$DW$T$184)
	.dwattr $C$DW$T$521, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$521, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$521, DW_AT_decl_line(0xa9)
	.dwattr $C$DW$T$521, DW_AT_decl_column(0x02)


$C$DW$T$185	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$185, DW_AT_byte_size(0x24)
$C$DW$940	.dwtag  DW_TAG_member
	.dwattr $C$DW$940, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$940, DW_AT_name("Ip")
	.dwattr $C$DW$940, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$940, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$940, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$940, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$940, DW_AT_decl_line(0xad)
	.dwattr $C$DW$940, DW_AT_decl_column(0x0b)

$C$DW$941	.dwtag  DW_TAG_member
	.dwattr $C$DW$941, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$941, DW_AT_name("IpDnsServer")
	.dwattr $C$DW$941, DW_AT_TI_symbol_name("IpDnsServer")
	.dwattr $C$DW$941, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$941, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$941, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$941, DW_AT_decl_line(0xae)
	.dwattr $C$DW$941, DW_AT_decl_column(0x0b)

$C$DW$942	.dwtag  DW_TAG_member
	.dwattr $C$DW$942, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$942, DW_AT_name("IpV6Flags")
	.dwattr $C$DW$942, DW_AT_TI_symbol_name("IpV6Flags")
	.dwattr $C$DW$942, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$942, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$942, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$942, DW_AT_decl_line(0xaf)
	.dwattr $C$DW$942, DW_AT_decl_column(0x0b)

	.dwattr $C$DW$T$185, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$185, DW_AT_decl_line(0xac)
	.dwattr $C$DW$T$185, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$185

$C$DW$T$522	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$522, DW_AT_name("SlNetCfgIpV6Args_t")
	.dwattr $C$DW$T$522, DW_AT_type(*$C$DW$T$185)
	.dwattr $C$DW$T$522, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$522, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$522, DW_AT_decl_line(0xb0)
	.dwattr $C$DW$T$522, DW_AT_decl_column(0x02)


$C$DW$T$186	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$186, DW_AT_byte_size(0x2c)
$C$DW$943	.dwtag  DW_TAG_member
	.dwattr $C$DW$943, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$943, DW_AT_name("Ip")
	.dwattr $C$DW$943, DW_AT_TI_symbol_name("Ip")
	.dwattr $C$DW$943, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$943, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$943, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$943, DW_AT_decl_line(0xb9)
	.dwattr $C$DW$943, DW_AT_decl_column(0x0a)

$C$DW$944	.dwtag  DW_TAG_member
	.dwattr $C$DW$944, DW_AT_type(*$C$DW$T$20)
	.dwattr $C$DW$944, DW_AT_name("MacAddr")
	.dwattr $C$DW$944, DW_AT_TI_symbol_name("MacAddr")
	.dwattr $C$DW$944, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$944, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$944, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$944, DW_AT_decl_line(0xba)
	.dwattr $C$DW$944, DW_AT_decl_column(0x0a)

$C$DW$945	.dwtag  DW_TAG_member
	.dwattr $C$DW$945, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$945, DW_AT_name("Status")
	.dwattr $C$DW$945, DW_AT_TI_symbol_name("Status")
	.dwattr $C$DW$945, DW_AT_data_member_location[DW_OP_plus_uconst 0xa]
	.dwattr $C$DW$945, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$945, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$945, DW_AT_decl_line(0xbb)
	.dwattr $C$DW$945, DW_AT_decl_column(0x0a)

$C$DW$946	.dwtag  DW_TAG_member
	.dwattr $C$DW$946, DW_AT_type(*$C$DW$T$19)
	.dwattr $C$DW$946, DW_AT_name("Name")
	.dwattr $C$DW$946, DW_AT_TI_symbol_name("Name")
	.dwattr $C$DW$946, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$946, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$946, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$946, DW_AT_decl_line(0xbc)
	.dwattr $C$DW$946, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$186, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$186, DW_AT_decl_line(0xb8)
	.dwattr $C$DW$T$186, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$186

$C$DW$T$523	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$523, DW_AT_name("SlNetCfgStaInfo_t")
	.dwattr $C$DW$T$523, DW_AT_type(*$C$DW$T$186)
	.dwattr $C$DW$T$523, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$523, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netcfg.h")
	.dwattr $C$DW$T$523, DW_AT_decl_line(0xbd)
	.dwattr $C$DW$T$523, DW_AT_decl_column(0x03)


$C$DW$T$187	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$187, DW_AT_byte_size(0x04)
$C$DW$947	.dwtag  DW_TAG_member
	.dwattr $C$DW$947, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$947, DW_AT_name("Padding")
	.dwattr $C$DW$947, DW_AT_TI_symbol_name("Padding")
	.dwattr $C$DW$947, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$947, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$947, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$947, DW_AT_decl_line(0xac)
	.dwattr $C$DW$947, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$187, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$187, DW_AT_decl_line(0xab)
	.dwattr $C$DW$T$187, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$187

$C$DW$T$258	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$258, DW_AT_name("SlNetUtilCryptoEcCustomCurveParam_t")
	.dwattr $C$DW$T$258, DW_AT_type(*$C$DW$T$187)
	.dwattr $C$DW$T$258, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$258, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$258, DW_AT_decl_line(0xad)
	.dwattr $C$DW$T$258, DW_AT_decl_column(0x03)


$C$DW$T$189	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$189, DW_AT_byte_size(0x05)
$C$DW$948	.dwtag  DW_TAG_member
	.dwattr $C$DW$948, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$948, DW_AT_name("CurveType")
	.dwattr $C$DW$948, DW_AT_TI_symbol_name("CurveType")
	.dwattr $C$DW$948, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$948, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$948, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$948, DW_AT_decl_line(0xc5)
	.dwattr $C$DW$948, DW_AT_decl_column(0x25)

$C$DW$949	.dwtag  DW_TAG_member
	.dwattr $C$DW$949, DW_AT_type(*$C$DW$T$188)
	.dwattr $C$DW$949, DW_AT_name("CurveParams")
	.dwattr $C$DW$949, DW_AT_TI_symbol_name("CurveParams")
	.dwattr $C$DW$949, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$949, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$949, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$949, DW_AT_decl_line(0xc6)
	.dwattr $C$DW$949, DW_AT_decl_column(0x25)

	.dwattr $C$DW$T$189, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$189, DW_AT_decl_line(0xc4)
	.dwattr $C$DW$T$189, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$189

$C$DW$T$260	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$260, DW_AT_name("SlNetUtilCryptoEcKeyParams_t")
	.dwattr $C$DW$T$260, DW_AT_type(*$C$DW$T$189)
	.dwattr $C$DW$T$260, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$260, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$260, DW_AT_decl_line(0xc7)
	.dwattr $C$DW$T$260, DW_AT_decl_column(0x03)


$C$DW$T$191	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$191, DW_AT_byte_size(0x08)
$C$DW$950	.dwtag  DW_TAG_member
	.dwattr $C$DW$950, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$950, DW_AT_name("KeyAlgo")
	.dwattr $C$DW$950, DW_AT_TI_symbol_name("KeyAlgo")
	.dwattr $C$DW$950, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$950, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$950, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$950, DW_AT_decl_line(0xd5)
	.dwattr $C$DW$950, DW_AT_decl_column(0x24)

$C$DW$951	.dwtag  DW_TAG_member
	.dwattr $C$DW$951, DW_AT_type(*$C$DW$T$190)
	.dwattr $C$DW$951, DW_AT_name("KeyParams")
	.dwattr $C$DW$951, DW_AT_TI_symbol_name("KeyParams")
	.dwattr $C$DW$951, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$951, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$951, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$951, DW_AT_decl_line(0xd6)
	.dwattr $C$DW$951, DW_AT_decl_column(0x24)

$C$DW$952	.dwtag  DW_TAG_member
	.dwattr $C$DW$952, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$952, DW_AT_name("KeyFileNameLen")
	.dwattr $C$DW$952, DW_AT_TI_symbol_name("KeyFileNameLen")
	.dwattr $C$DW$952, DW_AT_data_member_location[DW_OP_plus_uconst 0x6]
	.dwattr $C$DW$952, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$952, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$952, DW_AT_decl_line(0xd7)
	.dwattr $C$DW$952, DW_AT_decl_column(0x24)

$C$DW$953	.dwtag  DW_TAG_member
	.dwattr $C$DW$953, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$953, DW_AT_name("CertFileNameLen")
	.dwattr $C$DW$953, DW_AT_TI_symbol_name("CertFileNameLen")
	.dwattr $C$DW$953, DW_AT_data_member_location[DW_OP_plus_uconst 0x7]
	.dwattr $C$DW$953, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$953, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$953, DW_AT_decl_line(0xd8)
	.dwattr $C$DW$953, DW_AT_decl_column(0x24)

	.dwattr $C$DW$T$191, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$191, DW_AT_decl_line(0xd4)
	.dwattr $C$DW$T$191, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$191

$C$DW$T$524	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$524, DW_AT_name("SlNetUtilCryptoPubKeyInfo_t")
	.dwattr $C$DW$T$524, DW_AT_type(*$C$DW$T$191)
	.dwattr $C$DW$T$524, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$524, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$524, DW_AT_decl_line(0xd9)
	.dwattr $C$DW$T$524, DW_AT_decl_column(0x02)


$C$DW$T$192	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$192, DW_AT_byte_size(0x0c)
$C$DW$954	.dwtag  DW_TAG_member
	.dwattr $C$DW$954, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$954, DW_AT_name("ObjId")
	.dwattr $C$DW$954, DW_AT_TI_symbol_name("ObjId")
	.dwattr $C$DW$954, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$954, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$954, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$954, DW_AT_decl_line(0xe1)
	.dwattr $C$DW$954, DW_AT_decl_column(0x10)

$C$DW$955	.dwtag  DW_TAG_member
	.dwattr $C$DW$955, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$955, DW_AT_name("SigType")
	.dwattr $C$DW$955, DW_AT_TI_symbol_name("SigType")
	.dwattr $C$DW$955, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$955, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$955, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$955, DW_AT_decl_line(0xe2)
	.dwattr $C$DW$955, DW_AT_decl_column(0x10)

$C$DW$956	.dwtag  DW_TAG_member
	.dwattr $C$DW$956, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$956, DW_AT_name("Flags")
	.dwattr $C$DW$956, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$956, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$956, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$956, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$956, DW_AT_decl_line(0xe3)
	.dwattr $C$DW$956, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$192, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$192, DW_AT_decl_line(0xe0)
	.dwattr $C$DW$T$192, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$192

$C$DW$T$525	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$525, DW_AT_name("SlNetUtilCryptoCmdSignAttrib_t")
	.dwattr $C$DW$T$525, DW_AT_type(*$C$DW$T$192)
	.dwattr $C$DW$T$525, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$525, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$525, DW_AT_decl_line(0xe4)
	.dwattr $C$DW$T$525, DW_AT_decl_column(0x03)


$C$DW$T$193	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$193, DW_AT_byte_size(0x10)
$C$DW$957	.dwtag  DW_TAG_member
	.dwattr $C$DW$957, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$957, DW_AT_name("ObjId")
	.dwattr $C$DW$957, DW_AT_TI_symbol_name("ObjId")
	.dwattr $C$DW$957, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$957, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$957, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$957, DW_AT_decl_line(0xea)
	.dwattr $C$DW$957, DW_AT_decl_column(0x10)

$C$DW$958	.dwtag  DW_TAG_member
	.dwattr $C$DW$958, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$958, DW_AT_name("SigType")
	.dwattr $C$DW$958, DW_AT_TI_symbol_name("SigType")
	.dwattr $C$DW$958, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$958, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$958, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$958, DW_AT_decl_line(0xeb)
	.dwattr $C$DW$958, DW_AT_decl_column(0x10)

$C$DW$959	.dwtag  DW_TAG_member
	.dwattr $C$DW$959, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$959, DW_AT_name("Flags")
	.dwattr $C$DW$959, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$959, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$959, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$959, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$959, DW_AT_decl_line(0xec)
	.dwattr $C$DW$959, DW_AT_decl_column(0x10)

$C$DW$960	.dwtag  DW_TAG_member
	.dwattr $C$DW$960, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$960, DW_AT_name("MsgLen")
	.dwattr $C$DW$960, DW_AT_TI_symbol_name("MsgLen")
	.dwattr $C$DW$960, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$960, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$960, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$960, DW_AT_decl_line(0xed)
	.dwattr $C$DW$960, DW_AT_decl_column(0x10)

$C$DW$961	.dwtag  DW_TAG_member
	.dwattr $C$DW$961, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$961, DW_AT_name("SigLen")
	.dwattr $C$DW$961, DW_AT_TI_symbol_name("SigLen")
	.dwattr $C$DW$961, DW_AT_data_member_location[DW_OP_plus_uconst 0xe]
	.dwattr $C$DW$961, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$961, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$961, DW_AT_decl_line(0xee)
	.dwattr $C$DW$961, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$193, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$193, DW_AT_decl_line(0xe9)
	.dwattr $C$DW$T$193, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$193

$C$DW$T$526	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$526, DW_AT_name("SlNetUtilCryptoCmdVerifyAttrib_t")
	.dwattr $C$DW$T$526, DW_AT_type(*$C$DW$T$193)
	.dwattr $C$DW$T$526, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$526, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$526, DW_AT_decl_line(0xef)
	.dwattr $C$DW$T$526, DW_AT_decl_column(0x03)


$C$DW$T$194	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$194, DW_AT_byte_size(0x0c)
$C$DW$962	.dwtag  DW_TAG_member
	.dwattr $C$DW$962, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$962, DW_AT_name("ObjId")
	.dwattr $C$DW$962, DW_AT_TI_symbol_name("ObjId")
	.dwattr $C$DW$962, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$962, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$962, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$962, DW_AT_decl_line(0xf4)
	.dwattr $C$DW$962, DW_AT_decl_column(0x10)

$C$DW$963	.dwtag  DW_TAG_member
	.dwattr $C$DW$963, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$963, DW_AT_name("Flags")
	.dwattr $C$DW$963, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$963, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$963, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$963, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$963, DW_AT_decl_line(0xf5)
	.dwattr $C$DW$963, DW_AT_decl_column(0x10)

$C$DW$964	.dwtag  DW_TAG_member
	.dwattr $C$DW$964, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$964, DW_AT_name("SubCmd")
	.dwattr $C$DW$964, DW_AT_TI_symbol_name("SubCmd")
	.dwattr $C$DW$964, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$964, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$964, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$964, DW_AT_decl_line(0xf6)
	.dwattr $C$DW$964, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$194, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$194, DW_AT_decl_line(0xf3)
	.dwattr $C$DW$T$194, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$194

$C$DW$T$527	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$527, DW_AT_name("SlNetUtilCryptoCmdCreateCertAttrib_t")
	.dwattr $C$DW$T$527, DW_AT_type(*$C$DW$T$194)
	.dwattr $C$DW$T$527, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$527, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$527, DW_AT_decl_line(0xf7)
	.dwattr $C$DW$T$527, DW_AT_decl_column(0x03)


$C$DW$T$195	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$195, DW_AT_byte_size(0x0c)
$C$DW$965	.dwtag  DW_TAG_member
	.dwattr $C$DW$965, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$965, DW_AT_name("ObjId")
	.dwattr $C$DW$965, DW_AT_TI_symbol_name("ObjId")
	.dwattr $C$DW$965, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$965, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$965, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$965, DW_AT_decl_line(0xfd)
	.dwattr $C$DW$965, DW_AT_decl_column(0x10)

$C$DW$966	.dwtag  DW_TAG_member
	.dwattr $C$DW$966, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$966, DW_AT_name("Flags")
	.dwattr $C$DW$966, DW_AT_TI_symbol_name("Flags")
	.dwattr $C$DW$966, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$966, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$966, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$966, DW_AT_decl_line(0xfe)
	.dwattr $C$DW$966, DW_AT_decl_column(0x10)

$C$DW$967	.dwtag  DW_TAG_member
	.dwattr $C$DW$967, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$967, DW_AT_name("SubCmd")
	.dwattr $C$DW$967, DW_AT_TI_symbol_name("SubCmd")
	.dwattr $C$DW$967, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$967, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$967, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$967, DW_AT_decl_line(0xff)
	.dwattr $C$DW$967, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$195, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$195, DW_AT_decl_line(0xfc)
	.dwattr $C$DW$T$195, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$195

$C$DW$T$528	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$528, DW_AT_name("SlNetUtilCryptoCmdKeyMgnt_t")
	.dwattr $C$DW$T$528, DW_AT_type(*$C$DW$T$195)
	.dwattr $C$DW$T$528, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$528, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$528, DW_AT_decl_line(0x100)
	.dwattr $C$DW$T$528, DW_AT_decl_column(0x03)


$C$DW$T$196	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$196, DW_AT_byte_size(0x04)
$C$DW$968	.dwtag  DW_TAG_member
	.dwattr $C$DW$968, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$968, DW_AT_name("NumOfRetries")
	.dwattr $C$DW$968, DW_AT_TI_symbol_name("NumOfRetries")
	.dwattr $C$DW$968, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$968, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$968, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$968, DW_AT_decl_line(0x105)
	.dwattr $C$DW$968, DW_AT_decl_column(0x0a)

$C$DW$969	.dwtag  DW_TAG_member
	.dwattr $C$DW$969, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$969, DW_AT_name("Timeout")
	.dwattr $C$DW$969, DW_AT_TI_symbol_name("Timeout")
	.dwattr $C$DW$969, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$969, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$969, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$969, DW_AT_decl_line(0x106)
	.dwattr $C$DW$969, DW_AT_decl_column(0x0a)

	.dwattr $C$DW$T$196, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$196, DW_AT_decl_line(0x104)
	.dwattr $C$DW$T$196, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$196

$C$DW$T$529	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$529, DW_AT_name("NetUtilCmdArpLookupAttrib_t")
	.dwattr $C$DW$T$529, DW_AT_type(*$C$DW$T$196)
	.dwattr $C$DW$T$529, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$529, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$529, DW_AT_decl_line(0x107)
	.dwattr $C$DW$T$529, DW_AT_decl_column(0x02)


$C$DW$T$199	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$199, DW_AT_byte_size(0x4c)
$C$DW$970	.dwtag  DW_TAG_member
	.dwattr $C$DW$970, DW_AT_type(*$C$DW$T$197)
	.dwattr $C$DW$970, DW_AT_name("StaConnect")
	.dwattr $C$DW$970, DW_AT_TI_symbol_name("StaConnect")
	.dwattr $C$DW$970, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$970, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$970, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$970, DW_AT_decl_line(0x2a4)
	.dwattr $C$DW$970, DW_AT_decl_column(0x20)

$C$DW$971	.dwtag  DW_TAG_member
	.dwattr $C$DW$971, DW_AT_type(*$C$DW$T$198)
	.dwattr $C$DW$971, DW_AT_name("P2PConnect")
	.dwattr $C$DW$971, DW_AT_TI_symbol_name("P2PConnect")
	.dwattr $C$DW$971, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$971, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$971, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$971, DW_AT_decl_line(0x2a5)
	.dwattr $C$DW$971, DW_AT_decl_column(0x20)

	.dwattr $C$DW$T$199, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$199, DW_AT_decl_line(0x2a3)
	.dwattr $C$DW$T$199, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$199

$C$DW$T$24	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$24, DW_AT_name("SlWlanConnectionInfo_u")
	.dwattr $C$DW$T$24, DW_AT_type(*$C$DW$T$199)
	.dwattr $C$DW$T$24, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$24, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$24, DW_AT_decl_line(0x2a6)
	.dwattr $C$DW$T$24, DW_AT_decl_column(0x03)


$C$DW$T$206	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$206, DW_AT_byte_size(0x10)
$C$DW$972	.dwtag  DW_TAG_member
	.dwattr $C$DW$972, DW_AT_type(*$C$DW$T$201)
	.dwattr $C$DW$972, DW_AT_name("_S6_u8")
	.dwattr $C$DW$972, DW_AT_TI_symbol_name("_S6_u8")
	.dwattr $C$DW$972, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$972, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$972, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$972, DW_AT_decl_line(0x1c8)
	.dwattr $C$DW$972, DW_AT_decl_column(0x12)

$C$DW$973	.dwtag  DW_TAG_member
	.dwattr $C$DW$973, DW_AT_type(*$C$DW$T$203)
	.dwattr $C$DW$973, DW_AT_name("_S6_u16")
	.dwattr $C$DW$973, DW_AT_TI_symbol_name("_S6_u16")
	.dwattr $C$DW$973, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$973, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$973, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$973, DW_AT_decl_line(0x1c9)
	.dwattr $C$DW$973, DW_AT_decl_column(0x12)

$C$DW$974	.dwtag  DW_TAG_member
	.dwattr $C$DW$974, DW_AT_type(*$C$DW$T$205)
	.dwattr $C$DW$974, DW_AT_name("_S6_u32")
	.dwattr $C$DW$974, DW_AT_TI_symbol_name("_S6_u32")
	.dwattr $C$DW$974, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$974, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$974, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$974, DW_AT_decl_line(0x1ca)
	.dwattr $C$DW$974, DW_AT_decl_column(0x12)

	.dwattr $C$DW$T$206, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$206, DW_AT_decl_line(0x1c7)
	.dwattr $C$DW$T$206, DW_AT_decl_column(0x05)
	.dwendtag $C$DW$T$206


$C$DW$T$208	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$208, DW_AT_byte_size(0x04)
$C$DW$975	.dwtag  DW_TAG_member
	.dwattr $C$DW$975, DW_AT_type(*$C$DW$T$207)
	.dwattr $C$DW$975, DW_AT_name("Index")
	.dwattr $C$DW$975, DW_AT_TI_symbol_name("Index")
	.dwattr $C$DW$975, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$975, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$975, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$975, DW_AT_decl_line(0xbb)
	.dwattr $C$DW$975, DW_AT_decl_column(0x18)

$C$DW$976	.dwtag  DW_TAG_member
	.dwattr $C$DW$976, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$976, DW_AT_name("ErrorNumber")
	.dwattr $C$DW$976, DW_AT_TI_symbol_name("ErrorNumber")
	.dwattr $C$DW$976, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$976, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$976, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$976, DW_AT_decl_line(0xbc)
	.dwattr $C$DW$976, DW_AT_decl_column(0x18)

	.dwattr $C$DW$T$208, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$208, DW_AT_decl_line(0xba)
	.dwattr $C$DW$T$208, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$208

$C$DW$T$530	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$530, DW_AT_name("SlFsFileNameIndexOrError_t")
	.dwattr $C$DW$T$530, DW_AT_type(*$C$DW$T$208)
	.dwattr $C$DW$T$530, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$530, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/fs.h")
	.dwattr $C$DW$T$530, DW_AT_decl_line(0xbd)
	.dwattr $C$DW$T$530, DW_AT_decl_column(0x02)


$C$DW$T$209	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$209, DW_AT_byte_size(0x10)
$C$DW$977	.dwtag  DW_TAG_member
	.dwattr $C$DW$977, DW_AT_type(*$C$DW$T$141)
	.dwattr $C$DW$977, DW_AT_name("_S6_u8")
	.dwattr $C$DW$977, DW_AT_TI_symbol_name("_S6_u8")
	.dwattr $C$DW$977, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$977, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$977, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$977, DW_AT_decl_line(0xed)
	.dwattr $C$DW$977, DW_AT_decl_column(0x0f)

$C$DW$978	.dwtag  DW_TAG_member
	.dwattr $C$DW$978, DW_AT_type(*$C$DW$T$30)
	.dwattr $C$DW$978, DW_AT_name("_S6_u32")
	.dwattr $C$DW$978, DW_AT_TI_symbol_name("_S6_u32")
	.dwattr $C$DW$978, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$978, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$978, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$978, DW_AT_decl_line(0xee)
	.dwattr $C$DW$978, DW_AT_decl_column(0x0f)

	.dwattr $C$DW$T$209, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$209, DW_AT_decl_line(0xec)
	.dwattr $C$DW$T$209, DW_AT_decl_column(0x05)
	.dwendtag $C$DW$T$209


$C$DW$T$212	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$212, DW_AT_byte_size(0x84)
$C$DW$979	.dwtag  DW_TAG_member
	.dwattr $C$DW$979, DW_AT_type(*$C$DW$T$210)
	.dwattr $C$DW$979, DW_AT_name("SockTxFailData")
	.dwattr $C$DW$979, DW_AT_TI_symbol_name("SockTxFailData")
	.dwattr $C$DW$979, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$979, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$979, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$979, DW_AT_decl_line(0x11f)
	.dwattr $C$DW$979, DW_AT_decl_column(0x1f)

$C$DW$980	.dwtag  DW_TAG_member
	.dwattr $C$DW$980, DW_AT_type(*$C$DW$T$211)
	.dwattr $C$DW$980, DW_AT_name("SockAsyncData")
	.dwattr $C$DW$980, DW_AT_TI_symbol_name("SockAsyncData")
	.dwattr $C$DW$980, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$980, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$980, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$980, DW_AT_decl_line(0x120)
	.dwattr $C$DW$980, DW_AT_decl_column(0x1f)

	.dwattr $C$DW$T$212, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$212, DW_AT_decl_line(0x11e)
	.dwattr $C$DW$T$212, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$212

$C$DW$T$52	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$52, DW_AT_name("SlSockEventData_u")
	.dwattr $C$DW$T$52, DW_AT_type(*$C$DW$T$212)
	.dwattr $C$DW$T$52, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$52, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$52, DW_AT_decl_line(0x121)
	.dwattr $C$DW$T$52, DW_AT_decl_column(0x03)


$C$DW$T$221	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$221, DW_AT_byte_size(0x20)
$C$DW$981	.dwtag  DW_TAG_member
	.dwattr $C$DW$981, DW_AT_type(*$C$DW$T$213)
	.dwattr $C$DW$981, DW_AT_name("IpAcquiredV4")
	.dwattr $C$DW$981, DW_AT_TI_symbol_name("IpAcquiredV4")
	.dwattr $C$DW$981, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$981, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$981, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$981, DW_AT_decl_line(0x10c)
	.dwattr $C$DW$981, DW_AT_decl_column(0x22)

$C$DW$982	.dwtag  DW_TAG_member
	.dwattr $C$DW$982, DW_AT_type(*$C$DW$T$214)
	.dwattr $C$DW$982, DW_AT_name("IpAcquiredV6")
	.dwattr $C$DW$982, DW_AT_TI_symbol_name("IpAcquiredV6")
	.dwattr $C$DW$982, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$982, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$982, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$982, DW_AT_decl_line(0x10d)
	.dwattr $C$DW$982, DW_AT_decl_column(0x22)

$C$DW$983	.dwtag  DW_TAG_member
	.dwattr $C$DW$983, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$983, DW_AT_name("Sd")
	.dwattr $C$DW$983, DW_AT_TI_symbol_name("Sd")
	.dwattr $C$DW$983, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$983, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$983, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$983, DW_AT_decl_line(0x10e)
	.dwattr $C$DW$983, DW_AT_decl_column(0x22)

$C$DW$984	.dwtag  DW_TAG_member
	.dwattr $C$DW$984, DW_AT_type(*$C$DW$T$215)
	.dwattr $C$DW$984, DW_AT_name("IpLeased")
	.dwattr $C$DW$984, DW_AT_TI_symbol_name("IpLeased")
	.dwattr $C$DW$984, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$984, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$984, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$984, DW_AT_decl_line(0x10f)
	.dwattr $C$DW$984, DW_AT_decl_column(0x22)

$C$DW$985	.dwtag  DW_TAG_member
	.dwattr $C$DW$985, DW_AT_type(*$C$DW$T$216)
	.dwattr $C$DW$985, DW_AT_name("IpReleased")
	.dwattr $C$DW$985, DW_AT_TI_symbol_name("IpReleased")
	.dwattr $C$DW$985, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$985, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$985, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$985, DW_AT_decl_line(0x110)
	.dwattr $C$DW$985, DW_AT_decl_column(0x22)

$C$DW$986	.dwtag  DW_TAG_member
	.dwattr $C$DW$986, DW_AT_type(*$C$DW$T$217)
	.dwattr $C$DW$986, DW_AT_name("IpV4Lost")
	.dwattr $C$DW$986, DW_AT_TI_symbol_name("IpV4Lost")
	.dwattr $C$DW$986, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$986, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$986, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$986, DW_AT_decl_line(0x111)
	.dwattr $C$DW$986, DW_AT_decl_column(0x22)

$C$DW$987	.dwtag  DW_TAG_member
	.dwattr $C$DW$987, DW_AT_type(*$C$DW$T$218)
	.dwattr $C$DW$987, DW_AT_name("DhcpIpAcquireTimeout")
	.dwattr $C$DW$987, DW_AT_TI_symbol_name("DhcpIpAcquireTimeout")
	.dwattr $C$DW$987, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$987, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$987, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$987, DW_AT_decl_line(0x112)
	.dwattr $C$DW$987, DW_AT_decl_column(0x22)

$C$DW$988	.dwtag  DW_TAG_member
	.dwattr $C$DW$988, DW_AT_type(*$C$DW$T$219)
	.dwattr $C$DW$988, DW_AT_name("IpCollision")
	.dwattr $C$DW$988, DW_AT_TI_symbol_name("IpCollision")
	.dwattr $C$DW$988, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$988, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$988, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$988, DW_AT_decl_line(0x113)
	.dwattr $C$DW$988, DW_AT_decl_column(0x22)

$C$DW$989	.dwtag  DW_TAG_member
	.dwattr $C$DW$989, DW_AT_type(*$C$DW$T$220)
	.dwattr $C$DW$989, DW_AT_name("IpV6Lost")
	.dwattr $C$DW$989, DW_AT_TI_symbol_name("IpV6Lost")
	.dwattr $C$DW$989, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$989, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$989, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$989, DW_AT_decl_line(0x114)
	.dwattr $C$DW$989, DW_AT_decl_column(0x22)

	.dwattr $C$DW$T$221, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$221, DW_AT_decl_line(0x10b)
	.dwattr $C$DW$T$221, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$221

$C$DW$T$80	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$80, DW_AT_name("SlNetAppEventData_u")
	.dwattr $C$DW$T$80, DW_AT_type(*$C$DW$T$221)
	.dwattr $C$DW$T$80, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$80, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$80, DW_AT_decl_line(0x115)
	.dwattr $C$DW$T$80, DW_AT_decl_column(0x03)


$C$DW$T$223	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$223, DW_AT_byte_size(0x18)
$C$DW$990	.dwtag  DW_TAG_member
	.dwattr $C$DW$990, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$990, DW_AT_name("HttpTokenName")
	.dwattr $C$DW$990, DW_AT_TI_symbol_name("HttpTokenName")
	.dwattr $C$DW$990, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$990, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$990, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$990, DW_AT_decl_line(0x14b)
	.dwattr $C$DW$990, DW_AT_decl_column(0x27)

$C$DW$991	.dwtag  DW_TAG_member
	.dwattr $C$DW$991, DW_AT_type(*$C$DW$T$222)
	.dwattr $C$DW$991, DW_AT_name("HttpPostData")
	.dwattr $C$DW$991, DW_AT_TI_symbol_name("HttpPostData")
	.dwattr $C$DW$991, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$991, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$991, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$991, DW_AT_decl_line(0x14c)
	.dwattr $C$DW$991, DW_AT_decl_column(0x27)

	.dwattr $C$DW$T$223, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$223, DW_AT_decl_line(0x14a)
	.dwattr $C$DW$T$223, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$223

$C$DW$T$89	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$89, DW_AT_name("SlNetAppHttpServerEventData_u")
	.dwattr $C$DW$T$89, DW_AT_type(*$C$DW$T$223)
	.dwattr $C$DW$T$89, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$89, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$89, DW_AT_decl_line(0x14d)
	.dwattr $C$DW$T$89, DW_AT_decl_column(0x03)


$C$DW$T$224	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$224, DW_AT_byte_size(0x08)
$C$DW$992	.dwtag  DW_TAG_member
	.dwattr $C$DW$992, DW_AT_type(*$C$DW$T$87)
	.dwattr $C$DW$992, DW_AT_name("TokenValue")
	.dwattr $C$DW$992, DW_AT_TI_symbol_name("TokenValue")
	.dwattr $C$DW$992, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$992, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$992, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$992, DW_AT_decl_line(0x151)
	.dwattr $C$DW$992, DW_AT_decl_column(0x20)

	.dwattr $C$DW$T$224, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$224, DW_AT_decl_line(0x150)
	.dwattr $C$DW$T$224, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$224

$C$DW$T$91	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$91, DW_AT_name("SlNetAppHttpServerResponsedata_u")
	.dwattr $C$DW$T$91, DW_AT_type(*$C$DW$T$224)
	.dwattr $C$DW$T$91, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$91, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$91, DW_AT_decl_line(0x152)
	.dwattr $C$DW$T$91, DW_AT_decl_column(0x03)


$C$DW$T$237	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$237, DW_AT_byte_size(0x4c)
$C$DW$993	.dwtag  DW_TAG_member
	.dwattr $C$DW$993, DW_AT_type(*$C$DW$T$197)
	.dwattr $C$DW$993, DW_AT_name("Connect")
	.dwattr $C$DW$993, DW_AT_TI_symbol_name("Connect")
	.dwattr $C$DW$993, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$993, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$993, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$993, DW_AT_decl_line(0x255)
	.dwattr $C$DW$993, DW_AT_decl_column(0x32)

$C$DW$994	.dwtag  DW_TAG_member
	.dwattr $C$DW$994, DW_AT_type(*$C$DW$T$225)
	.dwattr $C$DW$994, DW_AT_name("Disconnect")
	.dwattr $C$DW$994, DW_AT_TI_symbol_name("Disconnect")
	.dwattr $C$DW$994, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$994, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$994, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$994, DW_AT_decl_line(0x256)
	.dwattr $C$DW$994, DW_AT_decl_column(0x32)

$C$DW$995	.dwtag  DW_TAG_member
	.dwattr $C$DW$995, DW_AT_type(*$C$DW$T$226)
	.dwattr $C$DW$995, DW_AT_name("STAAdded")
	.dwattr $C$DW$995, DW_AT_TI_symbol_name("STAAdded")
	.dwattr $C$DW$995, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$995, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$995, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$995, DW_AT_decl_line(0x257)
	.dwattr $C$DW$995, DW_AT_decl_column(0x32)

$C$DW$996	.dwtag  DW_TAG_member
	.dwattr $C$DW$996, DW_AT_type(*$C$DW$T$227)
	.dwattr $C$DW$996, DW_AT_name("STARemoved")
	.dwattr $C$DW$996, DW_AT_TI_symbol_name("STARemoved")
	.dwattr $C$DW$996, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$996, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$996, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$996, DW_AT_decl_line(0x258)
	.dwattr $C$DW$996, DW_AT_decl_column(0x32)

$C$DW$997	.dwtag  DW_TAG_member
	.dwattr $C$DW$997, DW_AT_type(*$C$DW$T$198)
	.dwattr $C$DW$997, DW_AT_name("P2PConnect")
	.dwattr $C$DW$997, DW_AT_TI_symbol_name("P2PConnect")
	.dwattr $C$DW$997, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$997, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$997, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$997, DW_AT_decl_line(0x259)
	.dwattr $C$DW$997, DW_AT_decl_column(0x32)

$C$DW$998	.dwtag  DW_TAG_member
	.dwattr $C$DW$998, DW_AT_type(*$C$DW$T$228)
	.dwattr $C$DW$998, DW_AT_name("P2PDisconnect")
	.dwattr $C$DW$998, DW_AT_TI_symbol_name("P2PDisconnect")
	.dwattr $C$DW$998, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$998, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$998, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$998, DW_AT_decl_line(0x25a)
	.dwattr $C$DW$998, DW_AT_decl_column(0x32)

$C$DW$999	.dwtag  DW_TAG_member
	.dwattr $C$DW$999, DW_AT_type(*$C$DW$T$229)
	.dwattr $C$DW$999, DW_AT_name("P2PClientAdded")
	.dwattr $C$DW$999, DW_AT_TI_symbol_name("P2PClientAdded")
	.dwattr $C$DW$999, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$999, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$999, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$999, DW_AT_decl_line(0x25b)
	.dwattr $C$DW$999, DW_AT_decl_column(0x32)

$C$DW$1000	.dwtag  DW_TAG_member
	.dwattr $C$DW$1000, DW_AT_type(*$C$DW$T$230)
	.dwattr $C$DW$1000, DW_AT_name("P2PClientRemoved")
	.dwattr $C$DW$1000, DW_AT_TI_symbol_name("P2PClientRemoved")
	.dwattr $C$DW$1000, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1000, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1000, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1000, DW_AT_decl_line(0x25c)
	.dwattr $C$DW$1000, DW_AT_decl_column(0x32)

$C$DW$1001	.dwtag  DW_TAG_member
	.dwattr $C$DW$1001, DW_AT_type(*$C$DW$T$231)
	.dwattr $C$DW$1001, DW_AT_name("P2PDevFound")
	.dwattr $C$DW$1001, DW_AT_TI_symbol_name("P2PDevFound")
	.dwattr $C$DW$1001, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1001, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1001, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1001, DW_AT_decl_line(0x25d)
	.dwattr $C$DW$1001, DW_AT_decl_column(0x32)

$C$DW$1002	.dwtag  DW_TAG_member
	.dwattr $C$DW$1002, DW_AT_type(*$C$DW$T$232)
	.dwattr $C$DW$1002, DW_AT_name("P2PRequest")
	.dwattr $C$DW$1002, DW_AT_TI_symbol_name("P2PRequest")
	.dwattr $C$DW$1002, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1002, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1002, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1002, DW_AT_decl_line(0x25e)
	.dwattr $C$DW$1002, DW_AT_decl_column(0x32)

$C$DW$1003	.dwtag  DW_TAG_member
	.dwattr $C$DW$1003, DW_AT_type(*$C$DW$T$233)
	.dwattr $C$DW$1003, DW_AT_name("P2PConnectFail")
	.dwattr $C$DW$1003, DW_AT_TI_symbol_name("P2PConnectFail")
	.dwattr $C$DW$1003, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1003, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1003, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1003, DW_AT_decl_line(0x25f)
	.dwattr $C$DW$1003, DW_AT_decl_column(0x32)

$C$DW$1004	.dwtag  DW_TAG_member
	.dwattr $C$DW$1004, DW_AT_type(*$C$DW$T$234)
	.dwattr $C$DW$1004, DW_AT_name("RxFilterInfo")
	.dwattr $C$DW$1004, DW_AT_TI_symbol_name("RxFilterInfo")
	.dwattr $C$DW$1004, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1004, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1004, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1004, DW_AT_decl_line(0x260)
	.dwattr $C$DW$1004, DW_AT_decl_column(0x32)

$C$DW$1005	.dwtag  DW_TAG_member
	.dwattr $C$DW$1005, DW_AT_type(*$C$DW$T$235)
	.dwattr $C$DW$1005, DW_AT_name("ProvisioningStatus")
	.dwattr $C$DW$1005, DW_AT_TI_symbol_name("ProvisioningStatus")
	.dwattr $C$DW$1005, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1005, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1005, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1005, DW_AT_decl_line(0x261)
	.dwattr $C$DW$1005, DW_AT_decl_column(0x32)

$C$DW$1006	.dwtag  DW_TAG_member
	.dwattr $C$DW$1006, DW_AT_type(*$C$DW$T$236)
	.dwattr $C$DW$1006, DW_AT_name("ProvisioningProfileAdded")
	.dwattr $C$DW$1006, DW_AT_TI_symbol_name("ProvisioningProfileAdded")
	.dwattr $C$DW$1006, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1006, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1006, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1006, DW_AT_decl_line(0x262)
	.dwattr $C$DW$1006, DW_AT_decl_column(0x32)

	.dwattr $C$DW$T$237, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$237, DW_AT_decl_line(0x254)
	.dwattr $C$DW$T$237, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$237

$C$DW$T$123	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$123, DW_AT_name("SlWlanEventData_u")
	.dwattr $C$DW$T$123, DW_AT_type(*$C$DW$T$237)
	.dwattr $C$DW$T$123, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$123, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$123, DW_AT_decl_line(0x264)
	.dwattr $C$DW$T$123, DW_AT_decl_column(0x03)


$C$DW$T$247	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$247, DW_AT_byte_size(0x24)
$C$DW$1007	.dwtag  DW_TAG_member
	.dwattr $C$DW$1007, DW_AT_type(*$C$DW$T$238)
	.dwattr $C$DW$1007, DW_AT_name("Pattern")
	.dwattr $C$DW$1007, DW_AT_TI_symbol_name("Pattern")
	.dwattr $C$DW$1007, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1007, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1007, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1007, DW_AT_decl_line(0x327)
	.dwattr $C$DW$1007, DW_AT_decl_column(0x21)

$C$DW$1008	.dwtag  DW_TAG_member
	.dwattr $C$DW$1008, DW_AT_type(*$C$DW$T$239)
	.dwattr $C$DW$1008, DW_AT_name("Ipv4")
	.dwattr $C$DW$1008, DW_AT_TI_symbol_name("Ipv4")
	.dwattr $C$DW$1008, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1008, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1008, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1008, DW_AT_decl_line(0x32a)
	.dwattr $C$DW$1008, DW_AT_decl_column(0x09)

$C$DW$1009	.dwtag  DW_TAG_member
	.dwattr $C$DW$1009, DW_AT_type(*$C$DW$T$240)
	.dwattr $C$DW$1009, DW_AT_name("Ipv4Ascii")
	.dwattr $C$DW$1009, DW_AT_TI_symbol_name("Ipv4Ascii")
	.dwattr $C$DW$1009, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1009, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1009, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1009, DW_AT_decl_line(0x32d)
	.dwattr $C$DW$1009, DW_AT_decl_column(0x09)

$C$DW$1010	.dwtag  DW_TAG_member
	.dwattr $C$DW$1010, DW_AT_type(*$C$DW$T$240)
	.dwattr $C$DW$1010, DW_AT_name("Ipv6")
	.dwattr $C$DW$1010, DW_AT_TI_symbol_name("Ipv6")
	.dwattr $C$DW$1010, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1010, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1010, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1010, DW_AT_decl_line(0x330)
	.dwattr $C$DW$1010, DW_AT_decl_column(0x09)

$C$DW$1011	.dwtag  DW_TAG_member
	.dwattr $C$DW$1011, DW_AT_type(*$C$DW$T$241)
	.dwattr $C$DW$1011, DW_AT_name("Mac")
	.dwattr $C$DW$1011, DW_AT_TI_symbol_name("Mac")
	.dwattr $C$DW$1011, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1011, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1011, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1011, DW_AT_decl_line(0x333)
	.dwattr $C$DW$1011, DW_AT_decl_column(0x09)

$C$DW$1012	.dwtag  DW_TAG_member
	.dwattr $C$DW$1012, DW_AT_type(*$C$DW$T$243)
	.dwattr $C$DW$1012, DW_AT_name("MacAscii")
	.dwattr $C$DW$1012, DW_AT_TI_symbol_name("MacAscii")
	.dwattr $C$DW$1012, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1012, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1012, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1012, DW_AT_decl_line(0x336)
	.dwattr $C$DW$1012, DW_AT_decl_column(0x09)

$C$DW$1013	.dwtag  DW_TAG_member
	.dwattr $C$DW$1013, DW_AT_type(*$C$DW$T$241)
	.dwattr $C$DW$1013, DW_AT_name("Bssid")
	.dwattr $C$DW$1013, DW_AT_TI_symbol_name("Bssid")
	.dwattr $C$DW$1013, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1013, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1013, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1013, DW_AT_decl_line(0x339)
	.dwattr $C$DW$1013, DW_AT_decl_column(0x09)

$C$DW$1014	.dwtag  DW_TAG_member
	.dwattr $C$DW$1014, DW_AT_type(*$C$DW$T$182)
	.dwattr $C$DW$1014, DW_AT_name("FrameLength")
	.dwattr $C$DW$1014, DW_AT_TI_symbol_name("FrameLength")
	.dwattr $C$DW$1014, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1014, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1014, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1014, DW_AT_decl_line(0x33c)
	.dwattr $C$DW$1014, DW_AT_decl_column(0x0a)

$C$DW$1015	.dwtag  DW_TAG_member
	.dwattr $C$DW$1015, DW_AT_type(*$C$DW$T$244)
	.dwattr $C$DW$1015, DW_AT_name("Port")
	.dwattr $C$DW$1015, DW_AT_TI_symbol_name("Port")
	.dwattr $C$DW$1015, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1015, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1015, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1015, DW_AT_decl_line(0x33f)
	.dwattr $C$DW$1015, DW_AT_decl_column(0x0a)

$C$DW$1016	.dwtag  DW_TAG_member
	.dwattr $C$DW$1016, DW_AT_type(*$C$DW$T$182)
	.dwattr $C$DW$1016, DW_AT_name("EtherType")
	.dwattr $C$DW$1016, DW_AT_TI_symbol_name("EtherType")
	.dwattr $C$DW$1016, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1016, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1016, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1016, DW_AT_decl_line(0x342)
	.dwattr $C$DW$1016, DW_AT_decl_column(0x0a)

$C$DW$1017	.dwtag  DW_TAG_member
	.dwattr $C$DW$1017, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$1017, DW_AT_name("IpVersion")
	.dwattr $C$DW$1017, DW_AT_TI_symbol_name("IpVersion")
	.dwattr $C$DW$1017, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1017, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1017, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1017, DW_AT_decl_line(0x345)
	.dwattr $C$DW$1017, DW_AT_decl_column(0x09)

$C$DW$1018	.dwtag  DW_TAG_member
	.dwattr $C$DW$1018, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$1018, DW_AT_name("Frametype")
	.dwattr $C$DW$1018, DW_AT_TI_symbol_name("Frametype")
	.dwattr $C$DW$1018, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1018, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1018, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1018, DW_AT_decl_line(0x348)
	.dwattr $C$DW$1018, DW_AT_decl_column(0x09)

$C$DW$1019	.dwtag  DW_TAG_member
	.dwattr $C$DW$1019, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$1019, DW_AT_name("FrameSubtype")
	.dwattr $C$DW$1019, DW_AT_TI_symbol_name("FrameSubtype")
	.dwattr $C$DW$1019, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1019, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1019, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1019, DW_AT_decl_line(0x34b)
	.dwattr $C$DW$1019, DW_AT_decl_column(0x09)

$C$DW$1020	.dwtag  DW_TAG_member
	.dwattr $C$DW$1020, DW_AT_type(*$C$DW$T$36)
	.dwattr $C$DW$1020, DW_AT_name("IpProtocol")
	.dwattr $C$DW$1020, DW_AT_TI_symbol_name("IpProtocol")
	.dwattr $C$DW$1020, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1020, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1020, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1020, DW_AT_decl_line(0x34e)
	.dwattr $C$DW$1020, DW_AT_decl_column(0x09)

$C$DW$1021	.dwtag  DW_TAG_member
	.dwattr $C$DW$1021, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$1021, DW_AT_name("IpVersionAscii")
	.dwattr $C$DW$1021, DW_AT_TI_symbol_name("IpVersionAscii")
	.dwattr $C$DW$1021, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1021, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1021, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1021, DW_AT_decl_line(0x351)
	.dwattr $C$DW$1021, DW_AT_decl_column(0x09)

$C$DW$1022	.dwtag  DW_TAG_member
	.dwattr $C$DW$1022, DW_AT_type(*$C$DW$T$239)
	.dwattr $C$DW$1022, DW_AT_name("FrametypeAscii")
	.dwattr $C$DW$1022, DW_AT_TI_symbol_name("FrametypeAscii")
	.dwattr $C$DW$1022, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1022, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1022, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1022, DW_AT_decl_line(0x354)
	.dwattr $C$DW$1022, DW_AT_decl_column(0x09)

$C$DW$1023	.dwtag  DW_TAG_member
	.dwattr $C$DW$1023, DW_AT_type(*$C$DW$T$246)
	.dwattr $C$DW$1023, DW_AT_name("IpProtocolAscii")
	.dwattr $C$DW$1023, DW_AT_TI_symbol_name("IpProtocolAscii")
	.dwattr $C$DW$1023, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1023, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1023, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1023, DW_AT_decl_line(0x358)
	.dwattr $C$DW$1023, DW_AT_decl_column(0x09)

	.dwattr $C$DW$T$247, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$247, DW_AT_decl_line(0x325)
	.dwattr $C$DW$T$247, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$247

$C$DW$T$143	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$143, DW_AT_name("SlWlanRxFilterHeaderArg_u")
	.dwattr $C$DW$T$143, DW_AT_type(*$C$DW$T$247)
	.dwattr $C$DW$T$143, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$143, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$143, DW_AT_decl_line(0x35a)
	.dwattr $C$DW$T$143, DW_AT_decl_column(0x02)


$C$DW$T$250	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$250, DW_AT_byte_size(0x38)
$C$DW$1024	.dwtag  DW_TAG_member
	.dwattr $C$DW$1024, DW_AT_type(*$C$DW$T$248)
	.dwattr $C$DW$1024, DW_AT_name("Header")
	.dwattr $C$DW$1024, DW_AT_TI_symbol_name("Header")
	.dwattr $C$DW$1024, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1024, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1024, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1024, DW_AT_decl_line(0x382)
	.dwattr $C$DW$1024, DW_AT_decl_column(0x31)

$C$DW$1025	.dwtag  DW_TAG_member
	.dwattr $C$DW$1025, DW_AT_type(*$C$DW$T$249)
	.dwattr $C$DW$1025, DW_AT_name("Combination")
	.dwattr $C$DW$1025, DW_AT_TI_symbol_name("Combination")
	.dwattr $C$DW$1025, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1025, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1025, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$1025, DW_AT_decl_line(0x383)
	.dwattr $C$DW$1025, DW_AT_decl_column(0x31)

	.dwattr $C$DW$T$250, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$250, DW_AT_decl_line(0x381)
	.dwattr $C$DW$T$250, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$250

$C$DW$T$531	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$531, DW_AT_name("SlWlanRxFilterRule_u")
	.dwattr $C$DW$T$531, DW_AT_type(*$C$DW$T$250)
	.dwattr $C$DW$T$531, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$531, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$531, DW_AT_decl_line(0x384)
	.dwattr $C$DW$T$531, DW_AT_decl_column(0x02)


$C$DW$T$253	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$253, DW_AT_byte_size(0x04)
$C$DW$1026	.dwtag  DW_TAG_member
	.dwattr $C$DW$1026, DW_AT_type(*$C$DW$T$251)
	.dwattr $C$DW$1026, DW_AT_name("ResetRequest")
	.dwattr $C$DW$1026, DW_AT_TI_symbol_name("ResetRequest")
	.dwattr $C$DW$1026, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1026, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1026, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$1026, DW_AT_decl_line(0x70)
	.dwattr $C$DW$1026, DW_AT_decl_column(0x24)

$C$DW$1027	.dwtag  DW_TAG_member
	.dwattr $C$DW$1027, DW_AT_type(*$C$DW$T$252)
	.dwattr $C$DW$1027, DW_AT_name("Error")
	.dwattr $C$DW$1027, DW_AT_TI_symbol_name("Error")
	.dwattr $C$DW$1027, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1027, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1027, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$1027, DW_AT_decl_line(0x71)
	.dwattr $C$DW$1027, DW_AT_decl_column(0x21)

	.dwattr $C$DW$T$253, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$253, DW_AT_decl_line(0x6f)
	.dwattr $C$DW$T$253, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$253

$C$DW$T$171	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$171, DW_AT_name("SlDeviceEventData_u")
	.dwattr $C$DW$T$171, DW_AT_type(*$C$DW$T$253)
	.dwattr $C$DW$T$171, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$171, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$171, DW_AT_decl_line(0x72)
	.dwattr $C$DW$T$171, DW_AT_decl_column(0x02)


$C$DW$T$257	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$257, DW_AT_byte_size(0x08)
$C$DW$1028	.dwtag  DW_TAG_member
	.dwattr $C$DW$1028, DW_AT_type(*$C$DW$T$254)
	.dwattr $C$DW$1028, DW_AT_name("DeviceAssert")
	.dwattr $C$DW$1028, DW_AT_TI_symbol_name("DeviceAssert")
	.dwattr $C$DW$1028, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1028, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1028, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$1028, DW_AT_decl_line(0x9a)
	.dwattr $C$DW$1028, DW_AT_decl_column(0x28)

$C$DW$1029	.dwtag  DW_TAG_member
	.dwattr $C$DW$1029, DW_AT_type(*$C$DW$T$255)
	.dwattr $C$DW$1029, DW_AT_name("NoCmdAck")
	.dwattr $C$DW$1029, DW_AT_TI_symbol_name("NoCmdAck")
	.dwattr $C$DW$1029, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1029, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1029, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$1029, DW_AT_decl_line(0x9b)
	.dwattr $C$DW$1029, DW_AT_decl_column(0x28)

$C$DW$1030	.dwtag  DW_TAG_member
	.dwattr $C$DW$1030, DW_AT_type(*$C$DW$T$256)
	.dwattr $C$DW$1030, DW_AT_name("CmdTimeout")
	.dwattr $C$DW$1030, DW_AT_TI_symbol_name("CmdTimeout")
	.dwattr $C$DW$1030, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1030, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1030, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$1030, DW_AT_decl_line(0x9c)
	.dwattr $C$DW$1030, DW_AT_decl_column(0x26)

	.dwattr $C$DW$T$257, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$257, DW_AT_decl_line(0x99)
	.dwattr $C$DW$T$257, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$257

$C$DW$T$175	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$175, DW_AT_name("SlDeviceFatalData_u")
	.dwattr $C$DW$T$175, DW_AT_type(*$C$DW$T$257)
	.dwattr $C$DW$T$175, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$175, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$175, DW_AT_decl_line(0x9d)
	.dwattr $C$DW$T$175, DW_AT_decl_column(0x02)


$C$DW$T$259	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$259, DW_AT_byte_size(0x04)
$C$DW$1031	.dwtag  DW_TAG_member
	.dwattr $C$DW$1031, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$1031, DW_AT_name("NamedCurveParams")
	.dwattr $C$DW$1031, DW_AT_TI_symbol_name("NamedCurveParams")
	.dwattr $C$DW$1031, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1031, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1031, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$1031, DW_AT_decl_line(0xb3)
	.dwattr $C$DW$1031, DW_AT_decl_column(0x2a)

$C$DW$1032	.dwtag  DW_TAG_member
	.dwattr $C$DW$1032, DW_AT_type(*$C$DW$T$258)
	.dwattr $C$DW$1032, DW_AT_name("CustomCurveParams")
	.dwattr $C$DW$1032, DW_AT_TI_symbol_name("CustomCurveParams")
	.dwattr $C$DW$1032, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1032, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1032, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$1032, DW_AT_decl_line(0xb4)
	.dwattr $C$DW$1032, DW_AT_decl_column(0x2a)

	.dwattr $C$DW$T$259, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$259, DW_AT_decl_line(0xb2)
	.dwattr $C$DW$T$259, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$259

$C$DW$T$188	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$188, DW_AT_name("SlNetUtilCryptoEcCurveParams_u")
	.dwattr $C$DW$T$188, DW_AT_type(*$C$DW$T$259)
	.dwattr $C$DW$T$188, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$188, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$188, DW_AT_decl_line(0xb5)
	.dwattr $C$DW$T$188, DW_AT_decl_column(0x03)


$C$DW$T$261	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$261, DW_AT_byte_size(0x05)
$C$DW$1033	.dwtag  DW_TAG_member
	.dwattr $C$DW$1033, DW_AT_type(*$C$DW$T$260)
	.dwattr $C$DW$1033, DW_AT_name("EcParams")
	.dwattr $C$DW$1033, DW_AT_TI_symbol_name("EcParams")
	.dwattr $C$DW$1033, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1033, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1033, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$1033, DW_AT_decl_line(0xcd)
	.dwattr $C$DW$1033, DW_AT_decl_column(0x25)

	.dwattr $C$DW$T$261, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$261, DW_AT_decl_line(0xcb)
	.dwattr $C$DW$T$261, DW_AT_decl_column(0x01)
	.dwendtag $C$DW$T$261

$C$DW$T$190	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$190, DW_AT_name("SlNetUtilCryptoPubKeyParams_u")
	.dwattr $C$DW$T$190, DW_AT_type(*$C$DW$T$261)
	.dwattr $C$DW$T$190, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$190, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netutil.h")
	.dwattr $C$DW$T$190, DW_AT_decl_line(0xd0)
	.dwattr $C$DW$T$190, DW_AT_decl_column(0x03)


$C$DW$T$262	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$262, DW_AT_name("ClockP_FreqHz")
	.dwattr $C$DW$T$262, DW_AT_byte_size(0x08)
$C$DW$1034	.dwtag  DW_TAG_member
	.dwattr $C$DW$1034, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1034, DW_AT_name("hi")
	.dwattr $C$DW$1034, DW_AT_TI_symbol_name("hi")
	.dwattr $C$DW$1034, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1034, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1034, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1034, DW_AT_decl_line(0x5c)
	.dwattr $C$DW$1034, DW_AT_decl_column(0x0e)

$C$DW$1035	.dwtag  DW_TAG_member
	.dwattr $C$DW$1035, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1035, DW_AT_name("lo")
	.dwattr $C$DW$1035, DW_AT_TI_symbol_name("lo")
	.dwattr $C$DW$1035, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1035, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1035, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1035, DW_AT_decl_line(0x5d)
	.dwattr $C$DW$1035, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$262, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$262, DW_AT_decl_line(0x5b)
	.dwattr $C$DW$T$262, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$262

$C$DW$T$532	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$532, DW_AT_name("ClockP_FreqHz")
	.dwattr $C$DW$T$532, DW_AT_type(*$C$DW$T$262)
	.dwattr $C$DW$T$532, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$532, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$532, DW_AT_decl_line(0x5e)
	.dwattr $C$DW$T$532, DW_AT_decl_column(0x03)


$C$DW$T$265	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$265, DW_AT_name("ClockP_Params")
	.dwattr $C$DW$T$265, DW_AT_byte_size(0x0c)
$C$DW$1036	.dwtag  DW_TAG_member
	.dwattr $C$DW$1036, DW_AT_type(*$C$DW$T$263)
	.dwattr $C$DW$1036, DW_AT_name("startFlag")
	.dwattr $C$DW$1036, DW_AT_TI_symbol_name("startFlag")
	.dwattr $C$DW$1036, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1036, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1036, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1036, DW_AT_decl_line(0x8d)
	.dwattr $C$DW$1036, DW_AT_decl_column(0x0f)

$C$DW$1037	.dwtag  DW_TAG_member
	.dwattr $C$DW$1037, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1037, DW_AT_name("period")
	.dwattr $C$DW$1037, DW_AT_TI_symbol_name("period")
	.dwattr $C$DW$1037, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1037, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1037, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1037, DW_AT_decl_line(0x8e)
	.dwattr $C$DW$1037, DW_AT_decl_column(0x0f)

$C$DW$1038	.dwtag  DW_TAG_member
	.dwattr $C$DW$1038, DW_AT_type(*$C$DW$T$264)
	.dwattr $C$DW$1038, DW_AT_name("arg")
	.dwattr $C$DW$1038, DW_AT_TI_symbol_name("arg")
	.dwattr $C$DW$1038, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1038, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1038, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1038, DW_AT_decl_line(0x8f)
	.dwattr $C$DW$1038, DW_AT_decl_column(0x0f)

	.dwattr $C$DW$T$265, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$265, DW_AT_decl_line(0x8c)
	.dwattr $C$DW$T$265, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$265

$C$DW$T$533	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$533, DW_AT_name("ClockP_Params")
	.dwattr $C$DW$T$533, DW_AT_type(*$C$DW$T$265)
	.dwattr $C$DW$T$533, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$533, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$533, DW_AT_decl_line(0x90)
	.dwattr $C$DW$T$533, DW_AT_decl_column(0x03)


$C$DW$T$534	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$534, DW_AT_name("ClockP_Status")
	.dwattr $C$DW$T$534, DW_AT_byte_size(0x01)
$C$DW$1039	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1039, DW_AT_name("ClockP_OK")
	.dwattr $C$DW$1039, DW_AT_const_value(0x00)
	.dwattr $C$DW$1039, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1039, DW_AT_decl_line(0x64)
	.dwattr $C$DW$1039, DW_AT_decl_column(0x05)

$C$DW$1040	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1040, DW_AT_name("ClockP_FAILURE")
	.dwattr $C$DW$1040, DW_AT_const_value(-1)
	.dwattr $C$DW$1040, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1040, DW_AT_decl_line(0x65)
	.dwattr $C$DW$1040, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$534, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$534, DW_AT_decl_line(0x63)
	.dwattr $C$DW$T$534, DW_AT_decl_column(0x0e)
	.dwendtag $C$DW$T$534

$C$DW$T$535	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$535, DW_AT_name("ClockP_Status")
	.dwattr $C$DW$T$535, DW_AT_type(*$C$DW$T$534)
	.dwattr $C$DW$T$535, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$535, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$535, DW_AT_decl_line(0x66)
	.dwattr $C$DW$T$535, DW_AT_decl_column(0x03)


$C$DW$T$268	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$268, DW_AT_name("ClockP_Struct")
	.dwattr $C$DW$T$268, DW_AT_byte_size(0x24)
$C$DW$1041	.dwtag  DW_TAG_member
	.dwattr $C$DW$1041, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1041, DW_AT_name("dummy")
	.dwattr $C$DW$1041, DW_AT_TI_symbol_name("dummy")
	.dwattr $C$DW$1041, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1041, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1041, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1041, DW_AT_decl_line(0x54)
	.dwattr $C$DW$1041, DW_AT_decl_column(0x0e)

$C$DW$1042	.dwtag  DW_TAG_member
	.dwattr $C$DW$1042, DW_AT_type(*$C$DW$T$267)
	.dwattr $C$DW$1042, DW_AT_name("data")
	.dwattr $C$DW$1042, DW_AT_TI_symbol_name("data")
	.dwattr $C$DW$1042, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1042, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1042, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$1042, DW_AT_decl_line(0x55)
	.dwattr $C$DW$1042, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$268, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$268, DW_AT_decl_line(0x53)
	.dwattr $C$DW$T$268, DW_AT_decl_column(0x0f)
	.dwendtag $C$DW$T$268

$C$DW$T$536	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$536, DW_AT_name("ClockP_Struct")
	.dwattr $C$DW$T$536, DW_AT_type(*$C$DW$T$268)
	.dwattr $C$DW$T$536, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$536, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$536, DW_AT_decl_line(0x56)
	.dwattr $C$DW$T$536, DW_AT_decl_column(0x03)


$C$DW$T$271	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$271, DW_AT_name("MutexP_Params")
	.dwattr $C$DW$T$271, DW_AT_byte_size(0x04)
$C$DW$1043	.dwtag  DW_TAG_member
	.dwattr $C$DW$1043, DW_AT_type(*$C$DW$T$270)
	.dwattr $C$DW$1043, DW_AT_name("callback")
	.dwattr $C$DW$1043, DW_AT_TI_symbol_name("callback")
	.dwattr $C$DW$1043, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1043, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1043, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$1043, DW_AT_decl_line(0x73)
	.dwattr $C$DW$1043, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$271, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$271, DW_AT_decl_line(0x72)
	.dwattr $C$DW$T$271, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$271

$C$DW$T$537	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$537, DW_AT_name("MutexP_Params")
	.dwattr $C$DW$T$537, DW_AT_type(*$C$DW$T$271)
	.dwattr $C$DW$T$537, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$537, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$537, DW_AT_decl_line(0x74)
	.dwattr $C$DW$T$537, DW_AT_decl_column(0x03)


$C$DW$T$538	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$538, DW_AT_name("MutexP_Status")
	.dwattr $C$DW$T$538, DW_AT_byte_size(0x01)
$C$DW$1044	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1044, DW_AT_name("MutexP_OK")
	.dwattr $C$DW$1044, DW_AT_const_value(0x00)
	.dwattr $C$DW$1044, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$1044, DW_AT_decl_line(0x5b)
	.dwattr $C$DW$1044, DW_AT_decl_column(0x05)

$C$DW$1045	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1045, DW_AT_name("MutexP_FAILURE")
	.dwattr $C$DW$1045, DW_AT_const_value(-1)
	.dwattr $C$DW$1045, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$1045, DW_AT_decl_line(0x5d)
	.dwattr $C$DW$1045, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$538, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$538, DW_AT_decl_line(0x59)
	.dwattr $C$DW$T$538, DW_AT_decl_column(0x0e)
	.dwendtag $C$DW$T$538

$C$DW$T$539	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$539, DW_AT_name("MutexP_Status")
	.dwattr $C$DW$T$539, DW_AT_type(*$C$DW$T$538)
	.dwattr $C$DW$T$539, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$539, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$539, DW_AT_decl_line(0x5e)
	.dwattr $C$DW$T$539, DW_AT_decl_column(0x03)


$C$DW$T$273	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$273, DW_AT_name("MutexP_Struct")
	.dwattr $C$DW$T$273, DW_AT_byte_size(0x28)
$C$DW$1046	.dwtag  DW_TAG_member
	.dwattr $C$DW$1046, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1046, DW_AT_name("dummy")
	.dwattr $C$DW$1046, DW_AT_TI_symbol_name("dummy")
	.dwattr $C$DW$1046, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1046, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1046, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$1046, DW_AT_decl_line(0x52)
	.dwattr $C$DW$1046, DW_AT_decl_column(0x0e)

$C$DW$1047	.dwtag  DW_TAG_member
	.dwattr $C$DW$1047, DW_AT_type(*$C$DW$T$272)
	.dwattr $C$DW$1047, DW_AT_name("data")
	.dwattr $C$DW$1047, DW_AT_TI_symbol_name("data")
	.dwattr $C$DW$1047, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1047, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1047, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$1047, DW_AT_decl_line(0x53)
	.dwattr $C$DW$1047, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$273, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$273, DW_AT_decl_line(0x51)
	.dwattr $C$DW$T$273, DW_AT_decl_column(0x0f)
	.dwendtag $C$DW$T$273

$C$DW$T$540	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$540, DW_AT_name("MutexP_Struct")
	.dwattr $C$DW$T$540, DW_AT_type(*$C$DW$T$273)
	.dwattr $C$DW$T$540, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$540, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$540, DW_AT_decl_line(0x54)
	.dwattr $C$DW$T$540, DW_AT_decl_column(0x03)


$C$DW$T$274	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$274, DW_AT_name("SemaphoreP_Mode")
	.dwattr $C$DW$T$274, DW_AT_byte_size(0x01)
$C$DW$1048	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1048, DW_AT_name("SemaphoreP_Mode_COUNTING")
	.dwattr $C$DW$1048, DW_AT_const_value(0x00)
	.dwattr $C$DW$1048, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1048, DW_AT_decl_line(0x78)
	.dwattr $C$DW$1048, DW_AT_decl_column(0x05)

$C$DW$1049	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1049, DW_AT_name("SemaphoreP_Mode_BINARY")
	.dwattr $C$DW$1049, DW_AT_const_value(0x01)
	.dwattr $C$DW$1049, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1049, DW_AT_decl_line(0x79)
	.dwattr $C$DW$1049, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$274, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$274, DW_AT_decl_line(0x77)
	.dwattr $C$DW$T$274, DW_AT_decl_column(0x0e)
	.dwendtag $C$DW$T$274

$C$DW$T$275	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$275, DW_AT_name("SemaphoreP_Mode")
	.dwattr $C$DW$T$275, DW_AT_type(*$C$DW$T$274)
	.dwattr $C$DW$T$275, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$275, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$275, DW_AT_decl_line(0x7a)
	.dwattr $C$DW$T$275, DW_AT_decl_column(0x03)


$C$DW$T$276	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$276, DW_AT_name("SemaphoreP_Params")
	.dwattr $C$DW$T$276, DW_AT_byte_size(0x08)
$C$DW$1050	.dwtag  DW_TAG_member
	.dwattr $C$DW$1050, DW_AT_type(*$C$DW$T$275)
	.dwattr $C$DW$1050, DW_AT_name("mode")
	.dwattr $C$DW$1050, DW_AT_TI_symbol_name("mode")
	.dwattr $C$DW$1050, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1050, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1050, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1050, DW_AT_decl_line(0x86)
	.dwattr $C$DW$1050, DW_AT_decl_column(0x15)

$C$DW$1051	.dwtag  DW_TAG_member
	.dwattr $C$DW$1051, DW_AT_type(*$C$DW$T$270)
	.dwattr $C$DW$1051, DW_AT_name("callback")
	.dwattr $C$DW$1051, DW_AT_TI_symbol_name("callback")
	.dwattr $C$DW$1051, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1051, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1051, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1051, DW_AT_decl_line(0x87)
	.dwattr $C$DW$1051, DW_AT_decl_column(0x0c)

	.dwattr $C$DW$T$276, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$276, DW_AT_decl_line(0x85)
	.dwattr $C$DW$T$276, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$276

$C$DW$T$541	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$541, DW_AT_name("SemaphoreP_Params")
	.dwattr $C$DW$T$541, DW_AT_type(*$C$DW$T$276)
	.dwattr $C$DW$T$541, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$541, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$541, DW_AT_decl_line(0x88)
	.dwattr $C$DW$T$541, DW_AT_decl_column(0x03)


$C$DW$T$542	.dwtag  DW_TAG_enumeration_type
	.dwattr $C$DW$T$542, DW_AT_name("SemaphoreP_Status")
	.dwattr $C$DW$T$542, DW_AT_byte_size(0x01)
$C$DW$1052	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1052, DW_AT_name("SemaphoreP_OK")
	.dwattr $C$DW$1052, DW_AT_const_value(0x00)
	.dwattr $C$DW$1052, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1052, DW_AT_decl_line(0x66)
	.dwattr $C$DW$1052, DW_AT_decl_column(0x05)

$C$DW$1053	.dwtag  DW_TAG_enumerator
	.dwattr $C$DW$1053, DW_AT_name("SemaphoreP_TIMEOUT")
	.dwattr $C$DW$1053, DW_AT_const_value(-1)
	.dwattr $C$DW$1053, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1053, DW_AT_decl_line(0x68)
	.dwattr $C$DW$1053, DW_AT_decl_column(0x05)

	.dwattr $C$DW$T$542, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$542, DW_AT_decl_line(0x64)
	.dwattr $C$DW$T$542, DW_AT_decl_column(0x0e)
	.dwendtag $C$DW$T$542

$C$DW$T$543	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$543, DW_AT_name("SemaphoreP_Status")
	.dwattr $C$DW$T$543, DW_AT_type(*$C$DW$T$542)
	.dwattr $C$DW$T$543, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$543, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$543, DW_AT_decl_line(0x69)
	.dwattr $C$DW$T$543, DW_AT_decl_column(0x03)


$C$DW$T$278	.dwtag  DW_TAG_union_type
	.dwattr $C$DW$T$278, DW_AT_name("SemaphoreP_Struct")
	.dwattr $C$DW$T$278, DW_AT_byte_size(0x1c)
$C$DW$1054	.dwtag  DW_TAG_member
	.dwattr $C$DW$1054, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1054, DW_AT_name("dummy")
	.dwattr $C$DW$1054, DW_AT_TI_symbol_name("dummy")
	.dwattr $C$DW$1054, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1054, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1054, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1054, DW_AT_decl_line(0x53)
	.dwattr $C$DW$1054, DW_AT_decl_column(0x0e)

$C$DW$1055	.dwtag  DW_TAG_member
	.dwattr $C$DW$1055, DW_AT_type(*$C$DW$T$277)
	.dwattr $C$DW$1055, DW_AT_name("data")
	.dwattr $C$DW$1055, DW_AT_TI_symbol_name("data")
	.dwattr $C$DW$1055, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1055, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1055, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$1055, DW_AT_decl_line(0x54)
	.dwattr $C$DW$1055, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$278, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$278, DW_AT_decl_line(0x52)
	.dwattr $C$DW$T$278, DW_AT_decl_column(0x0f)
	.dwendtag $C$DW$T$278

$C$DW$T$544	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$544, DW_AT_name("SemaphoreP_Struct")
	.dwattr $C$DW$T$544, DW_AT_type(*$C$DW$T$278)
	.dwattr $C$DW$T$544, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$544, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$544, DW_AT_decl_line(0x55)
	.dwattr $C$DW$T$544, DW_AT_decl_column(0x03)


$C$DW$T$280	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$280, DW_AT_name("SlEventsListNode_s")
	.dwattr $C$DW$T$280, DW_AT_byte_size(0x08)
$C$DW$1056	.dwtag  DW_TAG_member
	.dwattr $C$DW$1056, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$1056, DW_AT_name("event")
	.dwattr $C$DW$1056, DW_AT_TI_symbol_name("event")
	.dwattr $C$DW$1056, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1056, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1056, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$1056, DW_AT_decl_line(0x49)
	.dwattr $C$DW$1056, DW_AT_decl_column(0x0b)

$C$DW$1057	.dwtag  DW_TAG_member
	.dwattr $C$DW$1057, DW_AT_type(*$C$DW$T$279)
	.dwattr $C$DW$1057, DW_AT_name("next")
	.dwattr $C$DW$1057, DW_AT_TI_symbol_name("next")
	.dwattr $C$DW$1057, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1057, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1057, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$1057, DW_AT_decl_line(0x4a)
	.dwattr $C$DW$1057, DW_AT_decl_column(0x20)

	.dwattr $C$DW$T$280, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$T$280, DW_AT_decl_line(0x47)
	.dwattr $C$DW$T$280, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$280

$C$DW$T$545	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$545, DW_AT_name("SlEventsListNode_t")
	.dwattr $C$DW$T$545, DW_AT_type(*$C$DW$T$280)
	.dwattr $C$DW$T$545, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$545, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/eventreg.h")
	.dwattr $C$DW$T$545, DW_AT_decl_line(0x4b)
	.dwattr $C$DW$T$545, DW_AT_decl_column(0x02)

$C$DW$T$279	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$279, DW_AT_type(*$C$DW$T$280)
	.dwattr $C$DW$T$279, DW_AT_address_class(0x20)


$C$DW$T$282	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$282, DW_AT_name("SlFdSet_t")
	.dwattr $C$DW$T$282, DW_AT_byte_size(0x04)
$C$DW$1058	.dwtag  DW_TAG_member
	.dwattr $C$DW$1058, DW_AT_type(*$C$DW$T$281)
	.dwattr $C$DW$1058, DW_AT_name("fd_array")
	.dwattr $C$DW$1058, DW_AT_TI_symbol_name("fd_array")
	.dwattr $C$DW$1058, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1058, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1058, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1058, DW_AT_decl_line(0x1a3)
	.dwattr $C$DW$1058, DW_AT_decl_column(0x10)

	.dwattr $C$DW$T$282, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$282, DW_AT_decl_line(0x1a1)
	.dwattr $C$DW$T$282, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$282

$C$DW$T$546	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$546, DW_AT_name("SlFdSet_t")
	.dwattr $C$DW$T$546, DW_AT_type(*$C$DW$T$282)
	.dwattr $C$DW$T$546, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$546, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$546, DW_AT_decl_line(0x1a4)
	.dwattr $C$DW$T$546, DW_AT_decl_column(0x03)

$C$DW$T$547	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$547, DW_AT_type(*$C$DW$T$546)
	.dwattr $C$DW$T$547, DW_AT_address_class(0x20)


$C$DW$T$283	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$283, DW_AT_name("SlIn6Addr_t")
	.dwattr $C$DW$T$283, DW_AT_byte_size(0x10)
$C$DW$1059	.dwtag  DW_TAG_member
	.dwattr $C$DW$1059, DW_AT_type(*$C$DW$T$209)
	.dwattr $C$DW$1059, DW_AT_name("_S6_un")
	.dwattr $C$DW$1059, DW_AT_TI_symbol_name("_S6_un")
	.dwattr $C$DW$1059, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1059, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1059, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1059, DW_AT_decl_line(0xef)
	.dwattr $C$DW$1059, DW_AT_decl_column(0x07)

	.dwattr $C$DW$T$283, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$283, DW_AT_decl_line(0xe9)
	.dwattr $C$DW$T$283, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$283

$C$DW$T$66	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$66, DW_AT_name("SlIn6Addr_t")
	.dwattr $C$DW$T$66, DW_AT_type(*$C$DW$T$283)
	.dwattr $C$DW$T$66, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$66, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$66, DW_AT_decl_line(0xf0)
	.dwattr $C$DW$T$66, DW_AT_decl_column(0x02)


$C$DW$T$284	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$284, DW_AT_name("SlInAddr_t")
	.dwattr $C$DW$T$284, DW_AT_byte_size(0x04)
$C$DW$1060	.dwtag  DW_TAG_member
	.dwattr $C$DW$1060, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$1060, DW_AT_name("s_addr")
	.dwattr $C$DW$1060, DW_AT_TI_symbol_name("s_addr")
	.dwattr $C$DW$1060, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1060, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1060, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1060, DW_AT_decl_line(0xde)
	.dwattr $C$DW$1060, DW_AT_decl_column(0x14)

	.dwattr $C$DW$T$284, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$284, DW_AT_decl_line(0xdb)
	.dwattr $C$DW$T$284, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$284

$C$DW$T$64	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$64, DW_AT_name("SlInAddr_t")
	.dwattr $C$DW$T$64, DW_AT_type(*$C$DW$T$284)
	.dwattr $C$DW$T$64, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$64, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$64, DW_AT_decl_line(0xe6)
	.dwattr $C$DW$T$64, DW_AT_decl_column(0x02)


$C$DW$T$354	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$354, DW_AT_name("SlNetIf_Config_t")
	.dwattr $C$DW$T$354, DW_AT_byte_size(0x58)
$C$DW$1061	.dwtag  DW_TAG_member
	.dwattr $C$DW$1061, DW_AT_type(*$C$DW$T$288)
	.dwattr $C$DW$1061, DW_AT_name("sockCreate")
	.dwattr $C$DW$1061, DW_AT_TI_symbol_name("sockCreate")
	.dwattr $C$DW$1061, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1061, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1061, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1061, DW_AT_decl_line(0x9d)
	.dwattr $C$DW$1061, DW_AT_decl_column(0x0f)

$C$DW$1062	.dwtag  DW_TAG_member
	.dwattr $C$DW$1062, DW_AT_type(*$C$DW$T$291)
	.dwattr $C$DW$1062, DW_AT_name("sockClose")
	.dwattr $C$DW$1062, DW_AT_TI_symbol_name("sockClose")
	.dwattr $C$DW$1062, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1062, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1062, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1062, DW_AT_decl_line(0x9e)
	.dwattr $C$DW$1062, DW_AT_decl_column(0x0f)

$C$DW$1063	.dwtag  DW_TAG_member
	.dwattr $C$DW$1063, DW_AT_type(*$C$DW$T$293)
	.dwattr $C$DW$1063, DW_AT_name("sockShutdown")
	.dwattr $C$DW$1063, DW_AT_TI_symbol_name("sockShutdown")
	.dwattr $C$DW$1063, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1063, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1063, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1063, DW_AT_decl_line(0x9f)
	.dwattr $C$DW$1063, DW_AT_decl_column(0x0f)

$C$DW$1064	.dwtag  DW_TAG_member
	.dwattr $C$DW$1064, DW_AT_type(*$C$DW$T$299)
	.dwattr $C$DW$1064, DW_AT_name("sockAccept")
	.dwattr $C$DW$1064, DW_AT_TI_symbol_name("sockAccept")
	.dwattr $C$DW$1064, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1064, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1064, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1064, DW_AT_decl_line(0xa0)
	.dwattr $C$DW$1064, DW_AT_decl_column(0x0f)

$C$DW$1065	.dwtag  DW_TAG_member
	.dwattr $C$DW$1065, DW_AT_type(*$C$DW$T$303)
	.dwattr $C$DW$1065, DW_AT_name("sockBind")
	.dwattr $C$DW$1065, DW_AT_TI_symbol_name("sockBind")
	.dwattr $C$DW$1065, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1065, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1065, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1065, DW_AT_decl_line(0xa1)
	.dwattr $C$DW$1065, DW_AT_decl_column(0x0f)

$C$DW$1066	.dwtag  DW_TAG_member
	.dwattr $C$DW$1066, DW_AT_type(*$C$DW$T$293)
	.dwattr $C$DW$1066, DW_AT_name("sockListen")
	.dwattr $C$DW$1066, DW_AT_TI_symbol_name("sockListen")
	.dwattr $C$DW$1066, DW_AT_data_member_location[DW_OP_plus_uconst 0x14]
	.dwattr $C$DW$1066, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1066, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1066, DW_AT_decl_line(0xa2)
	.dwattr $C$DW$1066, DW_AT_decl_column(0x0f)

$C$DW$1067	.dwtag  DW_TAG_member
	.dwattr $C$DW$1067, DW_AT_type(*$C$DW$T$305)
	.dwattr $C$DW$1067, DW_AT_name("sockConnect")
	.dwattr $C$DW$1067, DW_AT_TI_symbol_name("sockConnect")
	.dwattr $C$DW$1067, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1067, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1067, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1067, DW_AT_decl_line(0xa3)
	.dwattr $C$DW$1067, DW_AT_decl_column(0x0f)

$C$DW$1068	.dwtag  DW_TAG_member
	.dwattr $C$DW$1068, DW_AT_type(*$C$DW$T$307)
	.dwattr $C$DW$1068, DW_AT_name("sockGetPeerName")
	.dwattr $C$DW$1068, DW_AT_TI_symbol_name("sockGetPeerName")
	.dwattr $C$DW$1068, DW_AT_data_member_location[DW_OP_plus_uconst 0x1c]
	.dwattr $C$DW$1068, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1068, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1068, DW_AT_decl_line(0xa4)
	.dwattr $C$DW$1068, DW_AT_decl_column(0x0f)

$C$DW$1069	.dwtag  DW_TAG_member
	.dwattr $C$DW$1069, DW_AT_type(*$C$DW$T$307)
	.dwattr $C$DW$1069, DW_AT_name("sockGetLocalName")
	.dwattr $C$DW$1069, DW_AT_TI_symbol_name("sockGetLocalName")
	.dwattr $C$DW$1069, DW_AT_data_member_location[DW_OP_plus_uconst 0x20]
	.dwattr $C$DW$1069, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1069, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1069, DW_AT_decl_line(0xa5)
	.dwattr $C$DW$1069, DW_AT_decl_column(0x0f)

$C$DW$1070	.dwtag  DW_TAG_member
	.dwattr $C$DW$1070, DW_AT_type(*$C$DW$T$313)
	.dwattr $C$DW$1070, DW_AT_name("sockSelect")
	.dwattr $C$DW$1070, DW_AT_TI_symbol_name("sockSelect")
	.dwattr $C$DW$1070, DW_AT_data_member_location[DW_OP_plus_uconst 0x24]
	.dwattr $C$DW$1070, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1070, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1070, DW_AT_decl_line(0xa6)
	.dwattr $C$DW$1070, DW_AT_decl_column(0x0f)

$C$DW$1071	.dwtag  DW_TAG_member
	.dwattr $C$DW$1071, DW_AT_type(*$C$DW$T$315)
	.dwattr $C$DW$1071, DW_AT_name("sockSetOpt")
	.dwattr $C$DW$1071, DW_AT_TI_symbol_name("sockSetOpt")
	.dwattr $C$DW$1071, DW_AT_data_member_location[DW_OP_plus_uconst 0x28]
	.dwattr $C$DW$1071, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1071, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1071, DW_AT_decl_line(0xa7)
	.dwattr $C$DW$1071, DW_AT_decl_column(0x0f)

$C$DW$1072	.dwtag  DW_TAG_member
	.dwattr $C$DW$1072, DW_AT_type(*$C$DW$T$317)
	.dwattr $C$DW$1072, DW_AT_name("sockGetOpt")
	.dwattr $C$DW$1072, DW_AT_TI_symbol_name("sockGetOpt")
	.dwattr $C$DW$1072, DW_AT_data_member_location[DW_OP_plus_uconst 0x2c]
	.dwattr $C$DW$1072, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1072, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1072, DW_AT_decl_line(0xa8)
	.dwattr $C$DW$1072, DW_AT_decl_column(0x0f)

$C$DW$1073	.dwtag  DW_TAG_member
	.dwattr $C$DW$1073, DW_AT_type(*$C$DW$T$319)
	.dwattr $C$DW$1073, DW_AT_name("sockRecv")
	.dwattr $C$DW$1073, DW_AT_TI_symbol_name("sockRecv")
	.dwattr $C$DW$1073, DW_AT_data_member_location[DW_OP_plus_uconst 0x30]
	.dwattr $C$DW$1073, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1073, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1073, DW_AT_decl_line(0xa9)
	.dwattr $C$DW$1073, DW_AT_decl_column(0x0f)

$C$DW$1074	.dwtag  DW_TAG_member
	.dwattr $C$DW$1074, DW_AT_type(*$C$DW$T$321)
	.dwattr $C$DW$1074, DW_AT_name("sockRecvFrom")
	.dwattr $C$DW$1074, DW_AT_TI_symbol_name("sockRecvFrom")
	.dwattr $C$DW$1074, DW_AT_data_member_location[DW_OP_plus_uconst 0x34]
	.dwattr $C$DW$1074, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1074, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1074, DW_AT_decl_line(0xaa)
	.dwattr $C$DW$1074, DW_AT_decl_column(0x0f)

$C$DW$1075	.dwtag  DW_TAG_member
	.dwattr $C$DW$1075, DW_AT_type(*$C$DW$T$325)
	.dwattr $C$DW$1075, DW_AT_name("sockSend")
	.dwattr $C$DW$1075, DW_AT_TI_symbol_name("sockSend")
	.dwattr $C$DW$1075, DW_AT_data_member_location[DW_OP_plus_uconst 0x38]
	.dwattr $C$DW$1075, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1075, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1075, DW_AT_decl_line(0xab)
	.dwattr $C$DW$1075, DW_AT_decl_column(0x0f)

$C$DW$1076	.dwtag  DW_TAG_member
	.dwattr $C$DW$1076, DW_AT_type(*$C$DW$T$327)
	.dwattr $C$DW$1076, DW_AT_name("sockSendTo")
	.dwattr $C$DW$1076, DW_AT_TI_symbol_name("sockSendTo")
	.dwattr $C$DW$1076, DW_AT_data_member_location[DW_OP_plus_uconst 0x3c]
	.dwattr $C$DW$1076, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1076, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1076, DW_AT_decl_line(0xac)
	.dwattr $C$DW$1076, DW_AT_decl_column(0x0f)

$C$DW$1077	.dwtag  DW_TAG_member
	.dwattr $C$DW$1077, DW_AT_type(*$C$DW$T$333)
	.dwattr $C$DW$1077, DW_AT_name("sockstartSec")
	.dwattr $C$DW$1077, DW_AT_TI_symbol_name("sockstartSec")
	.dwattr $C$DW$1077, DW_AT_data_member_location[DW_OP_plus_uconst 0x40]
	.dwattr $C$DW$1077, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1077, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1077, DW_AT_decl_line(0xad)
	.dwattr $C$DW$1077, DW_AT_decl_column(0x0f)

$C$DW$1078	.dwtag  DW_TAG_member
	.dwattr $C$DW$1078, DW_AT_type(*$C$DW$T$340)
	.dwattr $C$DW$1078, DW_AT_name("utilGetHostByName")
	.dwattr $C$DW$1078, DW_AT_TI_symbol_name("utilGetHostByName")
	.dwattr $C$DW$1078, DW_AT_data_member_location[DW_OP_plus_uconst 0x44]
	.dwattr $C$DW$1078, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1078, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1078, DW_AT_decl_line(0xb0)
	.dwattr $C$DW$1078, DW_AT_decl_column(0x0f)

$C$DW$1079	.dwtag  DW_TAG_member
	.dwattr $C$DW$1079, DW_AT_type(*$C$DW$T$344)
	.dwattr $C$DW$1079, DW_AT_name("ifGetIPAddr")
	.dwattr $C$DW$1079, DW_AT_TI_symbol_name("ifGetIPAddr")
	.dwattr $C$DW$1079, DW_AT_data_member_location[DW_OP_plus_uconst 0x48]
	.dwattr $C$DW$1079, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1079, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1079, DW_AT_decl_line(0xb3)
	.dwattr $C$DW$1079, DW_AT_decl_column(0x0f)

$C$DW$1080	.dwtag  DW_TAG_member
	.dwattr $C$DW$1080, DW_AT_type(*$C$DW$T$346)
	.dwattr $C$DW$1080, DW_AT_name("ifGetConnectionStatus")
	.dwattr $C$DW$1080, DW_AT_TI_symbol_name("ifGetConnectionStatus")
	.dwattr $C$DW$1080, DW_AT_data_member_location[DW_OP_plus_uconst 0x4c]
	.dwattr $C$DW$1080, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1080, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1080, DW_AT_decl_line(0xb4)
	.dwattr $C$DW$1080, DW_AT_decl_column(0x0f)

$C$DW$1081	.dwtag  DW_TAG_member
	.dwattr $C$DW$1081, DW_AT_type(*$C$DW$T$349)
	.dwattr $C$DW$1081, DW_AT_name("ifLoadSecObj")
	.dwattr $C$DW$1081, DW_AT_TI_symbol_name("ifLoadSecObj")
	.dwattr $C$DW$1081, DW_AT_data_member_location[DW_OP_plus_uconst 0x50]
	.dwattr $C$DW$1081, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1081, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1081, DW_AT_decl_line(0xb5)
	.dwattr $C$DW$1081, DW_AT_decl_column(0x0f)

$C$DW$1082	.dwtag  DW_TAG_member
	.dwattr $C$DW$1082, DW_AT_type(*$C$DW$T$353)
	.dwattr $C$DW$1082, DW_AT_name("ifCreateContext")
	.dwattr $C$DW$1082, DW_AT_TI_symbol_name("ifCreateContext")
	.dwattr $C$DW$1082, DW_AT_data_member_location[DW_OP_plus_uconst 0x54]
	.dwattr $C$DW$1082, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1082, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1082, DW_AT_decl_line(0xb6)
	.dwattr $C$DW$1082, DW_AT_decl_column(0x0f)

	.dwattr $C$DW$T$354, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$354, DW_AT_decl_line(0x9a)
	.dwattr $C$DW$T$354, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$354

$C$DW$T$355	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$355, DW_AT_name("SlNetIf_Config_t")
	.dwattr $C$DW$T$355, DW_AT_type(*$C$DW$T$354)
	.dwattr $C$DW$T$355, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$355, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$355, DW_AT_decl_line(0xb8)
	.dwattr $C$DW$T$355, DW_AT_decl_column(0x03)

$C$DW$T$356	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$356, DW_AT_type(*$C$DW$T$355)
	.dwattr $C$DW$T$356, DW_AT_address_class(0x20)


$C$DW$T$357	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$357, DW_AT_name("SlNetIf_t")
	.dwattr $C$DW$T$357, DW_AT_byte_size(0x14)
$C$DW$1083	.dwtag  DW_TAG_member
	.dwattr $C$DW$1083, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1083, DW_AT_name("ifID")
	.dwattr $C$DW$1083, DW_AT_TI_symbol_name("ifID")
	.dwattr $C$DW$1083, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1083, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1083, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1083, DW_AT_decl_line(0xc1)
	.dwattr $C$DW$1083, DW_AT_decl_column(0x17)

$C$DW$1084	.dwtag  DW_TAG_member
	.dwattr $C$DW$1084, DW_AT_type(*$C$DW$T$334)
	.dwattr $C$DW$1084, DW_AT_name("ifName")
	.dwattr $C$DW$1084, DW_AT_TI_symbol_name("ifName")
	.dwattr $C$DW$1084, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1084, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1084, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1084, DW_AT_decl_line(0xc2)
	.dwattr $C$DW$1084, DW_AT_decl_column(0x17)

$C$DW$1085	.dwtag  DW_TAG_member
	.dwattr $C$DW$1085, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$1085, DW_AT_name("flags")
	.dwattr $C$DW$1085, DW_AT_TI_symbol_name("flags")
	.dwattr $C$DW$1085, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1085, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1085, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1085, DW_AT_decl_line(0xc3)
	.dwattr $C$DW$1085, DW_AT_decl_column(0x17)

$C$DW$1086	.dwtag  DW_TAG_member
	.dwattr $C$DW$1086, DW_AT_type(*$C$DW$T$356)
	.dwattr $C$DW$1086, DW_AT_name("ifConf")
	.dwattr $C$DW$1086, DW_AT_TI_symbol_name("ifConf")
	.dwattr $C$DW$1086, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1086, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1086, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1086, DW_AT_decl_line(0xc4)
	.dwattr $C$DW$1086, DW_AT_decl_column(0x17)

$C$DW$1087	.dwtag  DW_TAG_member
	.dwattr $C$DW$1087, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$1087, DW_AT_name("ifContext")
	.dwattr $C$DW$1087, DW_AT_TI_symbol_name("ifContext")
	.dwattr $C$DW$1087, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1087, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1087, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$1087, DW_AT_decl_line(0xc5)
	.dwattr $C$DW$1087, DW_AT_decl_column(0x17)

	.dwattr $C$DW$T$357, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$357, DW_AT_decl_line(0xbf)
	.dwattr $C$DW$T$357, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$357

$C$DW$T$548	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$548, DW_AT_name("SlNetIf_t")
	.dwattr $C$DW$T$548, DW_AT_type(*$C$DW$T$357)
	.dwattr $C$DW$T$548, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$548, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetif.h")
	.dwattr $C$DW$T$548, DW_AT_decl_line(0xc6)
	.dwattr $C$DW$T$548, DW_AT_decl_column(0x03)


$C$DW$T$359	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$359, DW_AT_name("SlNetSock_AddrIn6_t")
	.dwattr $C$DW$T$359, DW_AT_byte_size(0x1c)
$C$DW$1088	.dwtag  DW_TAG_member
	.dwattr $C$DW$1088, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1088, DW_AT_name("sin6_family")
	.dwattr $C$DW$1088, DW_AT_TI_symbol_name("sin6_family")
	.dwattr $C$DW$1088, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1088, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1088, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1088, DW_AT_decl_line(0x256)
	.dwattr $C$DW$1088, DW_AT_decl_column(0x19)

$C$DW$1089	.dwtag  DW_TAG_member
	.dwattr $C$DW$1089, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1089, DW_AT_name("sin6_port")
	.dwattr $C$DW$1089, DW_AT_TI_symbol_name("sin6_port")
	.dwattr $C$DW$1089, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1089, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1089, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1089, DW_AT_decl_line(0x257)
	.dwattr $C$DW$1089, DW_AT_decl_column(0x19)

$C$DW$1090	.dwtag  DW_TAG_member
	.dwattr $C$DW$1090, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1090, DW_AT_name("sin6_flowinfo")
	.dwattr $C$DW$1090, DW_AT_TI_symbol_name("sin6_flowinfo")
	.dwattr $C$DW$1090, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1090, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1090, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1090, DW_AT_decl_line(0x258)
	.dwattr $C$DW$1090, DW_AT_decl_column(0x19)

$C$DW$1091	.dwtag  DW_TAG_member
	.dwattr $C$DW$1091, DW_AT_type(*$C$DW$T$358)
	.dwattr $C$DW$1091, DW_AT_name("sin6_addr")
	.dwattr $C$DW$1091, DW_AT_TI_symbol_name("sin6_addr")
	.dwattr $C$DW$1091, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1091, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1091, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1091, DW_AT_decl_line(0x259)
	.dwattr $C$DW$1091, DW_AT_decl_column(0x19)

$C$DW$1092	.dwtag  DW_TAG_member
	.dwattr $C$DW$1092, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1092, DW_AT_name("sin6_scope_id")
	.dwattr $C$DW$1092, DW_AT_TI_symbol_name("sin6_scope_id")
	.dwattr $C$DW$1092, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1092, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1092, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1092, DW_AT_decl_line(0x25a)
	.dwattr $C$DW$1092, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$359, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$359, DW_AT_decl_line(0x254)
	.dwattr $C$DW$T$359, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$359

$C$DW$T$549	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$549, DW_AT_name("SlNetSock_AddrIn6_t")
	.dwattr $C$DW$T$549, DW_AT_type(*$C$DW$T$359)
	.dwattr $C$DW$T$549, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$549, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$549, DW_AT_decl_line(0x25b)
	.dwattr $C$DW$T$549, DW_AT_decl_column(0x03)


$C$DW$T$363	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$363, DW_AT_name("SlNetSock_AddrIn_t")
	.dwattr $C$DW$T$363, DW_AT_byte_size(0x10)
$C$DW$1093	.dwtag  DW_TAG_member
	.dwattr $C$DW$1093, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1093, DW_AT_name("sin_family")
	.dwattr $C$DW$1093, DW_AT_TI_symbol_name("sin_family")
	.dwattr $C$DW$1093, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1093, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1093, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1093, DW_AT_decl_line(0x262)
	.dwattr $C$DW$1093, DW_AT_decl_column(0x18)

$C$DW$1094	.dwtag  DW_TAG_member
	.dwattr $C$DW$1094, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1094, DW_AT_name("sin_port")
	.dwattr $C$DW$1094, DW_AT_TI_symbol_name("sin_port")
	.dwattr $C$DW$1094, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1094, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1094, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1094, DW_AT_decl_line(0x263)
	.dwattr $C$DW$1094, DW_AT_decl_column(0x18)

$C$DW$1095	.dwtag  DW_TAG_member
	.dwattr $C$DW$1095, DW_AT_type(*$C$DW$T$360)
	.dwattr $C$DW$1095, DW_AT_name("sin_addr")
	.dwattr $C$DW$1095, DW_AT_TI_symbol_name("sin_addr")
	.dwattr $C$DW$1095, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1095, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1095, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1095, DW_AT_decl_line(0x264)
	.dwattr $C$DW$1095, DW_AT_decl_column(0x18)

$C$DW$1096	.dwtag  DW_TAG_member
	.dwattr $C$DW$1096, DW_AT_type(*$C$DW$T$362)
	.dwattr $C$DW$1096, DW_AT_name("sin_zero")
	.dwattr $C$DW$1096, DW_AT_TI_symbol_name("sin_zero")
	.dwattr $C$DW$1096, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1096, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1096, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1096, DW_AT_decl_line(0x265)
	.dwattr $C$DW$1096, DW_AT_decl_column(0x18)

	.dwattr $C$DW$T$363, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$363, DW_AT_decl_line(0x260)
	.dwattr $C$DW$T$363, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$363

$C$DW$T$550	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$550, DW_AT_name("SlNetSock_AddrIn_t")
	.dwattr $C$DW$T$550, DW_AT_type(*$C$DW$T$363)
	.dwattr $C$DW$T$550, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$550, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$550, DW_AT_decl_line(0x266)
	.dwattr $C$DW$T$550, DW_AT_decl_column(0x03)


$C$DW$T$365	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$365, DW_AT_name("SlNetSock_Addr_t")
	.dwattr $C$DW$T$365, DW_AT_byte_size(0x10)
$C$DW$1097	.dwtag  DW_TAG_member
	.dwattr $C$DW$1097, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1097, DW_AT_name("sa_family")
	.dwattr $C$DW$1097, DW_AT_TI_symbol_name("sa_family")
	.dwattr $C$DW$1097, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1097, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1097, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1097, DW_AT_decl_line(0x24d)
	.dwattr $C$DW$1097, DW_AT_decl_column(0x0e)

$C$DW$1098	.dwtag  DW_TAG_member
	.dwattr $C$DW$1098, DW_AT_type(*$C$DW$T$364)
	.dwattr $C$DW$1098, DW_AT_name("sa_data")
	.dwattr $C$DW$1098, DW_AT_TI_symbol_name("sa_data")
	.dwattr $C$DW$1098, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1098, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1098, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1098, DW_AT_decl_line(0x24e)
	.dwattr $C$DW$1098, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$365, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$365, DW_AT_decl_line(0x24b)
	.dwattr $C$DW$T$365, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$365

$C$DW$T$294	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$294, DW_AT_name("SlNetSock_Addr_t")
	.dwattr $C$DW$T$294, DW_AT_type(*$C$DW$T$365)
	.dwattr $C$DW$T$294, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$294, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$294, DW_AT_decl_line(0x24f)
	.dwattr $C$DW$T$294, DW_AT_decl_column(0x03)

$C$DW$T$300	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$300, DW_AT_type(*$C$DW$T$294)

$C$DW$T$301	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$301, DW_AT_type(*$C$DW$T$300)
	.dwattr $C$DW$T$301, DW_AT_address_class(0x20)

$C$DW$T$295	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$295, DW_AT_type(*$C$DW$T$294)
	.dwattr $C$DW$T$295, DW_AT_address_class(0x20)


$C$DW$T$366	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$366, DW_AT_name("SlNetSock_In6Addr_t")
	.dwattr $C$DW$T$366, DW_AT_byte_size(0x10)
$C$DW$1099	.dwtag  DW_TAG_member
	.dwattr $C$DW$1099, DW_AT_type(*$C$DW$T$206)
	.dwattr $C$DW$1099, DW_AT_name("_S6_un")
	.dwattr $C$DW$1099, DW_AT_TI_symbol_name("_S6_un")
	.dwattr $C$DW$1099, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1099, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1099, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1099, DW_AT_decl_line(0x1cb)
	.dwattr $C$DW$1099, DW_AT_decl_column(0x07)

	.dwattr $C$DW$T$366, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$366, DW_AT_decl_line(0x1c4)
	.dwattr $C$DW$T$366, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$366

$C$DW$T$358	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$358, DW_AT_name("SlNetSock_In6Addr_t")
	.dwattr $C$DW$T$358, DW_AT_type(*$C$DW$T$366)
	.dwattr $C$DW$T$358, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$358, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$358, DW_AT_decl_line(0x1cc)
	.dwattr $C$DW$T$358, DW_AT_decl_column(0x03)


$C$DW$T$367	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$367, DW_AT_name("SlNetSock_InAddr_t")
	.dwattr $C$DW$T$367, DW_AT_byte_size(0x04)
$C$DW$1100	.dwtag  DW_TAG_member
	.dwattr $C$DW$1100, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1100, DW_AT_name("s_addr")
	.dwattr $C$DW$1100, DW_AT_TI_symbol_name("s_addr")
	.dwattr $C$DW$1100, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1100, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1100, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1100, DW_AT_decl_line(0x1ad)
	.dwattr $C$DW$1100, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$367, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$367, DW_AT_decl_line(0x1aa)
	.dwattr $C$DW$T$367, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$367

$C$DW$T$360	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$360, DW_AT_name("SlNetSock_InAddr_t")
	.dwattr $C$DW$T$360, DW_AT_type(*$C$DW$T$367)
	.dwattr $C$DW$T$360, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$360, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$360, DW_AT_decl_line(0x1bf)
	.dwattr $C$DW$T$360, DW_AT_decl_column(0x03)


$C$DW$T$368	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$368, DW_AT_name("SlNetSock_IpMreq_t")
	.dwattr $C$DW$T$368, DW_AT_byte_size(0x08)
$C$DW$1101	.dwtag  DW_TAG_member
	.dwattr $C$DW$1101, DW_AT_type(*$C$DW$T$360)
	.dwattr $C$DW$1101, DW_AT_name("imr_multiaddr")
	.dwattr $C$DW$1101, DW_AT_TI_symbol_name("imr_multiaddr")
	.dwattr $C$DW$1101, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1101, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1101, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1101, DW_AT_decl_line(0x21b)
	.dwattr $C$DW$1101, DW_AT_decl_column(0x18)

$C$DW$1102	.dwtag  DW_TAG_member
	.dwattr $C$DW$1102, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1102, DW_AT_name("imr_interface")
	.dwattr $C$DW$1102, DW_AT_TI_symbol_name("imr_interface")
	.dwattr $C$DW$1102, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1102, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1102, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1102, DW_AT_decl_line(0x21c)
	.dwattr $C$DW$1102, DW_AT_decl_column(0x18)

	.dwattr $C$DW$T$368, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$368, DW_AT_decl_line(0x219)
	.dwattr $C$DW$T$368, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$368

$C$DW$T$551	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$551, DW_AT_name("SlNetSock_IpMreq_t")
	.dwattr $C$DW$T$551, DW_AT_type(*$C$DW$T$368)
	.dwattr $C$DW$T$551, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$551, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$551, DW_AT_decl_line(0x21d)
	.dwattr $C$DW$T$551, DW_AT_decl_column(0x03)


$C$DW$T$369	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$369, DW_AT_name("SlNetSock_IpV6Mreq_t")
	.dwattr $C$DW$T$369, DW_AT_byte_size(0x14)
$C$DW$1103	.dwtag  DW_TAG_member
	.dwattr $C$DW$1103, DW_AT_type(*$C$DW$T$358)
	.dwattr $C$DW$1103, DW_AT_name("ipv6mr_multiaddr")
	.dwattr $C$DW$1103, DW_AT_TI_symbol_name("ipv6mr_multiaddr")
	.dwattr $C$DW$1103, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1103, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1103, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1103, DW_AT_decl_line(0x224)
	.dwattr $C$DW$1103, DW_AT_decl_column(0x19)

$C$DW$1104	.dwtag  DW_TAG_member
	.dwattr $C$DW$1104, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1104, DW_AT_name("ipv6mr_interface")
	.dwattr $C$DW$1104, DW_AT_TI_symbol_name("ipv6mr_interface")
	.dwattr $C$DW$1104, DW_AT_data_member_location[DW_OP_plus_uconst 0x10]
	.dwattr $C$DW$1104, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1104, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1104, DW_AT_decl_line(0x225)
	.dwattr $C$DW$1104, DW_AT_decl_column(0x19)

	.dwattr $C$DW$T$369, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$369, DW_AT_decl_line(0x222)
	.dwattr $C$DW$T$369, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$369

$C$DW$T$552	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$552, DW_AT_name("SlNetSock_IpV6Mreq_t")
	.dwattr $C$DW$T$552, DW_AT_type(*$C$DW$T$369)
	.dwattr $C$DW$T$552, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$552, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$552, DW_AT_decl_line(0x226)
	.dwattr $C$DW$T$552, DW_AT_decl_column(0x03)


$C$DW$T$370	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$370, DW_AT_name("SlNetSock_Keepalive_t")
	.dwattr $C$DW$T$370, DW_AT_byte_size(0x04)
$C$DW$1105	.dwtag  DW_TAG_member
	.dwattr $C$DW$1105, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1105, DW_AT_name("keepaliveEnabled")
	.dwattr $C$DW$1105, DW_AT_TI_symbol_name("keepaliveEnabled")
	.dwattr $C$DW$1105, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1105, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1105, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1105, DW_AT_decl_line(0x1d3)
	.dwattr $C$DW$1105, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$370, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$370, DW_AT_decl_line(0x1d1)
	.dwattr $C$DW$T$370, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$370

$C$DW$T$553	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$553, DW_AT_name("SlNetSock_Keepalive_t")
	.dwattr $C$DW$T$553, DW_AT_type(*$C$DW$T$370)
	.dwattr $C$DW$T$553, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$553, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$553, DW_AT_decl_line(0x1d4)
	.dwattr $C$DW$T$553, DW_AT_decl_column(0x03)


$C$DW$T$371	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$371, DW_AT_name("SlNetSock_NonIpBoundary_t")
	.dwattr $C$DW$T$371, DW_AT_byte_size(0x04)
$C$DW$1106	.dwtag  DW_TAG_member
	.dwattr $C$DW$1106, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$1106, DW_AT_name("nonIpBoundaryEnabled")
	.dwattr $C$DW$1106, DW_AT_TI_symbol_name("nonIpBoundaryEnabled")
	.dwattr $C$DW$1106, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1106, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1106, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1106, DW_AT_decl_line(0x1db)
	.dwattr $C$DW$1106, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$371, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$371, DW_AT_decl_line(0x1d9)
	.dwattr $C$DW$T$371, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$371

$C$DW$T$554	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$554, DW_AT_name("SlNetSock_NonIpBoundary_t")
	.dwattr $C$DW$T$554, DW_AT_type(*$C$DW$T$371)
	.dwattr $C$DW$T$554, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$554, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$554, DW_AT_decl_line(0x1dc)
	.dwattr $C$DW$T$554, DW_AT_decl_column(0x03)


$C$DW$T$372	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$372, DW_AT_name("SlNetSock_Nonblocking_t")
	.dwattr $C$DW$T$372, DW_AT_byte_size(0x04)
$C$DW$1107	.dwtag  DW_TAG_member
	.dwattr $C$DW$1107, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1107, DW_AT_name("nonBlockingEnabled")
	.dwattr $C$DW$1107, DW_AT_TI_symbol_name("nonBlockingEnabled")
	.dwattr $C$DW$1107, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1107, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1107, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1107, DW_AT_decl_line(0x1eb)
	.dwattr $C$DW$1107, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$372, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$372, DW_AT_decl_line(0x1e9)
	.dwattr $C$DW$T$372, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$372

$C$DW$T$555	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$555, DW_AT_name("SlNetSock_Nonblocking_t")
	.dwattr $C$DW$T$555, DW_AT_type(*$C$DW$T$372)
	.dwattr $C$DW$T$555, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$555, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$555, DW_AT_decl_line(0x1ec)
	.dwattr $C$DW$T$555, DW_AT_decl_column(0x03)


$C$DW$T$374	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$374, DW_AT_name("SlNetSock_SdSet_t")
	.dwattr $C$DW$T$374, DW_AT_byte_size(0x04)
$C$DW$1108	.dwtag  DW_TAG_member
	.dwattr $C$DW$1108, DW_AT_type(*$C$DW$T$373)
	.dwattr $C$DW$1108, DW_AT_name("sdSetBitmap")
	.dwattr $C$DW$1108, DW_AT_TI_symbol_name("sdSetBitmap")
	.dwattr $C$DW$1108, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1108, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1108, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1108, DW_AT_decl_line(0x26d)
	.dwattr $C$DW$1108, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$374, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$374, DW_AT_decl_line(0x26b)
	.dwattr $C$DW$T$374, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$374

$C$DW$T$308	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$308, DW_AT_name("SlNetSock_SdSet_t")
	.dwattr $C$DW$T$308, DW_AT_type(*$C$DW$T$374)
	.dwattr $C$DW$T$308, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$308, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$308, DW_AT_decl_line(0x26e)
	.dwattr $C$DW$T$308, DW_AT_decl_column(0x03)

$C$DW$T$309	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$309, DW_AT_type(*$C$DW$T$308)
	.dwattr $C$DW$T$309, DW_AT_address_class(0x20)


$C$DW$T$378	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$378, DW_AT_name("SlNetSock_SecAttribNode_t")
	.dwattr $C$DW$T$378, DW_AT_byte_size(0x10)
$C$DW$1109	.dwtag  DW_TAG_member
	.dwattr $C$DW$1109, DW_AT_type(*$C$DW$T$376)
	.dwattr $C$DW$1109, DW_AT_name("attribName")
	.dwattr $C$DW$1109, DW_AT_TI_symbol_name("attribName")
	.dwattr $C$DW$1109, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1109, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1109, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1109, DW_AT_decl_line(0x1f3)
	.dwattr $C$DW$1109, DW_AT_decl_column(0x27)

$C$DW$1110	.dwtag  DW_TAG_member
	.dwattr $C$DW$1110, DW_AT_type(*$C$DW$T$347)
	.dwattr $C$DW$1110, DW_AT_name("attribBuff")
	.dwattr $C$DW$1110, DW_AT_TI_symbol_name("attribBuff")
	.dwattr $C$DW$1110, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1110, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1110, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1110, DW_AT_decl_line(0x1f4)
	.dwattr $C$DW$1110, DW_AT_decl_column(0x27)

$C$DW$1111	.dwtag  DW_TAG_member
	.dwattr $C$DW$1111, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$1111, DW_AT_name("attribBuffLen")
	.dwattr $C$DW$1111, DW_AT_TI_symbol_name("attribBuffLen")
	.dwattr $C$DW$1111, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1111, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1111, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1111, DW_AT_decl_line(0x1f5)
	.dwattr $C$DW$1111, DW_AT_decl_column(0x27)

$C$DW$1112	.dwtag  DW_TAG_member
	.dwattr $C$DW$1112, DW_AT_type(*$C$DW$T$377)
	.dwattr $C$DW$1112, DW_AT_name("next")
	.dwattr $C$DW$1112, DW_AT_TI_symbol_name("next")
	.dwattr $C$DW$1112, DW_AT_data_member_location[DW_OP_plus_uconst 0xc]
	.dwattr $C$DW$1112, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1112, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1112, DW_AT_decl_line(0x1f6)
	.dwattr $C$DW$1112, DW_AT_decl_column(0x27)

	.dwattr $C$DW$T$378, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$378, DW_AT_decl_line(0x1f1)
	.dwattr $C$DW$T$378, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$378

$C$DW$T$328	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$328, DW_AT_name("SlNetSock_SecAttribNode_t")
	.dwattr $C$DW$T$328, DW_AT_type(*$C$DW$T$378)
	.dwattr $C$DW$T$328, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$328, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$328, DW_AT_decl_line(0x1f7)
	.dwattr $C$DW$T$328, DW_AT_decl_column(0x03)

$C$DW$T$329	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$329, DW_AT_type(*$C$DW$T$328)
	.dwattr $C$DW$T$329, DW_AT_address_class(0x20)

$C$DW$T$330	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$330, DW_AT_name("SlNetSockSecAttrib_t")
	.dwattr $C$DW$T$330, DW_AT_type(*$C$DW$T$329)
	.dwattr $C$DW$T$330, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$330, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$330, DW_AT_decl_line(0x1fc)
	.dwattr $C$DW$T$330, DW_AT_decl_column(0x25)

$C$DW$T$331	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$331, DW_AT_type(*$C$DW$T$330)
	.dwattr $C$DW$T$331, DW_AT_address_class(0x20)

$C$DW$T$377	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$377, DW_AT_type(*$C$DW$T$378)
	.dwattr $C$DW$T$377, DW_AT_address_class(0x20)


$C$DW$T$379	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$379, DW_AT_name("SlNetSock_SecureALPN_t")
	.dwattr $C$DW$T$379, DW_AT_byte_size(0x04)
$C$DW$1113	.dwtag  DW_TAG_member
	.dwattr $C$DW$1113, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1113, DW_AT_name("secureALPN")
	.dwattr $C$DW$1113, DW_AT_TI_symbol_name("secureALPN")
	.dwattr $C$DW$1113, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1113, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1113, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1113, DW_AT_decl_line(0x203)
	.dwattr $C$DW$1113, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$379, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$379, DW_AT_decl_line(0x201)
	.dwattr $C$DW$T$379, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$379

$C$DW$T$556	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$556, DW_AT_name("SlNetSock_SecureALPN_t")
	.dwattr $C$DW$T$556, DW_AT_type(*$C$DW$T$379)
	.dwattr $C$DW$T$556, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$556, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$556, DW_AT_decl_line(0x204)
	.dwattr $C$DW$T$556, DW_AT_decl_column(0x03)


$C$DW$T$380	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$380, DW_AT_name("SlNetSock_SecureMask_t")
	.dwattr $C$DW$T$380, DW_AT_byte_size(0x04)
$C$DW$1114	.dwtag  DW_TAG_member
	.dwattr $C$DW$1114, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1114, DW_AT_name("secureMask")
	.dwattr $C$DW$1114, DW_AT_TI_symbol_name("secureMask")
	.dwattr $C$DW$1114, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1114, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1114, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1114, DW_AT_decl_line(0x20b)
	.dwattr $C$DW$1114, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$380, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$380, DW_AT_decl_line(0x209)
	.dwattr $C$DW$T$380, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$380

$C$DW$T$557	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$557, DW_AT_name("SlNetSock_SecureMask_t")
	.dwattr $C$DW$T$557, DW_AT_type(*$C$DW$T$380)
	.dwattr $C$DW$T$557, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$557, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$557, DW_AT_decl_line(0x20c)
	.dwattr $C$DW$T$557, DW_AT_decl_column(0x03)


$C$DW$T$381	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$381, DW_AT_name("SlNetSock_SecureMethod_t")
	.dwattr $C$DW$T$381, DW_AT_byte_size(0x01)
$C$DW$1115	.dwtag  DW_TAG_member
	.dwattr $C$DW$1115, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$1115, DW_AT_name("secureMethod")
	.dwattr $C$DW$1115, DW_AT_TI_symbol_name("secureMethod")
	.dwattr $C$DW$1115, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1115, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1115, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1115, DW_AT_decl_line(0x213)
	.dwattr $C$DW$1115, DW_AT_decl_column(0x0d)

	.dwattr $C$DW$T$381, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$381, DW_AT_decl_line(0x211)
	.dwattr $C$DW$T$381, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$381

$C$DW$T$558	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$558, DW_AT_name("SlNetSock_SecureMethod_t")
	.dwattr $C$DW$T$558, DW_AT_type(*$C$DW$T$381)
	.dwattr $C$DW$T$558, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$558, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$558, DW_AT_decl_line(0x214)
	.dwattr $C$DW$T$558, DW_AT_decl_column(0x03)


$C$DW$T$384	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$384, DW_AT_name("SlNetSock_Timeval_t")
	.dwattr $C$DW$T$384, DW_AT_byte_size(0x08)
$C$DW$1116	.dwtag  DW_TAG_member
	.dwattr $C$DW$1116, DW_AT_type(*$C$DW$T$382)
	.dwattr $C$DW$1116, DW_AT_name("tv_sec")
	.dwattr $C$DW$1116, DW_AT_TI_symbol_name("tv_sec")
	.dwattr $C$DW$1116, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1116, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1116, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1116, DW_AT_decl_line(0x23f)
	.dwattr $C$DW$1116, DW_AT_decl_column(0x1a)

$C$DW$1117	.dwtag  DW_TAG_member
	.dwattr $C$DW$1117, DW_AT_type(*$C$DW$T$383)
	.dwattr $C$DW$1117, DW_AT_name("tv_usec")
	.dwattr $C$DW$1117, DW_AT_TI_symbol_name("tv_usec")
	.dwattr $C$DW$1117, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1117, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1117, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1117, DW_AT_decl_line(0x240)
	.dwattr $C$DW$1117, DW_AT_decl_column(0x1a)

	.dwattr $C$DW$T$384, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$384, DW_AT_decl_line(0x23d)
	.dwattr $C$DW$T$384, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$384

$C$DW$T$310	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$310, DW_AT_name("SlNetSock_Timeval_t")
	.dwattr $C$DW$T$310, DW_AT_type(*$C$DW$T$384)
	.dwattr $C$DW$T$310, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$310, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$310, DW_AT_decl_line(0x241)
	.dwattr $C$DW$T$310, DW_AT_decl_column(0x03)

$C$DW$T$311	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$311, DW_AT_type(*$C$DW$T$310)
	.dwattr $C$DW$T$311, DW_AT_address_class(0x20)


$C$DW$T$385	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$385, DW_AT_name("SlNetSock_TransceiverRxOverHead_t")
	.dwattr $C$DW$T$385, DW_AT_byte_size(0x08)
$C$DW$1118	.dwtag  DW_TAG_member
	.dwattr $C$DW$1118, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$1118, DW_AT_name("rate")
	.dwattr $C$DW$1118, DW_AT_TI_symbol_name("rate")
	.dwattr $C$DW$1118, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1118, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1118, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1118, DW_AT_decl_line(0x276)
	.dwattr $C$DW$1118, DW_AT_decl_column(0x0e)

$C$DW$1119	.dwtag  DW_TAG_member
	.dwattr $C$DW$1119, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$1119, DW_AT_name("channel")
	.dwattr $C$DW$1119, DW_AT_TI_symbol_name("channel")
	.dwattr $C$DW$1119, DW_AT_data_member_location[DW_OP_plus_uconst 0x1]
	.dwattr $C$DW$1119, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1119, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1119, DW_AT_decl_line(0x277)
	.dwattr $C$DW$1119, DW_AT_decl_column(0x0e)

$C$DW$1120	.dwtag  DW_TAG_member
	.dwattr $C$DW$1120, DW_AT_type(*$C$DW$T$361)
	.dwattr $C$DW$1120, DW_AT_name("rssi")
	.dwattr $C$DW$1120, DW_AT_TI_symbol_name("rssi")
	.dwattr $C$DW$1120, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1120, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1120, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1120, DW_AT_decl_line(0x278)
	.dwattr $C$DW$1120, DW_AT_decl_column(0x0e)

$C$DW$1121	.dwtag  DW_TAG_member
	.dwattr $C$DW$1121, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$1121, DW_AT_name("padding")
	.dwattr $C$DW$1121, DW_AT_TI_symbol_name("padding")
	.dwattr $C$DW$1121, DW_AT_data_member_location[DW_OP_plus_uconst 0x3]
	.dwattr $C$DW$1121, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1121, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1121, DW_AT_decl_line(0x279)
	.dwattr $C$DW$1121, DW_AT_decl_column(0x0e)

$C$DW$1122	.dwtag  DW_TAG_member
	.dwattr $C$DW$1122, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1122, DW_AT_name("timestamp")
	.dwattr $C$DW$1122, DW_AT_TI_symbol_name("timestamp")
	.dwattr $C$DW$1122, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1122, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1122, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1122, DW_AT_decl_line(0x27a)
	.dwattr $C$DW$1122, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$385, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$385, DW_AT_decl_line(0x274)
	.dwattr $C$DW$T$385, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$385

$C$DW$T$559	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$559, DW_AT_name("SlNetSock_TransceiverRxOverHead_t")
	.dwattr $C$DW$T$559, DW_AT_type(*$C$DW$T$385)
	.dwattr $C$DW$T$559, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$559, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$559, DW_AT_decl_line(0x27b)
	.dwattr $C$DW$T$559, DW_AT_decl_column(0x03)


$C$DW$T$386	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$386, DW_AT_name("SlNetSock_Winsize_t")
	.dwattr $C$DW$T$386, DW_AT_byte_size(0x04)
$C$DW$1123	.dwtag  DW_TAG_member
	.dwattr $C$DW$1123, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1123, DW_AT_name("winSize")
	.dwattr $C$DW$1123, DW_AT_TI_symbol_name("winSize")
	.dwattr $C$DW$1123, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1123, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1123, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1123, DW_AT_decl_line(0x1e3)
	.dwattr $C$DW$1123, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$386, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$386, DW_AT_decl_line(0x1e1)
	.dwattr $C$DW$T$386, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$386

$C$DW$T$560	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$560, DW_AT_name("SlNetSock_Winsize_t")
	.dwattr $C$DW$T$560, DW_AT_type(*$C$DW$T$386)
	.dwattr $C$DW$T$560, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$560, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$560, DW_AT_decl_line(0x1e4)
	.dwattr $C$DW$T$560, DW_AT_decl_column(0x03)


$C$DW$T$387	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$387, DW_AT_name("SlNetSock_linger_t")
	.dwattr $C$DW$T$387, DW_AT_byte_size(0x08)
$C$DW$1124	.dwtag  DW_TAG_member
	.dwattr $C$DW$1124, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1124, DW_AT_name("l_onoff")
	.dwattr $C$DW$1124, DW_AT_TI_symbol_name("l_onoff")
	.dwattr $C$DW$1124, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1124, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1124, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1124, DW_AT_decl_line(0x22d)
	.dwattr $C$DW$1124, DW_AT_decl_column(0x0e)

$C$DW$1125	.dwtag  DW_TAG_member
	.dwattr $C$DW$1125, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$1125, DW_AT_name("l_linger")
	.dwattr $C$DW$1125, DW_AT_TI_symbol_name("l_linger")
	.dwattr $C$DW$1125, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1125, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1125, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$1125, DW_AT_decl_line(0x22e)
	.dwattr $C$DW$1125, DW_AT_decl_column(0x0e)

	.dwattr $C$DW$T$387, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$387, DW_AT_decl_line(0x22b)
	.dwattr $C$DW$T$387, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$387

$C$DW$T$561	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$561, DW_AT_name("SlNetSock_linger_t")
	.dwattr $C$DW$T$561, DW_AT_type(*$C$DW$T$387)
	.dwattr $C$DW$T$561, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$561, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$561, DW_AT_decl_line(0x22f)
	.dwattr $C$DW$T$561, DW_AT_decl_column(0x03)


$C$DW$T$388	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$388, DW_AT_name("SlSockAddrIn6_t")
	.dwattr $C$DW$T$388, DW_AT_byte_size(0x1c)
$C$DW$1126	.dwtag  DW_TAG_member
	.dwattr $C$DW$1126, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$1126, DW_AT_name("sin6_family")
	.dwattr $C$DW$1126, DW_AT_TI_symbol_name("sin6_family")
	.dwattr $C$DW$1126, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1126, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1126, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1126, DW_AT_decl_line(0x18b)
	.dwattr $C$DW$1126, DW_AT_decl_column(0x14)

$C$DW$1127	.dwtag  DW_TAG_member
	.dwattr $C$DW$1127, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$1127, DW_AT_name("sin6_port")
	.dwattr $C$DW$1127, DW_AT_TI_symbol_name("sin6_port")
	.dwattr $C$DW$1127, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1127, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1127, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1127, DW_AT_decl_line(0x18c)
	.dwattr $C$DW$1127, DW_AT_decl_column(0x14)

$C$DW$1128	.dwtag  DW_TAG_member
	.dwattr $C$DW$1128, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$1128, DW_AT_name("sin6_flowinfo")
	.dwattr $C$DW$1128, DW_AT_TI_symbol_name("sin6_flowinfo")
	.dwattr $C$DW$1128, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1128, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1128, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1128, DW_AT_decl_line(0x18d)
	.dwattr $C$DW$1128, DW_AT_decl_column(0x14)

$C$DW$1129	.dwtag  DW_TAG_member
	.dwattr $C$DW$1129, DW_AT_type(*$C$DW$T$66)
	.dwattr $C$DW$1129, DW_AT_name("sin6_addr")
	.dwattr $C$DW$1129, DW_AT_TI_symbol_name("sin6_addr")
	.dwattr $C$DW$1129, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1129, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1129, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1129, DW_AT_decl_line(0x18e)
	.dwattr $C$DW$1129, DW_AT_decl_column(0x14)

$C$DW$1130	.dwtag  DW_TAG_member
	.dwattr $C$DW$1130, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$1130, DW_AT_name("sin6_scope_id")
	.dwattr $C$DW$1130, DW_AT_TI_symbol_name("sin6_scope_id")
	.dwattr $C$DW$1130, DW_AT_data_member_location[DW_OP_plus_uconst 0x18]
	.dwattr $C$DW$1130, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1130, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1130, DW_AT_decl_line(0x18f)
	.dwattr $C$DW$1130, DW_AT_decl_column(0x14)

	.dwattr $C$DW$T$388, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$388, DW_AT_decl_line(0x189)
	.dwattr $C$DW$T$388, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$388

$C$DW$T$562	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$562, DW_AT_name("SlSockAddrIn6_t")
	.dwattr $C$DW$T$562, DW_AT_type(*$C$DW$T$388)
	.dwattr $C$DW$T$562, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$562, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$562, DW_AT_decl_line(0x190)
	.dwattr $C$DW$T$562, DW_AT_decl_column(0x02)


$C$DW$T$389	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$389, DW_AT_name("SlSockAddrIn_t")
	.dwattr $C$DW$T$389, DW_AT_byte_size(0x10)
$C$DW$1131	.dwtag  DW_TAG_member
	.dwattr $C$DW$1131, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$1131, DW_AT_name("sin_family")
	.dwattr $C$DW$1131, DW_AT_TI_symbol_name("sin_family")
	.dwattr $C$DW$1131, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1131, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1131, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1131, DW_AT_decl_line(0x196)
	.dwattr $C$DW$1131, DW_AT_decl_column(0x17)

$C$DW$1132	.dwtag  DW_TAG_member
	.dwattr $C$DW$1132, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$1132, DW_AT_name("sin_port")
	.dwattr $C$DW$1132, DW_AT_TI_symbol_name("sin_port")
	.dwattr $C$DW$1132, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1132, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1132, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1132, DW_AT_decl_line(0x197)
	.dwattr $C$DW$1132, DW_AT_decl_column(0x17)

$C$DW$1133	.dwtag  DW_TAG_member
	.dwattr $C$DW$1133, DW_AT_type(*$C$DW$T$64)
	.dwattr $C$DW$1133, DW_AT_name("sin_addr")
	.dwattr $C$DW$1133, DW_AT_TI_symbol_name("sin_addr")
	.dwattr $C$DW$1133, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1133, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1133, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1133, DW_AT_decl_line(0x198)
	.dwattr $C$DW$1133, DW_AT_decl_column(0x17)

$C$DW$1134	.dwtag  DW_TAG_member
	.dwattr $C$DW$1134, DW_AT_type(*$C$DW$T$58)
	.dwattr $C$DW$1134, DW_AT_name("sin_zero")
	.dwattr $C$DW$1134, DW_AT_TI_symbol_name("sin_zero")
	.dwattr $C$DW$1134, DW_AT_data_member_location[DW_OP_plus_uconst 0x8]
	.dwattr $C$DW$1134, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1134, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1134, DW_AT_decl_line(0x199)
	.dwattr $C$DW$1134, DW_AT_decl_column(0x17)

	.dwattr $C$DW$T$389, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$389, DW_AT_decl_line(0x194)
	.dwattr $C$DW$T$389, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$389

$C$DW$T$563	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$563, DW_AT_name("SlSockAddrIn_t")
	.dwattr $C$DW$T$563, DW_AT_type(*$C$DW$T$389)
	.dwattr $C$DW$T$563, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$563, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$563, DW_AT_decl_line(0x19a)
	.dwattr $C$DW$T$563, DW_AT_decl_column(0x02)


$C$DW$T$391	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$391, DW_AT_name("SlSockAddr_t")
	.dwattr $C$DW$T$391, DW_AT_byte_size(0x10)
$C$DW$1135	.dwtag  DW_TAG_member
	.dwattr $C$DW$1135, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$1135, DW_AT_name("sa_family")
	.dwattr $C$DW$1135, DW_AT_TI_symbol_name("sa_family")
	.dwattr $C$DW$1135, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1135, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1135, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1135, DW_AT_decl_line(0x185)
	.dwattr $C$DW$1135, DW_AT_decl_column(0x13)

$C$DW$1136	.dwtag  DW_TAG_member
	.dwattr $C$DW$1136, DW_AT_type(*$C$DW$T$390)
	.dwattr $C$DW$1136, DW_AT_name("sa_data")
	.dwattr $C$DW$1136, DW_AT_TI_symbol_name("sa_data")
	.dwattr $C$DW$1136, DW_AT_data_member_location[DW_OP_plus_uconst 0x2]
	.dwattr $C$DW$1136, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1136, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1136, DW_AT_decl_line(0x186)
	.dwattr $C$DW$1136, DW_AT_decl_column(0x13)

	.dwattr $C$DW$T$391, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$391, DW_AT_decl_line(0x183)
	.dwattr $C$DW$T$391, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$391

$C$DW$T$564	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$564, DW_AT_name("SlSockAddr_t")
	.dwattr $C$DW$T$564, DW_AT_type(*$C$DW$T$391)
	.dwattr $C$DW$T$564, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$564, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$564, DW_AT_decl_line(0x187)
	.dwattr $C$DW$T$564, DW_AT_decl_column(0x02)

$C$DW$T$565	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$565, DW_AT_type(*$C$DW$T$564)

$C$DW$T$566	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$566, DW_AT_type(*$C$DW$T$565)
	.dwattr $C$DW$T$566, DW_AT_address_class(0x20)

$C$DW$T$567	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$567, DW_AT_type(*$C$DW$T$564)
	.dwattr $C$DW$T$567, DW_AT_address_class(0x20)


$C$DW$T$394	.dwtag  DW_TAG_structure_type
	.dwattr $C$DW$T$394, DW_AT_name("SlTimeval_t")
	.dwattr $C$DW$T$394, DW_AT_byte_size(0x08)
$C$DW$1137	.dwtag  DW_TAG_member
	.dwattr $C$DW$1137, DW_AT_type(*$C$DW$T$392)
	.dwattr $C$DW$1137, DW_AT_name("tv_sec")
	.dwattr $C$DW$1137, DW_AT_TI_symbol_name("tv_sec")
	.dwattr $C$DW$1137, DW_AT_data_member_location[DW_OP_plus_uconst 0x0]
	.dwattr $C$DW$1137, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1137, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1137, DW_AT_decl_line(0x17c)
	.dwattr $C$DW$1137, DW_AT_decl_column(0x17)

$C$DW$1138	.dwtag  DW_TAG_member
	.dwattr $C$DW$1138, DW_AT_type(*$C$DW$T$393)
	.dwattr $C$DW$1138, DW_AT_name("tv_usec")
	.dwattr $C$DW$1138, DW_AT_TI_symbol_name("tv_usec")
	.dwattr $C$DW$1138, DW_AT_data_member_location[DW_OP_plus_uconst 0x4]
	.dwattr $C$DW$1138, DW_AT_accessibility(DW_ACCESS_public)
	.dwattr $C$DW$1138, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$1138, DW_AT_decl_line(0x17d)
	.dwattr $C$DW$1138, DW_AT_decl_column(0x17)

	.dwattr $C$DW$T$394, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$394, DW_AT_decl_line(0x17a)
	.dwattr $C$DW$T$394, DW_AT_decl_column(0x10)
	.dwendtag $C$DW$T$394

$C$DW$T$568	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$568, DW_AT_name("SlTimeval_t")
	.dwattr $C$DW$T$568, DW_AT_type(*$C$DW$T$394)
	.dwattr $C$DW$T$568, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$568, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$568, DW_AT_decl_line(0x17e)
	.dwattr $C$DW$T$568, DW_AT_decl_column(0x02)

$C$DW$T$569	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$569, DW_AT_type(*$C$DW$T$394)
	.dwattr $C$DW$T$569, DW_AT_address_class(0x20)

$C$DW$T$2	.dwtag  DW_TAG_unspecified_type
	.dwattr $C$DW$T$2, DW_AT_name("void")

$C$DW$T$3	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$3, DW_AT_type(*$C$DW$T$2)
	.dwattr $C$DW$T$3, DW_AT_address_class(0x20)

$C$DW$T$570	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$570, DW_AT_name("ClockP_Handle")
	.dwattr $C$DW$T$570, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$T$570, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$570, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$570, DW_AT_decl_line(0x6f)
	.dwattr $C$DW$T$570, DW_AT_decl_column(0x10)

$C$DW$T$571	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$571, DW_AT_name("MutexP_Handle")
	.dwattr $C$DW$T$571, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$T$571, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$571, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/MutexP.h")
	.dwattr $C$DW$T$571, DW_AT_decl_line(0x67)
	.dwattr $C$DW$T$571, DW_AT_decl_column(0x0f)

$C$DW$T$572	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$572, DW_AT_name("SemaphoreP_Handle")
	.dwattr $C$DW$T$572, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$T$572, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$572, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/SemaphoreP.h")
	.dwattr $C$DW$T$572, DW_AT_decl_line(0x72)
	.dwattr $C$DW$T$572, DW_AT_decl_column(0x10)

$C$DW$T$286	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$286, DW_AT_type(*$C$DW$T$3)
	.dwattr $C$DW$T$286, DW_AT_address_class(0x20)


$C$DW$T$269	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$269, DW_AT_language(DW_LANG_C)
	.dwendtag $C$DW$T$269

$C$DW$T$270	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$270, DW_AT_type(*$C$DW$T$269)
	.dwattr $C$DW$T$270, DW_AT_address_class(0x20)

$C$DW$T$576	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$576, DW_AT_name("SL_P_EVENT_HANDLER")
	.dwattr $C$DW$T$576, DW_AT_type(*$C$DW$T$270)
	.dwattr $C$DW$T$576, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$576, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$T$576, DW_AT_decl_line(0x53)
	.dwattr $C$DW$T$576, DW_AT_decl_column(0x10)

$C$DW$T$322	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$322, DW_AT_type(*$C$DW$T$2)

$C$DW$T$323	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$323, DW_AT_type(*$C$DW$T$322)
	.dwattr $C$DW$T$323, DW_AT_address_class(0x20)


$C$DW$T$577	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$577, DW_AT_language(DW_LANG_C)
$C$DW$1139	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1139, DW_AT_type(*$C$DW$T$264)

	.dwendtag $C$DW$T$577

$C$DW$T$578	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$578, DW_AT_type(*$C$DW$T$577)
	.dwattr $C$DW$T$578, DW_AT_address_class(0x20)

$C$DW$T$579	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$579, DW_AT_name("ClockP_Fxn")
	.dwattr $C$DW$T$579, DW_AT_type(*$C$DW$T$578)
	.dwattr $C$DW$T$579, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$579, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/dpl/ClockP.h")
	.dwattr $C$DW$T$579, DW_AT_decl_line(0x78)
	.dwattr $C$DW$T$579, DW_AT_decl_column(0x10)


$C$DW$T$580	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$580, DW_AT_language(DW_LANG_C)
$C$DW$1140	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1140, DW_AT_type(*$C$DW$T$477)

	.dwendtag $C$DW$T$580

$C$DW$T$581	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$581, DW_AT_type(*$C$DW$T$580)
	.dwattr $C$DW$T$581, DW_AT_address_class(0x20)

$C$DW$T$582	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$582, DW_AT_name("P_SL_DEV_PING_CALLBACK")
	.dwattr $C$DW$T$582, DW_AT_type(*$C$DW$T$581)
	.dwattr $C$DW$T$582, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$582, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/netapp.h")
	.dwattr $C$DW$T$582, DW_AT_decl_line(0x228)
	.dwattr $C$DW$T$582, DW_AT_decl_column(0x10)


$C$DW$T$583	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$583, DW_AT_language(DW_LANG_C)
$C$DW$1141	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1141, DW_AT_type(*$C$DW$T$13)

$C$DW$1142	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1142, DW_AT_type(*$C$DW$T$518)

	.dwendtag $C$DW$T$583

$C$DW$T$584	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$584, DW_AT_type(*$C$DW$T$583)
	.dwattr $C$DW$T$584, DW_AT_address_class(0x20)

$C$DW$T$585	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$585, DW_AT_name("P_INIT_CALLBACK")
	.dwattr $C$DW$T$585, DW_AT_type(*$C$DW$T$584)
	.dwattr $C$DW$T$585, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$585, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/device.h")
	.dwattr $C$DW$T$585, DW_AT_decl_line(0x124)
	.dwattr $C$DW$T$585, DW_AT_decl_column(0x10)

$C$DW$T$4	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$4, DW_AT_encoding(DW_ATE_boolean)
	.dwattr $C$DW$T$4, DW_AT_name("bool")
	.dwattr $C$DW$T$4, DW_AT_byte_size(0x01)

$C$DW$T$5	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$5, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr $C$DW$T$5, DW_AT_name("signed char")
	.dwattr $C$DW$T$5, DW_AT_byte_size(0x01)

$C$DW$T$150	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$150, DW_AT_name("SlWlanRxFilterID_t")
	.dwattr $C$DW$T$150, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$150, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$150, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$150, DW_AT_decl_line(0x2d7)
	.dwattr $C$DW$T$150, DW_AT_decl_column(0x11)


$C$DW$T$151	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$151, DW_AT_type(*$C$DW$T$150)
	.dwattr $C$DW$T$151, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$151, DW_AT_byte_size(0x02)
$C$DW$1143	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1143, DW_AT_upper_bound(0x01)

	.dwendtag $C$DW$T$151


$C$DW$T$49	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$49, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$49, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$49, DW_AT_byte_size(0x80)
$C$DW$1144	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1144, DW_AT_upper_bound(0x7f)

	.dwendtag $C$DW$T$49


$C$DW$T$58	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$58, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$58, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$58, DW_AT_byte_size(0x08)
$C$DW$1145	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1145, DW_AT_upper_bound(0x07)

	.dwendtag $C$DW$T$58


$C$DW$T$59	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$59, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$59, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$59, DW_AT_byte_size(0x10)
$C$DW$1146	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1146, DW_AT_upper_bound(0x0f)

	.dwendtag $C$DW$T$59


$C$DW$T$60	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$60, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$60, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$60, DW_AT_byte_size(0x02)
$C$DW$1147	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1147, DW_AT_upper_bound(0x01)

	.dwendtag $C$DW$T$60


$C$DW$T$128	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$128, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$128, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$128, DW_AT_byte_size(0x01)
$C$DW$1148	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1148, DW_AT_upper_bound(0x00)

	.dwendtag $C$DW$T$128

$C$DW$T$130	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$130, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$130, DW_AT_address_class(0x20)


$C$DW$T$133	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$133, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$133, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$133, DW_AT_byte_size(0x40)
$C$DW$1149	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1149, DW_AT_upper_bound(0x3f)

	.dwendtag $C$DW$T$133

$C$DW$T$361	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$361, DW_AT_name("int8_t")
	.dwattr $C$DW$T$361, DW_AT_type(*$C$DW$T$5)
	.dwattr $C$DW$T$361, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$361, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$361, DW_AT_decl_line(0x2a)
	.dwattr $C$DW$T$361, DW_AT_decl_column(0x1d)


$C$DW$T$362	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$362, DW_AT_type(*$C$DW$T$361)
	.dwattr $C$DW$T$362, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$362, DW_AT_byte_size(0x08)
$C$DW$1150	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1150, DW_AT_upper_bound(0x07)

	.dwendtag $C$DW$T$362

$C$DW$T$586	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$586, DW_AT_name("int_least8_t")
	.dwattr $C$DW$T$586, DW_AT_type(*$C$DW$T$361)
	.dwattr $C$DW$T$586, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$586, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$586, DW_AT_decl_line(0x37)
	.dwattr $C$DW$T$586, DW_AT_decl_column(0x17)

$C$DW$T$6	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$6, DW_AT_encoding(DW_ATE_unsigned_char)
	.dwattr $C$DW$T$6, DW_AT_name("unsigned char")
	.dwattr $C$DW$T$6, DW_AT_byte_size(0x01)

$C$DW$T$158	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$158, DW_AT_name("SlWlanRxFilterActionType_t")
	.dwattr $C$DW$T$158, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$158, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$158, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$158, DW_AT_decl_line(0x3bb)
	.dwattr $C$DW$T$158, DW_AT_decl_column(0x0d)

$C$DW$T$153	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$153, DW_AT_name("SlWlanRxFilterCounterId_t")
	.dwattr $C$DW$T$153, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$153, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$153, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$153, DW_AT_decl_line(0x39b)
	.dwattr $C$DW$T$153, DW_AT_decl_column(0x0d)

$C$DW$T$587	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$587, DW_AT_name("SlWlanRxFilterFlags_u")
	.dwattr $C$DW$T$587, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$587, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$587, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$587, DW_AT_decl_line(0x2f8)
	.dwattr $C$DW$T$587, DW_AT_decl_column(0x0d)

$C$DW$T$149	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$149, DW_AT_name("SlWlanRxFilterRuleCombinationOperator_t")
	.dwattr $C$DW$T$149, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$149, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$149, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$149, DW_AT_decl_line(0x36f)
	.dwattr $C$DW$T$149, DW_AT_decl_column(0x0d)

$C$DW$T$147	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$147, DW_AT_name("SlWlanRxFilterRuleHeaderCompareFunction_t")
	.dwattr $C$DW$T$147, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$147, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$147, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$147, DW_AT_decl_line(0x2ff)
	.dwattr $C$DW$T$147, DW_AT_decl_column(0x0d)

$C$DW$T$146	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$146, DW_AT_name("SlWlanRxFilterRuleHeaderField_t")
	.dwattr $C$DW$T$146, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$146, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$146, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$146, DW_AT_decl_line(0x30d)
	.dwattr $C$DW$T$146, DW_AT_decl_column(0x0d)

$C$DW$T$588	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$588, DW_AT_name("SlWlanRxFilterRuleType_t")
	.dwattr $C$DW$T$588, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$588, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$588, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$588, DW_AT_decl_line(0x2f3)
	.dwattr $C$DW$T$588, DW_AT_decl_column(0x0d)

$C$DW$T$589	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$589, DW_AT_name("SlWlanRxFilterSysFilters_t")
	.dwattr $C$DW$T$589, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$589, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$589, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$589, DW_AT_decl_line(0x2df)
	.dwattr $C$DW$T$589, DW_AT_decl_column(0x0e)

$C$DW$T$156	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$156, DW_AT_name("SlWlanRxFilterTriggerCompareFunction_t")
	.dwattr $C$DW$T$156, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$156, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$156, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$156, DW_AT_decl_line(0x306)
	.dwattr $C$DW$T$156, DW_AT_decl_column(0x0d)

$C$DW$T$154	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$154, DW_AT_name("SlWlanRxFilterTriggerConnectionStates_t")
	.dwattr $C$DW$T$154, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$154, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$154, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$154, DW_AT_decl_line(0x393)
	.dwattr $C$DW$T$154, DW_AT_decl_column(0x0d)

$C$DW$T$155	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$155, DW_AT_name("SlWlanRxFilterTriggerRoles_t")
	.dwattr $C$DW$T$155, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$155, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$155, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$155, DW_AT_decl_line(0x38c)
	.dwattr $C$DW$T$155, DW_AT_decl_column(0x0d)

$C$DW$T$590	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$590, DW_AT_name("_SlArgSize_t")
	.dwattr $C$DW$T$590, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$590, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$590, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$590, DW_AT_decl_line(0x17d)
	.dwattr $C$DW$T$590, DW_AT_decl_column(0x0f)


$C$DW$T$19	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$19, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$19, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$19, DW_AT_byte_size(0x20)
$C$DW$1151	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1151, DW_AT_upper_bound(0x1f)

	.dwendtag $C$DW$T$19


$C$DW$T$20	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$20, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$20, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$20, DW_AT_byte_size(0x06)
$C$DW$1152	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1152, DW_AT_upper_bound(0x05)

	.dwendtag $C$DW$T$20


$C$DW$T$241	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$241, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$241, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$241, DW_AT_byte_size(0x0c)
$C$DW$1153	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1153, DW_AT_upper_bound(0x01)

$C$DW$1154	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1154, DW_AT_upper_bound(0x05)

	.dwendtag $C$DW$T$241


$C$DW$T$22	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$22, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$22, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$22, DW_AT_byte_size(0x03)
$C$DW$1155	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1155, DW_AT_upper_bound(0x02)

	.dwendtag $C$DW$T$22


$C$DW$T$36	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$36, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$36, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$36, DW_AT_byte_size(0x02)
$C$DW$1156	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1156, DW_AT_upper_bound(0x01)

	.dwendtag $C$DW$T$36


$C$DW$T$69	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$69, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$69, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$69, DW_AT_byte_size(0x04)
$C$DW$1157	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1157, DW_AT_upper_bound(0x03)

	.dwendtag $C$DW$T$69

$C$DW$T$164	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$164, DW_AT_name("SlWlanRxFilterSysFiltersMask_t")
	.dwattr $C$DW$T$164, DW_AT_type(*$C$DW$T$69)
	.dwattr $C$DW$T$164, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$164, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$164, DW_AT_decl_line(0x2e9)
	.dwattr $C$DW$T$164, DW_AT_decl_column(0x0f)


$C$DW$T$239	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$239, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$239, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$239, DW_AT_byte_size(0x08)
$C$DW$1158	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1158, DW_AT_upper_bound(0x01)

$C$DW$1159	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1159, DW_AT_upper_bound(0x03)

	.dwendtag $C$DW$T$239

$C$DW$T$84	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$84, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$84, DW_AT_address_class(0x20)


$C$DW$T$99	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$99, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$99, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$99, DW_AT_byte_size(0x3c)
$C$DW$1160	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1160, DW_AT_upper_bound(0x3b)

	.dwendtag $C$DW$T$99


$C$DW$T$100	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$100, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$100, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$100, DW_AT_byte_size(0x40)
$C$DW$1161	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1161, DW_AT_upper_bound(0x3f)

	.dwendtag $C$DW$T$100


$C$DW$T$102	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$102, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$102, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$102, DW_AT_byte_size(0x100)
$C$DW$1162	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1162, DW_AT_upper_bound(0xff)

	.dwendtag $C$DW$T$102


$C$DW$T$113	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$113, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$113, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$113, DW_AT_byte_size(0x08)
$C$DW$1163	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1163, DW_AT_upper_bound(0x07)

	.dwendtag $C$DW$T$113


$C$DW$T$136	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$136, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$136, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$136, DW_AT_byte_size(0xfc)
$C$DW$1164	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1164, DW_AT_upper_bound(0xfb)

	.dwendtag $C$DW$T$136


$C$DW$T$141	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$141, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$141, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$141, DW_AT_byte_size(0x10)
$C$DW$1165	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1165, DW_AT_upper_bound(0x0f)

	.dwendtag $C$DW$T$141

$C$DW$T$160	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$160, DW_AT_name("SlWlanRxFilterIdMask_t")
	.dwattr $C$DW$T$160, DW_AT_type(*$C$DW$T$141)
	.dwattr $C$DW$T$160, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$160, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/wlan.h")
	.dwattr $C$DW$T$160, DW_AT_decl_line(0x2dd)
	.dwattr $C$DW$T$160, DW_AT_decl_column(0x0f)


$C$DW$T$240	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$240, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$240, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$240, DW_AT_byte_size(0x20)
$C$DW$1166	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1166, DW_AT_upper_bound(0x01)

$C$DW$1167	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1167, DW_AT_upper_bound(0x0f)

	.dwendtag $C$DW$T$240


$C$DW$T$242	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$242, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$242, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$242, DW_AT_byte_size(0x12)
$C$DW$1168	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1168, DW_AT_upper_bound(0x11)

	.dwendtag $C$DW$T$242


$C$DW$T$243	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$243, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$243, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$243, DW_AT_byte_size(0x24)
$C$DW$1169	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1169, DW_AT_upper_bound(0x01)

$C$DW$1170	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1170, DW_AT_upper_bound(0x11)

	.dwendtag $C$DW$T$243


$C$DW$T$245	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$245, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$245, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$245, DW_AT_byte_size(0x05)
$C$DW$1171	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1171, DW_AT_upper_bound(0x04)

	.dwendtag $C$DW$T$245


$C$DW$T$246	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$246, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$246, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$246, DW_AT_byte_size(0x0a)
$C$DW$1172	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1172, DW_AT_upper_bound(0x01)

$C$DW$1173	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1173, DW_AT_upper_bound(0x04)

	.dwendtag $C$DW$T$246


$C$DW$T$390	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$390, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$390, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$390, DW_AT_byte_size(0x0e)
$C$DW$1174	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1174, DW_AT_upper_bound(0x0d)

	.dwendtag $C$DW$T$390

$C$DW$T$591	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$591, DW_AT_type(*$C$DW$T$6)

$C$DW$T$592	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$592, DW_AT_type(*$C$DW$T$591)
	.dwattr $C$DW$T$592, DW_AT_address_class(0x20)

$C$DW$T$200	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$200, DW_AT_name("uint8_t")
	.dwattr $C$DW$T$200, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$200, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$200, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$200, DW_AT_decl_line(0x2b)
	.dwattr $C$DW$T$200, DW_AT_decl_column(0x1c)


$C$DW$T$201	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$201, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$T$201, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$201, DW_AT_byte_size(0x10)
$C$DW$1175	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1175, DW_AT_upper_bound(0x0f)

	.dwendtag $C$DW$T$201

$C$DW$T$347	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$347, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$T$347, DW_AT_address_class(0x20)


$C$DW$T$364	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$364, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$T$364, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$364, DW_AT_byte_size(0x0e)
$C$DW$1176	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1176, DW_AT_upper_bound(0x0d)

	.dwendtag $C$DW$T$364

$C$DW$T$338	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$338, DW_AT_type(*$C$DW$T$200)

$C$DW$T$593	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$593, DW_AT_name("uint_least8_t")
	.dwattr $C$DW$T$593, DW_AT_type(*$C$DW$T$200)
	.dwattr $C$DW$T$593, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$593, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$593, DW_AT_decl_line(0x38)
	.dwattr $C$DW$T$593, DW_AT_decl_column(0x16)

$C$DW$T$7	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$7, DW_AT_encoding(DW_ATE_signed_char)
	.dwattr $C$DW$T$7, DW_AT_name("wchar_t")
	.dwattr $C$DW$T$7, DW_AT_byte_size(0x02)

$C$DW$T$8	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$8, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$8, DW_AT_name("short")
	.dwattr $C$DW$T$8, DW_AT_byte_size(0x02)

$C$DW$T$594	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$594, DW_AT_name("_SlDataSize_t")
	.dwattr $C$DW$T$594, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$T$594, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$594, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$594, DW_AT_decl_line(0x17e)
	.dwattr $C$DW$T$594, DW_AT_decl_column(0x10)

$C$DW$T$595	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$595, DW_AT_name("_SlReturnVal_t")
	.dwattr $C$DW$T$595, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$T$595, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$595, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$595, DW_AT_decl_line(0x17f)
	.dwattr $C$DW$T$595, DW_AT_decl_column(0x10)


$C$DW$T$596	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$596, DW_AT_type(*$C$DW$T$595)
	.dwattr $C$DW$T$596, DW_AT_language(DW_LANG_C)
$C$DW$1177	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1177, DW_AT_type(*$C$DW$T$3)

	.dwendtag $C$DW$T$596

$C$DW$T$597	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$597, DW_AT_type(*$C$DW$T$596)
	.dwattr $C$DW$T$597, DW_AT_address_class(0x20)

$C$DW$T$598	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$598, DW_AT_name("_SlSpawnEntryFunc_t")
	.dwattr $C$DW$T$598, DW_AT_type(*$C$DW$T$597)
	.dwattr $C$DW$T$598, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$598, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$598, DW_AT_decl_line(0x36a)
	.dwattr $C$DW$T$598, DW_AT_decl_column(0x1a)


$C$DW$T$26	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$26, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$T$26, DW_AT_language(DW_LANG_C)
$C$DW$1178	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1178, DW_AT_type(*$C$DW$T$3)

	.dwendtag $C$DW$T$26

$C$DW$T$27	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$27, DW_AT_type(*$C$DW$T$26)
	.dwattr $C$DW$T$27, DW_AT_address_class(0x20)

$C$DW$T$28	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$28, DW_AT_name("P_OS_SPAWN_ENTRY")
	.dwattr $C$DW$T$28, DW_AT_type(*$C$DW$T$27)
	.dwattr $C$DW$T$28, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$28, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$T$28, DW_AT_decl_line(0x60)
	.dwattr $C$DW$T$28, DW_AT_decl_column(0x18)

$C$DW$T$285	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$285, DW_AT_name("int16_t")
	.dwattr $C$DW$T$285, DW_AT_type(*$C$DW$T$8)
	.dwattr $C$DW$T$285, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$285, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$285, DW_AT_decl_line(0x2c)
	.dwattr $C$DW$T$285, DW_AT_decl_column(0x1d)


$C$DW$T$287	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$287, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$T$287, DW_AT_language(DW_LANG_C)
$C$DW$1179	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1179, DW_AT_type(*$C$DW$T$3)

$C$DW$1180	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1180, DW_AT_type(*$C$DW$T$285)

$C$DW$1181	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1181, DW_AT_type(*$C$DW$T$285)

$C$DW$1182	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1182, DW_AT_type(*$C$DW$T$285)

$C$DW$1183	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1183, DW_AT_type(*$C$DW$T$286)

	.dwendtag $C$DW$T$287

$C$DW$T$288	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$288, DW_AT_type(*$C$DW$T$287)
	.dwattr $C$DW$T$288, DW_AT_address_class(0x20)


$C$DW$T$298	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$298, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$T$298, DW_AT_language(DW_LANG_C)
$C$DW$1184	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1184, DW_AT_type(*$C$DW$T$285)

$C$DW$1185	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1185, DW_AT_type(*$C$DW$T$3)

$C$DW$1186	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1186, DW_AT_type(*$C$DW$T$295)

$C$DW$1187	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1187, DW_AT_type(*$C$DW$T$297)

$C$DW$1188	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1188, DW_AT_type(*$C$DW$T$200)

$C$DW$1189	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1189, DW_AT_type(*$C$DW$T$286)

	.dwendtag $C$DW$T$298

$C$DW$T$299	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$299, DW_AT_type(*$C$DW$T$298)
	.dwattr $C$DW$T$299, DW_AT_address_class(0x20)

$C$DW$T$636	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$636, DW_AT_type(*$C$DW$T$285)


$C$DW$T$637	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$637, DW_AT_type(*$C$DW$T$636)
	.dwattr $C$DW$T$637, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$637, DW_AT_byte_size(0x14)
$C$DW$1190	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1190, DW_AT_upper_bound(0x09)

	.dwendtag $C$DW$T$637


$C$DW$T$638	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$638, DW_AT_type(*$C$DW$T$636)
	.dwattr $C$DW$T$638, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$638, DW_AT_byte_size(0x10)
$C$DW$1191	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1191, DW_AT_upper_bound(0x07)

	.dwendtag $C$DW$T$638

$C$DW$T$640	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$640, DW_AT_name("int_least16_t")
	.dwattr $C$DW$T$640, DW_AT_type(*$C$DW$T$285)
	.dwattr $C$DW$T$640, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$640, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$640, DW_AT_decl_line(0x3a)
	.dwattr $C$DW$T$640, DW_AT_decl_column(0x17)

$C$DW$T$9	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$9, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$9, DW_AT_name("unsigned short")
	.dwattr $C$DW$T$9, DW_AT_byte_size(0x02)

$C$DW$T$607	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$607, DW_AT_name("SlSocklen_t")
	.dwattr $C$DW$T$607, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$607, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$607, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$607, DW_AT_decl_line(0x180)
	.dwattr $C$DW$T$607, DW_AT_decl_column(0x0e)

$C$DW$T$608	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$608, DW_AT_type(*$C$DW$T$607)
	.dwattr $C$DW$T$608, DW_AT_address_class(0x20)

$C$DW$T$641	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$641, DW_AT_name("_SlOpcode_t")
	.dwattr $C$DW$T$641, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$641, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$641, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/simplelink.h")
	.dwattr $C$DW$T$641, DW_AT_decl_line(0x17c)
	.dwattr $C$DW$T$641, DW_AT_decl_column(0x0f)


$C$DW$T$125	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$125, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$125, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$125, DW_AT_byte_size(0x28)
$C$DW$1192	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1192, DW_AT_upper_bound(0x13)

	.dwendtag $C$DW$T$125


$C$DW$T$126	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$126, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$126, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$126, DW_AT_byte_size(0x0c)
$C$DW$1193	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1193, DW_AT_upper_bound(0x05)

	.dwendtag $C$DW$T$126


$C$DW$T$244	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$244, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$244, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$244, DW_AT_byte_size(0x04)
$C$DW$1194	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1194, DW_AT_upper_bound(0x01)

	.dwendtag $C$DW$T$244

$C$DW$T$629	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$629, DW_AT_type(*$C$DW$T$9)

$C$DW$T$633	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$633, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$633, DW_AT_address_class(0x20)

$C$DW$T$202	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$202, DW_AT_name("uint16_t")
	.dwattr $C$DW$T$202, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$202, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$202, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$202, DW_AT_decl_line(0x2d)
	.dwattr $C$DW$T$202, DW_AT_decl_column(0x1c)

$C$DW$T$296	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$296, DW_AT_name("SlNetSocklen_t")
	.dwattr $C$DW$T$296, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$T$296, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$296, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$296, DW_AT_decl_line(0x246)
	.dwattr $C$DW$T$296, DW_AT_decl_column(0x12)

$C$DW$T$297	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$297, DW_AT_type(*$C$DW$T$296)
	.dwattr $C$DW$T$297, DW_AT_address_class(0x20)


$C$DW$T$203	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$203, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$T$203, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$203, DW_AT_byte_size(0x10)
$C$DW$1195	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1195, DW_AT_upper_bound(0x07)

	.dwendtag $C$DW$T$203

$C$DW$T$337	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$337, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$T$337, DW_AT_address_class(0x20)

$C$DW$T$335	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$335, DW_AT_type(*$C$DW$T$202)

$C$DW$T$642	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$642, DW_AT_name("uint_least16_t")
	.dwattr $C$DW$T$642, DW_AT_type(*$C$DW$T$202)
	.dwattr $C$DW$T$642, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$642, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$642, DW_AT_decl_line(0x3b)
	.dwattr $C$DW$T$642, DW_AT_decl_column(0x16)

$C$DW$T$643	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$643, DW_AT_name("wchar_t")
	.dwattr $C$DW$T$643, DW_AT_type(*$C$DW$T$9)
	.dwattr $C$DW$T$643, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$643, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stddef.h")
	.dwattr $C$DW$T$643, DW_AT_decl_line(0x41)
	.dwattr $C$DW$T$643, DW_AT_decl_column(0x1a)

$C$DW$T$10	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$10, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$10, DW_AT_name("int")
	.dwattr $C$DW$T$10, DW_AT_byte_size(0x04)

$C$DW$T$644	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$644, DW_AT_name("Fd_t")
	.dwattr $C$DW$T$644, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$644, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$644, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/cc_pal.h")
	.dwattr $C$DW$T$644, DW_AT_decl_line(0x47)
	.dwattr $C$DW$T$644, DW_AT_decl_column(0x0d)

$C$DW$T$645	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$645, DW_AT_name("_SlFd_t")
	.dwattr $C$DW$T$645, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$645, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$645, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/porting/user.h")
	.dwattr $C$DW$T$645, DW_AT_decl_line(0x34)
	.dwattr $C$DW$T$645, DW_AT_decl_column(0x14)

$C$DW$T$289	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$289, DW_AT_name("int32_t")
	.dwattr $C$DW$T$289, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$289, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$289, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$289, DW_AT_decl_line(0x2e)
	.dwattr $C$DW$T$289, DW_AT_decl_column(0x1d)

$C$DW$T$382	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$382, DW_AT_name("SlNetSockTime_t")
	.dwattr $C$DW$T$382, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$382, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$382, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$382, DW_AT_decl_line(0x234)
	.dwattr $C$DW$T$382, DW_AT_decl_column(0x11)

$C$DW$T$383	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$383, DW_AT_name("SlNetSockuseconds_t")
	.dwattr $C$DW$T$383, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$383, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$383, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/net/slnetsock.h")
	.dwattr $C$DW$T$383, DW_AT_decl_line(0x238)
	.dwattr $C$DW$T$383, DW_AT_decl_column(0x11)


$C$DW$T$290	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$290, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$290, DW_AT_language(DW_LANG_C)
$C$DW$1196	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1196, DW_AT_type(*$C$DW$T$285)

$C$DW$1197	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1197, DW_AT_type(*$C$DW$T$3)

	.dwendtag $C$DW$T$290

$C$DW$T$291	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$291, DW_AT_type(*$C$DW$T$290)
	.dwattr $C$DW$T$291, DW_AT_address_class(0x20)


$C$DW$T$292	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$292, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$292, DW_AT_language(DW_LANG_C)
$C$DW$1198	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1198, DW_AT_type(*$C$DW$T$285)

$C$DW$1199	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1199, DW_AT_type(*$C$DW$T$3)

$C$DW$1200	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1200, DW_AT_type(*$C$DW$T$285)

	.dwendtag $C$DW$T$292

$C$DW$T$293	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$293, DW_AT_type(*$C$DW$T$292)
	.dwattr $C$DW$T$293, DW_AT_address_class(0x20)


$C$DW$T$302	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$302, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$302, DW_AT_language(DW_LANG_C)
$C$DW$1201	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1201, DW_AT_type(*$C$DW$T$285)

$C$DW$1202	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1202, DW_AT_type(*$C$DW$T$3)

$C$DW$1203	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1203, DW_AT_type(*$C$DW$T$301)

$C$DW$1204	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1204, DW_AT_type(*$C$DW$T$285)

	.dwendtag $C$DW$T$302

$C$DW$T$303	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$303, DW_AT_type(*$C$DW$T$302)
	.dwattr $C$DW$T$303, DW_AT_address_class(0x20)


$C$DW$T$304	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$304, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$304, DW_AT_language(DW_LANG_C)
$C$DW$1205	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1205, DW_AT_type(*$C$DW$T$285)

$C$DW$1206	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1206, DW_AT_type(*$C$DW$T$3)

$C$DW$1207	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1207, DW_AT_type(*$C$DW$T$301)

$C$DW$1208	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1208, DW_AT_type(*$C$DW$T$296)

$C$DW$1209	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1209, DW_AT_type(*$C$DW$T$200)

	.dwendtag $C$DW$T$304

$C$DW$T$305	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$305, DW_AT_type(*$C$DW$T$304)
	.dwattr $C$DW$T$305, DW_AT_address_class(0x20)


$C$DW$T$306	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$306, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$306, DW_AT_language(DW_LANG_C)
$C$DW$1210	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1210, DW_AT_type(*$C$DW$T$285)

$C$DW$1211	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1211, DW_AT_type(*$C$DW$T$3)

$C$DW$1212	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1212, DW_AT_type(*$C$DW$T$295)

$C$DW$1213	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1213, DW_AT_type(*$C$DW$T$297)

	.dwendtag $C$DW$T$306

$C$DW$T$307	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$307, DW_AT_type(*$C$DW$T$306)
	.dwattr $C$DW$T$307, DW_AT_address_class(0x20)


$C$DW$T$312	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$312, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$312, DW_AT_language(DW_LANG_C)
$C$DW$1214	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1214, DW_AT_type(*$C$DW$T$3)

$C$DW$1215	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1215, DW_AT_type(*$C$DW$T$285)

$C$DW$1216	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1216, DW_AT_type(*$C$DW$T$309)

$C$DW$1217	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1217, DW_AT_type(*$C$DW$T$309)

$C$DW$1218	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1218, DW_AT_type(*$C$DW$T$309)

$C$DW$1219	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1219, DW_AT_type(*$C$DW$T$311)

	.dwendtag $C$DW$T$312

$C$DW$T$313	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$313, DW_AT_type(*$C$DW$T$312)
	.dwattr $C$DW$T$313, DW_AT_address_class(0x20)


$C$DW$T$314	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$314, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$314, DW_AT_language(DW_LANG_C)
$C$DW$1220	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1220, DW_AT_type(*$C$DW$T$285)

$C$DW$1221	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1221, DW_AT_type(*$C$DW$T$3)

$C$DW$1222	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1222, DW_AT_type(*$C$DW$T$285)

$C$DW$1223	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1223, DW_AT_type(*$C$DW$T$285)

$C$DW$1224	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1224, DW_AT_type(*$C$DW$T$3)

$C$DW$1225	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1225, DW_AT_type(*$C$DW$T$296)

	.dwendtag $C$DW$T$314

$C$DW$T$315	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$315, DW_AT_type(*$C$DW$T$314)
	.dwattr $C$DW$T$315, DW_AT_address_class(0x20)


$C$DW$T$316	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$316, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$316, DW_AT_language(DW_LANG_C)
$C$DW$1226	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1226, DW_AT_type(*$C$DW$T$285)

$C$DW$1227	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1227, DW_AT_type(*$C$DW$T$3)

$C$DW$1228	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1228, DW_AT_type(*$C$DW$T$285)

$C$DW$1229	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1229, DW_AT_type(*$C$DW$T$285)

$C$DW$1230	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1230, DW_AT_type(*$C$DW$T$3)

$C$DW$1231	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1231, DW_AT_type(*$C$DW$T$297)

	.dwendtag $C$DW$T$316

$C$DW$T$317	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$317, DW_AT_type(*$C$DW$T$316)
	.dwattr $C$DW$T$317, DW_AT_address_class(0x20)


$C$DW$T$318	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$318, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$318, DW_AT_language(DW_LANG_C)
$C$DW$1232	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1232, DW_AT_type(*$C$DW$T$285)

$C$DW$1233	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1233, DW_AT_type(*$C$DW$T$3)

$C$DW$1234	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1234, DW_AT_type(*$C$DW$T$3)

$C$DW$1235	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1235, DW_AT_type(*$C$DW$T$204)

$C$DW$1236	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1236, DW_AT_type(*$C$DW$T$204)

	.dwendtag $C$DW$T$318

$C$DW$T$319	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$319, DW_AT_type(*$C$DW$T$318)
	.dwattr $C$DW$T$319, DW_AT_address_class(0x20)


$C$DW$T$320	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$320, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$320, DW_AT_language(DW_LANG_C)
$C$DW$1237	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1237, DW_AT_type(*$C$DW$T$285)

$C$DW$1238	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1238, DW_AT_type(*$C$DW$T$3)

$C$DW$1239	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1239, DW_AT_type(*$C$DW$T$3)

$C$DW$1240	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1240, DW_AT_type(*$C$DW$T$204)

$C$DW$1241	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1241, DW_AT_type(*$C$DW$T$204)

$C$DW$1242	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1242, DW_AT_type(*$C$DW$T$295)

$C$DW$1243	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1243, DW_AT_type(*$C$DW$T$297)

	.dwendtag $C$DW$T$320

$C$DW$T$321	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$321, DW_AT_type(*$C$DW$T$320)
	.dwattr $C$DW$T$321, DW_AT_address_class(0x20)


$C$DW$T$324	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$324, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$324, DW_AT_language(DW_LANG_C)
$C$DW$1244	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1244, DW_AT_type(*$C$DW$T$285)

$C$DW$1245	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1245, DW_AT_type(*$C$DW$T$3)

$C$DW$1246	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1246, DW_AT_type(*$C$DW$T$323)

$C$DW$1247	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1247, DW_AT_type(*$C$DW$T$204)

$C$DW$1248	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1248, DW_AT_type(*$C$DW$T$204)

	.dwendtag $C$DW$T$324

$C$DW$T$325	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$325, DW_AT_type(*$C$DW$T$324)
	.dwattr $C$DW$T$325, DW_AT_address_class(0x20)


$C$DW$T$326	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$326, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$326, DW_AT_language(DW_LANG_C)
$C$DW$1249	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1249, DW_AT_type(*$C$DW$T$285)

$C$DW$1250	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1250, DW_AT_type(*$C$DW$T$3)

$C$DW$1251	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1251, DW_AT_type(*$C$DW$T$323)

$C$DW$1252	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1252, DW_AT_type(*$C$DW$T$204)

$C$DW$1253	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1253, DW_AT_type(*$C$DW$T$204)

$C$DW$1254	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1254, DW_AT_type(*$C$DW$T$301)

$C$DW$1255	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1255, DW_AT_type(*$C$DW$T$296)

	.dwendtag $C$DW$T$326

$C$DW$T$327	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$327, DW_AT_type(*$C$DW$T$326)
	.dwattr $C$DW$T$327, DW_AT_address_class(0x20)


$C$DW$T$332	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$332, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$332, DW_AT_language(DW_LANG_C)
$C$DW$1256	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1256, DW_AT_type(*$C$DW$T$285)

$C$DW$1257	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1257, DW_AT_type(*$C$DW$T$3)

$C$DW$1258	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1258, DW_AT_type(*$C$DW$T$331)

$C$DW$1259	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1259, DW_AT_type(*$C$DW$T$200)

	.dwendtag $C$DW$T$332

$C$DW$T$333	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$333, DW_AT_type(*$C$DW$T$332)
	.dwattr $C$DW$T$333, DW_AT_address_class(0x20)


$C$DW$T$339	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$339, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$339, DW_AT_language(DW_LANG_C)
$C$DW$1260	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1260, DW_AT_type(*$C$DW$T$3)

$C$DW$1261	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1261, DW_AT_type(*$C$DW$T$334)

$C$DW$1262	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1262, DW_AT_type(*$C$DW$T$335)

$C$DW$1263	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1263, DW_AT_type(*$C$DW$T$336)

$C$DW$1264	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1264, DW_AT_type(*$C$DW$T$337)

$C$DW$1265	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1265, DW_AT_type(*$C$DW$T$338)

	.dwendtag $C$DW$T$339

$C$DW$T$340	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$340, DW_AT_type(*$C$DW$T$339)
	.dwattr $C$DW$T$340, DW_AT_address_class(0x20)


$C$DW$T$343	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$343, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$343, DW_AT_language(DW_LANG_C)
$C$DW$1266	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1266, DW_AT_type(*$C$DW$T$3)

$C$DW$1267	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1267, DW_AT_type(*$C$DW$T$342)

$C$DW$1268	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1268, DW_AT_type(*$C$DW$T$337)

$C$DW$1269	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1269, DW_AT_type(*$C$DW$T$336)

	.dwendtag $C$DW$T$343

$C$DW$T$344	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$344, DW_AT_type(*$C$DW$T$343)
	.dwattr $C$DW$T$344, DW_AT_address_class(0x20)


$C$DW$T$345	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$345, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$345, DW_AT_language(DW_LANG_C)
$C$DW$1270	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1270, DW_AT_type(*$C$DW$T$3)

	.dwendtag $C$DW$T$345

$C$DW$T$346	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$346, DW_AT_type(*$C$DW$T$345)
	.dwattr $C$DW$T$346, DW_AT_address_class(0x20)


$C$DW$T$348	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$348, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$348, DW_AT_language(DW_LANG_C)
$C$DW$1271	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1271, DW_AT_type(*$C$DW$T$3)

$C$DW$1272	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1272, DW_AT_type(*$C$DW$T$202)

$C$DW$1273	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1273, DW_AT_type(*$C$DW$T$334)

$C$DW$1274	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1274, DW_AT_type(*$C$DW$T$285)

$C$DW$1275	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1275, DW_AT_type(*$C$DW$T$347)

$C$DW$1276	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1276, DW_AT_type(*$C$DW$T$285)

	.dwendtag $C$DW$T$348

$C$DW$T$349	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$349, DW_AT_type(*$C$DW$T$348)
	.dwattr $C$DW$T$349, DW_AT_address_class(0x20)


$C$DW$T$352	.dwtag  DW_TAG_subroutine_type
	.dwattr $C$DW$T$352, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$352, DW_AT_language(DW_LANG_C)
$C$DW$1277	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1277, DW_AT_type(*$C$DW$T$202)

$C$DW$1278	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1278, DW_AT_type(*$C$DW$T$351)

$C$DW$1279	.dwtag  DW_TAG_formal_parameter
	.dwattr $C$DW$1279, DW_AT_type(*$C$DW$T$286)

	.dwendtag $C$DW$T$352

$C$DW$T$353	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$353, DW_AT_type(*$C$DW$T$352)
	.dwattr $C$DW$T$353, DW_AT_address_class(0x20)

$C$DW$T$646	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$646, DW_AT_name("int_fast16_t")
	.dwattr $C$DW$T$646, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$646, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$646, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$646, DW_AT_decl_line(0x47)
	.dwattr $C$DW$T$646, DW_AT_decl_column(0x17)

$C$DW$T$647	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$647, DW_AT_name("int_fast32_t")
	.dwattr $C$DW$T$647, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$647, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$647, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$647, DW_AT_decl_line(0x4a)
	.dwattr $C$DW$T$647, DW_AT_decl_column(0x17)

$C$DW$T$648	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$648, DW_AT_name("int_fast8_t")
	.dwattr $C$DW$T$648, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$648, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$648, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$648, DW_AT_decl_line(0x45)
	.dwattr $C$DW$T$648, DW_AT_decl_column(0x17)

$C$DW$T$649	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$649, DW_AT_name("int_least32_t")
	.dwattr $C$DW$T$649, DW_AT_type(*$C$DW$T$289)
	.dwattr $C$DW$T$649, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$649, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$649, DW_AT_decl_line(0x3c)
	.dwattr $C$DW$T$649, DW_AT_decl_column(0x17)

$C$DW$T$650	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$650, DW_AT_name("intptr_t")
	.dwattr $C$DW$T$650, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$650, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$650, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$650, DW_AT_decl_line(0x52)
	.dwattr $C$DW$T$650, DW_AT_decl_column(0x1a)

$C$DW$T$651	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$651, DW_AT_name("ptrdiff_t")
	.dwattr $C$DW$T$651, DW_AT_type(*$C$DW$T$10)
	.dwattr $C$DW$T$651, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$651, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stddef.h")
	.dwattr $C$DW$T$651, DW_AT_decl_line(0x36)
	.dwattr $C$DW$T$651, DW_AT_decl_column(0x1c)

$C$DW$T$11	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$11, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$11, DW_AT_name("unsigned int")
	.dwattr $C$DW$T$11, DW_AT_byte_size(0x04)

$C$DW$T$573	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$573, DW_AT_name("size_t")
	.dwattr $C$DW$T$573, DW_AT_type(*$C$DW$T$11)
	.dwattr $C$DW$T$573, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$573, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/string.h")
	.dwattr $C$DW$T$573, DW_AT_decl_line(0x3f)
	.dwattr $C$DW$T$573, DW_AT_decl_column(0x19)

$C$DW$T$204	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$204, DW_AT_name("uint32_t")
	.dwattr $C$DW$T$204, DW_AT_type(*$C$DW$T$11)
	.dwattr $C$DW$T$204, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$204, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$204, DW_AT_decl_line(0x2f)
	.dwattr $C$DW$T$204, DW_AT_decl_column(0x1c)


$C$DW$T$205	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$205, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$205, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$205, DW_AT_byte_size(0x10)
$C$DW$1280	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1280, DW_AT_upper_bound(0x03)

	.dwendtag $C$DW$T$205

$C$DW$T$336	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$336, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$336, DW_AT_address_class(0x20)


$C$DW$T$373	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$373, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$373, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$373, DW_AT_byte_size(0x04)
$C$DW$1281	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1281, DW_AT_upper_bound(0x00)

	.dwendtag $C$DW$T$373

$C$DW$T$652	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$652, DW_AT_name("uint_fast16_t")
	.dwattr $C$DW$T$652, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$652, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$652, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$652, DW_AT_decl_line(0x48)
	.dwattr $C$DW$T$652, DW_AT_decl_column(0x16)

$C$DW$T$653	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$653, DW_AT_name("uint_fast32_t")
	.dwattr $C$DW$T$653, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$653, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$653, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$653, DW_AT_decl_line(0x4b)
	.dwattr $C$DW$T$653, DW_AT_decl_column(0x16)

$C$DW$T$654	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$654, DW_AT_name("uint_fast8_t")
	.dwattr $C$DW$T$654, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$654, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$654, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$654, DW_AT_decl_line(0x46)
	.dwattr $C$DW$T$654, DW_AT_decl_column(0x16)

$C$DW$T$655	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$655, DW_AT_name("uint_least32_t")
	.dwattr $C$DW$T$655, DW_AT_type(*$C$DW$T$204)
	.dwattr $C$DW$T$655, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$655, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$655, DW_AT_decl_line(0x3d)
	.dwattr $C$DW$T$655, DW_AT_decl_column(0x16)

$C$DW$T$264	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$264, DW_AT_name("uintptr_t")
	.dwattr $C$DW$T$264, DW_AT_type(*$C$DW$T$11)
	.dwattr $C$DW$T$264, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$264, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$264, DW_AT_decl_line(0x53)
	.dwattr $C$DW$T$264, DW_AT_decl_column(0x1a)

$C$DW$T$12	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$12, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$12, DW_AT_name("long")
	.dwattr $C$DW$T$12, DW_AT_byte_size(0x04)

$C$DW$T$393	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$393, DW_AT_name("SlSuseconds_t")
	.dwattr $C$DW$T$393, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$T$393, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$393, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$393, DW_AT_decl_line(0x178)
	.dwattr $C$DW$T$393, DW_AT_decl_column(0x10)

$C$DW$T$392	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$392, DW_AT_name("SlTime_t")
	.dwattr $C$DW$T$392, DW_AT_type(*$C$DW$T$12)
	.dwattr $C$DW$T$392, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$392, DW_AT_decl_file("/vagrant/Q4_04/sdk_root/source/ti/drivers/net/wifi/sl_socket.h")
	.dwattr $C$DW$T$392, DW_AT_decl_line(0x177)
	.dwattr $C$DW$T$392, DW_AT_decl_column(0x10)

$C$DW$T$599	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$599, DW_AT_type(*$C$DW$T$12)

$C$DW$T$13	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$13, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$13, DW_AT_name("unsigned long")
	.dwattr $C$DW$T$13, DW_AT_byte_size(0x04)


$C$DW$T$30	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$30, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$T$30, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$30, DW_AT_byte_size(0x10)
$C$DW$1282	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1282, DW_AT_upper_bound(0x03)

	.dwendtag $C$DW$T$30


$C$DW$T$178	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$178, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$T$178, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$178, DW_AT_byte_size(0x0c)
$C$DW$1283	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1283, DW_AT_upper_bound(0x02)

	.dwendtag $C$DW$T$178


$C$DW$T$182	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$182, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$T$182, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$182, DW_AT_byte_size(0x08)
$C$DW$1284	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1284, DW_AT_upper_bound(0x01)

	.dwendtag $C$DW$T$182


$C$DW$T$281	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$281, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$T$281, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$281, DW_AT_byte_size(0x04)
$C$DW$1285	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1285, DW_AT_upper_bound(0x00)

	.dwendtag $C$DW$T$281

$C$DW$T$600	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$600, DW_AT_type(*$C$DW$T$13)

$C$DW$T$630	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$630, DW_AT_type(*$C$DW$T$13)
	.dwattr $C$DW$T$630, DW_AT_address_class(0x20)

$C$DW$T$14	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$14, DW_AT_encoding(DW_ATE_signed)
	.dwattr $C$DW$T$14, DW_AT_name("long long")
	.dwattr $C$DW$T$14, DW_AT_byte_size(0x08)

$C$DW$T$660	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$660, DW_AT_name("int64_t")
	.dwattr $C$DW$T$660, DW_AT_type(*$C$DW$T$14)
	.dwattr $C$DW$T$660, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$660, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$660, DW_AT_decl_line(0x32)
	.dwattr $C$DW$T$660, DW_AT_decl_column(0x21)

$C$DW$T$661	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$661, DW_AT_name("int_fast64_t")
	.dwattr $C$DW$T$661, DW_AT_type(*$C$DW$T$660)
	.dwattr $C$DW$T$661, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$661, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$661, DW_AT_decl_line(0x4e)
	.dwattr $C$DW$T$661, DW_AT_decl_column(0x17)

$C$DW$T$662	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$662, DW_AT_name("int_least64_t")
	.dwattr $C$DW$T$662, DW_AT_type(*$C$DW$T$660)
	.dwattr $C$DW$T$662, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$662, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$662, DW_AT_decl_line(0x40)
	.dwattr $C$DW$T$662, DW_AT_decl_column(0x17)

$C$DW$T$663	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$663, DW_AT_name("intmax_t")
	.dwattr $C$DW$T$663, DW_AT_type(*$C$DW$T$14)
	.dwattr $C$DW$T$663, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$663, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$663, DW_AT_decl_line(0x56)
	.dwattr $C$DW$T$663, DW_AT_decl_column(0x20)

$C$DW$T$15	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$15, DW_AT_encoding(DW_ATE_unsigned)
	.dwattr $C$DW$T$15, DW_AT_name("unsigned long long")
	.dwattr $C$DW$T$15, DW_AT_byte_size(0x08)

$C$DW$T$664	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$664, DW_AT_name("uint64_t")
	.dwattr $C$DW$T$664, DW_AT_type(*$C$DW$T$15)
	.dwattr $C$DW$T$664, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$664, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$664, DW_AT_decl_line(0x33)
	.dwattr $C$DW$T$664, DW_AT_decl_column(0x20)

$C$DW$T$665	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$665, DW_AT_name("uint_fast64_t")
	.dwattr $C$DW$T$665, DW_AT_type(*$C$DW$T$664)
	.dwattr $C$DW$T$665, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$665, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$665, DW_AT_decl_line(0x4f)
	.dwattr $C$DW$T$665, DW_AT_decl_column(0x16)

$C$DW$T$666	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$666, DW_AT_name("uint_least64_t")
	.dwattr $C$DW$T$666, DW_AT_type(*$C$DW$T$664)
	.dwattr $C$DW$T$666, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$666, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$666, DW_AT_decl_line(0x41)
	.dwattr $C$DW$T$666, DW_AT_decl_column(0x16)

$C$DW$T$667	.dwtag  DW_TAG_typedef
	.dwattr $C$DW$T$667, DW_AT_name("uintmax_t")
	.dwattr $C$DW$T$667, DW_AT_type(*$C$DW$T$15)
	.dwattr $C$DW$T$667, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$667, DW_AT_decl_file("/opt/ti/ccs-latest/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include/stdint.h")
	.dwattr $C$DW$T$667, DW_AT_decl_line(0x57)
	.dwattr $C$DW$T$667, DW_AT_decl_column(0x20)

$C$DW$T$16	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$16, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$16, DW_AT_name("float")
	.dwattr $C$DW$T$16, DW_AT_byte_size(0x04)

$C$DW$T$17	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$17, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$17, DW_AT_name("double")
	.dwattr $C$DW$T$17, DW_AT_byte_size(0x08)

$C$DW$T$18	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$18, DW_AT_encoding(DW_ATE_float)
	.dwattr $C$DW$T$18, DW_AT_name("long double")
	.dwattr $C$DW$T$18, DW_AT_byte_size(0x08)

$C$DW$T$263	.dwtag  DW_TAG_base_type
	.dwattr $C$DW$T$263, DW_AT_encoding(DW_ATE_unsigned_char)
	.dwattr $C$DW$T$263, DW_AT_name("unsigned char")
	.dwattr $C$DW$T$263, DW_AT_byte_size(0x01)


$C$DW$T$267	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$267, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$267, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$267, DW_AT_byte_size(0x24)
$C$DW$1286	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1286, DW_AT_upper_bound(0x23)

	.dwendtag $C$DW$T$267


$C$DW$T$272	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$272, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$272, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$272, DW_AT_byte_size(0x28)
$C$DW$1287	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1287, DW_AT_upper_bound(0x27)

	.dwendtag $C$DW$T$272


$C$DW$T$277	.dwtag  DW_TAG_array_type
	.dwattr $C$DW$T$277, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$277, DW_AT_language(DW_LANG_C)
	.dwattr $C$DW$T$277, DW_AT_byte_size(0x1c)
$C$DW$1288	.dwtag  DW_TAG_subrange_type
	.dwattr $C$DW$1288, DW_AT_upper_bound(0x1b)

	.dwendtag $C$DW$T$277

$C$DW$T$334	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$334, DW_AT_type(*$C$DW$T$6)
	.dwattr $C$DW$T$334, DW_AT_address_class(0x20)

$C$DW$T$350	.dwtag  DW_TAG_const_type
	.dwattr $C$DW$T$350, DW_AT_type(*$C$DW$T$6)

$C$DW$T$351	.dwtag  DW_TAG_pointer_type
	.dwattr $C$DW$T$351, DW_AT_type(*$C$DW$T$350)
	.dwattr $C$DW$T$351, DW_AT_address_class(0x20)

	.dwattr $C$DW$CU, DW_AT_language(DW_LANG_C)

;***************************************************************
;* DWARF CIE ENTRIES                                           *
;***************************************************************

$C$DW$CIE	.dwcie 14
	.dwcfi	cfa_register, 13
	.dwcfi	cfa_offset, 0
	.dwcfi	same_value, 4
	.dwcfi	same_value, 5
	.dwcfi	same_value, 6
	.dwcfi	same_value, 7
	.dwcfi	same_value, 8
	.dwcfi	same_value, 9
	.dwcfi	same_value, 10
	.dwcfi	same_value, 11
	.dwendentry

;***************************************************************
;* DWARF REGISTER MAP                                          *
;***************************************************************

$C$DW$1289	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1289, DW_AT_name("A1")
	.dwattr $C$DW$1289, DW_AT_location[DW_OP_reg0]

$C$DW$1290	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1290, DW_AT_name("A2")
	.dwattr $C$DW$1290, DW_AT_location[DW_OP_reg1]

$C$DW$1291	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1291, DW_AT_name("A3")
	.dwattr $C$DW$1291, DW_AT_location[DW_OP_reg2]

$C$DW$1292	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1292, DW_AT_name("A4")
	.dwattr $C$DW$1292, DW_AT_location[DW_OP_reg3]

$C$DW$1293	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1293, DW_AT_name("V1")
	.dwattr $C$DW$1293, DW_AT_location[DW_OP_reg4]

$C$DW$1294	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1294, DW_AT_name("V2")
	.dwattr $C$DW$1294, DW_AT_location[DW_OP_reg5]

$C$DW$1295	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1295, DW_AT_name("V3")
	.dwattr $C$DW$1295, DW_AT_location[DW_OP_reg6]

$C$DW$1296	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1296, DW_AT_name("V4")
	.dwattr $C$DW$1296, DW_AT_location[DW_OP_reg7]

$C$DW$1297	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1297, DW_AT_name("V5")
	.dwattr $C$DW$1297, DW_AT_location[DW_OP_reg8]

$C$DW$1298	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1298, DW_AT_name("V6")
	.dwattr $C$DW$1298, DW_AT_location[DW_OP_reg9]

$C$DW$1299	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1299, DW_AT_name("V7")
	.dwattr $C$DW$1299, DW_AT_location[DW_OP_reg10]

$C$DW$1300	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1300, DW_AT_name("V8")
	.dwattr $C$DW$1300, DW_AT_location[DW_OP_reg11]

$C$DW$1301	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1301, DW_AT_name("V9")
	.dwattr $C$DW$1301, DW_AT_location[DW_OP_reg12]

$C$DW$1302	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1302, DW_AT_name("SP")
	.dwattr $C$DW$1302, DW_AT_location[DW_OP_reg13]

$C$DW$1303	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1303, DW_AT_name("LR")
	.dwattr $C$DW$1303, DW_AT_location[DW_OP_reg14]

$C$DW$1304	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1304, DW_AT_name("PC")
	.dwattr $C$DW$1304, DW_AT_location[DW_OP_reg15]

$C$DW$1305	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1305, DW_AT_name("SR")
	.dwattr $C$DW$1305, DW_AT_location[DW_OP_reg17]

$C$DW$1306	.dwtag  DW_TAG_TI_assign_register
	.dwattr $C$DW$1306, DW_AT_name("AP")
	.dwattr $C$DW$1306, DW_AT_location[DW_OP_reg7]

	.dwendtag $C$DW$CU

