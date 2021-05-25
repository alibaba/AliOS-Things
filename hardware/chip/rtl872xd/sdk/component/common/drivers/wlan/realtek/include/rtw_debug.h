/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved. 
  *
******************************************************************************/

#ifndef __RTW_DEBUG_H__
#define __RTW_DEBUG_H__

#ifdef PLATFORM_WINDOWS
	#define RTW_PRINT do {} while (0)
	#define RTW_ERR do {} while (0)
	#define RTW_WARN do {} while (0)
	#define RTW_INFO do {} while (0)
	#define RTW_DBG do {} while (0)
	#define RTW_PRINT_SEL do {} while (0)
	#define _RTW_PRINT do {} while (0)
	#define _RTW_ERR do {} while (0)
	#define _RTW_WARN do {} while (0)
	#define _RTW_INFO do {} while (0)
	#define _RTW_DBG do {} while (0)
	#define _RTW_PRINT_SEL do {} while (0)
#else
	#define RTW_PRINT(x, ...) do {} while (0)
	#define RTW_ERR(x, ...) do {} while (0)
	#define RTW_WARN(x,...) do {} while (0)
	#define RTW_INFO(x,...) do {} while (0)
	#define RTW_DBG(x,...) do {} while (0)
	#define RTW_PRINT_SEL(x,...) do {} while (0)
	#define _RTW_PRINT(x, ...) do {} while (0)
	#define _RTW_ERR(x, ...) do {} while (0)
	#define _RTW_WARN(x,...) do {} while (0)
	#define _RTW_INFO(x,...) do {} while (0)
	#define _RTW_DBG(x,...) do {} while (0)
	#define _RTW_PRINT_SEL(x,...) do {} while (0)
#endif

#define _drv_always_		1
#define _drv_emerg_			2
#define _drv_alert_			3
#define _drv_crit_			4
#define _drv_err_			5
#define	_drv_warning_		6
#define _drv_notice_		7
#define _drv_info_			8
#define _drv_dump_			9
#define	_drv_debug_			10


#define _module_rtl871x_xmit_c_		BIT(0)
#define _module_xmit_osdep_c_		BIT(1)
#define _module_rtl871x_recv_c_		BIT(2)
#define _module_recv_osdep_c_		BIT(3)
#define _module_rtl871x_mlme_c_		BIT(4)
#define _module_mlme_osdep_c_		BIT(5)
#define _module_rtl871x_sta_mgt_c_		BIT(6)
#define _module_rtl871x_cmd_c_			BIT(7)
#define _module_cmd_osdep_c_		BIT(8)
#define _module_rtl871x_io_c_				BIT(9)
#define _module_io_osdep_c_		BIT(10)
#define _module_os_intfs_c_			BIT(11)
#define _module_rtl871x_security_c_		BIT(12)
#define _module_rtl871x_eeprom_c_			BIT(13)
#define _module_hal_init_c_		BIT(14)
#define _module_hci_hal_init_c_		BIT(15)
#define _module_rtl871x_ioctl_c_		BIT(16)
#define _module_rtl871x_ioctl_set_c_		BIT(17)
#define _module_rtl871x_ioctl_query_c_	BIT(18)
#define _module_rtl871x_pwrctrl_c_			BIT(19)
#define _module_hci_intfs_c_			BIT(20)
#define _module_hci_ops_c_			BIT(21)
#define _module_osdep_service_c_			BIT(22)
#define _module_mp_			BIT(23)
#define _module_hci_ops_os_c_			BIT(24)
#define _module_rtl871x_ioctl_os_c		BIT(25)
#define _module_rtl8712_cmd_c_		BIT(26)
#define _module_fwcmd_c_			BIT(27)
#define _module_rtl8192c_xmit_c_ BIT(28)
#define _module_hal_xmit_c_	BIT(28)
#define _module_efuse_			BIT(29)
#define _module_rtl8712_recv_c_		BIT(30)
#define _module_rtl8712_led_c_		BIT(31)

#undef _MODULE_DEFINE_

#if defined _RTW_XMIT_C_
	#define _MODULE_DEFINE_	_module_rtl871x_xmit_c_
#elif defined _XMIT_OSDEP_C_
	#define _MODULE_DEFINE_	_module_xmit_osdep_c_
#elif defined _RTW_RECV_C_
	#define _MODULE_DEFINE_	_module_rtl871x_recv_c_
