/************************************************************************/
/**	\file	BootThread.h
 *	\brief	Boot Thread for LwIP test
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/

#ifndef _BootThread_H_    
#define _BootThread_H_

#pragma diag(push)
#pragma diag(suppress: 177,401,451,826,831,1462)

#include "VDK.h"

#pragma diag(pop)

class BootThread : public VDK::Thread 
{
public:
    BootThread(VDK::Thread::ThreadCreationBlock&);
    virtual ~BootThread();
    virtual void Run();
    virtual int ErrorHandler();
    static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
};

#endif /* _BootThread_H_ */
