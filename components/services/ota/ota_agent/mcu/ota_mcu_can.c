#if defined OTA_CONFIG_MCU_CAN
#include "ota_log.h"
#include "ota_hal_os.h"
#include "ota_mcu_upgrade.h"
#include "canopen/canopen.h"
#include "canopen/canopen_od.h"
#include "ota_hal_param.h"
#include "board.h"

int ota_mcu_data_package(unsigned short cmd, unsigned char* data, unsigned int data_len, unsigned char* buf, unsigned int *len, unsigned int data_idx)
{
    unsigned short crc16 = 0;
    unsigned int cmd_len = 0;
    int ret = OTA_MCU_INIT_FAIL;
    if(buf == NULL || len == NULL) {
         return ret;
    }
    memset(buf, 0, OTA_MCU_BLOCK_SIZE);
    /* cmd type */
    buf[0] = 0xAB;
    buf[3] = (unsigned char)((cmd & 0xff00) >> 8);
    buf[4] = (unsigned char)(cmd & 0x00ff);
    cmd_len += 5;
    /* cmd data */
    if((data != NULL) && (data_len != 0)) {
         buf[cmd_len] = (unsigned char)((data_idx & 0xff000000) >> 24);
         buf[cmd_len + 1] = (unsigned char)((data_idx & 0xff0000) >> 16);
         buf[cmd_len + 2] = (unsigned char)((data_idx & 0xff00) >> 8);
         buf[cmd_len + 3] = (unsigned char)(data_idx & 0x00ff);
         cmd_len += 4;
         memcpy(buf + cmd_len, data, data_len);
         cmd_len += data_len;
    }
    /* cmd len */
    buf[1] = (unsigned char)(((cmd_len + 2) & 0xff00) >> 8);
    buf[2] = (unsigned char)((cmd_len +2 ) & 0x00ff);
    /* cmd crc */
    crc16 = ota_get_data_crc16(buf, cmd_len);
    buf[cmd_len] = (unsigned char)((crc16 & 0xff00) >> 8);
    buf[cmd_len + 1] = (unsigned char)(crc16 & 0x00ff);
    cmd_len += 2;
    *len = cmd_len;
    OTA_LOG_I("cmd:0x%02x len:%d  data len:%d crc16:0x%02x data_idx:%d \n", cmd, cmd_len, data_len, crc16, data_idx);
    return 0;
}

