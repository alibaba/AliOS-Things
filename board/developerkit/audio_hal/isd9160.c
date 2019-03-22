#include "isd9160.h"
#include "stm32l4xx_hal.h"
#include "k_api.h"
#include "crc16.h"
#include "audio.h"
#include <string.h>
#include <fcntl.h>

#define ISD9160_I2C_ADDR                (0x15 << 1)
#define ISD9160_I2C_TIMEOUT             AOS_WAIT_FOREVER
#define ISD9160_ROM_SIZE                0x24400
#define UPG_FRAME_HEAD_SIZE             sizeof(UPG_FRAME_HEAD)
#define UPG_PAYLOAD_HAED_SIZE           sizeof(UPG_PAYLOAD_HEAD)
#define UPG_PAYLOAD_DATA_SIZE           128
#define SLAVE_DATA_MAX                  (UPG_PAYLOAD_HAED_SIZE + UPG_PAYLOAD_DATA_SIZE)
#define UPG_FRAME_MAGIC                 0x18
#define UPG_HINT_GRANU                  5
#define UPG_HINT_DIV                    (100 / UPG_HINT_GRANU)

#define MCU_FLASH_START_ADDR            0x8000000
#define UPG_FLASH_BASE                  ((256 << 10) + MCU_FLASH_START_ADDR)
#define FLASH_MAGIC_HEAD                "9160"
#define FLASH_MAGIC_FILE                0x20180511

#define ISD9160_I2C_TIMEOUT             1000
#define MAX_PATH_SIZE                   256
#define MAX_VERSION_SIZE                8

typedef enum {
	I2C_CMD_SLPRT = 0,
	I2C_CMD_UPGRADE,
	I2C_CMD_RESPONSE,
	I2C_CMD_RECORD,
	I2C_CMD_PLAYBACK,
	I2C_CMD_AUDIOSTOP,
	
	I2C_CMD_END
} I2C_PROC_CMD;

typedef enum {
	RESP_UPG_ALL_SUCCESS = 0xa8,
	RESP_UPG_ALL_FAILED,
	RESP_UPG_BINSIZE_SUCCESS,
	RESP_UPG_BINSIZE_FAILED,
	RESP_UPG_FRAMEHEAD_SUCCESS,
	RESP_UPG_FRAMEHEAD_FAILED,
	RESP_UPG_PAYLOAD_SUCCESS,
	RESP_UPG_PAYLOAD_FAILED,
	RESP_UPG_PAYLOAD_CRC,
	
	RESP_LB_OPENED,
	RESP_LB_CLOSED,
	RESP_LB_FAILED,
	
	RESP_AUDIO_RECORD,
	RESP_AUDIO_PLAYBACK,
	RESP_AUDIO_STOP,
	
	RESP_FLAG_NORMAL
} RESPONSE_FLAG;

typedef enum {
	BINTYPE_LDROM = 0,
	BINTYPE_APROM,
	BINTYPE_DATAROM,

	BINTYPE_END
} BINTYPE_UPG;

typedef enum {
	SLPRT_HINT_SWVER = 0,
	SLPRT_HINT_VR,

	SLPRT_HINT_END
} SLPRT_HINT_FLAG;

typedef struct {
	uint32_t addr;
	uint32_t size;
} UPG_FLASH_MAP;

#pragma pack(1)
typedef struct {
	uint8_t magic;
	uint8_t payload_size;
} UPG_FRAME_HEAD;

typedef struct {
	uint16_t crc;
	uint32_t offset;
} UPG_PAYLOAD_HEAD;

typedef struct {
	uint32_t magic_head;
	uint32_t file_num;
	uint8_t version[MAX_VERSION_SIZE];
} FLASH_UPG_HEAD;

typedef struct {
	uint32_t magic_file;
	uint32_t bintype;
	uint32_t size;
	uint32_t reserve;
} FLASH_FILE_HEAD;
#pragma pack()

static const UPG_FLASH_MAP g_isd9160_map_table[BINTYPE_END] = {
	{0x00100000, 4  << 10},
	{0x00000000, 60 << 10},
	{0x0000f000, 81 << 10},
};

static const char *g_slprt_hint_matching[] = {
    "isd9160 software version: ",
    "isd9160 Voice Recognition: ",
};

