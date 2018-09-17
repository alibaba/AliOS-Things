/************************************************************************/
/**	\file	sys_thread.cpp
 *	\brief	LwIP threading for VDK on blackfin
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

/* Includes */
#include "sys_thread.h"

/* Macros */

/* Types */

/* Externals */

/* Variables */
VDK::ThreadTemplate	LwIP_ThreadTemplate = INIT_THREADTEMPLATE_("LwIP Thread", VDK::kPriority10, 1024, LwIP_ThreadType::Create, ksystem_heap, ksystem_heap, true);

/* Functions */

void LwIP_ThreadType::Run()
{
	p.fn(p.arg);
}

int LwIP_ThreadType::ErrorHandler()
{
	return VDK::Thread::ErrorHandler();
}

LwIP_ThreadType::LwIP_ThreadType(VDK::Thread::ThreadCreationBlock &tcb) : VDK::Thread(tcb)
{
	p = *(sys_thread_params*)tcb.user_data_ptr;
}

LwIP_ThreadType::~LwIP_ThreadType()
{
}

VDK::Thread* LwIP_ThreadType::Create(VDK::Thread::ThreadCreationBlock &tcb)
{
	return new (tcb) LwIP_ThreadType(tcb);
}