int ota_mcu_can_upgrade(unsigned int size, char* ver, char *md5)
{
    int ret = OTA_MCU_INIT_FAIL;
    int idx = 0;
    unsigned int len = size;
    unsigned int cmd_len = 0;
    can_node_id_t *node_dev = NULL;
    unsigned char buf[OTA_MCU_BLOCK_SIZE] = {0};
    unsigned char res_buf[OTA_VER_LEN] = {0};
    unsigned int  offset = 0;
    unsigned int  img_size = 0;
    unsigned int  data_idx = 0;
    unsigned char* block = NULL;

    if(ver == NULL) {
         OTA_LOG_I(" version is NULL\n");
         return ret;
    }
    /* Find sub devices. */
    while(NULL != (node_dev = foreach_can_node(PORT_CAN_IO, &idx))){
         idx++;
         OTA_LOG_I("CAN MCU OTA idx:%d node:0x%p \n", idx, node_dev);
         /* 1.Start CAN upgrade */
         ret = co_upgrade_start(node_dev, true);
         if (ret < 0) {
              ret = 0;
              OTA_LOG_I("NO found node:%d \n", idx);
              continue;
         }
         /* 2.Query MCU device vesion */
         memset(res_buf, 0 , OTA_VER_LEN);
         memset(buf, 0 , OTA_MCU_BLOCK_SIZE);
         ret = ota_mcu_hal_version(node_dev, (char *)res_buf);
         if (ret < 0) {
              ret = OTA_MCU_VERSION_FAIL;
              OTA_LOG_I("version err:%d\n", ret);
              return ret;
         }
         if(strncmp(ver, (const char *)res_buf, sizeof(res_buf)) == 0) {
              OTA_LOG_I("same version, skip it.");
              continue;
         }
         OTA_LOG_I("mcu version:%s.", res_buf);
         /* 3.CMD: MCU CAN device ready */
         ret = ota_mcu_data_package(OTA_MCU_CMD_READY, NULL, 0, buf, &cmd_len, 0);
         if (ret < 0) {
              OTA_LOG_I("ready err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_send(node_dev, buf, cmd_len);
         if (ret < 0) {
              ret = OTA_MCU_NOT_READY;
              OTA_LOG_I("send ready err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_recv(node_dev, res_buf, sizeof(res_buf));
         if (ret < 0 || res_buf[0] != 0x14) {
              ret = OTA_MCU_NOT_READY;
              OTA_LOG_I("ready res err:%d\n", ret);
              return ret;
         }
         /* 4.CMD: MCU CAN device reboot */
         ret = ota_mcu_data_package(OTA_MCU_CMD_REBOOT, NULL, 0, buf, &cmd_len, 0);
         if (ret < 0) {
              OTA_LOG_I("reboot err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_send(node_dev, buf, cmd_len);
         if (ret < 0) {
              ret = OTA_MCU_REBOOT_FAIL;
              OTA_LOG_I("send reboot err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_recv(node_dev, res_buf, sizeof(res_buf));
         if (ret < 0 || res_buf[0] != 0x14) {
              ret = OTA_MCU_REBOOT_FAIL;
              OTA_LOG_I("reboot res err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_recv(node_dev, res_buf, sizeof(res_buf));
         if (ret < 0 || res_buf[0] != 0x28) {
              ret = OTA_MCU_REBOOT_FAIL;
              OTA_LOG_I("not allow upgrade err:%d\n", ret);
              return ret;
         }
         /* 5. MCU CAN device send header */
         unsigned char header[36] = {0};
         header[0] = (unsigned char)((size & 0xff000000) >> 24);
         header[1] = (unsigned char)((size & 0xff0000) >> 16);
         header[2] = (unsigned char)((size & 0xff00) >> 8);
         header[3] = (unsigned char)(size & 0x00ff);
         data_idx = 0;
         ret = ota_mcu_data_package(OTA_MCU_CMD_DATA, header, sizeof(header), buf, &cmd_len, data_idx);
         if (ret < 0) {
              OTA_LOG_I("header err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_send(node_dev, buf, cmd_len);
         if (ret < 0) {
              ret = OTA_MCU_HEADER_FAIL;
              OTA_LOG_I("send header err:%d\n", ret);
              return ret;
         }
         ret = ota_mcu_hal_recv(node_dev, res_buf, sizeof(res_buf));
         if (ret < 0 || res_buf[0] != 0x14) {
              ret = OTA_MCU_HEADER_FAIL;
              OTA_LOG_I("send header err:%d\n", ret);
              return ret;
         }
         offset = 0;
         data_idx = 1;
         img_size = 0;
         memset(buf, 0 , OTA_MCU_BLOCK_SIZE);
         ret = OTA_MCU_UPGRADE_FAIL;
         OTA_LOG_I("MCU start upgrade ...\n");
         /* 6. MCU CAN device send data loop */
         while (img_size  < size) {
              unsigned int block_size = 0 ;
              block_size = (len > (OTA_MCU_BLOCK_SIZE - 32) )? (OTA_MCU_BLOCK_SIZE - 32): len;
              block = ota_malloc(block_size);
              if(block == NULL) {
                   OTA_LOG_I("memory err\n");
                   return ret;
              }
              ret = ota_mcu_hal_read(&offset, block, block_size);
              if (ret < 0) {
                   if(block != NULL) {
                        ota_free(block);
                        block = NULL;
                   }
                   ret = OTA_MCU_UPGRADE_FAIL;
                   OTA_LOG_I("mcu read err:%d\n", ret);
                   return ret;
              }
              ret = ota_mcu_data_package(OTA_MCU_CMD_DATA, block, block_size, buf, &cmd_len, data_idx);
              if (ret < 0) {
                   OTA_LOG_I("data err:%d\n", ret);
                   if(block != NULL) {
                        ota_free(block);
                        block = NULL;
                   }
                   return ret;
              }
              ret = ota_mcu_hal_send(node_dev, buf, cmd_len);
              if (ret < 0) {
                   OTA_LOG_I("send data err:%d\n", ret);
                   if(block != NULL) {
                        ota_free(block);
                        block = NULL;
                   }
                   ret = OTA_MCU_UPGRADE_FAIL;
                   return ret;
              }
              ret = ota_mcu_hal_recv(node_dev, res_buf, sizeof(res_buf));
              if (ret < 0 || res_buf[0] != 0x14) {
                   OTA_LOG_I("recv data err:%d\n", ret);
                   if(block != NULL) {
                        ota_free(block);
                        block = NULL;
                   }
                   ret = OTA_MCU_UPGRADE_FAIL;
                   return ret;
              }
              img_size += block_size;
              len -= block_size;
              data_idx++;
              OTA_LOG_I("mcu send %s block res:0x%02x size:0x%x\n", data_idx, res_buf[0], img_size);
         }
         /* 7. upgrade complete to stop CAN */
         ret = co_upgrade_start(node_dev, false);
         if (ret < 0) {
              ret = OTA_MCU_UPGRADE_FAIL;
              if(block != NULL) {
                   ota_free(block);
		   block = NULL;
              }
              ret = OTA_MCU_UPGRADE_FAIL;
              return ret;
         }
    }
    if(block != NULL) {
	     ota_free(block);
	     block = NULL;
    }
    OTA_LOG_I("CAN OTA complete ret:%d \n", ret);
    return ret;
}
#endif
