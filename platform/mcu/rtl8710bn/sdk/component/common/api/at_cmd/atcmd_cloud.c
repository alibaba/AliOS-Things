#include <stdio.h>
#include "log_service.h"
#include "cmsis_os.h"
#include <platform/platform_stdlib.h>

#if CONFIG_JOYLINK
#if 1
void fATCJ(void *arg)
{
	extern void joylink_erase(void);
	printf("\r\n[ATCJ] Erase wifi and joylink info.");
	if(arg){
		printf("\r\n[ATCJ]Usage : ATCJ");
		return;
	}        
	joylink_erase();
}
#else
void fATCJ(void *arg)
{
	extern void cmd_jd_smart(int argc, char **argv);
	int argc;
	char *argv[MAX_ARGC] = {0};
	printf("[ATCJ]:simple config command for jdsmart\n\r");
	if(!arg){
		printf("[ATCJ]Usage: ATCJ=simple_config\n\r");
		return;
	}        
	argv[0] = "simple_config";
	if((argc = parse_param(arg, argv)) > 1){
		cmd_jd_smart(argc, argv);
	}
	else
		printf("[ATCJ]Usage: ATCJ=simple_config\n\r");
}
#endif
#endif


#if CONFIG_GAGENT
void fATCG(void *arg)
{
	example_gagent();
}
void fATCE(void *arg)
{//Erase gagent config flash
	extern int GAgent_DevEraseConfigData();
	GAgent_DevEraseConfigData();
}
#endif

#if CONFIG_QQ_LINK
void fATCQ(void *arg)
{	
	int argc;
	unsigned char *argv[MAX_ARGC] = {0};
	extern void device_write_sn_license(int argc, unsigned char **argv);
	extern void device_erase_all(int argc, unsigned char **argv);
	
	if(!arg)
	{
		printf("\r\n[ATCQ] Write sn/license into flash or Erase all info\r\n");
		printf("\r\n[ATCQ] Usage: ATCQ=erase");
		printf("\r\n[ATCQ] Usage: ATCQ=sn,xxxxxxxx\r\n             ATCQ=licensepart1,xxxxxxxx\r\n             ATCQ=licensepart2,xxxxxxxx");
		return;
	}        
	argv[0] = "sn&&license&&erase";
	argc = parse_param(arg, argv);
	if(argc == 3)		// Write sn&&license
	{
		device_write_sn_license(argc, argv);
	}
	else if(argc == 2)		// Erase all info : ATCQ=erase
	{
		device_erase_all(argc, argv);
	}
	else
	{
		printf("\r\n[ATCQ] Usage: ATCQ=erase");
		printf("\r\n[ATCQ]Usage: ATCQ=sn,xxxxxxxx\r\n             ATCQ=licensepart1,xxxxxxxx\r\n             ATCQ=licensepart2,xxxxxxxx");
	}
}
#endif

#if CONFIG_AIRKISS_CLOUD
void fATCW(void *arg)
{	
	int argc;
	unsigned char *argv[MAX_ARGC] = {0};
	extern void airkiss_cloud_write_device_info(int argc, unsigned char **argv);
	extern void airkiss_cloud_erase_ap_profile(int argc, unsigned char **argv);
	
	if(!arg) goto USAGE;
	
	argv[0] = "type/id/licese/erase";
	argc = parse_param(arg, argv);
	if(argc == 3)			// Write typw/id/license
	{
		airkiss_cloud_write_device_info(argc, argv);
		return;
	}
	else if(argc == 2)		// Erase wifi profile : ATCW=erase
	{
		airkiss_cloud_erase_ap_profile(argc, argv);
		return;
	}
	else
		goto USAGE;

USAGE:	
	printf("\r\n[ATCW] Write ORDERLY device's type/id/license into flash or Erase wifi profile");
	printf("\r\n[ATCW] Usage: ATCW=type,xxxxxxxx");
	printf("\r\n[ATCW] Usage: ATCW=id,xxxxxxxx");
	printf("\r\n[ATCW] Usage: ATCW=licensepart1,xxxxxxxx\t(80-Byte long)");
	printf("\r\n[ATCW] Usage: ATCW=licensepart2,xxxxxxxx\t(80-Byte long)");
	printf("\r\n[ATCW] Usage: ATCW=erase");
	return;	
}
#endif

#if CONFIG_ALINK
extern void example_alink(void);
extern int alink_erase_wifi_config();
extern void alink_reset_to_factory(void *arg);

void fATCA(void *arg)
{
	example_alink();
}

void fATCZ(void *arg)
{
	//Erase alink config flash
	alink_erase_wifi_config();
}

void fATCT(void *arg)
{
	alink_reset_to_factory(NULL);
}
#endif

void fATCx(void *arg)
{	
}

log_item_t at_cloud_items[ ] = {
#if CONFIG_JOYLINK
	{"ATCJ", fATCJ,},
#endif
#if CONFIG_GAGENT
	{"ATCG", fATCG,},
	{"ATCE", fATCE,},
#endif	
#if CONFIG_QQ_LINK
	{"ATCQ", fATCQ,},
#endif
#if CONFIG_AIRKISS_CLOUD
	{"ATCW", fATCW},
#endif
#if CONFIG_ALINK
	{"ATCA", fATCA,},
	{"ATCZ", fATCZ,},
	{"ATCT", fATCT,},
#endif
	{"ATC?", fATCx,},
};
void at_cloud_init(void)
{
	log_service_add_table(at_cloud_items, sizeof(at_cloud_items)/sizeof(at_cloud_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_cloud_init);
#endif
