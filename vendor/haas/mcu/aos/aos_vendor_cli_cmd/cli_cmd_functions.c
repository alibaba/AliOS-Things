#include <stdlib.h>
#include <time.h>
#include "cli_cmd_functions.h"
#include "hal_trace.h"
#include "aos/vfs.h"
#include "fcntl.h"
#include "factory_functions.h"
#include "aos/kernel.h"
#include "iwpriv_functions.h"
#include "aos/hal/gpio.h"
#include "tg_security.h"
#include "audio_player.h"
#include "cli_api.h"
#include "bt_drv_interface.h"
#include "apps.h"
#include "hal_location.h"
#include "fcntl.h"
#include "tg_audio.h"
#include "mcu_audio.h"
#include "hal_timer.h"
#include "net_dump.h"
#include "app_cmd.h"
#include "ota_port.h"
#include "k_time.h"
#include "tg_speech.h"
#include "cli_cmd_list.h"

extern void btdrv_hci_bridge_loop(uint8_t *hci_tx_p,uint8_t hci_tx_len,uint8_t *hci_rx_p,uint8_t *hci_rx_len);
extern void btdrv_hci_bridge_start(void);
extern void btdrv_hci_receive_loop(uint8_t *hci_rx_p,uint8_t *hci_rx_len);
extern int netdev_set_epta_params(int wlan_duraiton, int bt_duration, int hw_epta_enable);

typedef struct{
	uint16_t opcode;
	uint8_t  len;
	uint8_t  data[64];
}uart_cmd_type_T;

static uint8_t hexChar2Hex(const char c)
{
  int r = 0;
  if ((c >= '0') && (c <= '9'))
    r = c-'0';
  else if ((c >= 'a') && (c <= 'f'))
    r = c-'a'+10;
  else if ((c >= 'A') && (c <= 'F'))
    r = c-'A'+10;
  else
    r = 16; /* invalid hex character */

  return (uint8_t)r;
}
static int hexString2CharBuf(const char *string, uint8_t *charBuf, uint32_t charBufLength)
{
  uint32_t i, k = 0;
  uint8_t hNibble, lNibble;

  /* sanity checks */
  if (string[0] == '\0') {
    return -1; /* invalid string size */
  }

  if (charBufLength<=0){
    return -2; /* invalid buffer size */
  }

  /* convert to hex characters to corresponding 8bit value */
  for (i=0;(string[i]!='\0')&&((i>>1)<charBufLength);i+=2) {
    k = i>>1;
    hNibble = hexChar2Dec(string[i]);
    lNibble = hexChar2Dec(string[i+1]);
    if ((hNibble == 16) || (lNibble == 16)) {
      return -3; /* invalid character */
    }
    charBuf[k] = ((hNibble<<4)&0xf0) + lNibble;
  }

  /* check if last character was string terminator */
  if ((string[i-2]!=0) && (string[i]!=0)) {
    return -1; /* invalid string size */
  }

  /* fill charBuffer with zeros */
  for (i=k+1;i<charBufLength;i++) {
    charBuf[i] = 0;
  }

  return 0;
}

