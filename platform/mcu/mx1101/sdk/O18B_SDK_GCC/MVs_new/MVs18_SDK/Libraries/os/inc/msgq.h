////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:msgq.h
//
//  maintainer: Halley
//
//		Description	:
//			define user & system message type
//          【注意】此文件包含OS内核正在使用的消息集，二次开发用户不要做修改，对消息集定制和扩展可统一修改msgq_plus.h文件
//           *** 详细说明，请参考文档《系统消息集详细说明.doc》
//           *** 大致说明：
//           1. 消息分四种类型：广播消息、系统消息、设备消息、Misc消息；
//           2. 广播消息：消息区间 - [0x0000 ~ 0x00FF]，主要特点是所有的Task都可以收到广播消息；
//           3. 系统消息：消息区间 - [0x0100 ~ 0x0FFF]，主要定义一些OS相关的消息，以及公共模块类的消息；
//           4. 设备消息：消息区间 - [0x1000 ~ 0x2FFF]，主要定义各种外围设备消息集；
//           5. Misc消息：消息区间 - [0x3000 ~ 0x3FFF]，定义应用模块的消息集，或者各种杂项消息(不好分类的)；
//           6. 消息ID格式(32Bits)：4Bits(进程ID) + 2Bits(优先级) + 8Bits(声明周期) + 4Bits(消息状态) + (6Bits(消息集) + 8Bits(消息值))(消息ID)
//		Changelog	:
///////////////////////////////////////////////////////////////////////////////

#ifndef	__MSGQ_H__
#define __MSGQ_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#pragma pack(2)
/*
 * message group type
 */
typedef enum
{
    /*
     * 第一类消息:广播消息
     * [0x0000	~	0x00FF]
     */
    //广播消息.空消息
    //[[0x0000	~	0x0000]]
    MSGNULL = 0x000,

#define	MSGBRDCST_CLASS	0x00FF
    //广播消息.工作模式
    //[[0x0001	~	0x009F]]
#define	MSGBRDCST_BASE	0x0001
    MSGBRDCST_PWROFF = 0x0001,
    MSGBRDCST_RESET,
    MSGBRDCST_SLEEP,
    MSGBRDCST_SLEEP_DEEP,
    MSGBRDCST_WAKEUP,
    //广播消息.工作模式.用户扩展
    //[[0x00A0	~	0x0FF]]
#define	MSGBRDCST_BASE_USREX	0x00A0
    MSGBRDCST_FREQCHG_REQ = MSGBRDCST_BASE_USREX,//系统主频变化请求
    MSGBRDCST_FREQCHG_RES,//系统主频变化呼应
    MSGBRDCST_FILL_FIFO_START,//为处理LineIn录音开始时的短音向InitTask发送的填充fifo的消息  开始
    MSGBRDCST_FILL_FIFO_END,//为处理LineIn录音开始时的短音向InitTask发送的填充fifo的消息  结束

    /*
     * 第二类消息:系统消息
     * [0x0100	~	0x0FFF]
     */
#define	MSGSYS_BASE		(0x0100)
#define	MSGSYS_CLASS		(0x01FF)

    //系统消息.内核
    //[[0x0100	~	0x019F]]
#define	MSGSYS_KERNEL_CLASS	(0x01FF)
    MSGSYS_KERNEL_INITING = 0x0100,
    MSGSYS_KERNEL_INITED,
    MSGSYS_KERNEL_SUSPEND,
    MSGSYS_KERNEL_RESUME,

    //系统消息.任务
    //[[0x0200	~	0x029F]]
#define	MSGSYS_TASK_CLASS	(0x02FF)
    MSGSYS_TASK_CREATE = 0x0200,
    MSGSYS_TASK_DELETE,
    MSGSYS_TASK_SUSPEND,
    MSGSYS_TASK_RESUME,
    MSGSYS_TASK_SCHEDULE,
	
    //系统消息.定时器
    //[[0x0400	~	0x049F]]
#define	MSGSYS_TIMER_CLASS	(0x04FF)
    MSGSYS_TIMER_START = 0x0400,
    MSGSYS_TIMER_STOP,
    MSGSYS_TIMER_CHANGE_PEROID,
    MSGSYS_TIMER_DELETE,

	//设备消息.COM UART
	MSGDEV_BUART_CLASS		= 0x0500,
    MSGDEV_BUART_DATRDY,
    MSGDEV_BUART_DATDON,
    MSGDEV_BUART_DATERR,


    //设备消息.DEBUG UART
	MSGDEV_FUART_CLASS		= 0x0600,
    MSGDEV_FUART_DATREQ,
    MSGDEV_FUART_DATRDY,


#define	MSGALL_TOTAL_NUM	(0x3FFF + 1)
} msg_type_t, MSGTYPE;
#pragma pack()

#define	MSGPRIO_LEVEL_HI	2
#define	MSGPRIO_LEVEL_MD	1
#define	MSGPRIO_LEVEL_LO	0

extern int msgq_task_register(int pid);
extern void msgq_task_unregister(int pid);

// 消息接收
extern int msgq_msg_recv(void* buf, int* len, int wait);

// 消息发送
extern int msgq_msg_send(int msgcode, void* buf, int len, unsigned char prio, unsigned char async);
extern int msgq_msg_send_fromisr(int msgcode, void* buf, int len, unsigned char prio);

// 发送消息控制命令
extern int msgq_msg_ioctl(int cmd, unsigned int opt);
//
extern int msgq_sleepon_msgqueue(unsigned int wait);

extern int msgq_tmrchk_cbfun(unsigned short msg, unsigned char* buf, int buf_len);

// 注册、注销  接收 消息集
// 注册单个接收消息集(接收消息前有选择的接收特定消息集)
extern void MsgAddSet(uint32_t Msg);
// 注销单个接收消息集
extern void MsgDelSet(uint32_t Msg);
// 注销所有接收消息集
extern void MsgEmptySet(void);
// 注册所有消息集为 接收消息集
extern void MsgFillSet(void);

// 注册、注销 消息集 为 高优先级的 替代消息集

// 【替代消息集】如果一个TASK已注册了正常的接收消息集，但是某些特殊时候只是想接收某一，
// 几类更高优先级的消息时，可采用该消息阻塞机制,暂时先不接收替代消息集以外的消息，
// 这种机制的缺陷是，替代消息集意外的消息将有超时丢失的危险，所以此项功能慎用！

// 注册单个替代消息集
extern void AltMsgAddSet(uint32_t Msg);
// 注销单个替代消息集
extern void AltMsgDelSet(uint32_t Msg);
// 注销所有替代消息集
extern void AltMsgEmptySet(void);
// 注册所有消息集为替代消息集
extern void AltMsgFillSet(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__MSGQ_H__
