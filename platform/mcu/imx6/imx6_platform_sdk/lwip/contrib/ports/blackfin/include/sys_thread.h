/************************************************************************/
/**	\file	sys_thread.h
 *	\brief	LwIP threading for VDK on blackfin
 *
 *	\date	2011/10/06
 *	\author	Stephane Lesage for LwIP
 *
 */
/************************************************************************/
#ifndef _SYS_THREAD_H_
#define _SYS_THREAD_H_

/* Includes */
#include <VDK.h>

/* Macros */

/* Types */
struct sys_thread_params
{
	void	(*fn)(void* arg);
	void*	arg;
};

/* Externals */

/* Functions */

/* Classes */

/**	LwIP thread
*/
class LwIP_ThreadType : public VDK::Thread 
{
	public:
						 LwIP_ThreadType(VDK::Thread::ThreadCreationBlock&);
		virtual			~LwIP_ThreadType();
		virtual void	Run();
		virtual int		ErrorHandler();
		static VDK::Thread* Create(VDK::Thread::ThreadCreationBlock&);
		
	private:
		sys_thread_params	p;
};


#endif // _SYS_THREAD_H_