#if 1
void ali_factory_mode(int argc, char **argv)
{
    cli_printf("%s ... %d, %s \n",__func__,argc,argv[1]);
    if(argc < 1)
    {
        factory_usage();
        return;
    }

    if(argc >= 2)
    {
        int ret = -1;
        if(strncmp(argv[1], "ATE_ENTER_FACTORY", strlen(argv[1])) == 0)
        {
            ret = ate_enter_factory();
        }
        else if(strncmp(argv[1], "ATE_GET_VER", strlen(argv[1])) == 0)
        {
            ret = ate_get_version();
        }
        else if(strncmp(argv[1], "ATE_SET_PCBSN", strlen(argv[1])) == 0)
        {
            ret = ate_set_pcb_sn(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_PCBSN", strlen(argv[1])) == 0)
        {
            ret = ate_get_pcb_sn();
        }
        else if(strncmp(argv[1], "ATE_SET_WMAC", strlen(argv[1])) == 0)
        {
            //ret = ate_set_wmac(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_WMAC", strlen(argv[1])) == 0)
        {
            //ret = ate_get_wmac();
        }
        else if(strncmp(argv[1], "ATE_SET_BTMAC", strlen(argv[1])) == 0)
        {
            //ret = ate_set_btmac(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_BTMAC", strlen(argv[1])) == 0)
        {
            //ret = ate_get_btmac();
        }
        else if(strncmp(argv[1], "ATE_SET_BOOTKEY", strlen(argv[1])) == 0)
        {
            ret = ate_set_bootkey(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_BOOTKEY", strlen(argv[1])) == 0)
        {
            ret = ate_get_bootkey();
        }
        else if(strncmp(argv[1], "ATE_SET_SECRETKEY_MD5", strlen(argv[1])) == 0)
        {
            ret = ate_set_secretKey_md5(argv[2],argv[3]);
        }
        else if(strncmp(argv[1], "ATE_GET_SECRETKEY", strlen(argv[1])) == 0)
        {
            ret = ate_get_secretKey();
        }
        else if(strncmp(argv[1], "ATE_SET_ODM_NAME", strlen(argv[1])) == 0)
        {
            ret = ate_set_odm_name(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_ROM", strlen(argv[1])) == 0)
        {
            ret = ate_get_rom();
        }
        else if(strncmp(argv[1], "ATE_GET_RAM", strlen(argv[1])) == 0)
        {
            //ret = ate_get_ram();
        }
        else if(strncmp(argv[1], "ATE_GET_NAND_MODEL", strlen(argv[1])) == 0)
        {
            ret = ate_get_nand_model();
        }
        else if(strncmp(argv[1], "ATE_RECORD_START", strlen(argv[1])) == 0)
        {
            //ret = ate_get_record_start(atol(argv[2]));
        }
        else if(strncmp(argv[1], "ATE_LED_ALL_ON", strlen(argv[1])) == 0)
        {
            //ret = ate_led_all_on();
        }
        else if(strncmp(argv[1], "ATE_LED_ALL_OFF", strlen(argv[1])) == 0)
        {
            //ret = ate_led_all_off();
        }
        else if(strncmp(argv[1], "ATE_SET_VOL", strlen(argv[1])) == 0)
        {
            //ret = ate_set_vol(atol(argv[2]));
        }
        else if(strncmp(argv[1], "ATE_MIC_QUALITY_TEST", strlen(argv[1])) == 0)
        {
            ret = ate_mic_quality_test();
        }
        else if(strncmp(argv[1], "ATE_PLAY_FILE", strlen(argv[1])) == 0)
        {
            ret = ate_play_file(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_ALS_VALUE", strlen(argv[1])) == 0)
        {
            ret = ate_get_als_value();
        }
        else if(strncmp(argv[1], "ATE_SET_LIGHT_CALIBRATION", strlen(argv[1])) == 0)
        {
            ret = ate_set_light_calibration(argv[2],argv[3]);
        }
        else if(strncmp(argv[1], "ATE_GET_LIGHT_CALIBRATION", strlen(argv[1])) == 0)
        {
            ret = ate_get_light_calibration(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_HW_ID", strlen(argv[1])) == 0)
        {
            ret = ate_get_hw_id();
        }
        else if(strncmp(argv[1], "ATE_GET_STATION_TEST_RESULT", strlen(argv[1])) == 0)
        {
            ret = ate_get_station_test_result();
        }
        else if(strncmp(argv[1], "ATE_GET_MAX_TEMP", strlen(argv[1])) == 0)
        {
            ret = ate_get_max_temp();
        }
        else if(strncmp(argv[1], "ATE_SET_SN", strlen(argv[1])) == 0)
        {
            ret = ate_set_sn(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_GET_SN", strlen(argv[1])) == 0)
        {
            ret = ate_get_sn(argv[2]);
        }
        else if(strncmp(argv[1], "ATE_ENTER_USER", strlen(argv[1])) == 0)
        {
            ret = ate_enter_user();
        }
        else if (strncmp(argv[1], "ATE_FLASH_ID", strlen(argv[1])) == 0)
        {
            //ret = ate_get_flash_uid();
        }
        else
            factory_usage();

        if(ret >= 0)
            cli_printf("success \n");
        else
            cli_printf("failed \n");

    }
}
#endif

void ali_volume(int argc, char **argv)
{
    cli_printf("%s ... \n",__func__);
    if(argc < 1)
    {
        factory_usage();
        return;
    }

    tg_volume_set(atol(argv[1]));
}


static aos_sem_t audio_sem;
static int8_t audio_play_inited = 0;

static void play_finish_cb(void)
{
    aos_sem_signal(&audio_sem);
}

static int8_t play_init(void)
{
    if(audio_play_inited){
        return 0;
    }
    audio_play_inited = 1;
    audio_player_file_opt_t file_opt;
    file_opt.open = aos_open;
    file_opt.close = aos_close;
    file_opt.read = aos_read;
    audio_player_file_ops_register(&file_opt);
    return 0;
}

static int play_file(const char * name)
{
    play_init();
    if(0 != aos_sem_new(&audio_sem, 1)){
        return -1;
    }
    aos_sem_wait(&audio_sem, AOS_WAIT_FOREVER);
    if( 0 == audio_player_play_file(name, play_finish_cb)){
        aos_sem_wait(&audio_sem, AOS_WAIT_FOREVER);
    }
    aos_sem_free(&audio_sem);
    return 0;
}

void ali_tmall_play(int argc, char **argv)
{
    if(argc < 1)
    {
        factory_usage();
        return;
    }

    cli_printf("%s ... \n",__func__);
    if(0 != play_file(argv[1])){
        cli_printf("fail \n");
    }else{
        cli_printf("success \n");
    }
}

void ali_rsa_verify(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_start_remote_debugger(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_ulog_encode_fs(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_watchdog_debug(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_spinor_test(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

#if 0
void ali_audiocodec_dump_reg(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}
#endif

void ali_firmware_set_env(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_firmware_print_env(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_dump_nor_part(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_cat_file(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t cat <file_path> \n");
        return;
    }
    aos_vfs_init();
    int fd = aos_open(argv[1], O_CREAT | O_RDWR);
    char rbuf[128] = {0};
    while (aos_read(fd, rbuf, sizeof(rbuf)) > 0)
    {
        cli_printf("%s \n",rbuf);
    }
    aos_close(fd);
}

void ali_copy_file(int argc, char **argv)
{
    if(argc < 3)
    {
        cli_printf("Usage:\t cp <src_file_path> <dst_file_path> \n");
        return;
    }
    aos_vfs_init();
    int fd_src = aos_open(argv[1], O_CREAT | O_RDWR);
    int fd_dst = aos_open(argv[2], O_CREAT | O_RDWR);
    char rbuf[128] = {0};
    int r_ret = -1;
    while ((r_ret = aos_read(fd_src, rbuf, sizeof(rbuf))) > 0)
    {
        int w_ret= -1;
        if((w_ret = aos_write(fd_dst,rbuf,r_ret)) < 0)
        {
            cli_printf("cp %s to %s failed \n",argv[1],argv[2]);
            if(aos_remove(argv[1]) < 0)
                cli_printf("rm %s failed \n",argv[1]);
            break;
        }
    }
    aos_close(fd_src);
    aos_close(fd_dst);
}

void ali_df_cmd(int argc, char **argv)
{
    char *path = "/data";
    struct aos_statfs buf;
    int ret = aos_statfs(path,&buf);
    if (ret != 0)
    {
        cli_printf("get file system info failed ret:%d \n",ret);
        return;
    }
    cli_printf("%-7s\t%5s %5s %5s %5s %-15s","FileSys","Size","Used","Avail","Use\%","Mounted on \n");
    cli_printf("%-7s\t%4dk %4dk %4dk %4d%% %-15s \n",path,buf.f_blocks*4,(buf.f_blocks-buf.f_bfree)*4,buf.f_bfree*4,100*(buf.f_blocks-buf.f_bfree)/buf.f_blocks,path);
}

void ali_hexdump_c(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_hexdump(int argc, char **argv)
{
	cli_printf("%s ... \n",__func__);
	//todo
}

void ali_list_all(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t ll <file_path> \n");
        return;
    }
    aos_vfs_init();
    char *path = argv[1];

    if (aos_access(path, F_OK) == 0) {

        char f_type = '-';
        char f_permission[10] = "---------";

        struct aos_stat var_stat;
        if(aos_stat(path,&var_stat) == 0)
        {
            if(var_stat.st_mode & S_IFREG)
            {
                for (int i = 0; i < strlen(f_permission); i += 3)
                {
                    f_permission[i] = ((var_stat.st_mode & S_IRWXU) & S_IRUSR) ? 'r' : '-';
                    f_permission[i+1] = ((var_stat.st_mode & S_IRWXU) & S_IWUSR) ? 'w' : '-';
                    f_permission[i+2] = ((var_stat.st_mode & S_IRWXU) & S_IXUSR) ? 'x' : '-';
                }
                char time_buf[13] = {0};
                char *cur_time = hal_sys_format_time(var_stat.st_modtime);
                memcpy(time_buf, &cur_time[4], 12);
                cli_printf("-%s %-8d %s %s \n",f_permission,var_stat.st_size,time_buf,path);
            }
            else
            {
                aos_dir_t *dir_ptr = aos_opendir(path);
                if(dir_ptr != NULL)
                {
                    aos_dirent_t *ptr = NULL;
                    while((ptr = aos_readdir(dir_ptr))!=NULL)
                    {
                        char f_path[128]={0};
                        memcpy(f_path, path, strlen(path));
                        f_path[strlen(path)] = '/';
                        memcpy(&f_path[strlen(path)+1], ptr->d_name, strlen(ptr->d_name));
                        struct aos_stat var_sub_stat;
                        if(aos_stat(f_path,&var_sub_stat) == 0)
                        {
                            int fsize = var_sub_stat.st_size;
                            switch(var_sub_stat.st_mode & S_IFMT){
                                case S_IFSOCK:f_type = 's';break;
                                case S_IFLNK :f_type = 'l';break;
                                case S_IFREG :f_type = '-';break;
                                case S_IFBLK :f_type = 'b';break;
                                case S_IFDIR :
                                    f_type = 'd';
                                    fsize = 4096;
                                    break;
                                case S_IFCHR :f_type = 'c';break;
                                case S_IFIFO :f_type = 'p';break;
                            }
                            for (int i = 0; i < strlen(f_permission); i += 3)
                            {
                                f_permission[i] = ((var_sub_stat.st_mode & S_IRWXU) & S_IRUSR) ? 'r' : '-';
                                f_permission[i+1] = ((var_sub_stat.st_mode & S_IRWXU) & S_IWUSR) ? 'w' : '-';
                                f_permission[i+2] = ((var_sub_stat.st_mode & S_IRWXU) & S_IXUSR) ? 'x' : '-';
                            }
                            cli_printf("%c%s %-8d %s%c \n",f_type,f_permission,fsize,ptr->d_name,(f_type == 'd')?'/':' ');
                        }
                    }
                    int ret = aos_closedir(dir_ptr);
                    if(ret < 0)
                        cli_printf("ll:close %s failed\n",path);
                }
            }
        }

    }
    else
        cli_printf("ls: cannot access '%s': No such file or directory \n",path);

}

void ali_list(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t ls <file_path> \n");
        return;
    }
    aos_vfs_init();
    char *path = argv[1];

    if (aos_access(path, F_OK) == 0) {

        aos_dir_t *dir_ptr = aos_opendir(path);
        if(dir_ptr != NULL)
        {
            aos_dirent_t *ptr = NULL;
            while((ptr = aos_readdir(dir_ptr))!=NULL)
                cli_printf("%s\n",ptr->d_name);

            int ret = aos_closedir(dir_ptr);
            if(ret < 0)
                cli_printf("ls: close '%s': failed \n",path);
        }

    }
    else
        cli_printf("ls: cannot access '%s': No such file or directory \n",path);
}

void ali_make_directory(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t mkdir <file_path> \n");
        return;
    }
    aos_vfs_init();
    if(aos_mkdir(argv[1]) < 0)
        cli_printf("mkdir %s failed \n",argv[1]);
}

void ali_move_file(int argc, char **argv)
{
    if(argc <3)
    {
        cli_printf("Usage:\t mv <src_file_path> <src_file_path> \n");
        return;
    }
    aos_vfs_init();
    if(aos_rename(argv[1],argv[2]) < 0)
        cli_printf("mv %s to %s failed \n",argv[1],argv[2]);
}

static void remove_sub_nodes(char *path,int flag_r);

static void remove_dir(char *path)
{
    aos_dir_t *dir_ptr = aos_opendir(path);
    if(dir_ptr != NULL)
    {
        aos_dirent_t *ptr = NULL;
        while((ptr = aos_readdir(dir_ptr))!=NULL)
        {
            if (strncmp(ptr->d_name, ".", strlen(ptr->d_name)) == 0)
                continue;
            if (strncmp(ptr->d_name, "..", strlen(ptr->d_name)) == 0)
                continue;

            char f_path[128]={0};
            memcpy(f_path, path, strlen(path));
            if(f_path[strlen(path)-1] != '/')
            {
                f_path[strlen(path)] = '/';
                memcpy(&f_path[strlen(path)+1], ptr->d_name, strlen(ptr->d_name));
            }
            else
                memcpy(&f_path[strlen(path)], ptr->d_name, strlen(ptr->d_name));

            cli_printf("remove_sub_nodes f_path %s \n",f_path);
            remove_sub_nodes(f_path,1);
        }
        int ret = aos_closedir(dir_ptr);
        if( ret < 0)
            cli_printf("remove dir:close %s failed\n",path);
    }
}

static void remove_sub_nodes(char *path,int flag_r)
{
    struct aos_stat var_stat;
    if(aos_stat(path,&var_stat) == 0)
    {
        switch(var_stat.st_mode & S_IFMT)
        {
            case S_IFSOCK:break;
            case S_IFLNK :break;
            case S_IFREG :
                if(aos_remove(path) < 0)
                    cli_printf("rm file %s failed \n",path);
                break;
            case S_IFBLK :break;
            case S_IFDIR :
                if(flag_r == 0)
                {
                    cli_printf("rm: cannot remove '%s': Is a directory\n",path);
                    break;
                }
                cli_printf("rm dir %s \n",path);
                remove_dir(path);
                int ret = aos_rmdir(path);
                if (ret < 0)
                    cli_printf("rm dir %s failed ret:%d\n",path,ret);

                break;
            case S_IFCHR :break;
            case S_IFIFO :break;
        }
    }
}

void ali_remove_file(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t rm <file_path> \n");
        cli_printf("Usage:\t rm -r <dir_path> \n");
        return;
    }

    aos_vfs_init();
    int flag_r = 0;
    for (int i = 1;i < argc; ++i)
    {
        if (strncmp(argv[i], "-r", strlen(argv[i])) == 0)
        {
            flag_r = 1;
            continue;
        }

        char *path = argv[i];
        if (aos_access(path, F_OK) == 0)
        {
            remove_sub_nodes(path,flag_r);
        }
        else
            cli_printf("rm: cannot remove '%s': No such file or directory \n",path);
    }

}

void ali_rwcheck(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_rw_dev(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_rw_speed(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_touch(int argc, char **argv)
{
    if(argc < 2)
    {
        cli_printf("Usage:\t touch <file_path>\n");
        return;
    }
    aos_vfs_init();
    int fd = aos_open(argv[1], O_CREAT | O_RDWR);
    aos_close(fd);
}

void ali_sona_checksum(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_check_toc1(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_upgrade_app(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_mp_stop(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_mp_start(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	btdrv_hci_bridge_start();
	//todo
}

void ali_bt_fifo_all(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_fifo(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_bqb(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_avrcp(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_test(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_mp_test(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//cli_printf("argc 0x%02x\n",argc);
	//cli_printf("*argv[1]%s\n",argv[1]);
	//cli_printf("*argv[2]%s\n",argv[2]);
	uart_cmd_type_T uart_cmd;
	uint8_t dataBuff[2],ret,length=0;
	uint8_t HciBuff[64], HciBuffLen;
	uint8_t HciRxBuff[64],HciRxBuffLen;
	uint8_t ParamBuff[64];
    uint8_t BT_Tx_Stop[] = {0x01,0x87,0xFC,0x14,0x00,0xe8,0x03,0x00,0x00,0x00,0x00,0x02,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x00,0x0F,0x03,0x53,0x01};
	if(argc >= 2)
	{
		hexString2CharBuf(&argv[1][2],dataBuff,2);
		uart_cmd.opcode = dataBuff[0]*0x100 + dataBuff[1];
		uart_cmd.len = argc - 2;
		for(int i=0;i<uart_cmd.len;i++)
		{
			hexString2CharBuf(&argv[i+2][2],dataBuff,1);
			uart_cmd.data[i] = dataBuff[0];
		}
		cli_printf("opcode 0x%04x\n", uart_cmd.opcode);
		switch(uart_cmd.opcode)
		{
			case 0xFCE0:
				length = 0;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x87;
				HciBuff[2] = 0xFC;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0xFCE1:
                length = 0;
				btdrv_hci_bridge_loop(BT_Tx_Stop,sizeof(BT_Tx_Stop),HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0x201E:
				length = 0;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x1E;
				HciBuff[2] = 0x20;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0x201F:
				length = 2;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x1F;
				HciBuff[2] = 0x20;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				ParamBuff[0] = 0xe4;
				ParamBuff[1] = 0x01;
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0xFCE2:
				length = 0;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x87;
				HciBuff[2] = 0xFC;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0xFCE5:
			break;

			case 0xFCE3:
			break;

			case 0x201D:
				length = 0;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x1D;
				HciBuff[2] = 0x20;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0xFC40:
			break;

			case 0xFCE8:
			break;

			case 0xFCE7:
                length = 3;
                ParamBuff[0] = uart_cmd.data[2];
                ParamBuff[1] = uart_cmd.data[1];
                ParamBuff[2] = 0xD7;
                if(uart_cmd.data[1] == 0x22)
                {
                   btdrv_set_bdr_ble_txpower(5,uart_cmd.data[2]);
                }
                else if(uart_cmd.data[1] == 0x23)
                {
                    btdrv_set_edr_txpower(5,uart_cmd.data[2]);
                }
                else if(uart_cmd.data[1] == 0x24)
                {
                    btdrv_set_bdr_ble_txpower(5,uart_cmd.data[2]);
                }
			break;

			case 0x1009:
				length = 6;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x09;
				HciBuff[2] = 0x10;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				memcpy(ParamBuff,&HciRxBuff[7],length);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			case 0x0C03:
				length = 0;
				HciBuff[0] = 0x01;
				HciBuff[1] = 0x03;
				HciBuff[2] = 0x0C;
				memcpy(&HciBuff[3],uart_cmd.data,uart_cmd.len);
				btdrv_hci_bridge_loop(HciBuff,uart_cmd.len+3,HciRxBuff,&HciRxBuffLen);
				cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
				DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
			break;

			default:
			break;

		}
		ret = 0;
		cli_printf("success:%d  opcode:0x%04x length:%d param:\n",ret,uart_cmd.opcode,length);
		DUMP8("0x%02x ",ParamBuff,length);
	}
	else
	{
		/* code */
	}
}

void ali_bt_debug(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_off(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_on(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_uart3(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_close_test_cmw500(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_open_test_cmw500(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_colog(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_debug(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_iwpriv(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
    if(argc < 1)
    {
        iwpriv_usage();
        return;
    }

    if(argc >= 2)
    {
        int ret = -1;
        if(strncmp(argv[1], "mp_start", strlen(argv[1])) == 0)
        {
            ret = mp_start();
        }
        else if(strncmp(argv[1], "mp_channel", strlen(argv[1])) == 0)
        {
            ret = mp_channel(argv[2]);
        }
        else if(strncmp(argv[1], "mp_bandwidth", strlen(argv[1])) == 0)
        {
            if(argc >= 3)
            {
                char *p1 = strtok(argv[2],",");
	            char *p2 = strtok(NULL,",");
                char *bw_val = NULL;
				char *shortGI_val = NULL;
                if (p1)
                {
                    char *p = strtok(p1,"=");
                    if (p && (strncmp(p, "40M", strlen(p)) == 0))
						bw_val = strtok(NULL,"=");
                }
                if (p2)
                {
                    char *p = strtok(p2,"=");
                    if (p && (strncmp(p, "shortGI", strlen(p)) == 0))
						shortGI_val = strtok(NULL,"=");
                }
				if (bw_val != NULL && shortGI_val != NULL)
					ret = mp_bandwidth(atoi(bw_val),atoi(shortGI_val));
            }
        }
        else if(strncmp(argv[1], "mp_ant_rx", strlen(argv[1])) == 0)
        {
            ret = mp_ant_rx(argv[2]);
        }
        else if(strncmp(argv[1], "mp_ant_tx", strlen(argv[1])) == 0)
        {
            ret = mp_ant_tx(argv[2]);
        }

        else if(strncmp(argv[1], "mp_txpower", strlen(argv[1])) == 0)
        {
            if(argc >= 3)
            {
                char *p1 = strtok(argv[2],",");
	            char *p2 = strtok(NULL,",");
				char *patha_val = NULL;
				char *pathb_val = NULL;
				if (p1)
                {
                    char *p = strtok(p1,"=");
                    if (p && (strncmp(p, "patha", strlen(p)) == 0))
						patha_val = strtok(NULL,"=");
                }
				if (p2)
                {
                    char *p = strtok(p2,"=");
                    if (p && (strncmp(p, "pathb", strlen(p)) == 0))
						pathb_val = strtok(NULL,"=");
                }
                if (patha_val != NULL && pathb_val != NULL)
                    ret = mp_txpower(atoi(patha_val),atoi(pathb_val));
            }
        }
        else if(strncmp(argv[1], "mp_rate", strlen(argv[1])) == 0)
        {
            ret = mp_rate(atoi(argv[2]));
        }
        else if(strncmp(argv[1], "mp_ctx", strlen(argv[1])) == 0)
        {
            if(strncmp(argv[2], "stop", strlen(argv[2])) == 0)
            {
                mp_ctx_stop();
            }
            else if(strncmp(argv[2], "count=%100,pkt", strlen(argv[2])) == 0)
            {
                mp_ctx_pkt(100);
            }
        }
        else if(strncmp(argv[1], "mp_phypara", strlen(argv[1])) == 0)
        {
            char *p = strtok(argv[1],"=");
            if (p && (strncmp(p, "xcap", strlen(p)) == 0))
            {
                char * xcap_val = strtok(NULL,"=");
                ret = mp_phypara(atoi(xcap_val));
            }
        }
        else if(strncmp(argv[1], "mp_reset_stats", strlen(argv[1])) == 0)
        {
            ret = mp_reset_stats();
        }
        else if(strncmp(argv[1], "mp_ther", strlen(argv[1])) == 0)
        {
            ret = mp_ther();
        }
        else if(strncmp(argv[1], "mp_arx", strlen(argv[1])) == 0)
        {
            if(strncmp(argv[2], "stop", strlen(argv[2])) == 0)
            {
                mp_arx_stop();
            }
            else if(strncmp(argv[2], "start", strlen(argv[2])) == 0)
            {
                mp_arx_start();
            }
            else if(strncmp(argv[2], "phy", strlen(argv[2])) == 0)
            {
                mp_arx_phy();
            }

        }
        else if(strncmp(argv[1], "efuse_set", strlen(argv[1])) == 0)
        {
            efuse_set(argv[2]);
        }
        else if(strncmp(argv[1], "efuse_masks", strlen(argv[1])) == 0)
        {
            efuse_masks(argv[2]);
        }
        else if(strncmp(argv[1], "efuse_get", strlen(argv[1])) == 0)
        {
            efuse_get(argv[2]);
        }
        else
            iwpriv_usage();

        if(ret >= 0)
            cli_printf("success\n");
        else
            cli_printf("failed\n");

    }

}


extern uint32_t rf_simple_strtoul(const char *cp,char **endp,unsigned int base);

void ali_wifi_get_power_list (int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    besphy_rf_get_power_list_from_reg();
}


static uint8_t test_runing_flag = 0;
static uint8_t wifi_ch = 0;
static uint8_t wifi_mode = 0;
void ali_wifi_tx_signal_thread(void *argument);
osThreadDef(ali_wifi_tx_signal_thread, osPriorityAboveNormal, 1, (2*1024), "ali_wifi_tx_signal_thread");
extern void set_wifi_test_continue_mode(uint8 mode);

void ali_wifi_tx_test_help(void)
{
    cli_printf("please input:\n");
    cli_printf("wifi_tx n or wifi_tx b\n");
    cli_printf("wifi_tx ch <id>\n");
    cli_printf("id is 1~14\n");
}

void ali_wifi_tx_signal_thread(void *argument)
{
    test_runing_flag = 1;
    set_wifi_test_continue_mode(wifi_mode);
    test_runing_flag = 0;
	osThreadExitPub();
}

void ali_wifi_test_tx(int argc, char **argv)
{
    osThreadId test_tid;
    cli_printf("%s ...argc = %d\n",__func__,argc);

    if(test_runing_flag){
        cli_printf("%s sending is running!!\n",__func__);
        return;
    }

    if(argc == 2){
        if(strncmp(argv[1], "n", strlen(argv[1])) == 0){
            wifi_mode = 1;
            cli_printf("%s continue = wifi mode=%d \n",__func__,wifi_mode);
        }
        else if(strncmp(argv[1], "b", strlen(argv[1])) == 0){
            wifi_mode = 0;
            cli_printf("%s continue = wifi mode=%d \n",__func__,wifi_mode);
        }
        else{
            ali_wifi_tx_test_help();
        }
    }
    else if(argc == 3){
        if(strncmp(argv[1], "ch", strlen(argv[1])) == 0){
            wifi_ch = atoi(argv[2]);
            cli_printf("%s wifi_ch =%d \n",__func__,wifi_ch);
            if((wifi_ch < 1) || (wifi_ch > 14))
            {
                cli_printf("wifi_ch is error!wifi_ch=%d \n",wifi_ch);
                return;
            }
            wifidrv_switch_channel(wifi_ch);
        }
        else{
            ali_wifi_tx_test_help();
        }
    }
    else
    {
        ali_wifi_tx_test_help();
    }

    test_tid = osThreadCreate(osThread(ali_wifi_tx_signal_thread), NULL);
	if (test_tid == NULL)  {
        cli_printf("Failed to create wifi_tx_signal_thread\n");
         return;
	}

    cli_printf("%s ...done\n",__func__);
}

void ali_wifi_rw_data(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    if(strncmp(argv[1], "read", strlen(argv[1])) == 0)
    {
        besphy_rf_mem_read(argv);
    }
    else if (strncmp(argv[1], "write", strlen(argv[1])) == 0)
    {
        besphy_rf_mem_write(argv);
    }
    else if (strncmp(argv[1], "debug", strlen(argv[1])) == 0)
    {
        besphy_rf_debug_thoughtput_status();
    }
    else if (strncmp(argv[1], "get_temp", strlen(argv[1])) == 0)
    {
        besphy_rf_get_temp();
    }
    else if (strncmp(argv[1], "tpt_en", strlen(argv[1])) == 0)
    {
        besphy_rf_open_temp_calibration();
    }
    else if (strncmp(argv[1], "temp_log_en", strlen(argv[1])) == 0)
    {
        cli_printf("open the log\n");
        system_temp_log_enable();
    }
    else if (strncmp(argv[1], "temp_log_dis", strlen(argv[1])) == 0)
    {
        cli_printf("close the log\n");
        system_temp_log_disable();
    }
    else
    {
        cli_printf("please inpu:reg read addr/reg write addr value\n");
    }
}
static int tx_power_config[9]= {0};
void ali_wifi_power_config(int argc, char **argv)
{
    int i = 0;
    cli_printf("%s ...\n",__func__);

    if(strncmp(argv[1], "reg", strlen(argv[1])) == 0)
    {
        besphy_rf_get_power_list_from_reg();
    }
    else if (strncmp(argv[1], "readflash", strlen(argv[1])) == 0)
    {
        besphy_rf_get_power_from_flash();
    }
    else if (strncmp(argv[1], "set", strlen(argv[1])) == 0)
    {
        for(i; i< 9; i++){
            tx_power_config[i] = rf_simple_strtoul(argv[2+i], NULL, 0);
        }
        besphy_rf_set_tx_power_config(tx_power_config);
    }
    else if (strncmp(argv[1], "agc", strlen(argv[1])) == 0)
    {
        int id,band_limit;
        id = atoi(argv[2]);
        band_limit = rf_simple_strtoul(argv[3], NULL, 0);
        besphy_rf_agc_select(id,band_limit);
    }
    else
    {
        cli_printf("please inpu:power reg/power readflash/power set 9 power value\n");
    }
}

void ali_wifi_tx_stat_config(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    if((strncmp(argv[1], "on", strlen(argv[1])) == 0) && argc==3)
    {
        uint8_t second = atoi(argv[2]);
        bwifi_trans_stat_en(1, second);
    }
    else if((strncmp(argv[1], "off", strlen(argv[1])) == 0) && argc==3)
    {
        uint8_t second = atoi(argv[2]);
        bwifi_trans_stat_en(0, second);
    }
    else
    {
        cli_printf("please inpu: tx_stat on/off number_of_seconds\n");
    }
}

#ifdef __A7_DSP_ENABLE__
void ali_a7_dsp_reboot(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    a7_dsp_reboot();
}
#endif

void ali_wdt_disable(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    app_wdt_close();
}

#if 0
extern void codec_set_dac_silence_mode(uint32_t enable);
extern void codec_set_adc_silence_mode(uint32_t enable);
void ali_codec_silence(int argc, char **argv)
{
    uint32_t silence;

    if(argc != 3) {
        cli_printf("please input codec_silence playback/capture 0/1");
        return ;
    }
    silence = atoi(argv[2]);
    if (strcmp(argv[1], "playback") == 0) {
        if (silence)
            cli_printf("playback silence\n");
        else
            cli_printf("playback unsilence\n");
        codec_set_dac_silence_mode(silence);
    } else if (strcmp(argv[1], "capture") == 0) {
        if (silence)
            cli_printf("capture silence\n");
        else
            cli_printf("capture unsilence\n");
        codec_set_adc_silence_mode(silence);
    } else {
        cli_printf("please input codec_silence playback/capture 0/1");
    }
}
#endif

void ali_wifi_bt_freq_offset(int argc, char **argv)
{
    int ret = 0;
    cli_printf("%s ...\n",__func__);
    if(argc < 1)
    {
        cli_printf("please input param such:besoffset 0x12\n");
    }
    if(argc >= 2)
    {
        if(strncmp(argv[1], "adjust", strlen(argv[1])) == 0)
        {
            unsigned short value =  rf_simple_strtoul(argv[2], NULL ,0);
            ret = besphy_rf_freq_offset(value);
        }
    }
    if(ret >= 0)
        cli_printf("success\n");
    else
        cli_printf("failed\n");
}


void ali_wifi_besphy(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);

    if(argc < 1)
    {
        besphy_rf_usage();
        return;
    }
    if(argc >= 2)
    {
        int ret = -1;
        if(strncmp(argv[1], "start", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_start();
        }
        else if( true != app_enter_factory_wifi_test())
        {
            cli_printf("error:please input:besphy start into wifi rf test mode\n");
            return ;
        }
        else if (strncmp(argv[1], "tpt_en", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_open_temp_calibration();
        }
        else if (strncmp(argv[1], "tpt_open", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_temp_calibration_open();
        }
        else if (strncmp(argv[1], "tpt_close", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_temp_calibration_close();
        }
        else if(strncmp(argv[1], "set_band", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_set_band(argv[2]);
        }
        else if(strncmp(argv[1], "tx", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_tx_start();
        }
        else if(strncmp(argv[1], "tx_tone", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_tx_tone();
        }
        else if(strncmp(argv[1], "tx_cn", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_tx_ctn();
        }

        else if(strncmp(argv[1], "rate", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_rate(atoi(argv[2]));
        }
        else if(strncmp(argv[1], "get_rate", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_rate();
        }
        else if(strncmp(argv[1], "channel", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_channel(atoi(argv[2]));
        }
        else if(strncmp(argv[1], "get_channel", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_channel();
        }
        else if(strncmp(argv[1], "powerlevel", strlen(argv[1])) == 0)
        {
            unsigned short value =  rf_simple_strtoul(argv[2], NULL ,0);
            cli_printf("%s ...argv[2] = %d\n", __func__,value );
            ret = besphy_rf_txpower(value);
        }
        else if(strncmp(argv[1], "default_power", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_set_default_power();
        }
        else if(strncmp(argv[1], "get_powerlevel", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_get_txpower();
        }
        else if(strncmp(argv[1], "freqOffset", strlen(argv[1])) == 0)
        {
			unsigned short value =  rf_simple_strtoul(argv[2], NULL ,0);
			cli_printf("%s ...argv[2] = %d\n", __func__,value );
            ret = besphy_rf_freq_offset(value);
        }
        else if(strncmp(argv[1], "get_freqOffset", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_freq_offset();
        }
        else if(strncmp(argv[1], "rx", strlen(argv[1])) == 0)
        {
	    ret = besphy_rf_rx_start();
        }
		else if(strncmp(argv[1], "rx_ackSet", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_rx_ackset();

        }
		else if(strncmp(argv[1], "rx_ackRead", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_rx_ackread();
        }
		else if(strncmp(argv[1], "rx_stop", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_rx_stop();
        }
		else if(strncmp(argv[1], "save", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_data_save_to_flash();
        }
		else if(strncmp(argv[1], "get_save", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_get_power_from_flash();
        }
		else if(strncmp(argv[1], "get_temp", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_get_temp();
        }
		else if(strncmp(argv[1], "save_temp", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_save_temp();
        }
		else if(strncmp(argv[1], "get_save_temp", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_get_save_temp();
        }
		else if(strncmp(argv[1], "save_freqoffset", strlen(argv[1])) == 0)
        {
			ret = besphy_rf_save_freq_offset();
        }
        else if(strncmp(argv[1], "get_save_freqoffset", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_save_freq_offset();
        }
        else if(strncmp(argv[1], "save_all_pwr", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_save_all_ch_power();
        }
        else if(strncmp(argv[1], "get_save_pwr", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_all_save_ch_power();
        }
        else if(strncmp(argv[1], "tx_interval", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_set_tx_interval( atoi(argv[2]) );
        }
        else if(strncmp(argv[1], "get_tx_interval", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_get_tx_interval();
        }
        else if(strncmp(argv[1], "tx_stop", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_tx_stop();
        }
        else if(strncmp(argv[1], "dump", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_dump();
        }
        else if(strncmp(argv[1], "tobt", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_switch_to_bt();
        }
        else if(strncmp(argv[1], "green_mode", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_green_mix_switch(0);
        }
        else if(strncmp(argv[1], "mix_mode", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_green_mix_switch(1);
        }
        else if(strncmp(argv[1], "wifi_stop", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_wifi_reset();
        }
        else if(strncmp(argv[1], "dpd_chk", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_dpd_check();
        }
        else if(strncmp(argv[1], "dpd_resume", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_dpd_resume();
        }
        else if(strncmp(argv[1], "tx_dc_tone", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_tx_dc_tone();
        }
        else if(strncmp(argv[1], "dpd_rw_loop", strlen(argv[1])) == 0)
        {
            ret = besphy_rf_dpd_rw_mem_loop();
        }
        else
            besphy_rf_usage();

        if(ret >= 0)
            cli_printf("success\n");
        else
            cli_printf("failed\n");

    }

}

void ali_wifi_reoder_scan(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_scan(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_scan_with_ssid(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_long_time(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_off(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
#if 0
    int ret = tg_wifi_enable(0);
    if (ret)
        goto exit;
    cli_printf("WIFI is already off");

exit:
    cli_printf("WIFI close failed");
    return;
#endif
}

void ali_wifi_on(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
#if 0
    int ret = tg_wifi_init();
    if (ret)
        goto exit;

	ret = tg_wifi_enable(1);
    if (ret)
        goto exit;
    cli_printf("WIFI is already running");
exit:
    cli_printf("WIFI is not running");
    return;
#endif
}

void ali_wifi_info(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_promisc(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_dhcp_start(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_connect(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_io_debug(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_test(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_tcpip(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_ifconfig(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_bt_reset(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_wifi_reset(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_thermal_get_temp(int argc, char **argv)
{
    extern int get_chip_temp(void);
    int temp = get_chip_temp();
    cli_printf("temperature:%d centigrade\n", temp);
}

void ali_efuse_write(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_efuse_read(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_adbd_service(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_adbd_forward(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_snd_adb(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_iperf(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_ping(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_sendfile(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_dns(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_arp(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_lsfd(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_pktprint(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_network_config(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

#if 0
void ali_play(int argc, char **argv)
{
    if(argc < 1)
    {
        factory_usage();
        return;
    }
#if 1
    if(argc >= 3)
    {
        int timer = atoi(argv[2]);
        cli_printf("timer :%d",timer);
        for(int i = 0;i < timer;i++)
        {
            if(0 != play_file(argv[1])){
                cli_printf("fail\n");
            }else{
                cli_printf("success\n");
            }
            osDelay(2500);
        }
    }

#endif
}
#endif


#if 0
extern void tg_volume_set(int volume);
extern int tg_volume_get();
extern int8_t alsa_adc_vol_level_to_db(uint8_t adc_vol);
void ali_mixer(int argc, char **argv)
{
    int ret = -1;

    if( (0 == memcmp(argv[1], "setvol", 6)) && (argc >= 3)){
        tg_volume_set(atoi(argv[2]));
        ret = 0;
    }

    if( (0 == memcmp(argv[1], "getvol", 6))){
        cli_printf("get vol %d\n", tg_volume_get());
        ret = 0;
    }

    if( (0 == memcmp(argv[1], "setgain", 7)) && (argc == 4) ){
        int mic_id = atoi(argv[2]);
        int gain = atoi(argv[3]);
        cli_printf("setgain mic_id %d gain %d\n", mic_id, gain);
        ret = tg_gain_set(mic_id, gain);
    }

    if( (0 == memcmp(argv[1], "getgain", 7)) ){
        int mic_id = atoi(argv[2]);
        int gain = tg_gain_get(mic_id);
	    if (gain > 0) {
            cli_printf("getgain mic_id %d gain %d = %d db\n", mic_id, gain, alsa_adc_vol_level_to_db(gain));
            ret = 0;
	    } else {
	        ret = -1;
	    }
    }

    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ\n",__func__);
    }
}
#endif

#ifdef __A7_DSP_ENABLE__
#if 0
#include "tg_decoder.h"
#include "data_dump.h"
static void audiodump_to_ftp(record_dump_t *record_dump)
{
    int data_src = record_dump->channels;
    int duration_s = record_dump->duration_s;
    char *server_ip = record_dump->server_ip;
    int port = record_dump->port;
    char *path_file = record_dump->path_file;
    char *username = record_dump->username;
    char *password = record_dump->password;
    net_dump_handle_t net_dump_t = NULL;
    int test_func_begin_time = 0;
    int _begin_time = 0;
    int _read_size = 0;
    void *handle = NULL;
    unsigned int size_KB = 32;
    int size = 4096;

    net_dump_t = net_dump_init();
    if(!net_dump_t) return;
    if(server_ip == NULL){
        goto exit3;
    }

    if(net_dump_connect(net_dump_t,server_ip, port)<0){
        cli_printf("%s net_dump connect server error!\n", __FUNCTION__);
        goto exit3;
    }
    if(net_dump_login(net_dump_t,username, password)!=0){
        cli_printf("%s net_dump login error!\n", __FUNCTION__);
        goto exit2;
    }
    if(net_dump_notice_server(net_dump_t,path_file) !=0 ){
        cli_printf("%s net_dump_notice_server fail\n", __FUNCTION__);
        goto exit2;
    }

    if (data_src == 0) { // mic origin
        handle = data_dump_open(mcu_audio_cb_register, size_KB, MIC_BITRATE, 1);
    } else if (data_src == 1) { // before alg
        aud_dump_set(AUD_DUMP_SRC_BEFORE_ALG, 1);
        handle = data_dump_open(aud_dump_cb_register, size_KB, MIC_BITRATE, 1);
        data_dump_set_timeout(handle, 3000);
    } else if (data_src == 2) { // after alg
        aud_dump_set(AUD_DUMP_SRC_AFTER_ALG, 1);
        handle = data_dump_open(aud_dump_cb_register, size_KB, MIC_BITRATE, 1);
        data_dump_set_timeout(handle, 3000);
    } else if (data_src == 3) { // before speex
        // TODO
    } else if (data_src == 4) { // after speex
        // TODO
    } else if (data_src == 5) { // before decode
        handle = data_dump_open(before_decoder_audiodump_cb_register, size_KB, MIC_BITRATE, 1);
    } else if (data_src == 6) { // after decode
        handle = data_dump_open(after_decoder_audiodump_cb_register, size_KB, MIC_BITRATE, 1);
    } else if (data_src == 7) { // after render
        handle = data_dump_open(pcm_audiodump_cb_register, size_KB, MIC_BITRATE, 1);
    } else if (data_src == 8) {
        size_KB = 256;
        handle = data_dump_open(ultrasound_audiodump_cb_register, size_KB, MIC_BITRATE, 1);
    }else {
        cli_printf("%s data_src=%d not support!\n", __FUNCTION__, data_src);
        goto exit2;
    }
    char *buf = (char *)rt_malloc(size);
    if (!buf) {
        cli_printf("%s fail since malloc fail for %d\n", __FUNCTION__, size);
        goto exit1;
    }
    cli_printf("%s size unit=%d\n", __FUNCTION__, size);
    test_func_begin_time = osKernelSysTick();
    int now_time = osKernelSysTick();
    while (now_time < test_func_begin_time+duration_s*1000) {
        _read_size = data_dump_read(handle, buf, size);
        _begin_time = osKernelSysTick();
        _read_size = net_dump_data(net_dump_t,buf, size);
        cli_printf("W-%d %dms \r\n", _read_size, osKernelSysTick()-_begin_time);
        osDelay(5);
        now_time = osKernelSysTick();
    }
    cli_printf("%s dump to [%s], totol %d\n", __FUNCTION__, path_file, net_get_dump_size(net_dump_t,path_file));
    rt_free(buf);
exit1:
    if (data_src == 0) { // mic origin
        data_dump_close(handle, mcu_audio_cb_unregister);
    } else if (data_src == 1) { // before alg
        data_dump_close(handle, aud_dump_cb_unregister);
        aud_dump_set(AUD_DUMP_SRC_BEFORE_ALG, 0);
    } else if (data_src == 2) { // after alg
        data_dump_close(handle, aud_dump_cb_unregister);
        aud_dump_set(AUD_DUMP_SRC_AFTER_ALG, 0);
    } else if (data_src == 3) { // before speex
        // TODO
    } else if (data_src == 4) { // after speex
        // TODO
    } else if (data_src == 5) { // before decode
        data_dump_close(handle, before_decoder_audiodump_cb_unregister);
    } else if (data_src == 6) { // after decode
        data_dump_close(handle, after_decoder_audiodump_cb_unregister);
    } else if (data_src == 7) { // after render
        data_dump_close(handle, pcm_audiodump_cb_unregister);
    } else if (data_src == 8) {
        data_dump_close(handle, ultrasound_audiodump_cb_unregister);
    }else {
    }
exit2:
    net_dump_disconnect(net_dump_t);
exit3:
    net_dump_free(net_dump_t);
}

static int ali_audiodump_get_src(char *src)
{
    // mic/before-alg/after-alg/before-encode/after-encode/before-decode/after-decode/render
    int data_src = -1;
    if (strcmp(src, "mic") == 0) {
        data_src = 0;
    } else if (strcmp(src, "before-alg") == 0) {
        data_src = 1;
    } else if (strcmp(src, "after-alg") == 0) {
        data_src = 2;
    } else if (strcmp(src, "before-encode") == 0) {
        data_src = 3;
    } else if (strcmp(src, "after-encode") == 0) {
        data_src = 4;
    } else if (strcmp(src, "before-decode") == 0) {
        data_src = 5;
    } else if (strcmp(src, "after-decode") == 0) {
        data_src = 6;
    } else if (strcmp(src, "render") == 0) {
        data_src = 7;
    } else if (strcmp(src,"ultrasound") == 0){
        data_src = 8;
    }else {
    }
    cli_printf("data src=%d[%s]\n", data_src, src);
    return data_src;
}
void ali_audiodump(int argc, char **argv)
{
    // audiodump -i before-alg -d 10 -s 192.168.9.184 –p 21 –u username -w password -h /test.wav
    cli_printf("%s ...\n",__func__);
    int data_src = 0;
    int duration_s = 0;
    int o; /* getopt options */
    extern char *optarg;
    extern int optind;
    char *server_ip = NULL;
    int port = 21;
    char *path_file;
    char *username = "anonymous";
    char *password = "anonymous@example.com";
    int i_name=0,j_word=0;
    optind= 0;
    record_dump_t record_dump;
    while((o=getopt(argc, argv, "i:d:s:p:u:w:h:")) != -1) {
        switch(o) {
            case 'i':
                data_src = ali_audiodump_get_src(optarg);
                break;
            case 'd':
                duration_s = atoi(optarg);
                break;
            case 's':
                server_ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                username = optarg;
                i_name++;
                break;
            case 'w':
                password = optarg;
                j_word++;
                break;
            case 'h':
                path_file = optarg;
                break;
            case 't':
                break;
            default:
                break;
        }
    }
   if (duration_s == 0) {
        //duration_s = 10; // limit 10s (96KB per second)
        cli_printf("dump time must > 0\n");
        return;
    }
    if(server_ip != NULL){
        if(i_name>0&&j_word==0){//use a username,but no password
            cli_printf("use a username,but no password\n");
            password = " ";
        }
        cli_printf("argc=%d, data_src=%d, duration=%ds, server_ip=%s,port=%d,username=%s,password=%s,path=%s\n",
	    argc, data_src, duration_s, server_ip, port, username,password,path_file);
        memset(&record_dump, 0, sizeof(record_dump_t));
        record_dump.channels = data_src;
        record_dump.duration_s = duration_s;
        record_dump.path_file = path_file;
        record_dump.server_ip = server_ip;
        record_dump.username = username;
        record_dump.password = password;
        record_dump.port = port;
        audiodump_to_ftp(&record_dump);
    }
}

#include "genie_test.h"
void ali_agrecord(int argc, char **argv)
{
    // ftp full:  agrecord -c bit-map(mic1-mic2-ref1-ref2) -d 10 -s 192.168.9.184 –p 21 –u username -w password -h /test.wav
    // ftp:  agrecord [-c 1111] -d 10 -s 192.168.9.184 -h /test.wav
    // data: agrecord [-c 1111] -d 10 -h /test.wav
    cli_printf("%s ...\n",__func__);
    int ret = -1;
    int channel = TG_CH_ALL; // bitmap default all
    char channel_s[16];
    int duration_s = 0;
    int o; /* getopt options */
    extern char *optarg;
    extern int optind;
    char *server_ip = NULL;
    int port = 21;
    char *path_file;
    char *username = "anonymous";
    char *password = "anonymous@example.com";
    int i_name=0,j_word=0;
    optind= 0;
    record_dump_t record_dump;
    while((o=getopt(argc, argv, "c:d:s:p:u:w:h:")) != -1) {
        switch(o) {
            case 'c':
                channel = strtol(optarg, NULL, 2);
                break;
            case 'd':
                duration_s = atoi(optarg);
                break;
            case 's':
                server_ip = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                username = optarg;
                i_name++;
                break;
            case 'w':
                password = optarg;
                j_word++;
                break;
            case 'h':
                path_file = optarg;
                break;
            case 't':
                break;
            default:
                break;
        }
    }
    if (duration_s == 0) {
        //duration_s = 10; // limit 10s (96KB per second)
        cli_printf("dump time must > 0\n");
        ret = -1;
        goto RETURN;
    }
    if (channel == 0) {
        cli_printf("ch map must != 0\n");
        ret = -1;
        goto RETURN;
    }
    itoa(channel, channel_s, 2);
    if(server_ip == NULL){
        cli_printf("argc=%d, channels=0x%X[%s], duration=%ds, path=%s\n", argc, channel, channel_s, duration_s,path_file);
        record_dump.channels = channel;
        record_dump.duration_s = duration_s;
        record_dump.path_file = path_file;
        agrecord_to_data(&record_dump);
    }else{
        if(i_name>0&&j_word==0){//use a username,but no password
            cli_printf("use a username,but no password\n");
            password = " ";
        }
        cli_printf("argc=%d, channel=0x%X[%s], duration=%ds, server_ip=%s, port=%d, username=%s, password=%s, path=%s\n",
        argc, channel, channel_s, duration_s, server_ip, port, username,password,path_file);
        memset(&record_dump, 0, sizeof(record_dump_t));
        record_dump.channels = channel;
        record_dump.duration_s = duration_s;
        record_dump.path_file = path_file;
        record_dump.server_ip = server_ip;
        record_dump.username = username;
        record_dump.password = password;
        record_dump.port = port;
        agrecord_to_ftp(&record_dump);
    }
    ret = 0;

RETURN:
    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ\n",__func__);
    }
}

#include "a7_cmd.h"
extern void mcu_cmd_send(A7_CMD_T *cmd);
void ali_dsp_cmd(int argc, char **argv)
{
    int ret = -1;
    int interval = 0;
    A7_CMD_T cmd;

    if(0 == memcmp(argv[1], "debug_mm", 8)) {
        if (argc >= 3) {
            interval = atoi(argv[2]);
        }
        cli_printf("%s: A7_CMD_TYPE_DEBUG_MM interval=%dms\n",__func__, interval);
        cmd.type = A7_CMD_TYPE_DEBUG_MM;
        cmd.p1 = interval;
        cmd.p2 = 0;
        mcu_cmd_send(&cmd);
        ret = 0;
    } else if(0 == memcmp(argv[1], "statprint", 9)) {
        if (argc >= 3) {
            interval = atoi(argv[2]);
        }
        cli_printf("%s: A7_CMD_TYPE_STATPRINT interval=%ds\n",__func__, interval);
        cmd.type = A7_CMD_TYPE_STATPRINT;
        cmd.p1 = interval;
        cmd.p2 = 0;
        mcu_cmd_send(&cmd);
        ret = 0;
    } else if (0 == strcmp(argv[1], "panic")) {
        cli_printf("%s: A7_CMD_TYPE_PANIC\n", __func__);
        cmd.type = A7_CMD_TYPE_PANIC;
        cmd.p1 = 0;
        cmd.p2 = 0;
        mcu_cmd_send(&cmd);
        ret = 0;
    }

    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ\n",__func__);
    }
}

#ifdef CONFIG_GENIE_DEBUG
static int _mic_hook_fd = -1;
void mcu_af_capture_hook(uint8_t *buf, uint32_t len)
{
    if ((_mic_hook_fd >= 0) && (aos_read(_mic_hook_fd, buf, len) < len)) {
        aos_close(_mic_hook_fd);
        _mic_hook_fd = -1;
    }
}
void ali_mic(int argc, char **argv)
{
    int ret = -1;
    char *path = argv[1];
    int start = TICKS_TO_MS(hal_sys_timer_get());

    cli_printf("%s: path=%s\n",__func__,path);
    _mic_hook_fd = aos_open(path, O_RDONLY);
    while (_mic_hook_fd != -1) {
        osDelay(100);
    }
    ret = 0;

    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ, used %dms\n",__func__,TICKS_TO_MS(hal_sys_timer_get())-start);
    }
}
#endif
#endif

#endif // __A7_DSP_ENABLE__

void ali_stat_print(int argc, char **argv)
{
    int ret = -1;
    int interval = 0;

    interval = atoi(argv[1]);
    cli_printf("%s: interval=%ds\n",__func__,interval);
    bes_kv_item_get(interval>0, interval);
    ret = 0;

    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ\n",__func__);
    }
}

// uart2 tgdb/swd/eq_pc_cmd/a7_trace/ble_nosignal
extern int32_t cli2_init(void);
extern int32_t cli2_stop(void);
static char last_function[16] = "tgdb";
void ali_uart2(int argc, char **argv)
{
    int ret = -1;
    int interval = 0;
    char *cur_function = argv[1];

    extern int32_t cli2_init(void);
    extern int32_t cli2_stop(void);
    extern void krhino_idle_hook_onoff(int onoff);
    if (strcmp(last_function, cur_function) != 0) {
        cli_printf("%s: %s -> %s\n",__func__,last_function,cur_function);
        // stop last function
        if (strcmp(last_function, "tgdb") == 0) {
#if CLI_CONFIG_MULTI_CONSOLE
            cli2_stop();
#endif
        } else if (strcmp(last_function, "swd") == 0) {
            krhino_idle_hook_onoff(1);
        } else if (strcmp(last_function, "eq_pc_cmd") == 0) {
            app_cmd_close();
#ifdef __A7_DSP_ENABLE__
        } else if (strcmp(last_function, "a7_trace") == 0) {
            A7_CMD_T cmd;
            cmd.type = A7_CMD_TYPE_UART2_TRACE;
            cmd.p1 = 0;
            cmd.p2 = 0;
            mcu_cmd_send(&cmd);
#endif
        } else if (strcmp(last_function, "ble_nosignal") == 0) {
            ali_ble_nosignal_stop(0,NULL);
        }
        // start function
        if (strcmp(cur_function, "tgdb") == 0) {
#if CLI_CONFIG_MULTI_CONSOLE
            ret = cli2_init();
#endif
        } else if (strcmp(cur_function, "swd") == 0) {
            ret = hal_iomux_set_jtag();
            krhino_idle_hook_onoff(0);
        } else if (strcmp(cur_function, "eq_pc_cmd") == 0) {
            app_cmd_open();
            ret = 0;
#ifdef __A7_DSP_ENABLE__
        } else if (strcmp(cur_function, "a7_trace") == 0) {
            A7_CMD_T cmd;
            cmd.type = A7_CMD_TYPE_UART2_TRACE;
            cmd.p1 = 1;
            cmd.p2 = 0;
            mcu_cmd_send(&cmd);
            ret = 0;
#endif
        } else if (strcmp(cur_function, "ble_nosignal") == 0) {
            cli_printf("%s: ali_ble_nosignal_start\n",__func__);
            btdrv_hci_bridge_start();
            ali_ble_nosignal_start(0,NULL);
            ret = 0;
        }
        memset(last_function,0,sizeof(last_function));
        memcpy(last_function,cur_function,strlen(cur_function));
    }

    if(ret < 0)
    {
        cli_printf("%s: error\n",__func__);
        factory_usage();
    }
    else
    {
        cli_printf("%s: succ\n",__func__);
    }
}

void ali_netmgr(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_autotest_client(int argc, char **argv)
{
	cli_printf("%s ...\n",__func__);
	//todo
}

void ali_gpio_set(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    gpio_dev_t gpio_dev;

    gpio_dev.port = atoi(argv[1]);
    /* set as output mode */
    gpio_dev.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&gpio_dev);
    if (atoi(argv[2]))
        hal_gpio_output_high(&gpio_dev);
    else
        hal_gpio_output_low(&gpio_dev);
}

void ali_gpio_get(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    gpio_dev_t gpio_dev;
    uint32_t value = -1;
    gpio_dev.port = atoi(argv[1]);
    /* set as output mode */
    gpio_dev.config = OUTPUT_PUSH_PULL;
    /* configure GPIO with the given settings */
    hal_gpio_init(&gpio_dev);
	hal_gpio_input_get(&gpio_dev, &value);
    if(value < 0)
        cli_printf("%s gpio %d get failed\n",__func__,gpio_dev.port);
    else
        cli_printf("%s gpio %d get value %d\n",__func__,gpio_dev.port,value);
}

#if 0
void ali_secboot_hash_set(int argc, char **argv)
{
    cli_printf("%s ...\n",__func__);
    int ret = tg_secure_set_secboot_hash(argv[1]);
}
#endif

uint16_t check_paylaod_size(uint8_t edr,uint8_t type)
{
    uint16_t paylaod_size = 0;
    switch(type)
    {
        case 0x00:    //ID_NUL_TYPE
            paylaod_size = 0;
        break;
        case 0x01:   //POLL_TYPE
            paylaod_size = 0;
        break;
        case 0x02:    //FHS_TYPE
            paylaod_size = 18;
        break;
        case 0x03:
            paylaod_size = 17;
        break;
        case 0x04:
            if(edr)
                paylaod_size = 54;
            else
                paylaod_size = 27;
        break;
        case 0x05:
            paylaod_size = 10;
        break;
        case 0x06:
            if(edr)
                paylaod_size = 60;
            else
                paylaod_size = 20;
        break;
        case 0x07:
            if(edr)
                paylaod_size = 90;
            else
                paylaod_size = 30;
        break;
        case 0x08:
            if(edr)
                paylaod_size = 83;
            else
                paylaod_size = 10;
        break;
        case 0x09:
            paylaod_size = 29;
        break;
        case 0x0A:
            if(edr)
                paylaod_size = 367;
            else
                paylaod_size = 121;
        break;
        case 0x0B:
            if(edr)
                paylaod_size = 552;
            else
                paylaod_size = 183;
        break;
        case 0x0C:
            if(edr)
                paylaod_size = 360;
            else
                paylaod_size = 120;
        break;
        case 0x0D:
            if(edr)
                paylaod_size = 540;
            else
                paylaod_size = 180;
        break;
        case 0x0E:
            if(edr)
                paylaod_size = 679;
            else
                paylaod_size = 224;
        break;
        case 0x0F:
            if(edr)
                paylaod_size = 1021;
            else
                paylaod_size = 339;
        break;
        default :
        break;
    }
    return paylaod_size;
}

SRAM_DATA_LOC uint8_t HciTxBuff[32] = {0x01,0x87,0xFC,0x1C,0x00,0xe8,0x03,0x00,0x00,0x00,0x00,0x00,0x55,0x55,0x55,0x55,0x00,0x00,
                        0x01,0x00,0x0F,0x03,0x53,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
SRAM_DATA_LOC uint8_t hci_cmd_enable_dut[] = {0x01,0x03, 0x18, 0x00};
SRAM_DATA_LOC uint8_t hci_cmd_enable_allscan[] = {0x01, 0x1a, 0x0c, 0x01, 0x03};
SRAM_DATA_LOC uint8_t hci_cmd_autoaccept_connect[] = {0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02};
SRAM_BSS_LOC uint8_t HciRxBuff[64],HciRxBuffLen;
SRAM_BSS_LOC uint16_t paylaod_size;
SRAM_BSS_LOC uint8_t dataBuff[24];
SRAM_BSS_LOC uint8_t HciBuff[32];

void ali_bt_bdr_tx_signal(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;
    uint8_t dataBuff[24];
    uint8_t HciRxBuff[64],HciRxBuffLen;
    uint16_t paylaod_size;
	cli_printf("%s ...\n",__func__);

    nosignal_set_max_power_level();

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciTxBuff[4] = 0x00;
    HciTxBuff[19] = 0x00;
    HciTxBuff[9] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_edr_tx_signal(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

	cli_printf("%s ...",__func__);

    nosignal_set_max_power_level();

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciTxBuff[4] = 0x00;
    HciTxBuff[19] = 0x01;
    HciTxBuff[9] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_bdr_rx_signal(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

	cli_printf("%s ...",__func__);

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciTxBuff[4] = 0x01;
    HciTxBuff[19] = 0x00;
    HciTxBuff[10] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_edr_rx_signal(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

	cli_printf("%s ...",__func__);

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciTxBuff[4] = 0x01;
    HciTxBuff[19] = 0x01;
    HciTxBuff[10] = dataBuff[0];
    HciTxBuff[20] = dataBuff[1];
    HciTxBuff[21] = dataBuff[2];
    paylaod_size = check_paylaod_size(HciTxBuff[19],HciTxBuff[20]);
    HciTxBuff[22] = paylaod_size%0x100;
    HciTxBuff[23] = paylaod_size/0x100;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_stop_tx_signal(int argc, char **argv)
{
    uint16_t length = 0,ret = 0;
    HciTxBuff[4] = 0x02;

    nosignal_clear_max_power_level();

    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    btdrv_hci_receive_loop(HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100+HciRxBuff[7];
    cli_printf("success:%d send packet num length:%d \n",ret,length);
}

void ali_bt_stop_rx_signal(int argc, char **argv)
{
    uint16_t length = 0,ret = 0;
    uint16_t head_error,payload_error,adv_estsw,adv_esttpl;
    uint32_t payload_bit_error;
    HciTxBuff[4] = 0x02;
    btdrv_hci_bridge_loop(HciTxBuff,sizeof(HciTxBuff),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    btdrv_hci_receive_loop(HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100+HciRxBuff[7];
    head_error = HciRxBuff[10]*0x100+HciRxBuff[9];
    payload_error = HciRxBuff[12]*0x100+HciRxBuff[11];
    adv_estsw = HciRxBuff[14]*0x100+HciRxBuff[13];
    adv_esttpl = HciRxBuff[16]*0x100+HciRxBuff[15];
    payload_bit_error = HciRxBuff[20]*0x1000000+HciRxBuff[19]*0x10000+HciRxBuff[18]*0x100+HciRxBuff[17];

    cli_printf("success:%d receive packet num length:%d \n",ret,length);
    cli_printf("head_error:%d \n",head_error);
    cli_printf("payload_error:%d \n",payload_error);
    cli_printf("adv_estsw:%d \n",adv_estsw);
    cli_printf("adv_esttpl:%d \n",adv_esttpl);
    cli_printf("payload_bit_error:%d \n",payload_bit_error);
}

void ali_bt_le_transmitter_test(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1E;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x03;

    nosignal_set_max_power_level();

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciBuff[4] = dataBuff[0];
    HciBuff[5] = dataBuff[1];
    HciBuff[6] = dataBuff[2];
    btdrv_hci_bridge_loop(HciBuff,7,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_le_receiver_test(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1D;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x01;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    HciBuff[4] = dataBuff[0];

    btdrv_hci_bridge_loop(HciBuff,5,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_le_test_end(int argc, char **argv)
{
    uint16_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1F;
    HciBuff[2] = 0x20;
    HciBuff[3] = 0x00;

    nosignal_clear_max_power_level();

    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    length = HciRxBuff[8]*0x100 + HciRxBuff[7];
    cli_printf("success:%d length:%d \n",ret,length);
}

void ali_bt_set_bdr_power(int argc, char **argv)
{
    uint8_t length = 1,ret = 0;

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    btdrv_set_bdr_ble_txpower(dataBuff[0],dataBuff[1]);
    cli_printf("success:%d length:%d param:\n",ret,length);
    DUMP8("0x%02x ",dataBuff,length);
}

void ali_bt_set_edr_power(int argc, char **argv)
{
    uint8_t length = 1,ret = 0;

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    btdrv_set_edr_txpower(dataBuff[0],dataBuff[1]);
    cli_printf("success:%d length:%d param:\n",ret,length);
    DUMP8("0x%02x ",dataBuff,length);
}

void ali_bt_set_ble_power(int argc, char **argv)
{
    uint8_t length = 1,ret = 0;

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&dataBuff[i],1);
    }
    btdrv_set_bdr_ble_txpower(dataBuff[0],dataBuff[1]);
    cli_printf("success:%d length:%d param:\n",ret,length);
    DUMP8("0x%02x ",dataBuff,length);
}

void ali_bt_set_mac(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x72;
    HciBuff[2] = 0xFC;
    HciBuff[3] = 0x07;
    HciBuff[4] = 0x00;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[5+i],1);
    }
    btdrv_hci_bridge_loop(HciBuff,11,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_read_mac(int argc, char **argv)
{
    uint8_t length = 6,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x09;
    HciBuff[2] = 0x10;
    HciBuff[3] = 0x00;
    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
    DUMP8("0x%02x ",&HciRxBuff[7],length);
}

void ali_bt_hci_reset(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x03;
    HciBuff[2] = 0x0C;
    HciBuff[3] = 0x00;
    btdrv_hci_bridge_loop(HciBuff,4,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_carrier_start(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;

    cli_printf("%s ...",__func__);
    ali_bt_hci_reset(0,NULL);
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    btdrv_vco_test_start(HciBuff[0]);
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_carrier_stop(int argc, char **argv)
{
    uint8_t length = 0,ret = 0;
    cli_printf("%s ...\n",__func__);
    ali_bt_hci_reset(0,NULL);
    btdrv_vco_test_stop();
    cli_printf("success:%d length:%d param:\n",ret,length);
}

void ali_bt_write_reg32(int argc, char **argv)
{
    uint8_t HciBuff[64];
    uint32_t reg,value;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0]*0x1000000 + HciBuff[1]*0x10000 + HciBuff[2]*0x100 +HciBuff[3];
    value = HciBuff[4]*0x1000000 + HciBuff[5]*0x10000 + HciBuff[6]*0x100 +HciBuff[7];

    write_reg32(reg, value);
}

void ali_bt_read_reg32(int argc, char **argv)
{
    uint32_t reg;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0]*0x1000000 + HciBuff[1]*0x10000 + HciBuff[2]*0x100 +HciBuff[3];
    read_reg32(reg);
}
extern int hal_uart0_i2c_swich(void);
void ali_i2c_debug(int argc, char **argv)
{
    hal_uart0_i2c_swich();
    cli_printf("switch to i2c\n");
}

void ali_bt_enter_signal_mode(int argc, char **argv)
{
    ali_bt_hci_reset(0,NULL);

    btdrv_hci_bridge_loop(hci_cmd_enable_dut,sizeof(hci_cmd_enable_dut),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);

    btdrv_hci_bridge_loop(hci_cmd_enable_allscan,sizeof(hci_cmd_enable_allscan),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);

    btdrv_hci_bridge_loop(hci_cmd_autoaccept_connect,sizeof(hci_cmd_autoaccept_connect),HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);
    cli_printf("enter signal test mode\n");
}

void ali_set_tport(int argc, char **argv)
{
    cli_printf("%s\n",__func__);
    btdrv_set_tport();
}

void ali_bt_write_reg16(int argc, char **argv)
{
    uint32_t reg,value;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0];
    value = HciBuff[1]*0x100 +HciBuff[2];

    write_reg16(reg, value);
}

void ali_bt_read_reg16(int argc, char **argv)
{
    uint32_t reg;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0];
    read_reg16(reg);
}

void ali_wifi_write_reg16(int argc, char **argv)
{
    uint32_t reg,value;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0];
    value = HciBuff[1]*0x100 +HciBuff[2];

    wifi_write_reg16(reg, value);
}

void ali_wifi_read_reg16(int argc, char **argv)
{
    uint32_t reg;
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    reg = HciBuff[0];
    wifi_read_reg16(reg);
}

SRAM_DATA_LOC uint8_t ble_nosignal_start_flag = 0;
extern void ble_nosignal_start(void);
extern void ble_nosignal_close(void);

void ali_ble_nosignal_start(int argc, char **argv)
{
    ble_nosignal_start_flag = 1;
    ble_nosignal_start();

    cli_printf("enter ble nosignal test mode\n");
    while(0)
    {
        ble_nosignal_uart_bridge_loop();
    }
}

void ali_ble_nosignal_stop(int argc, char **argv)
{
    ble_nosignal_start_flag = 0;
    ble_nosignal_close();

    cli_printf("eixt ble nosignal test mode\n");
}

SRAM_BSS_LOC int wifi_duration,bt_duration,hw_epta_enable;
void ali_set_epta_para(int argc, char **argv)
{

    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[i],1);
    }
    wifi_duration = HciBuff[0]*0x1000000 + HciBuff[1]*0x10000 + HciBuff[2]*0x100 + HciBuff[3];
    bt_duration = HciBuff[4]*0x1000000 + HciBuff[5]*0x10000 + HciBuff[6]*0x100 + HciBuff[7];
    hw_epta_enable = HciBuff[8];
    cli_printf("wifi_duration %d\n",wifi_duration);
    cli_printf("bt_duration %d\n",bt_duration);
    cli_printf("hw_epta_enable %d\n",hw_epta_enable);
    netdev_set_epta_params(wifi_duration, bt_duration, hw_epta_enable);
}

#define BTDIGITAL_REG(a)                        (*(volatile uint32_t *)(a))
void ali_read_bdr_power(int argc, char **argv)
{
    cli_printf("BDR power: 0x%02x  0x%02x\n",BTDIGITAL_REG(0xd0350300)>>4, BTDIGITAL_REG(0xd0350308)%0x100 - 1);
}

void ali_read_edr_power(int argc, char **argv)
{
    cli_printf("EDR power: 0x%02x  0x%02x\n",BTDIGITAL_REG(0xd0350340), BTDIGITAL_REG(0xd0350344) - 1);
}

void ali_read_ble_power(int argc, char **argv)
{
    cli_printf("BLE power: 0x%02x  0x%02x\n",BTDIGITAL_REG(0xd0350300)>>4, BTDIGITAL_REG(0xd0350308)%0x100 - 1);
}

#define HW_READ_REG(a) *(volatile uint32_t *)(a)
#define HW_WRITE_REG(a,v) *(volatile uint32_t *)(a) = v

void ali_cli_reg_read(int argc, char **argv)
{
    if (argc < 2)
    {
        cli_printf("invalid param");
        return;
    }
    int addr = rf_simple_strtoul(argv[1], NULL ,0);
    int val = HW_READ_REG(addr);
    cli_printf("read reg 0x%x=0x%x", addr, val);
}

void ali_cli_reg_write(int argc, char **argv)
{
    if (argc < 3)
    {
        cli_printf("invalid param");
        return;
    }
    int addr = rf_simple_strtoul(argv[1], NULL ,0);
    int val = rf_simple_strtoul(argv[2], NULL ,0);
    HW_WRITE_REG(addr, val);
    cli_printf("success");
}

int ota_set_link(enum ota_link select_link)
{
	int32_t ret = 0;
    struct ota_boot_info boot_info;
	enum bootinfo_zone zone;

	zone = ota_get_valid_bootinfo_zone();
    ret = ota_get_bootinfo(&boot_info, zone);
    if (ret) {
        return -1;
    }

    if (boot_info.update_link == select_link) {
        //do nothing
    } else {
        boot_info.update_link = select_link;
		if (select_link == OTA_LINK_A ) {
			boot_info.linkA_used_flag = OTA_LINK_USED;
		} else if (select_link == OTA_LINK_B ) {
			boot_info.linkB_used_flag = OTA_LINK_USED;
		}
		ota_set_bootinfo_to_zoneAB(&boot_info);
    }

    return 0;
}

#define ERASE_OFFSET_LEN 0xC000

void ota_show_bootinfo_both_zone(void)
{
    struct ota_boot_info info;
    uint32_t num = 0;

    for (num = 0; num < OTA_BOOTINFO_ZONEMAX; num ++) {
        ota_get_bootinfo(&info, num);
        cli_printf("show zone_%d boot info:\n", num);
        cli_printf("crc32:0x%x\n", info.crc32);
        cli_printf("info_len:0x%x\n", info.info_len);
        cli_printf("linkA_used_flag:0x%x\n", info.linkA_used_flag);
        cli_printf("linkB_used_flag:0x%x\n", info.linkB_used_flag);
        cli_printf("update_link:0x%x\n", info.update_link);
        cli_printf("crash_reboot_count:0x%x\n", info.crash_reboot_count);
        cli_printf("secureERR_reboot_count:0x%x\n", info.secureERR_reboot_count);
        cli_printf("reboot_count_max:0x%x\n", info.reboot_count_max);
        cli_printf("reboot_reason:0x%x\n", info.reboot_reason);
        cli_printf("fallback_disable:0x%x\n", info.fallback_disable);
        cli_printf("\n");
    }
}

void ali_ota_cmd(int argc, char **argv)
{
	int32_t ret = 0;
    hal_partition_t partition_index;
    hal_logic_partition_t partition_info;
    struct ota_boot_info boot_info;
	enum bootinfo_zone zone;
	enum ota_link selected_link = 0;
	unsigned int write_addr = 0;
	char test[8] = "test";

    if (argc < 3) {
        cli_printf("ota get info\n");
        cli_printf("ota select A\n");
        cli_printf("ota select B\n");
        cli_printf("ota break boot2a\n");
        cli_printf("ota break boot2b\n");
        cli_printf("ota break rtosa\n");
        cli_printf("ota break rtosb\n");

        return;
    }

	zone = ota_get_valid_bootinfo_zone();
	ret = ota_get_bootinfo(&boot_info, zone);
	if (ret) {
		return ;
	}

    if (!strncmp(argv[1], "get", strlen(argv[1]))) {
		if (!strncmp(argv[2], "info", strlen(argv[2]))) {
		    if (boot_info.update_link == OTA_LINK_A) {
		        cli_printf("slot A\n");
		    } else if (boot_info.update_link == OTA_LINK_B) {
		        cli_printf("slot B\n");
		    }
		} else {
			cli_printf("%s %d, error parameter, argv[2]:%s\n", __func__, __LINE__, argv[2]);
			return ;
		}
    } else if (!strncmp(argv[1], "select", strlen(argv[1]))) {
        if (!strncmp(argv[2], "A", strlen(argv[2]))) {
			ota_set_link(OTA_LINK_A);
        } else if (!strncmp(argv[2], "B", strlen(argv[2]))) {
			ota_set_link(OTA_LINK_B);
        } else {
			cli_printf("%s %d, error parameter, argv[2]:%s\n", __func__, __LINE__, argv[2]);
			return ;
        }
    } else if (!strncmp(argv[1], "break", strlen(argv[1]))) {
        if (!strncmp(argv[2], "boot2a", strlen(argv[2]))) {
			partition_index = HAL_PARTITION_BOOT1;
			selected_link = OTA_LINK_A;
        } else if(!strncmp(argv[2], "boot2b", strlen(argv[2]))) {
            partition_index = HAL_PARTITION_BOOT1_REDUND;
			selected_link = OTA_LINK_B;
        } else if(!strncmp(argv[2], "rtosa", strlen(argv[2]))) {
		    if (boot_info.update_link == OTA_LINK_B) {
				//ota_flash_remap_flag = 1; //Now run rtosB, write rtosA.
		    }
			partition_index = HAL_PARTITION_RTOSA;
			selected_link = OTA_LINK_A;
        } else if(!strncmp(argv[2], "rtosb", strlen(argv[2]))) {
            partition_index = HAL_PARTITION_RTOSB;
			selected_link = OTA_LINK_B;
        } else {
			cli_printf("%s %d, error parameter, argv[2]:%s\n", __func__, __LINE__, argv[2]);
			return ;
        }

		ret = hal_flash_info_get(partition_index, &partition_info);
        if (ret) {
            cli_printf("error %s %d, hal_flash_info_get return != 0\n", __func__, __LINE__);
            return ;
        }

		write_addr = partition_info.partition_start_addr + ERASE_OFFSET_LEN;
		cli_printf("write_addr:0x%x\n", write_addr);

		ret = ota_adapt_flash_write(partition_index, write_addr, test, strlen(test));
		if (ret) {
	        cli_printf("error %s %d, ret:%d\n", __func__, __LINE__, ret);
		} else {
			ota_set_link(selected_link);
		}
    } else {
        cli_printf("%s %d, error parameter, argv[1]:%s\n", __func__, __LINE__, argv[1]);
        return ;
    }

	ota_show_bootinfo_both_zone();
	cli_printf("%s %s %s success\n", argv[0], argv[1], argv[2]);
}

#define RW_DATA_SZ 4096
void ali_flash_rw_check(int argc, char **argv)
{
    int times = 1;
    if (argc >= 2) {
        times = atoi(argv[1]);
    }
    int total_times = times;
    int failed_count = 0;
    char *str = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    aos_vfs_init();

    char dir_path[] = "/data/rw_check";
    char file[] = "file_";
    if(aos_mkdir(dir_path) < 0)
        cli_printf("mkdir %s failed \n",dir_path);
    char path[128] = {0};

    memcpy(path,dir_path,strlen(dir_path));
    path[strlen(dir_path)] = '/'; // "/data/rw_check/"
    memcpy(path+strlen(dir_path)+1,file,strlen(file)); // "/data/rw_check/file_"
    while(times--)
    {
        int fail_flag = 0;
        cli_printf("\nrw_check run %d \n",total_times-times);

        /* get rand data generate buf*/
        aos_srand(aos_now_ms());

        /* get index of str */
        for(int i = 0;i < total_times/62+1;i++)
        {
            int index = aos_rand()%62;
            /* /data/rw_check/file_xxx */
            path[strlen(dir_path)+1+strlen(file)+i] = str[index];
        }

        cli_printf("rw path %s \n",path);

        unsigned char buf[RW_DATA_SZ] = {0};
        for(int i = 0 ;i < sizeof(buf);i += 4)
        {
            int rand = aos_rand();
            //unsigned char *p = &rand;
            //cli_printf("0x%02x 0x%02x 0x%02x 0x%02x\n",(unsigned char)p[0],(unsigned char)p[1],(unsigned char)p[2],(unsigned char)p[3]);
            memcpy(buf+i,&rand,sizeof(int));
        }

        /* write data to flash */
        int w_fd = aos_open(path, O_CREAT | O_RDWR);
        ssize_t ret = aos_write(w_fd,buf,sizeof(buf));
        aos_close(w_fd);

        /* read data from flash and compare*/
        int r_fd = aos_open(path, O_CREAT | O_RDWR);
        for(int id = 0;id < sizeof(buf);id += 4)
        {
            int r_buf;
            aos_read(r_fd,&r_buf,sizeof(r_buf));
            unsigned char *p = &r_buf;
            if(p[0] != buf[id] || p[1] != buf[id+1] || p[2] != buf[id+2] || p[3] != buf[id+3])
            {
                cli_printf("read  buf:0x%02x 0x%02x 0x%02x 0x%02x\n",(unsigned char)p[0],(unsigned char)p[1],(unsigned char)p[2],(unsigned char)p[3]);
                cli_printf("write buf:0x%02x 0x%02x 0x%02x 0x%02x\n",(unsigned char)buf[id],(unsigned char)p[id+1],(unsigned char)p[id+2],(unsigned char)p[id+3]);
                fail_flag = 1;
                break;
            }
        }
        aos_close(r_fd);

        /* count failed count */
        if(fail_flag == 1)
        {
            failed_count++;
            cli_printf("rw_check run %d failed\n",total_times-times);
        }
        else
            cli_printf("rw_check run %d success\n",total_times-times);
    };
    /* delete dir */
    if (aos_access(dir_path, F_OK) == 0)
    {
        remove_sub_nodes(dir_path,1);
    }
    else
        cli_printf("rm: cannot remove '%s': No such file or directory \n",path);
    cli_printf("flash rw check run %d times, fail:%d\n",total_times,failed_count);
}

void ali_bt_scan_control(int argc, char **argv)
{
    for(int i=0;i< (argc - 1);i++)
    {
        hexString2CharBuf(&argv[i+1][2],&HciBuff[4],1);
    }

    if(HciBuff[4] == 0x03)
        cli_printf("open bt scan \n");
    else if(HciBuff[4] == 0x00)
        cli_printf("close bt scan \n");

    HciBuff[0] = 0x01;
    HciBuff[1] = 0x1a;
    HciBuff[2] = 0x0c;
    HciBuff[3] = 0x01;

    btdrv_hci_bridge_loop(HciBuff,5,HciRxBuff,&HciRxBuffLen);
    cli_printf("HciRxBuffLen %d\n",HciRxBuffLen);
    DUMP8("%02x ",HciRxBuff,HciRxBuffLen);

}

void ali_flash_read_loop(int argc, char **argv)
{
    int max_time = 0;
    if(argc > 1)
    {
        max_time = atoi(argv[1]);
        cli_printf("max_time %d \n",max_time);
    }
    for(int time = 0;time < max_time;time++)
    {
        aos_vfs_init();
        char *mp3_path = "/data/prompt";
        if (aos_access(mp3_path, F_OK) == 0) {
            aos_dir_t *dir_ptr = aos_opendir(mp3_path);
            if(dir_ptr != NULL)
            {
                aos_dirent_t *ptr = NULL;
                while((ptr = aos_readdir(dir_ptr))!=NULL)
                {
                    char f_path[128]={0};
                    memcpy(f_path, mp3_path, strlen(mp3_path));
                    f_path[strlen(mp3_path)] = '/';
                    memcpy(&f_path[strlen(mp3_path)+1], ptr->d_name, strlen(ptr->d_name));
                    cli_printf("play %s \n",f_path);

                    int fd = aos_open(f_path, O_CREAT | O_RDWR);
                    uint8_t rbuf[128] = {0};
                    while (aos_read(fd, rbuf, sizeof(rbuf)) > 0)
                    {
                        for(int j = 0;j < sizeof(rbuf);j++)
                        {
                            uint8_t hex_data = (uint8_t)rbuf[j];
                            cli_printf("0x%02x ",hex_data);
                        }
                        cli_printf("\n\n");
                    }
                    aos_close(fd);
                }
            }
            int ret = aos_closedir(dir_ptr);
            if(ret < 0)
                cli_printf("close %s failed\n",mp3_path);
        }
    }
}

void ali_wakeup_response(int argc, char **argv)
{
    if(argc<2){
        cli_printf("wakeup_response on/off(support ON/OFF)\n");
        return;
    }
    if((strcmp("on",argv[1])==0)||(strcmp("ON",argv[1])==0)){
        set_wakeup_response_flag(TRUE);
    }else if((strcmp("off",argv[1])==0)||(strcmp("OFF",argv[1])==0)){
        set_wakeup_response_flag(FALSE);
    }
}

#if 0
extern int alsa_audio_process_cfg_info_test(void);
void ali_audio_tuning_cfg(int argc, char **argv)
{
#if 1
    alsa_audio_process_cfg_info_test();
#else
    char *s = NULL;
    alsa_audio_process_cfg_info_create();
    s = alsa_audio_process_get_cfg_info();
    cli_printf("%s", s);
    alsa_audio_process_cfg_info_destory();
#endif
}

extern void alsa_audio_process_bypass_eq_set(uint8_t enable);
void ali_audio_process_bypass_eq(int argc, char **argv)
{
    if(argc<2)
    {
        cli_printf("bypass EQ arg error");
        return;
    }

    if(strcmp("on", argv[1]) == 0)
    {
        alsa_audio_process_bypass_eq_set(1);
        cli_printf("start Bypass EQ: disable EQ iir filter and disable DRC process but still support limiter process");
    }
    else if(strcmp("off", argv[1]) == 0)
    {
        alsa_audio_process_bypass_eq_set(0);
        cli_printf("stop Bypass EQ");
    }
    else
    {
        cli_printf("bypass EQ arg error");
        return;
    }
}

#include "alsa_wav_player.h"
void ali_ultrasound_test(int argc, char ** argv)
{
    if(argc<3)
    {
        cli_printf("ultrasound test arg error");
        return;
    }

    alsa_wav_player_file_opt_t opt;
    opt.close = aos_close;
    opt.open = aos_open;
    opt.read = aos_read;
    alsa_wav_player_file_opt_register(&opt);

    if(strcmp("start", argv[1]) == 0)
    {
        cli_printf("start ultrasound test path_1 %s, path_2 %s", argv[2], argv[3]);
        alsa_ultra_sound_test(1, argv[2], argv[3]);
    }
    else if(strcmp("stop", argv[1]) == 0)
    {
        alsa_ultra_sound_test(0, NULL, NULL);
        cli_printf("stop ultrasound test");
    }
    else
    {
        cli_printf("ultrasound test arg error");
        return;
    }
}
#endif
#include "tg_light.h"
void ali_pwm_test(int argc, char **argv)
{
    int times = 1;
    int color_ch = 1;
    if (argc >= 3) {
        times = atoi(argv[2]);
        color_ch = atoi(argv[1]);
    }
    else
    {
        cli_printf("pwm_breath color_channel times : pwm_breath 1 5 \r\n");
    }
    while(times--)
    {
        for(int i = 0;i <= 255;i++)
        {
            tg_set_pwm_led_target_channel_brightness(color_ch,i);
            osDelay(5);
        }
        for(int i = 255;i >= 0;i--)
        {
            tg_set_pwm_led_target_channel_brightness(color_ch,i);
            osDelay(5);
        }
    }
    cli_printf("success \r\n");
}

void ali_pwm_led(int argc, char **argv)
{
    int lightness = 0;
    int color_ch = 0;
    if (argc >= 3) {
        lightness = atoi(argv[2]);
        color_ch = atoi(argv[1]);
    }
    else
    {
        cli_printf("pwm_led <channel> <lightness> : pwm_led <0~2>> <0~255> \r\n");
    }
    if (lightness > 255 || lightness < 0)
    {
        cli_printf("lightness = <0 ~ 255> failed \r\n");
    }

    tg_set_pwm_led_target_channel_brightness(color_ch,lightness);

    cli_printf("success \r\n");
}

extern void cpu_stats_show_set(uint32_t flag);
void ali_cpuusage_show(int argc, char **argv)
{
    int flag = 0;

    if(argc < 2){
        cli_printf("cpuusage_show open/close\r\n");
    }
    else{
        flag = atoi(argv[1]);
        cpu_stats_show_set(flag);
    }
}

extern struct finsh_syscall *_usrcmd_table_begin, *_usrcmd_table_end;
void vendor_cli_register_init(void)
{
    int ret;
#if 0
    struct finsh_syscall  *index;
    struct cli_command_st *cmd;

    for (index = (struct finsh_syscall *)&_usrcmd_table_begin; (uint32_t)index < (uint32_t)&_usrcmd_table_end; index++)
    {
        cmd = (struct cli_command_st *)cli_malloc(sizeof(struct cli_command_st));
        if (cmd == NULL) {
            cli_printf("usr cli malloc fail\n");
            return;
        }

        cmd->name     = index->name;
        cmd->help     = index->desc;
        cmd->function = index->func;

        ret = cli_register_command(cmd);
        if (ret != CLI_OK) {
            cli_printf("vendor cli register fail\n");
            return;
        }
    }
#endif
}


