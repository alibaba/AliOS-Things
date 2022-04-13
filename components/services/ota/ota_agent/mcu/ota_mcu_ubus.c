#if defined OTA_CONFIG_MCU_UBUS
#include "ota_log.h"
#include "ota_hal_os.h"
#include "board.h"
#include "ubus/ubus.h"
#include "ubus/ubus_profile.h"
#include "aos/kernel.h"
#include "ota_import.h"

#define FILE_SEND_BEGIN               (0x00)
#define FILE_SEND_DOING               (0x01)
#define FILE_SEND_DONE                (0x02)
#define SEND_BUF_SIZE                 (1100)

#define OTA_UBUS_QUEUE_TMOUT          (10000)
#define OTA_UBUS_MCU_OTA_RESULT_TMOUT (180000)
#define OTA_UBUS_RECVD_CMD_NUM        (5)
#define OTA_UBUS_CMD_SIZE             (32)

int ble_file_translate_ubus_init(void);
static void handle_upgrade_ubus_recvd_msg(const dbus_data_t data_type, const uint16_t len, const void *data, const void *user_data);
static unsigned char ota_ubus_cmd_buf[OTA_UBUS_CMD_SIZE * OTA_UBUS_RECVD_CMD_NUM];
static unsigned char file_send_buf[SEND_BUF_SIZE];
static aos_queue_t ota_ubus_queue;

static rx_handler_node_t ble_ubus_rx_handler_node[] = {
    {ubus_data_slave_bin_resp, handle_upgrade_ubus_recvd_msg, NULL},
};

#ifdef  DEBUG_MCU_UPGRADE
static void *ota_mcu_upgrade_thread = NULL;
static volatile ota_mcu_upgrade_debug_flag = 0;
static volatile ota_mcu_upgrade_cb_init_flag = 0;
#endif

static unsigned char tmp_send_buf[1100];
int ble_ota_ubus_send(unsigned short data_type, unsigned char *buf, unsigned short send_len)
{
     int ret = -1;
     tmp_send_buf[0] = 1;
     if((buf != NULL) && (send_len > 0) && (send_len <= 1050)) {
          if(data_type == 0) {
               tmp_send_buf[1] = ubus_data_upgrade_header;
          }
          else {
               tmp_send_buf[1] = ubus_data_upgrade_text;
          }
          SET_CUBE_LEN(&tmp_send_buf[1], send_len);
          memcpy(&tmp_send_buf[sizeof(ubus_data_cube_t)+1], buf, send_len);
          LOGI("Pre", "len %d 0x%x 0x%x 0x%x 0x%x 0x%x", send_len + 4, *((uint32_t *)(&tmp_send_buf[0])),
          *((uint32_t *)(&tmp_send_buf[4])), *((uint32_t *)(&tmp_send_buf[8])),
          *((uint32_t *)(&tmp_send_buf[12])), *((uint32_t *)(&tmp_send_buf[16])));
          ret = ubus_send(ubus_comm_service, send_len + 4, tmp_send_buf); 
    }
    if(ret != 0) {
          OTA_LOG_E("Send uBUS Failed %d %d 0x%x", send_len, data_type, buf);
    }
    return ret;
}

static int ble_ota_ubus_recvd(unsigned char *buf, unsigned int *recvd_len, unsigned int timeout)
{
    int ret = -1;
    unsigned int tmp_len = 0;
    if((buf != NULL) && (recvd_len != NULL)) {
        ret = aos_queue_recv(&ota_ubus_queue, timeout, buf, &tmp_len);
        *recvd_len = tmp_len;
    }
    return ret;
}

static unsigned char cmd_recvd_buf[100];
static void handle_upgrade_ubus_recvd_msg(const dbus_data_t data_type, const uint16_t len, const void *data, const void *user_data)
{
     int ret = 0;
     if((data != NULL) && (len <= sizeof(cmd_recvd_buf))) {
          memcpy(cmd_recvd_buf, data, len);
          ret = aos_queue_send(&ota_ubus_queue, cmd_recvd_buf, len);
          if(ret != 0) {
               OTA_LOG_E("ota msg queue send err");
          }
    }
    
}