static FLASH_UPG_HEAD g_upg_head;
static FLASH_FILE_HEAD g_file_head[BINTYPE_END];
static char g_fw_ver[MAX_VERSION_SIZE + 1] = {0};
static aos_mutex_t isd9160_mutex;
static CB_SWVER svfunc = NULL;
static CB_VRCMD vrfunc = NULL;

static inline void hton_4(uint8_t *data, uint32_t value)
{
	data[0] = (value >> (8 * 3)) & 0xff;
	data[1] = (value >> (8 * 2)) & 0xff;
	data[2] = (value >> (8 * 1)) & 0xff;
	data[3] = (value >> (8 * 0)) & 0xff;
}

static inline void ntoh_4(const uint8_t *data, uint32_t *value)
{
	*value = 0;
	*value |= data[0] << (8 * 3);
	*value |= data[1] << (8 * 2);
	*value |= data[2] << (8 * 1);
	*value |= data[3] << (8 * 0);
}

static inline void hton_2(uint8_t *data, uint16_t value)
{
	data[0] = (value >> (8 * 1)) & 0xff;
	data[1] = (value >> (8 * 0)) & 0xff;
}

static int can_version_upgrade(void)
{
	uint32_t ver_maj_cur = 0;
	uint32_t ver_min_cur = 0;
	uint32_t ver_maj_upg = 0;
	uint32_t ver_min_upg = 0;
	char upg_version[MAX_VERSION_SIZE + 1] = {0};
	const char str_cur[] = "current firmware version";
	const char str_upg[] = "upgrade firmware version";
	int ret = 0;

	memcpy(upg_version, g_upg_head.version, MAX_VERSION_SIZE);
	if (g_fw_ver[0] == '\0') {
		printf("%s may be the factory version, try to upgrade.\n", str_cur);
		ret = 1;
		goto END;
	}
	do {
		ret = sscanf(g_fw_ver, "v%u.%u", &ver_maj_cur, &ver_min_cur);
		if (ret != 2) {
			printf("%s is not mainstream style, try compare pure string.\n", str_cur);
			break;
		}
		ret = sscanf(upg_version, "v%u.%u", &ver_maj_upg, &ver_min_upg);
		if (ret != 2) {
			KIDS_A10_PRT("%s is invalid.\n", str_upg);
			ret = 0;
			goto END;
		}
		if (ver_maj_cur != ver_maj_upg) {
			printf("Major version is different, try to upgrade.\n");
			ret = 1;
			goto END;
		}
		if (ver_min_upg > ver_min_cur) {
			ret = 1;
			goto END;
		} else {
			printf("%s is less than or equal to %s.\n", str_upg, str_cur);
			ret = 0;
			goto END;
		}
	} while (0);

	ret = strcmp(upg_version, g_fw_ver);
	if (ret > 0) {
		ret = 1;
	} else {
		printf("%s is less than or equal to %s.\n", str_upg, str_cur);
		ret = 0;
	}

END:
	printf("%s is %s, %s is %s\n", str_cur, g_fw_ver, str_upg, upg_version);
	return ret;
}

static int ready_for_upgrade(int fd)
{
	uint32_t bin_size_total = 0;
	int i;
	ssize_t ret_size = 0;

	ret_size = aos_read(fd, &g_upg_head, sizeof(g_upg_head));
	if (ret_size != sizeof(g_upg_head)) {
		KIDS_A10_PRT("aos_read return failed.\n");
		return -1;
	}
	if (memcmp(&g_upg_head.magic_head, FLASH_MAGIC_HEAD, 4)) {
		KIDS_A10_PRT("magic_head is invalid.\n");
		return -1;
	}
	if (!can_version_upgrade()) {
		return -1;
	}
	if (g_upg_head.file_num > BINTYPE_END) {
		KIDS_A10_PRT("file_num is invalid.\n");
		return -1;
	}
	
	for (i = 0; i < g_upg_head.file_num; ++i) {
		ret_size = aos_read(fd, &g_file_head[i], sizeof(FLASH_FILE_HEAD));
		if (ret_size != sizeof(FLASH_FILE_HEAD)) {
			KIDS_A10_PRT("aos_read return failed.\n");
			return -1;
		}
		if (g_file_head[i].magic_file != FLASH_MAGIC_FILE) {
			KIDS_A10_PRT("file %d magic_file is invalid.\n", i);
			return -1;
		}
		if (g_file_head[i].bintype >= BINTYPE_END) {
			KIDS_A10_PRT("file %d bintype is invalid.\n", i);
			return -1;
		}
		bin_size_total += g_file_head[i].size;
		aos_lseek(fd, g_file_head[i].size, SEEK_CUR);
	}
	if (bin_size_total > ISD9160_ROM_SIZE) {
		KIDS_A10_PRT("bin_size_total exceeds ISD9160_ROM_SIZE.\n");
		return -1;
	}
	
	return 0;
}

