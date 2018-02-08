
/*
* uvc video capture example
*/
#include "example_media_tl.h"
#include "mjpeg2jpeg.h"

#include "ff.h"
#include <fatfs_ext/inc/ff_driver.h>
#include "sdio_host.h"
#include <disk_if/inc/sdcard.h>

void example_tl_fatfs_thread(unsigned char* write_buf, const int write_buf_len, const int iter){
  
  u8 *tl_WRBuf;
  const int sizeof_write_buf = ( write_buf_len*(sizeof(u8)) );
  tl_WRBuf = (u8*)malloc(sizeof_write_buf);
  int drv_num = 0;
  int Fatfs_ok = 0;
  FRESULT res; 
  FATFS 	m_fs;
  FIL		m_file;
  char	  logical_drv[4]; /* root diretor */
 
  char f_num[15];
  sprintf(f_num, "%d", iter);
  char filename[64] = "img";
  strcat(filename,f_num);
  strcat(filename,".jpeg");
  char path[64];
  int bw;
  int test_result = 1;
  // Set sdio debug level
  DBG_INFO_MSG_OFF(_DBG_SDIO_);  
  DBG_WARN_MSG_OFF(_DBG_SDIO_);
  DBG_ERR_MSG_ON(_DBG_SDIO_);
  if(sdio_init_host()!=0){
    printf("SDIO host init fail.\n");
    exit(NULL);
  }
  //1 register disk driver to fatfs
  drv_num = FATFS_RegisterDiskDriver(&SD_disk_Driver);
  if(drv_num < 0)
    printf("Rigester disk driver to FATFS fail.\n");
  else{
    Fatfs_ok = 1;
    logical_drv[0] = drv_num + '0';
    logical_drv[1] = ':';
    logical_drv[2] = '/';
    logical_drv[3] = 0;
  }
  //1 Fatfs write 
  if(Fatfs_ok){
    if(f_mount(&m_fs, logical_drv, 1)!= FR_OK){
      printf("FATFS mount logical drive fail.\n");
      goto fail;
    }
    // write and read test
    strcpy(path, logical_drv);
    sprintf(&path[strlen(path)],"%s",filename);
    //Open source file
    res = f_open(&m_file, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    if(res){
      printf("open file (%s) fail.\n", filename);
      goto fail;
    }
    printf("Test file name:%s\n\n",filename);
    DWORD size;
    size = (&m_file)->fsize;
    res = f_lseek(&m_file,size);
    if(res){
        f_lseek(&m_file, 0); 
        printf("file Seek error.\n");
      }
    // clean write and read buffer
    memset(tl_WRBuf, 0x00, write_buf_len);
    memcpy(tl_WRBuf, write_buf, write_buf_len);
    do{
      res = f_write(&m_file, tl_WRBuf, sizeof_write_buf, (u32*)&bw);
      if(res){
        f_lseek(&m_file, 0); 
        printf("Write error.\n");
      }
      printf("Write %d bytes.\n", bw);
    } while (bw < strlen(tl_WRBuf));
    // close source file
    res = f_close(&m_file);
    if(res)
      printf("close file (%s) fail.\n", filename);
    if(f_mount(NULL, logical_drv, 1) != FR_OK)
      printf("FATFS unmount logical drive fail.\n");
    if(FATFS_UnRegisterDiskDriver(drv_num))	
      printf("Unregister disk driver from FATFS fail.\n");
  }
fail:
  sdio_deinit_host();
  
  free(tl_WRBuf);
}


void example_media_tl_main(void *param)
{
  printf("\r\n Enter Timelapse Example\n");
  int err = 0;
  
  /*init usb driver prior to init uvc driver*/
  _usb_init();
  if(wait_usb_ready() < 0){
    printf("\n USB Not Ready \n");
    exit(NULL);
  }
  /*init uvc driver*/
  if(uvc_stream_init() < 0){
    printf("\n Unable to initialize UVC Stream \n");
    exit(NULL);
  }
  struct uvc_context *uvc_ctx = malloc(sizeof(struct uvc_context)); //Creation of UVC Context
  if(!uvc_ctx){
    printf("\n UVC Context Creation Failed \n");
    exit(NULL);
  }
  /* Set UVC Parameters */
  uvc_ctx->fmt_type = UVC_FORMAT_MJPEG;
  uvc_ctx->width = 640;
  uvc_ctx->height = 480;
  uvc_ctx->frame_rate = 30;  
  uvc_ctx->compression_ratio = 50;
  if(uvc_set_param(uvc_ctx->fmt_type, &uvc_ctx->width, &uvc_ctx->height, &uvc_ctx->frame_rate, &uvc_ctx->compression_ratio) < 0){
    printf("\n UVC Set Parameters Failed \n");
    exit(NULL);
  }
  if(uvc_stream_on() < 0){
    printf("\n Unable to turn on UVC Stream\n");
    exit(NULL);
  }
  err = -EAGAIN;
  register int iter = 0;
  int ret = 0;
  struct uvc_buf_context buf;  /* Global, need to change*/
  
  /*Creating AV Packet for conversion*/
  AVFrame in_Frame;
  
   
    do{ 
      ret = uvc_dqbuf(&buf); 
      if(buf.index < 0){
        printf("\r\n dequeue fail\n");
        err = -EAGAIN;
      }
      if((uvc_buf_check(&buf)<0)||(ret < 0)){
        printf("\n\rbuffer error!");
        uvc_stream_off();
        err = -EIO;	// should return error
      }
      in_Frame.FrameData = (char*)buf.data;
      in_Frame.FrameLength = buf.len;
      mjpeg2jpeg(&in_Frame);
      example_tl_fatfs_thread(in_Frame.FrameData, in_Frame.FrameLength, iter);
      ret = uvc_qbuf(&buf);
      
      ++iter;
      vTaskDelay(2000);
    }while( (err == -EAGAIN) );
    
  
  /*Close UVC Stream */
  uvc_stream_free();
  /* Free UVC Context */
  free((struct uvc_context *)uvc_ctx);
  /* De-initialize USB */
  _usb_deinit();
  /*Delete Task */
  vTaskDelete(NULL);
}


void example_media_tl(void)
{
  /*user can start their own task here*/
  if(xTaskCreate(example_media_tl_main, ((const char*)"example_media_tl"), 512, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
    printf("\r\n example_media_tl_main: Create Task Error\n");
  }	
}


/************************************************************end of rtsp/rtp with motion-jpeg************************************************/