#elif defined _RECV_OSDEP_C_
	#define _MODULE_DEFINE_	_module_recv_osdep_c_
#elif defined _RTW_MLME_C_
	#define _MODULE_DEFINE_	_module_rtl871x_mlme_c_
#elif defined _MLME_OSDEP_C_
	#define _MODULE_DEFINE_	_module_mlme_osdep_c_
#elif defined _RTW_MLME_EXT_C_
	#define _MODULE_DEFINE_ 1
#elif defined _RTW_STA_MGT_C_
	#define _MODULE_DEFINE_	_module_rtl871x_sta_mgt_c_
#elif defined _RTW_CMD_C_
	#define _MODULE_DEFINE_	_module_rtl871x_cmd_c_
#elif defined _CMD_OSDEP_C_
	#define _MODULE_DEFINE_	_module_cmd_osdep_c_
#elif defined _RTW_IO_C_
	#define _MODULE_DEFINE_	_module_rtl871x_io_c_
#elif defined _IO_OSDEP_C_
	#define _MODULE_DEFINE_	_module_io_osdep_c_
#elif defined _OS_INTFS_C_
	#define	_MODULE_DEFINE_	_module_os_intfs_c_
#elif defined _RTW_SECURITY_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_security_c_
#elif defined _RTW_EEPROM_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_eeprom_c_
#elif defined _HAL_INTF_C_
	#define	_MODULE_DEFINE_	_module_hal_init_c_
#elif (defined _HCI_HAL_INIT_C_) || (defined _SDIO_HALINIT_C_)
	#define	_MODULE_DEFINE_	_module_hci_hal_init_c_
#elif defined _RTL871X_IOCTL_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_ioctl_c_
#elif defined _RTL871X_IOCTL_SET_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_ioctl_set_c_
#elif defined _RTL871X_IOCTL_QUERY_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_ioctl_query_c_
#elif defined _RTL871X_PWRCTRL_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_pwrctrl_c_
#elif defined _RTW_PWRCTRL_C_
	#define	_MODULE_DEFINE_	1
#elif defined _HCI_INTF_C_
	#define	_MODULE_DEFINE_	_module_hci_intfs_c_
#elif defined _HCI_OPS_C_
	#define	_MODULE_DEFINE_	_module_hci_ops_c_
#elif defined _SDIO_OPS_C_
	#define	_MODULE_DEFINE_ 1
#elif defined _OSDEP_HCI_INTF_C_
	#define	_MODULE_DEFINE_	_module_hci_intfs_c_
#elif defined _OSDEP_SERVICE_C_
	#define	_MODULE_DEFINE_	_module_osdep_service_c_
#elif defined _HCI_OPS_OS_C_
	#define	_MODULE_DEFINE_	_module_hci_ops_os_c_
#elif defined _RTL871X_IOCTL_LINUX_C_
	#define	_MODULE_DEFINE_	_module_rtl871x_ioctl_os_c
#elif defined _RTL8712_CMD_C_
	#define	_MODULE_DEFINE_	_module_rtl8712_cmd_c_
#elif defined _RTL8192C_XMIT_C_
	#define	_MODULE_DEFINE_	1
#elif defined _RTL8723AS_XMIT_C_
	#define	_MODULE_DEFINE_	1
#elif defined _RTL8712_RECV_C_
	#define	_MODULE_DEFINE_	_module_rtl8712_recv_c_
#elif defined _RTL8192CU_RECV_C_
	#define	_MODULE_DEFINE_	_module_rtl8712_recv_c_
#elif defined _RTL871X_MLME_EXT_C_
	#define _MODULE_DEFINE_	_module_mlme_osdep_c_
#elif defined _RTW_MP_C_
	#define	_MODULE_DEFINE_	_module_mp_
#elif defined _RTW_MP_IOCTL_C_
	#define	_MODULE_DEFINE_	_module_mp_
#elif defined _RTW_EFUSE_C_
	#define	_MODULE_DEFINE_	_module_efuse_
#endif

#ifdef PLATFORM_OS_CE
extern void rtl871x_cedbg(const char *fmt, ...);
#endif

extern u32 GlobalDebugEnable;
extern u8 OtherDebugPortEnable;
extern u16 GlobalDebugLevel;

#define RT_TRACE_F(_Comp, _Level, Fmt) do{}while(0)

