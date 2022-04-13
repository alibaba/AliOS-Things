#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "upack_data_file.h"
#include "ota_log.h"

int data_file_unpack(void *pack_file, unsigned int pack_size, void *upack_path)
{
   int ret = -1;
   int i = 0;
   char tmp_file_name[128];
   int tmp_file_name_len = 0;
   FILE *read_fd = NULL;
   unsigned int upack_path_len = 0;
   unsigned int read_len = 0;
   data_file_pack_head_t pack_head;
   data_file_infor_t file_info;
   if((pack_file != NULL) &&
      (upack_path != NULL) &&
      (strlen(upack_path) <= 64)) {
       OTA_LOG_E("upack_path = %d\r\n", strlen(upack_path));
       memset(tmp_file_name, 0x00, sizeof(tmp_file_name));
       memset(&pack_head, 0x00, sizeof(data_file_pack_head_t));
       memset(&file_info, 0x00, sizeof(data_file_infor_t));
       upack_path_len = strlen(upack_path);
       strcpy(tmp_file_name, upack_path);
       if(tmp_file_name[upack_path_len] != '/') {
          tmp_file_name[upack_path_len++] = '/'; 
       }
       OTA_LOG_I("upack_path = %s\r\n", upack_path);
       read_fd = ota_fopen(pack_file, "rb");
       if(read_fd != NULL) {
           read_len = ota_fread(&pack_head, 1, sizeof(data_file_pack_head_t), read_fd);
           OTA_LOG_E("read_len = %d\r\n", read_len);
           if(read_len != sizeof(data_file_pack_head_t) ||
              pack_head.pack_size != pack_size) {
               OTA_LOG_E("file: %s is len erro, pack size = %d\r\n", pack_file, pack_head.pack_size);
           }
           else {
                ret = 0;
                for(i = 0; i < pack_head.file_numb; i++) {
                    read_len = ota_fread(&file_info, 1, sizeof(data_file_infor_t), read_fd);
                    if(read_len != sizeof(data_file_infor_t)) {
                        ret = -1;
                        OTA_LOG_E("file:%s reand file info len err\r\n", read_len);
                        break;
                    }
                    else {
                        tmp_file_name_len = file_info.head_size - sizeof(data_file_infor_t);
                        if(tmp_file_name_len > sizeof(tmp_file_name) - upack_path_len) {
                            ret = -1;
                            OTA_LOG_E("file%d name too long\r\n", i);
                            break;
                        }
                        else {
                            read_len = ota_fread(&tmp_file_name[upack_path_len], 1, tmp_file_name_len, read_fd);
                            if(read_len != tmp_file_name_len) {
                                ret = -1;
                                OTA_LOG_E("read file%d name err\r\n", i);
                                break;
                            }
                            else {
                               FILE *write_fd = NULL;
                               int tmp_read_len = 0;
                               int has_read_len = 0;
                               int tmp_write_len = 0;
                               char tmp_buf[128];
                               OTA_LOG_I("file = %s\r\n", tmp_file_name);
                               write_fd = ota_fopen(tmp_file_name, "wb");
                               while(has_read_len < file_info.file_size) {
                                   file_info.file_size - has_read_len > sizeof(tmp_buf) ? (tmp_read_len = sizeof(tmp_buf)) : (tmp_read_len = file_info.file_size - has_read_len);
                                   read_len = ota_fread(tmp_buf, 1, tmp_read_len, read_fd);
                                   if(read_len != tmp_read_len) {
                                       ota_fclose(write_fd);
                                       write_fd = NULL;
                                       OTA_LOG_E("file%d, read file data err\r\n", i);
                                       ret = -1;
                                       break;
                                   }
                                   else {
                                      tmp_write_len = ota_fwrite(tmp_buf, 1, tmp_read_len, write_fd);
                                      if(tmp_write_len != tmp_read_len) {
                                          ota_fclose(write_fd);
                                          write_fd = NULL;
                                          OTA_LOG_E("file%d, write file failed\r\n", i);
                                          ret = -1;
                                          break;
                                      }
                                      else {
                                          has_read_len += tmp_read_len;
                                      }
                                   }
                               }
                               memset(&tmp_file_name[upack_path_len], 0x00, sizeof(tmp_file_name) - upack_path_len);
                               if(ret == 0) {
                                   ota_fclose(write_fd);
                                   write_fd = NULL;
                                   OTA_LOG_E("file%d,write suc!\r\n", i);
                               }
                               else {
                                   OTA_LOG_E("file%d, write err!\r\n", i);
                                   break;
                               }
                            }
                        }
                    }
                }
           }
       }
       else {
           OTA_LOG_E("pack faile: %s open failed\n", pack_file);
       }
   }
   if(read_fd != NULL) {
       ota_fclose(read_fd);
       read_fd = NULL;
   }
   if(ret < 0) {
       OTA_LOG_E("unpack failed\n");
   }
   return ret;
}
