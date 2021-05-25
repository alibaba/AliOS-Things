#include "ameba_soc.h"

/* 
 * The following example shows how to call secure entry function from NS functions. 
 *	 (1) In NS code, users can't call internal_secure_func() directly, but can call secure_entry() instead.
 */

/* Internal secure function that is not available for NS functions.  */
int internal_secure_func (void)
{
    DBG_8195A("Print log from secure code\n");
	return 0;
}
 
/* Use attribute `cmse_nonsecure_entry' to make `secure_entry' available for NS functions.  */
IMAGE3_ENTRY_SECTION
NS_ENTRY int secure_entry (void)
{
    return internal_secure_func ();
}



/* 
 * The following example shows how to call  NS function from secure functions. 
 *	(1) Users should call entry() in NS function to pass a non-secure functioin pointer to fp.
 *	(2) Then in secure code, users can call call_callback() to execute the non-secure function.
 */
 
void default_callback(void) {}
 
// fp can point to a secure function or a non-secure function
nsfunc *fp = (nsfunc *) default_callback;        // secure function pointer

IMAGE3_ENTRY_SECTION
void NS_ENTRY entry(nsfunc *callback)
{
    fp = cmse_nsfptr_create(callback);  // non-secure function pointer
}

void call_callback(void)
{
    if (cmse_is_nsfptr(fp)) fp();       // non-secure function call
    else ((void (*)(void)) fp)();       // normal function call
}