int ble_file_translate_ubus_init()
{
     int ret = 0;
     uint8_t rx_list_size = sizeof(ble_ubus_rx_handler_node) / sizeof(ble_ubus_rx_handler_node[0]);
#ifdef  DEBUG_MCU_UPGRADE
     if(ota_mcu_upgrade_cb_init_flag == 1) {
          return 0;
     }
     ota_mcu_upgrade_cb_init_flag = 1;
#endif
     if (rx_list_size != ubus_register_rx_handler_list(rx_list_size, ble_ubus_rx_handler_node)) {
          OTA_LOG_E("Subscribe data Failed");
          ret = -1;
     }
     else {
          ret = aos_queue_new(&ota_ubus_queue, ota_ubus_cmd_buf, OTA_UBUS_CMD_SIZE * OTA_UBUS_RECVD_CMD_NUM, OTA_UBUS_CMD_SIZE);
          if(ret != 0) {
               OTA_LOG_E("ota creat ubus queue fail");
          }
     }
     return ret;
}

#define OTA_MCU_SEND_SIZE     (1024)
int ota_mcu_ubus_upgrade(unsigned int partition_offset, unsigned char file_type, unsigned int size, 
                         char *ver, unsigned short file_crc16)
{
     int ret = -1;
     unsigned char loop_lock = 1;
     unsigned char status = 0;
     unsigned char ver_len = 0;
     unsigned short send_len = 0;
     unsigned short src_crc16 = 0;
     unsigned short frame_crc16 = 0;
     unsigned char recv_buf[100] = {0};
     unsigned int recvd_len = 0;
     unsigned char frame_header = 0;
     unsigned int retry_cnt = 0;
     unsigned int has_send_len = 0;
     unsigned int offset = 0;
     unsigned short  frame_index = 0;
     unsigned int last_ack_tmout = OTA_UBUS_QUEUE_TMOUT;
#ifdef  DEBUG_MCU_UPGRADE
     if(ota_mcu_upgrade_debug_flag == 1) {
          OTA_LOG_E("ota mcu debug upgrade is on going");
          return ret;
     }
#endif
     if((ver == NULL) || (size == 0) ||
        (file_crc16 == 0) || (file_crc16 == 0xff) || (file_type == 0)) {
          OTA_LOG_E("mcu param err");
          return ret;
     }
     ver_len = strlen(ver);
     if(ver_len > 32) {
          OTA_LOG_E("file ver err");
          return ret;
     }
     memset(file_send_buf, 0x00, sizeof(file_send_buf));
     OTA_LOG_I("send size = %d\r\n", size);
     while(loop_lock) {
          switch (status) {
               case FILE_SEND_BEGIN:
                    frame_index = 0;
                    has_send_len = 0;
                    send_len = 47;
                    if(file_type == OTA_RECVD_BLE_SLAVER_IMAGE) {
                         last_ack_tmout = OTA_UBUS_MCU_OTA_RESULT_TMOUT;
                    }
                    file_send_buf[0] = 0xAB;
                    file_send_buf[1] = (unsigned char)((send_len >> 8) & 0x00ff);
                    file_send_buf[2] = (unsigned char)(send_len & 0x00ff);
                    file_send_buf[3] = file_type;
                    file_send_buf[4] = 0x00;
                    file_send_buf[5] = (unsigned char)((frame_index >> 8) & 0x00ff);
                    file_send_buf[6] = (unsigned char)(frame_index & 0x00ff);
                    file_send_buf[7] = (unsigned char)((size & 0xff000000) >> 24);
                    file_send_buf[8] = (unsigned char)((size & 0x00ff0000) >> 16);
                    file_send_buf[9] = (unsigned char)((size & 0x0000ff00) >> 8);
                    file_send_buf[10] = (unsigned char)(size & 0x000000ff);
                    memcpy(&file_send_buf[11], ver, ver_len);
                    file_send_buf[43] = (unsigned char)((file_crc16 >> 8) & 0x00ff);
                    file_send_buf[44] = (unsigned char)(file_crc16 & 0x00ff);
                    src_crc16 = ota_get_data_crc16(file_send_buf, ver_len + 13);
                    file_send_buf[45] = (char)((src_crc16 >> 8) & 0x00ff);
                    file_send_buf[46] = (char)(src_crc16 & 0x00ff);
                    ret = ble_ota_ubus_send(0, file_send_buf, send_len);
                    if(ret == 0) {
                         aos_msleep(1);
                         ret = ble_ota_ubus_recvd(recv_buf, &recvd_len, OTA_UBUS_QUEUE_TMOUT);
                         if(ret == 0) {
                              ret = -1;
                              frame_header = recv_buf[0];
                              src_crc16 = L_EXTRACT_U16(&recv_buf[recvd_len - 2]);
                              frame_crc16 = ota_get_data_crc16(recv_buf, recvd_len - 2);
                              if((recvd_len >= 9) && (frame_header == 0xBA) &&
                                   (src_crc16 == frame_crc16)) {
                                   if(recv_buf[6] == 0x00) {
                                        ret = 0;
                                        status = FILE_SEND_DOING;
                                        retry_cnt = 0;
                                        has_send_len = 0;
                                        frame_index++;
                                        OTA_LOG_I("ok success!");
                                   }
                                   else {
                                        OTA_LOG_E("head recvd err:%d\r\n", recv_buf[6]);
                                   }
                              }
                         }
                         else {
                              OTA_LOG_E("head recvd failed ret:%d", ret);
                         }
                    }
                    if(ret != 0) {
                         retry_cnt++;
                         if(retry_cnt > 2) {
                              loop_lock = 0;
                              OTA_LOG_E("mcu upgrade head send failed");
                              break;
                         }
                    }
                    break;
               case FILE_SEND_DOING:
                    if(has_send_len < size) {
                         (size - has_send_len > OTA_MCU_SEND_SIZE) ? (send_len = OTA_MCU_SEND_SIZE) : (send_len = size - has_send_len);
                         file_send_buf[0] = 0xAB;
                         file_send_buf[3] = file_type;
                         file_send_buf[4] = 0x00;
                         file_send_buf[5] = (unsigned char)((frame_index >> 8) & 0x00ff);
                         file_send_buf[6] = (unsigned char)(frame_index & 0x00ff);
                         offset = partition_offset + has_send_len;
                         ret = ota_hal_read(&offset, &file_send_buf[7], send_len);
                         if(ret >= 0) {
                              send_len += 9;
                              file_send_buf[1] = (unsigned char)((send_len >> 8) & 0x00ff);
                              file_send_buf[2] = (unsigned char)(send_len & 0x00ff);
                              src_crc16 = ota_get_data_crc16(file_send_buf, send_len - 2);
                              file_send_buf[send_len - 2] = (char)((src_crc16 >> 8) & 0x00ff);
                              file_send_buf[send_len - 1] = (char)(src_crc16 & 0x00ff);
                              (void)ble_ota_ubus_send(1, file_send_buf, send_len);
                              aos_msleep(1);
                              ret = ble_ota_ubus_recvd(recv_buf, &recvd_len, OTA_UBUS_QUEUE_TMOUT);
                              if(ret == 0) {
                                   ret = -1;
                                   frame_header = recv_buf[0];
                                   src_crc16 = L_EXTRACT_U16(&recv_buf[recvd_len - 2]);
                                   frame_crc16 = ota_get_data_crc16(recv_buf, recvd_len - 2);
                                   if((recvd_len >= 9) && (frame_header == 0xBA) &&
                                        (src_crc16 == frame_crc16)) {
                                        if(recv_buf[6] == 0x00) {
                                             ret = 0;
                                             retry_cnt = 0;
                                             frame_index++;
                                             has_send_len += send_len - 9;
                                             if(has_send_len == size) {
                                                  status = FILE_SEND_DONE;
                                                  OTA_LOG_I("file translate over!");
                                                  break;
                                             }
                                        }
                                        else {
                                             OTA_LOG_E("mcu upgrade ack err!");  
                                        }
                                   }
                              }
                         }
                    }
                    else {
                         ret = -1;
                         loop_lock = 0;
                         OTA_LOG_E("mcu upgrade send_len err size = 0x%x, has_send_len = 0x%x", size, has_send_len);
                         break;
                    }
                    if(ret != 0) {
                         retry_cnt++;
                         if(retry_cnt > 2) {
                              loop_lock = 0;
                              OTA_LOG_E("mcu upgrade data send failed");
                              break;
                         }
                    }
                    break;
               case FILE_SEND_DONE:
                    send_len = 13;
                    file_send_buf[0] = 0xAB;
                    file_send_buf[1] = (unsigned char)((send_len >> 8) & 0x00ff);
                    file_send_buf[2] = (unsigned char)(send_len & 0x00ff);
                    file_send_buf[3] = file_type;
                    file_send_buf[4] = 0xF0;
                    file_send_buf[5] = (unsigned char)((frame_index >> 8) & 0x00ff);
                    file_send_buf[6] = (unsigned char)(frame_index & 0x00ff);
                    file_send_buf[7] = 0x00;
                    file_send_buf[8] = 0x00;
                    file_send_buf[9] = 0x00;
                    file_send_buf[10] = 0x00;
                    src_crc16 = ota_get_data_crc16(file_send_buf, send_len);
                    file_send_buf[11] = (char)((src_crc16 >> 8) & 0x00ff);
                    file_send_buf[12] = (char)(src_crc16 & 0x00ff);
                    (void)ble_ota_ubus_send(1, file_send_buf, send_len);
                    aos_msleep(1);
                    ret = ble_ota_ubus_recvd(recv_buf, &recvd_len, last_ack_tmout);
                    if(ret == 0) {
                         ret = -1;
                         OTA_LOG_I("rcvd slaver dev ack!");
                         frame_header = recv_buf[0];
                         src_crc16 = L_EXTRACT_U16(&recv_buf[recvd_len - 2]);
                         frame_crc16 = ota_get_data_crc16(recv_buf, recvd_len - 2);
                         if((recvd_len >= 9) && (frame_header == 0xBA) &&
                              (src_crc16 == frame_crc16)) {
                              if(recv_buf[6] == 0x00) {
                                   retry_cnt = 0;
                                   loop_lock = 0;
                                   ret = 0;
                                   OTA_LOG_I("data upgrade ok!");
                                   break;
                              }
                              else {
                                  OTA_LOG_E("mcu upgrade last packet ack failed"); 
                              }
                         }
                    }
                    if(ret != 0) {
                         retry_cnt = 0;
                         loop_lock = 0;
                         OTA_LOG_E("mcu upgrade data recvd failed");
                         break;
                    }
                    break;
               default:
                    break;
          }
     }
#ifdef  DEBUG_MCU_UPGRADE
     ota_mcu_upgrade_debug_flag = 0;
#endif
     return ret;
}

#ifdef  DEBUG_MCU_UPGRADE
static void ota_mcu_upgrade_debug_start(void)
{
     int ret = 0;
     char *filename = "CPIN-2.0.0.0";
     ret = ota_mcu_ubus_upgrade(146752, OTA_RECVD_BLE_SLAVER_IMAGE, 188760, filename, 0x421b);
     if(ret != 0) {
          OTA_LOG_E("mcu upgrade failed");
     }
}

int ota_mcu_upgrade_debug_task_init()
{
     int ret = 0;
     ble_file_translate_ubus_init();
     if(ota_mcu_upgrade_debug_flag == 1) {
          return -1;
     }  
     ret = ota_thread_create(&ota_mcu_upgrade_thread, (void *)ota_mcu_upgrade_debug_start, (void *)NULL, NULL, 1024 * 8);
     if(ret >= 0) {
          ota_mcu_upgrade_debug_flag = 1;
     }
     else {
          OTA_LOG_E("ota mcu upgrade task creat failed");
          ret = -1;
     }
     return ret;
}
#endif
#endif