static int isd9160_slprt_size(uint32_t *size)
{
	int ret = 0;
	uint8_t data = I2C_CMD_SLPRT;
	uint8_t size_data[4] = {0};
	
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, &data, 1, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	
	ret = hal_i2c_master_recv(&brd_i2c4_dev, ISD9160_I2C_ADDR, size_data, 4, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	
	ntoh_4(size_data, size);
	
	return 0;
}

static int isd9160_slprt_data(uint8_t *data, uint32_t size)
{
	return hal_i2c_master_recv(&brd_i2c4_dev, ISD9160_I2C_ADDR, data, size, ISD9160_I2C_TIMEOUT);
}

static int detect_slprt_hint(const char *buf, char **key)
{
	int i;

	if (buf == NULL || key == NULL) {
		KIDS_A10_PRT("Parameters is invalid.\n");
		return -1;
	}
	*key = NULL;
	for (i = SLPRT_HINT_SWVER; i < SLPRT_HINT_END; ++i) {
		*key = strstr(buf, g_slprt_hint_matching[i]);
		if (*key) {
			*key += strlen(g_slprt_hint_matching[i]);
			break;
		}
	}
	if (i >= SLPRT_HINT_END) {
		i = -1;
	}

	return i;
}

static void hint_software_version(const char *key)
{
	char *ptemp = NULL;

	strncpy(g_fw_ver, key, MAX_VERSION_SIZE);
	ptemp = strchr(g_fw_ver, '\n');
	if (ptemp) {
		*ptemp = '\0';
	}
	printf("%s%s\n", g_slprt_hint_matching[SLPRT_HINT_SWVER], g_fw_ver);
}

static void hint_voice_recognition(const char *key)
{
	char *ptemp = NULL;

	strncpy(g_fw_ver, key, MAX_VERSION_SIZE);
	ptemp = strchr(g_fw_ver, '\n');
	if (ptemp) {
		*ptemp = '\0';
	}
	printf("%s%s\n", g_slprt_hint_matching[SLPRT_HINT_SWVER], g_fw_ver);
}

static int handle_slprt(void)
{
	int ret = 0;
	uint32_t size = 0;
	int slprt_num = 0;
	char buf[SLAVE_DATA_MAX] = {0};
	char *str_key = NULL;

	if (!aos_mutex_is_valid(&isd9160_mutex)) {
		KIDS_A10_PRT("isd9160_mutex is invalid.\n");
		return -1;
	}
	ret = aos_mutex_lock(&isd9160_mutex, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("ISD9160 is very busy now.\n");
		return -1;
	}
	
	while (1) {
		ret = isd9160_slprt_size(&size);
		if (ret != 0) {
			KIDS_A10_PRT("isd9160_slprt_size return failed.\n");
			ret = -1;
			break;
		}
		if (size == 0) {
			ret = slprt_num;
			break;
		} else if (size > SLAVE_DATA_MAX) {
			KIDS_A10_PRT("isd9160_slprt_size return size is invalid.\n");
			ret = -1;
			break;
		}
		memset(buf, 0, sizeof(buf));
		ret = isd9160_slprt_data((uint8_t *)buf, size);
		if (ret != 0) {
			KIDS_A10_PRT("isd9160_slprt_data return failed.\n");
			ret = -1;
			break;
		}
		ret = detect_slprt_hint(buf, &str_key);
		if (ret < 0) {
			printf("slave_print: ");
			printf("%s", buf);
		} else {
			switch (ret) {
				case SLPRT_HINT_SWVER:
					hint_software_version(str_key);
					if (svfunc) {
						svfunc(g_fw_ver);
					}
					break;
				case SLPRT_HINT_VR:
					if (vrfunc) {
						vrfunc(g_fw_ver, atoi(str_key));
					}
					break;
				default:
					break;
			}
		}

		++slprt_num;
	}
	
	ret = aos_mutex_unlock(&isd9160_mutex);
	if (ret != 0) {
		KIDS_A10_PRT("ISD9160 release failed.\n");
		ret = -1;
	}
	
	return ret;
}

static int get_cmdresp(uint8_t cmd, uint8_t *resp)
{
	uint8_t data = cmd;
	int ret = 0;
	
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, &data, 1, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	
	return hal_i2c_master_recv(&brd_i2c4_dev, ISD9160_I2C_ADDR, resp, 1, ISD9160_I2C_TIMEOUT);
}

static int request_binsize(uint8_t *resp, uint32_t type, uint32_t size)
{
	uint8_t data[5] = {0};
	int ret = 0;
	
	data[0] = I2C_CMD_UPGRADE;
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, data, 1, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	data[0] = (uint8_t)type;
	hton_4(&data[1], size);
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, data, 5, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	ret = get_cmdresp(I2C_CMD_RESPONSE, resp);
	if (ret != 0) {
		return ret;
	}
	
	return 0;
}

static int request_framehead(uint8_t *resp, uint8_t fsize)
{
	uint8_t data[2] = {0};
	int ret = 0;
	
	data[0] = UPG_FRAME_MAGIC;
	data[1] = fsize;
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, data, 2, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	ret = get_cmdresp(I2C_CMD_RESPONSE, resp);
	if (ret != 0) {
		return ret;
	}
	
	return 0;
}

static int request_payload(uint8_t *resp, uint32_t bintype, int fd, uint32_t offset, uint32_t size, int last_flag)
{
	uint32_t isd9160_addr = 0;
	uint8_t data[SLAVE_DATA_MAX] = {0};
	uint16_t crc_size = 0;
	uint16_t crc = 0;
	ssize_t ret_size = 0;
	int ret = 0;
	
	if (size > UPG_PAYLOAD_DATA_SIZE || offset + size > g_isd9160_map_table[bintype].size) {
		return -1;
	}
	
	isd9160_addr = g_isd9160_map_table[bintype].addr + offset;
	hton_4(&data[2], isd9160_addr);
	ret_size = aos_read(fd, &data[UPG_PAYLOAD_HAED_SIZE], size);
	if (ret_size != size) {
		KIDS_A10_PRT("aos_read return failed.\n");
		return -1;
	}
	crc_size = size + UPG_PAYLOAD_HAED_SIZE - 2;
	if (crc_size % 2) {
		data[UPG_PAYLOAD_HAED_SIZE + size] = 0xff;
		++crc_size;
	}
	crc = checksum_crc16(&data[2], crc_size);
	hton_2(data, crc);
	ret = hal_i2c_master_send(&brd_i2c4_dev, ISD9160_I2C_ADDR, data, UPG_PAYLOAD_HAED_SIZE + size, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		return ret;
	}
	if (last_flag == 1 && bintype == BINTYPE_APROM) {
		krhino_task_sleep(krhino_ms_to_ticks(100));
	}
	ret = get_cmdresp(I2C_CMD_RESPONSE, resp);
	if (ret != 0) {
		return ret;
	}
	
	return 0;
}

static void hint_percent(uint32_t now_bytes, uint32_t total_bytes, int bintype)
{
	static uint32_t last_quotient = 0;
	uint32_t now_quotient = 0;
	const char *str_type[] = {"LDROM", "APROM", "DATAROM"};
	
	if (now_bytes == 0 || total_bytes == 0) {
		printf("isd9160 upgrade %s current progress is 0%%\n", str_type[bintype]);
		last_quotient = 0;
		return;
	}
	now_quotient = now_bytes * UPG_HINT_DIV / total_bytes;
	if (now_quotient > last_quotient) {
		printf("isd9160 upgrade %s current progress is %u%%\n", str_type[bintype], now_quotient * UPG_HINT_GRANU);
		last_quotient = now_quotient;
	}
}

static int send_upgrade(int fd, FLASH_FILE_HEAD *file_head)
{
	uint8_t frame_size = 0;
	uint32_t send_bytes = 0;
	uint32_t remain_bytes = 0;
	uint8_t resp = RESP_FLAG_NORMAL;
	int last_flag = 0;
	int ret = 0;
	
	ret = request_binsize(&resp, file_head->bintype, file_head->size);
	if (ret != 0) {
		KIDS_A10_PRT("request_binsize return failed.\n");
		return -1;
	}
	if (resp != RESP_UPG_BINSIZE_SUCCESS) {
		KIDS_A10_PRT("request_binsize return response abnormal, resp = 0x%02x.\n", resp);
		return -1;
	}
	remain_bytes = file_head->size - send_bytes;
	hint_percent(0, 0, file_head->bintype);
	while (remain_bytes > 0) {
		if (remain_bytes <= UPG_PAYLOAD_DATA_SIZE) {
			frame_size = (uint8_t)remain_bytes + UPG_PAYLOAD_HAED_SIZE;
			last_flag = 1;
		} else {
			frame_size = SLAVE_DATA_MAX;
		}
		ret = request_framehead(&resp, frame_size);
		if (ret != 0) {
			KIDS_A10_PRT("request_framehead return failed.\n");
			return -1;
		}
		if (resp != RESP_UPG_FRAMEHEAD_SUCCESS) {
			KIDS_A10_PRT("request_framehead return response abnormal, resp = 0x%02x.\n", resp);
			return -1;
		}
RESEND:
		ret = request_payload(&resp, file_head->bintype, fd, send_bytes, frame_size - UPG_PAYLOAD_HAED_SIZE, last_flag);
		if (ret != 0) {
			KIDS_A10_PRT("request_payload return failed.\n");
			return -1;
		}
		if (resp != RESP_UPG_PAYLOAD_SUCCESS) {
			if (resp == RESP_UPG_ALL_SUCCESS) {
				
			} else if (resp == RESP_UPG_PAYLOAD_CRC) {
				printf("CRC checksum error, send_bytes %u, resend data.\n", send_bytes);
				aos_lseek(fd, -(frame_size - UPG_PAYLOAD_HAED_SIZE), SEEK_CUR);
				goto RESEND;
			} else {
				KIDS_A10_PRT("request_payload return response abnormal, resp = 0x%02x.\n", resp);
				return -1;
			}
		}
		send_bytes += frame_size - UPG_PAYLOAD_HAED_SIZE;
		remain_bytes = file_head->size - send_bytes;
		hint_percent(send_bytes, file_head->size, file_head->bintype);
	}
	
	if (resp != RESP_UPG_ALL_SUCCESS) {
		KIDS_A10_PRT("unknow error. resp = 0x%02x\n", resp);
		return -1;
	}
	
	return 0;
}

int handle_upgrade(const char *file_in_sd)
{
	int ret = 0;
	int uret = 0;
	char file[MAX_PATH_SIZE] = {0};
	int fd = 0;
	int i;
	
	sprintf(file, "/sdcard/%s", file_in_sd);
	fd = aos_open(file, O_RDONLY);
	if (fd < 0) {
		KIDS_A10_PRT("aos_open return failed.\n");
		return -1;
	}
	ret = ready_for_upgrade(fd);
	if (ret != 0) {
		goto END;
	}
	if (!aos_mutex_is_valid(&isd9160_mutex)) {
		KIDS_A10_PRT("isd9160_mutex is invalid.\n");
		ret = -1;
		goto END;
	}
	ret = aos_mutex_lock(&isd9160_mutex, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("ISD9160 is very busy now.\n");
		goto END;
	}
	
	aos_lseek(fd, sizeof(FLASH_UPG_HEAD), SEEK_SET);
	for (i = 0; i < g_upg_head.file_num; ++i) {
		aos_lseek(fd, sizeof(FLASH_FILE_HEAD), SEEK_CUR);
		ret = send_upgrade(fd, &g_file_head[i]);
		if (ret != 0) {
			break;
		}
	}
	
	uret = aos_mutex_unlock(&isd9160_mutex);
	if (uret != 0) {
		KIDS_A10_PRT("ISD9160 release failed.\n");
	}
END:
	uret = aos_close(fd);
	if (uret != 0) {
		KIDS_A10_PRT("aos_close return failed.\n");
	}
	
	return ret;
}

int handle_record(const char *file_in_sd, int *stop_flag)
{
	int ret = 0;
	int uret = 0;
	uint8_t resp = RESP_FLAG_NORMAL;

	if (!aos_mutex_is_valid(&isd9160_mutex)) {
		KIDS_A10_PRT("isd9160_mutex is invalid.\n");
		return -1;
	}
	ret = aos_mutex_lock(&isd9160_mutex, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("ISD9160 is very busy now.\n");
		return -1;
	}

	ret = get_cmdresp(I2C_CMD_RECORD, &resp);
	if (ret != 0 || resp != RESP_AUDIO_RECORD) {
		KIDS_A10_PRT("Record failed, get_cmdresp return %d, resp = 0x%x.\n", ret, resp);
		ret = -1;
		goto END;
	}

	ret = record_to_sdcard(file_in_sd, stop_flag);
	if (ret != 0) {
		KIDS_A10_PRT("record_to_sdcard return failed.\n");
		goto END;
	}

	ret = get_cmdresp(I2C_CMD_AUDIOSTOP, &resp);
	if (ret != 0 || resp != RESP_AUDIO_STOP) {
		KIDS_A10_PRT("Record stop failed, get_cmdresp return %d, resp = 0x%x.\n", ret, resp);
		ret = -1;
		goto END;
	}

END:
	uret = aos_mutex_unlock(&isd9160_mutex);
	if (uret != 0) {
		KIDS_A10_PRT("ISD9160 release failed.\n");
	}

	return ret;
}

int handle_playback(const char *file_in_sd)
{
	int ret = 0;
	int uret = 0;
	uint8_t resp = RESP_FLAG_NORMAL;

	if (!aos_mutex_is_valid(&isd9160_mutex)) {
		KIDS_A10_PRT("isd9160_mutex is invalid.\n");
		return -1;
	}
	ret = aos_mutex_lock(&isd9160_mutex, ISD9160_I2C_TIMEOUT);
	if (ret != 0) {
		KIDS_A10_PRT("ISD9160 is very busy now.\n");
		return -1;
	}

	ret = get_cmdresp(I2C_CMD_PLAYBACK, &resp);
	if (ret != 0 || resp != RESP_AUDIO_PLAYBACK) {
		KIDS_A10_PRT("Playback failed, get_cmdresp return %d, resp = 0x%x.\n", ret, resp);
		ret = -1;
		goto END;
	}

	ret = playback_from_sdcard(file_in_sd);
	if (ret != 0) {
		KIDS_A10_PRT("playback_from_sdcard return failed.\n");
		goto END;
	}

	ret = get_cmdresp(I2C_CMD_AUDIOSTOP, &resp);
	if (ret != 0 || resp != RESP_AUDIO_STOP) {
		KIDS_A10_PRT("Playback stop failed, get_cmdresp return %d, resp = 0x%x.\n", ret, resp);
		ret = -1;
		goto END;
	}

END:
	uret = aos_mutex_unlock(&isd9160_mutex);
	if (uret != 0) {
		KIDS_A10_PRT("ISD9160 release failed.\n");
	}

	return ret;
}

int register_swver_callback(CB_SWVER pfunc)
{
	svfunc = pfunc;

	return 0;
}

int register_vrcmd_callback(CB_VRCMD pfunc)
{
	vrfunc = pfunc;

	return 0;
}

int isd9160_loop_once(void)
{
	return handle_slprt();
}

void isd9160_reset(void)
{
	hal_gpio_output_low(&brd_gpio_table[GPIO_AUDIO_RST]);
	krhino_task_sleep(krhino_ms_to_ticks(50));
	hal_gpio_output_high(&brd_gpio_table[GPIO_AUDIO_RST]);
}

int isd9160_i2c_init(void)
{
	int ret = 0;

	if (aos_mutex_is_valid(&isd9160_mutex)) {
		KIDS_A10_PRT("ISD9160 module initialization had completed before now.\n");
		return -1;
	}
	ret = aos_mutex_new(&isd9160_mutex);
	if (ret != 0) {
		KIDS_A10_PRT("aos_mutex_new return failed.\n");
		return -1;
	}
	isd9160_reset();
	
	return 0;
}