#define _func_enter_ do{}while(0)
#define _func_exit_ do{}while(0)
#define RT_PRINT_DATA(_Comp, _Level, _TitleString, _HexData, _HexDataLen) do{}while(0)

#ifdef PLATFORM_WINDOWS
	#define DBG_871X do {} while(0)
	#define MSG_8192C do {} while(0)
	#define DBG_8192C do {} while(0)
	#define DBG_871X_LEVEL do {} while(0)
#else
	#define DBG_871X(x, ...) do {} while(0)
	#define MSG_8192C(x, ...) do {} while(0)
	#define DBG_8192C(x,...) do {} while(0)
	#define DBG_871X_LEVEL(x,...) do {} while(0)
#endif

#ifdef CONFIG_BT_COEXIST
	#define RTW_INFO(x,...) do {} while (0)
	#define RTW_DBG_DUMP(_TitleString, _HexData, _HexDataLen) do {} while (0)
#endif

#undef	_dbgdump
#ifdef PLATFORM_WINDOWS

	#ifdef PLATFORM_OS_XP
		#define _dbgdump	DbgPrint
	#elif defined PLATFORM_OS_CE
		#define _dbgdump	rtl871x_cedbg
	#endif

#elif defined PLATFORM_LINUX
	#define _dbgdump	printk
#elif defined PLATFORM_ECOS
	#define _dbgdump	diag_printf
#elif defined(PLATFORM_FREERTOS) || defined(PLATFORM_CMSIS_RTOS) || defined(PLATFORM_CUSTOMER_RTOS)
	#define _dbgdump_nr	printf("\n\r"); printf
	#define _dbgdump	printf
#elif defined PLATFORM_FREEBSD
	#define _dbgdump	printf
#endif

#if !defined(CONFIG_PLATFORM_8195A) && !defined(CONFIG_PLATFORM_8711B) \
	&& !defined(CONFIG_PLATFORM_8721D) && !defined(CONFIG_PLATFORM_8195B) \
	&& !defined(CONFIG_PLATFORM_8710C)
#define DRIVER_PREFIX	"RTL871X: "
#endif

#if 	defined (_dbgdump)
	#undef DBG_871X_LEVEL
#if defined (__ICCARM__) || defined (__CC_ARM) ||defined(__GNUC__)
	#define DBG_871X_LEVEL(level, ...)     \
	do {\
		if(GlobalDebugEnable){\
			if (level <= GlobalDebugLevel) {\
				_dbgdump_nr(DRIVER_PREFIX __VA_ARGS__);\
			}\
		}\
	}while(0)
	extern int msg_uart_port(const char *fmt, ...);
	#define RTW_PRINT_MSG(...) 	\
	do {\
		if(OtherDebugPortEnable){\
			msg_uart_port(__VA_ARGS__);\
		} else {\
			_dbgdump(__VA_ARGS__);\
		} \
	}while(0)
#else
	#define DBG_871X_LEVEL(level, fmt, arg...)     \
	do {\
		if(GlobalDebugEnable){\
			if (level <= GlobalDebugLevel) {\
				if (level <= _drv_err_ && level > _drv_always_) {\
					_dbgdump_nr(DRIVER_PREFIX"ERROR " fmt, ##arg);\
				} \
				else {\
					_dbgdump_nr(DRIVER_PREFIX fmt, ##arg);\
				} \
			}\
		}\
	}while(0)

#endif	//#ifdef __CC_ARM
#endif

#ifdef CONFIG_DEBUG
#if	defined (_dbgdump)
	#undef DBG_871X
	#define DBG_871X(...)     do {\
		_dbgdump_nr(DRIVER_PREFIX __VA_ARGS__);\
	}while(0)

	#undef MSG_8192C
	#define MSG_8192C(...)     do {\
		_dbgdump_nr(DRIVER_PREFIX __VA_ARGS__);\
	}while(0)

	#undef DBG_8192C
	#define DBG_8192C(...)     do {\
		_dbgdump_nr(DRIVER_PREFIX __VA_ARGS__);\
	}while(0)
#endif
#endif /* CONFIG_DEBUG */

