#include "hal_sys.h"
#include "joylink_porting_layer.h"
#include "joylink_log.h"
#include "joylink_ret_code.h"
#include "joylink_dev.h"
#include "crc.h"

joylink_fota_ctx_t _g_fota_ctx;
httpclient_t _g_fota_httpclient = {0};

static const char *fota_status_desc[] = {
    {"downloading"},
    {"crc32 not match"},
    {"download fail"},
    {"connect server fail"}
};

void joylink_v2_init()
{
	return;
}

void joylink_v2_task(void *args)
{
    joylink_main_start();
}

int joylink_get_feedid(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;
        
    status = nvdm_read_data_item(
        "User", 
        "joylink_feedid", 
        buf, 
        &buf_sz);
    
    if('\0' == *buf)
        return 1;    
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

void* joylink_malloc(size_t sz)
{  
    void *buf = NULL;
    buf = pvPortMalloc(sz);

    if(NULL == buf)
        log_error("joylink_malloc() Error! size:%d", sz);
    
    return buf;
}

void joylink_free(void *ptr)
{
    if(ptr)
        vPortFree(ptr);
}

int joylink_set_feedid(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "joylink_feedid", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_accesskey(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;
        
    status = nvdm_read_data_item(
        "User", 
        "joylink_accesskey", 
        buf, 
        &buf_sz);
    
    if('\0' == *buf)
        return 1;        
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_accesskey(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "joylink_accesskey", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)json_in, 
        strlen(json_in));
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_localkey(char *buf, unsigned int buf_sz)   
{
    nvdm_status_t status;
        
    status = nvdm_read_data_item(
        "User", 
        "joylink_localkey", 
        buf, 
        &buf_sz);
    
    if('\0' == *buf)
        return 1;    
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_localkey(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "joylink_localkey", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)json_in, 
        strlen(json_in));

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_server_info(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;
        
    status = nvdm_read_data_item(
        "User", 
        "joylink_server_info", 
        buf, 
        &buf_sz);
    
    if('\0' == *buf)
        return 1;   
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_server_info(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "joylink_server_info", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)json_in, 
        strlen(json_in));

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_version(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;
        
    status = nvdm_read_data_item(
        "User", 
        "joylink_version", 
        buf, 
        &buf_sz);
    
    if('\0' == *buf)
        return 1;
    
    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_version(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User", 
        "joylink_version", 
        NVDM_DATA_ITEM_TYPE_STRING, 
        (uint8_t *)json_in, 
        strlen(json_in));

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

void joylink_fota_set_upgrade_status(joylink_fota_status_t status, uint32_t progress, joylink_fota_error_code_t error_code)
{
    _g_fota_ctx.upgrade_status = status;
    _g_fota_ctx.progress = progress;
    _g_fota_ctx.error_code = error_code;
}

int32_t joylink_fota_http_retrieve_get(char* buf, uint32_t len, uint32_t *crc32)
{
	if(!crc32){
		return -1;
	}
	
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    int32_t write_ret;
    httpclient_data_t client_data = {0};
    char *get_url = _g_fota_ctx.download_url;
    int32_t count = 0;
    uint32_t data_len = 0;
	uint32_t recv_temp = 0;

	uint32_t crc = 0;
	
    client_data.response_buf = buf;
    client_data.response_buf_len = len;		

    ret = httpclient_send_request(&_g_fota_httpclient, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_DOWNLOAD_ERROR);
        return ret;
    }

    do {
        ret = httpclient_recv_response(&_g_fota_httpclient, &client_data);
        if (ret < 0) {
            return ret;
        }
		
		if (recv_temp == 0){
            recv_temp = client_data.response_content_len;
        }

		data_len = recv_temp - client_data.retrieve_len;
        count += data_len;
		recv_temp = client_data.retrieve_len;
		
        write_ret = fota_write(FOTA_PARITION_TMP, (const uint8_t*)client_data.response_buf, data_len);
        if (FOTA_STATUS_OK != write_ret) {
            printf("fail to write flash, write_ret = %d\r\n", write_ret);
            return ret;
        }
		
        joylink_fota_set_upgrade_status(FOTA_STATUS_DOWNLOAD, count/client_data.response_content_len * 100, FOTA_ERROR_CODE_NONE);
		printf("data received %d, total %d, retrieve %d\n", data_len, count, recv_temp); 
        
    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);

    printf("total length: %d\n", client_data.response_content_len);
    if (count != client_data.response_content_len || httpclient_get_response_code(&_g_fota_httpclient) != 200) {
        printf("data received not completed, or invalid error code\r\n");
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_DOWNLOAD_ERROR);
        return -1;
    }
	
	else if (count == 0) {
        LOG_E(fota_dl_api, "[FOTA DL] receive length is zero, file not found \n");
        return -2;
    }
    else {
        LOG_I(fota_dl_api, "[FOTA DL] download success \n");        
    }

    _g_fota_ctx.data_len = count;
	
	*crc32 = crc;
	
    return ret;
}


void 
joylink_fota_download_task(void *parameter)
{
    char *get_url = _g_fota_ctx.download_url;
    int32_t ret = HTTPCLIENT_ERROR_CONN;
	
    uint32_t crc32 = 0;
    uint32_t data_len;
	fota_status_t read_ret;
	fota_ret_t fota_ret;
#if 1	
	 if(fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        printf("[FOTA CLI] fota init fail. \n");
        return -2;
    }
    
    char* buf = joylink_malloc(FOTA_BUF_SIZE);
    if (buf == NULL) {
        printf("buf malloc failed.\r\n");
        return;
    }

    ret = httpclient_connect(&_g_fota_httpclient, get_url);

    if (!ret) {
        ret = joylink_fota_http_retrieve_get(buf, FOTA_BUF_SIZE, &crc32);
		if(!ret){
			// download success
			joylink_fota_set_upgrade_status(FOTA_STATUS_INSTALL, 0, FOTA_ERROR_CODE_NONE);
			joylink_dev_ota_status_upload();
		}
    }else {
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_CONNECT_SERVER_ERROR);
    }

    printf ("Download result = %d \r\n", ret);    
   
    httpclient_close(&_g_fota_httpclient);

#if 0
    // crc32 check
    uint32_t len;
    data_len = _g_fota_ctx.data_len;
    make_crc32_table();
    while (data_len > 0) { /* read 1KB data every time */
        if (data_len > FOTA_BUF_SIZE) {
            len = FOTA_BUF_SIZE;
            data_len -= FOTA_BUF_SIZE;
        }else { /* read the last time when < 1KB */
            len = data_len;
            data_len = 0;
        }
		
        read_ret = fota_read(FOTA_PARITION_TMP, buf, len);
        if (FOTA_STATUS_OK != read_ret) {
            printf("fail to read flash, %d\r\n", read_ret);
            break; 
        }

        crc = make_crc(crc, buf, len);
    }
#endif

    joylink_free(buf);
    buf = NULL; 
#else
	ret = fota_download_by_http(get_url);
#endif
	vTaskDelay(1000);
    if (!ret /*_g_fota_ctx.crc32 == crc32*/) {
		printf("crc32 ok, [%d]-[%d]!\r\n", _g_fota_ctx.crc32, crc32);        
		
        fota_ret = fota_trigger_update();
		if(fota_ret < FOTA_TRIGGER_SUCCESS){
			printf("fota_trigger_update error:%d\n", fota_ret);
			return;
		}
		
		// download success
        joylink_fota_set_upgrade_status(FOTA_STATUS_SUCCESS, 0, FOTA_ERROR_CODE_NONE);		
		joylink_dev_ota_status_upload();
		
		printf("-----------hal_wdt_software_reset----------\n");

        // TODO: reboot device
        hal_wdt_config_t wdt_config;
        wdt_config.mode = HAL_WDT_MODE_RESET;
        wdt_config.seconds = 1;
        hal_wdt_init(&wdt_config);  
        hal_wdt_software_reset();	
    }else {
		printf("crc32 error, [%d]-[%d]!\r\n", _g_fota_ctx.crc32, crc32);
        // download fail, crc32 not match.
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_CRC32_ERROR);
    }
    
    vTaskDelete(NULL);
}


void joylink_fota_download_package()
{
    // create task to execute download operation.
    xTaskCreate(joylink_fota_download_task, "FOTADownloadTask", 5*1024/sizeof(portSTACK_TYPE), NULL, 1, NULL);
}

const char* joylink_fota_get_status_desc(joylink_fota_error_code_t error_code)
{
    return fota_status_desc[error_code];
}
