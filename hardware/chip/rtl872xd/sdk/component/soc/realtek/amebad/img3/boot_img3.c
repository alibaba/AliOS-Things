/* this file is build in no secure img, function in this file will call NSC functions */

#include "ameba_soc.h"

#if defined ( __ICCARM__ )
#pragma section=".ram_image3.bss"

u8* __image3_bss_start__;
u8* __image3_bss_end__;
#endif

IMAGE3_ENTRY_SECTION
void NS_ENTRY BOOT_IMG3(void)
{

#if defined ( __ICCARM__ )
	/* it is dummy code, but IAR linker need this */
	__iar_data_init3();

	__image3_bss_start__			= (u8*)__section_begin(".ram_image3.bss");
	__image3_bss_end__ 				= (u8*)__section_end(".ram_image3.bss");

#endif

	DBG_8195A("BOOT_IMG3: BSS [%08x~%08x] SEC: %x \n", __image3_bss_start__, __image3_bss_end__,
		TrustZone_IsSecure());
	
	/* reset img3 bss */
	_memset((void *) __image3_bss_start__, 0, (__image3_bss_end__ - __image3_bss_start__));
}