//define RTW_INFO to get debug message of phydm
//command phydm dbg can open debug message from uart interface
#if (DBG == 1)
#undef RTW_INFO
#define RTW_INFO(fmt, arg...)	  \
do {\
		_dbgdump_nr(DRIVER_PREFIX fmt, ##arg);\
} while (0)
#endif

#ifdef CONFIG_DEBUG_RTL871X
#ifndef _RTL871X_DEBUG_C_
	extern u64 GlobalDebugComponents;
#endif

#if	defined (_dbgdump) && defined (_MODULE_DEFINE_)

		#undef RT_TRACE_F
		#define RT_TRACE_F(_Comp, _Level, Fmt)\
		do {\
			if((_Comp & GlobalDebugComponents) && (_Level <= GlobalDebugLevel)) {\
				_dbgdump_nr("%s [0x%08x,%d]", DRIVER_PREFIX, (unsigned int)_Comp, _Level);\
				_dbgdump_nr Fmt;\
			}\
		}while(0)

#endif


#if	defined (_dbgdump)

		#undef  _func_enter_
		#define _func_enter_ \
		do {	\
			if (GlobalDebugLevel >= _drv_debug_) \
			{																	\
				_dbgdump_nr("\n %s : %s enters at %d\n", DRIVER_PREFIX, __FUNCTION__, __LINE__);\
			}		\
		} while(0)

		#undef  _func_exit_
		#define _func_exit_ \
		do {	\
			if (GlobalDebugLevel >= _drv_debug_) \
			{																	\
				_dbgdump_nr("\n %s : %s exits at %d\n", DRIVER_PREFIX, __FUNCTION__, __LINE__); \
			}	\
		} while(0)

		#undef RT_PRINT_DATA
		#define RT_PRINT_DATA(_Comp, _Level, _TitleString, _HexData, _HexDataLen)			\
			if(((_Comp) & GlobalDebugComponents) && (_Level <= GlobalDebugLevel))	\
			{									\
				int __i;								\
				u8	*ptr = (u8 *)_HexData;				\
				_dbgdump("\r\n%s", DRIVER_PREFIX);						\
				_dbgdump(_TitleString "--------Len=%d\n\r", _HexDataLen);						\
				for( __i=0; __i<(int)_HexDataLen; __i++ )				\
				{								\
					_dbgdump("%02X%s", ptr[__i], (((__i + 1) % 4) == 0)?"  ":" ");	\
					if (((__i + 1) % 16) == 0)	_dbgdump("\n\r");			\
				}								\
				_dbgdump("\n\r");							\
			}
#endif
#endif /* CONFIG_DEBUG_RTL871X */


#ifdef CONFIG_PROC_DEBUG

	int proc_get_drv_version(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_write_reg(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

 	int proc_set_write_reg(struct file *file, const char *buffer,
		unsigned long count, void *data);

	int proc_get_read_reg(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_set_read_reg(struct file *file, const char *buffer,
		unsigned long count, void *data);


	int proc_get_fwstate(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_sec_info(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_mlmext_state(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_qos_option(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_ht_option(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rf_info(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_ap_info(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_adapter_state(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_trx_info(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_mac_reg_dump1(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_mac_reg_dump2(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_mac_reg_dump3(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_bb_reg_dump1(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_bb_reg_dump2(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_bb_reg_dump3(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rf_reg_dump1(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rf_reg_dump2(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rf_reg_dump3(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rf_reg_dump4(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

#ifdef CONFIG_AP_MODE

	int proc_get_all_sta_info(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

#endif

#ifdef DBG_MEMORY_LEAK
	int proc_get_malloc_cnt(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);
#endif

#ifdef CONFIG_FIND_BEST_CHANNEL
	int proc_get_best_channel(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);
#endif

	int proc_get_rx_signal(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_set_rx_signal(struct file *file, const char *buffer,
		unsigned long count, void *data);
#ifdef CONFIG_80211N_HT
	int proc_get_cbw40_enable(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_set_cbw40_enable(struct file *file, const char *buffer,
		unsigned long count, void *data);
	
	int proc_get_ampdu_enable(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);
			  
	int proc_set_ampdu_enable(struct file *file, const char *buffer,
		unsigned long count, void *data);
	
	int proc_get_rx_stbc(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);
		
	int proc_set_rx_stbc(struct file *file, const char *buffer,
		unsigned long count, void *data);
#endif //CONFIG_80211N_HT

	int proc_get_two_path_rssi(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_get_rssi_disp(char *page, char **start,
			  off_t offset, int count,
			  int *eof, void *data);

	int proc_set_rssi_disp(struct file *file, const char *buffer,
		unsigned long count, void *data);


#endif //CONFIG_PROC_DEBUG

#endif	//__RTW_DEBUG_H__

