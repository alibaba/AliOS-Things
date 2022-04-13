/*
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * VERSION		DATE			AUTHOR
 *
 */
#include "tlsc6x_update.h"
#include "tlsc6x.h"

#define EPERM	1
unsigned int g_tlsc6x_cfg_ver;
unsigned int g_tlsc6x_boot_ver;
unsigned int g_needKeepRamCode;
unsigned short g_tlsc6x_chip_code;

int tlsc_boot_version = -1;
int tlsc_cfg_version = -1;
int tlsc_vendor_id = -1;

unsigned int g_mccode;	/* 0:3535, 1:3536 */

struct i2c_client *g_tlsc6x_client;
extern struct mutex i2c_rw_access;

#ifdef TLSC_BUILDIN_BOOT

static int new_idx_active = -1;
#endif

/* Telink CTP */
unsigned int MTK_TXRX_BUF;
unsigned int CMD_ADDR;
unsigned int RSP_ADDR;

typedef struct __test_cmd_wr {
	/* offset 0; */
	unsigned char id;	/* cmd_id; */
	unsigned char idv;	/* inverse of cmd_id */
	unsigned short d0;	/* data 0 */
	unsigned short d1;	/* data 1 */
	unsigned short d2;	/* data 2 */
	/* offset 8; */
	unsigned char resv;	/* offset 8 */
	unsigned char tag;	/* offset 9 */
	unsigned short chk;	/* 16 bit checksum */
	unsigned short s2Pad0;	/*  */
	unsigned short s2Pad1;	/*  */
} ctp_test_wr_t;

typedef struct __test_cmd_rd {
	/* offset 0; */
	unsigned char id;	/* cmd_id; */
	unsigned char cc;	/* complete code */
	unsigned short d0;	/* data 0 */
	unsigned short sn;	/* session number */
	unsigned short chk;	/* 16 bit checksum */
} ctp_test_rd_t;
#define DIRECTLY_MODE   (0x0)
#define DEDICATE_MODE   (0x1)
#define MAX_TRX_LEN (64)	/* max IIC data length */
/* #define CMD_ADDR    (0xb400) */
/* #define RSP_ADDR    (0xb440) */
/* #define MTK_TXRX_BUF    (0xcc00)  // 1k, buffer used for memory r &w */

#define LEN_CMD_CHK_TX  (10)
#define LEN_CMD_PKG_TX  (16)

#define LEN_RSP_CHK_RX  (8)
#define MAX_BULK_SIZE    (1024)
unsigned short tl_target_cfg[102];
unsigned short tl_buf_tmpcfg[102];
/* to direct memory access mode */
unsigned char cmd_2dma_42bd[6] = {
/*0x42, 0xbd, */0x28, 0x35, 0xc1, 0x00, 0x35, 0xae };

static inline int msleep(int ms)
{
	aos_msleep(ms);
}

/* in directly memory access mode */
/* RETURN:0->pass else->fail */
int tlsc6x_read_bytes_u16addr_sub(struct tlsc6x_drv_data *data, u16 addr,
				  u8 *rxbuf, u16 len)
{
	int err = 0;
	int retry = 0;
	u16 offset = 0;
	u8 buffer[2];
	int read_len;

	//struct i2c_msg msgs[2] = { {.addr = client->addr, .flags = 0, .len = 2,	/* 16bit memory address */
	//			    .buf = buffer,}, {.addr =
	//					      client->addr, .flags =
	//					      I2C_M_RD,},
	//};

	if (rxbuf == NULL) {
		return -EPERM;
	}
	/* mutex_lock(&g_mutex_i2c_access); */

	while (len > 0) {
		buffer[0] = (u8) ((addr + offset) >> 8);
		buffer[1] = (u8) (addr + offset);

		//msgs[1].buf = &rxbuf[offset];
		if (len > MAX_TRX_LEN) {
			len -= MAX_TRX_LEN;
			read_len = MAX_TRX_LEN;
		} else {
			read_len = len;
			len = 0;
		}

		retry = 0;
		while (tlsc6x_i2c_read(data, buffer, 2, &rxbuf[offset],
					   read_len) < 0) {
			if (retry++ == 3) {
				err = -1;
				break;
			}
		}
		offset += MAX_TRX_LEN;
		if (err < 0) {
			break;
		}
	}

	/* mutex_unlock(&g_mutex_i2c_access); */

	return err;
}

int tlsc6x_read_bytes_u16addr(struct tlsc6x_drv_data *data, u16 addr, u8 *rxbuf,
			      u16 len)
{
	int ret = 0;

	//mutex_lock(&i2c_rw_access);
	ret = tlsc6x_read_bytes_u16addr_sub(data, addr, rxbuf, len);
	//mutex_unlock(&i2c_rw_access);
	return ret;
}

/* in directly memory access mode */
/* RETURN:0->pass else->fail */
int tlsc6x_write_bytes_u16addr_sub(struct tlsc6x_drv_data *data, u16 addr,
				   u8 *txbuf, u16 len)
{
	u8 buffer[MAX_TRX_LEN];
	u16 offset = 0;
	u8 retry = 0;
	int err = 0;
	int write_len;

	//struct i2c_msg msg = {.addr = client->addr, .flags = 0, .buf = buffer, };

	if (txbuf == NULL) {
		return -EPERM;
	}
	/* mutex_lock(&g_mutex_i2c_access); */

	while (len) {
		buffer[0] = (u8) ((addr + offset) >> 8);
		buffer[1] = (u8) (addr + offset);

		if (len > (MAX_TRX_LEN - 2)) {	/* (sizeof(addr)+payload) <= MAX_TRX_LEN */
			memcpy(&buffer[2], &txbuf[offset], (MAX_TRX_LEN - 2));
			len -= (MAX_TRX_LEN - 2);
			offset += (MAX_TRX_LEN - 2);
			write_len = MAX_TRX_LEN;
		} else {
			memcpy(&buffer[2], &txbuf[offset], len);
			write_len = len + 2;
			len = 0;
		}

		retry = 0;
		while (tlsc6x_i2c_write(data, buffer, write_len) < 0) {
			if (retry++ == 3) {
				err = -1;
				break;
			}
		}
		if (err < 0) {
			break;
		}
	}

	/* mutex_unlock(&g_mutex_i2c_access); */

	return err;
}

int tlsc6x_write_bytes_u16addr(struct i2c_client *client, u16 addr, u8 *txbuf,
			       u16 len)
{
	int ret = 0;

	//mutex_lock(&i2c_rw_access);
	ret = tlsc6x_write_bytes_u16addr_sub(client, addr, txbuf, len);
	//mutex_unlock(&i2c_rw_access);
	return ret;
}

/* <0 : i2c error */
/* 0: direct address mode */
/* 1: protect mode */
int tlsc6x_get_i2cmode(struct tlsc6x_drv_data *data)
{
	u8 regData[4];

	hal_trace_printf("tlsc6x_get_i2cmode start");
	if (tlsc6x_read_bytes_u16addr_sub(data, 0x01, regData, 3)) {
		return -EPERM;
	}

	if (((u8) (data->config->addr) == (regData[0] >> 1))
	    && (regData[2] == 0X01)) {
		hal_trace_printf("tlsc6x_get_i2cmode  direct mode\n");
		return DIRECTLY_MODE;
	}
	hal_trace_printf("tlsc6x_get_i2cmode  dedicate mode\n");
	return DEDICATE_MODE;
}

/* 0:successful */
int tlsc6x_set_dd_mode_sub(struct tlsc6x_drv_data *data)
{
	int mod = -1;
	int retry = 0;

	int ret = 0;

	ret = tlsc6x_get_i2cmode(data);

	if (ret < 0) {
		return ret;
	}

	if (ret == DIRECTLY_MODE) {
		return 0;
	}

	while (retry++ < 5) {
		msleep(20);
		ret = tlsc6x_write_bytes_u16addr_sub(data, 0x42bd,
					       cmd_2dma_42bd, 6);
		msleep(50);
		mod = tlsc6x_get_i2cmode(data);
		if (mod == DIRECTLY_MODE) {
			break;
		}
	}

	if (mod == DIRECTLY_MODE) {
		return 0;
	} else {
		return -EPERM;
	}
}

int tlsc6x_set_dd_mode(struct tlsc6x_drv_data *data)
{
	int ret = 0;

	//mutex_lock(&i2c_rw_access);
	ret = tlsc6x_set_dd_mode_sub(data);
	//mutex_unlock(&i2c_rw_access);
	return ret;
}

/* 0:successful */
int tlsc6x_set_nor_mode_sub(struct tlsc6x_drv_data *data)
{
	int mod = -1;
	int retry = 0;
	u8 reg = 0x05;

	while (retry++ < 5) {
		tlsc6x_write_bytes_u16addr_sub(data, 0x03, &reg, 1);
		//usleep_range(5000, 5500);
		msleep(5);
		mod = tlsc6x_get_i2cmode(data);
		if (mod == DEDICATE_MODE) {
			break;
		}
		msleep(50);
	}
	if (mod == DEDICATE_MODE) {
		return 0;
	} else {
		return -EPERM;
	}
}

int tlsc6x_set_nor_mode(struct tlsc6x_drv_data *data)
{
	int ret = 0;

	//mutex_lock(&i2c_rw_access);
	ret = tlsc6x_set_nor_mode_sub(data);
	//mutex_unlock(&i2c_rw_access);
	return ret;
}

/* ret=0 : successful */
/* write with read-back check, in dd mode */
static int tlsc6x_bulk_down_check(struct tlsc6x_drv_data *data, u8 *pbuf, u16 addr, u16 len)
{
	unsigned int j, k, retry;
	u8 rback[128];

	TLSC_FUNC_ENTER();
	while (len) {
		k = (len < 128) ? len : 128;
		retry = 0;
		do {
			rback[k - 1] = pbuf[k - 1] + 1;
			tlsc6x_write_bytes_u16addr(data, addr, pbuf,
						   k);
			tlsc6x_read_bytes_u16addr(data, addr, rback,
						  k);
			for (j = 0; j < k; j++) {
				if (pbuf[j] != rback[j]) {
					break;
				}
			}
			if (j >= k) {
				break;	/* match */
			}
		} while (++retry < 3);

		if (j < k) {
			break;
		}

		addr += k;
		pbuf += k;
		len -= k;
	}

	return (int)len;
}

static u16 tlsc6x_checksum_u16(u16 *buf, u16 length)
{
	unsigned short sum, len, i;

	sum = 0;

	len = length >> 1;

	for (i = 0; i < len; i++) {
		sum += buf[i];
	}
	tlsc_info("sum=%d,  0-succ\n", sum);
	return sum;
}

static u32 tlsc6x_checksumEx(u8 *buf, u16 length)
{
	u32 combChk;
	u16 k, check, checkEx;

	check = 0;
	checkEx = 0;
	for (k = 0; k < length; k++) {
		check += buf[k];
		checkEx += (u16) (k * buf[k]);
	}
	combChk = (checkEx << 16) | check;

	return combChk;

}

/* 0:successful */
int tlsc6x_download_ramcode(struct tlsc6x_drv_data *data, u8 *pcode, u16 len)
{
	u8 dwr, retry;
	int ret = -2;
	int sig;

	TLSC_FUNC_ENTER();
	if (tlsc6x_set_dd_mode(data)) {
		return -EPERM;
	}

	sig = (int)pcode[3];
	sig = (sig << 8) + (int)pcode[2];
	sig = (sig << 8) + (int)pcode[1];
	sig = (sig << 8) + (int)pcode[0];

	if (sig == 0x6d6f8008) {
		sig = 0;
		tlsc6x_read_bytes_u16addr(data, 0x8000, (u8 *) &sig,
					  4);
		if (sig == 0x6d6f8008) {
			return 0;
		}
	}

	dwr = 0x05;
	if (tlsc6x_bulk_down_check(data, &dwr, 0x0602, 1) == 0) {	/* stop mcu */
		dwr = 0x00;
		tlsc6x_bulk_down_check(data, &dwr, 0x0643, 1);	/* disable irq */
	} else {
		return -EPERM;
	}
	if (tlsc6x_bulk_down_check(data, pcode, 0x8000, len) == 0) {
		dwr = 0x88;
		retry = 0;
		do {
			ret =
			    tlsc6x_write_bytes_u16addr(data, 0x0602,
						       &dwr, 1);
		} while ((++retry < 3) && (ret != 0));
	}
	msleep(50);

	return ret;
}

/* return 0=successful: send cmd and get rsp. */
static int tlsc6x_cmd_send(struct tlsc6x_drv_data *data, ctp_test_wr_t *ptchcw, ctp_test_rd_t *pcr)
{
	int ret;
	u32 retry;

	TLSC_FUNC_ENTER();
	retry = 0;
	tlsc6x_write_bytes_u16addr(data, RSP_ADDR, (u8 *) &retry,
				   1);

	/* send command */
	ptchcw->idv = ~(ptchcw->id);
	ptchcw->tag = 0x35;
	ptchcw->chk =
	    1 + ~(tlsc6x_checksum_u16((u16 *) ptchcw, LEN_CMD_CHK_TX));
	ptchcw->tag = 0x30;
	ret =
	    tlsc6x_write_bytes_u16addr(data, CMD_ADDR, (u8 *) ptchcw,
				       LEN_CMD_PKG_TX);
	if (ret) {
		goto exit;
	}
	ptchcw->tag = 0x35;
	ret = tlsc6x_write_bytes_u16addr(data, CMD_ADDR + 9,
					 (u8 *) &(ptchcw->tag), 1);
	if (ret) {
		goto exit;
	}
	/* polling rsp, the caller must init rsp buffer. */
	ret = -1;
	retry = 0;
	while (retry++ < 100) {	/* 2s */
		msleep(20);
		if (tlsc6x_read_bytes_u16addr
		    (data, RSP_ADDR, (u8 *) pcr, 1)) {
			break;
		}

		if (ptchcw->id != pcr->id) {
			continue;
		}
		/* msleep(50); */
		tlsc6x_read_bytes_u16addr(data, RSP_ADDR, (u8 *) pcr,
					  LEN_RSP_CHK_RX);
		if (!tlsc6x_checksum_u16((u16 *) pcr, LEN_RSP_CHK_RX)) {
			if ((ptchcw->id == pcr->id) && (pcr->cc == 0)) {
				ret = 0;
			}
		}
		break;
	}
exit:
	/* clean rsp buffer */
	/* retry = 0; */
	/* tlsc6x_write_bytes_u16addr(g_tlsc6x_client, RSP_ADDR, (u8*)&retry, 1); */

	return ret;

}

/* return 0=successful */
int tlsc6x_read_burn_space(struct tlsc6x_drv_data *data, u8 *pdes, u16 adr, u16 len)
{
	int rsp;
	u32 left = len;
	u32 combChk, retry;
	ctp_test_wr_t m_cmd;
	ctp_test_rd_t m_rsp;

	TLSC_FUNC_ENTER();
	m_cmd.id = 0x31;
	m_cmd.resv = 0x03;
	while (left) {
		len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;

		m_cmd.d0 = adr;
		m_cmd.d1 = len;

		rsp = -1;
		retry = 0;
		while (retry++ < 3) {
			m_rsp.id = 0;
			if (tlsc6x_cmd_send(data, &m_cmd, &m_rsp) == 0X0) {
				tlsc6x_read_bytes_u16addr(data,
							  MTK_TXRX_BUF, pdes,
							  len);
				combChk = tlsc6x_checksumEx(pdes, len);
				if (m_rsp.d0 == (unsigned short)combChk) {
					if (m_rsp.sn ==
					    (unsigned short)(combChk >> 16)) {
						rsp = 0;
						break;
					}
				}
			}
		}

		if (rsp < 0) {
			break;
		}
		left -= len;
		adr += len;
		pdes += len;
	}

	return rsp;
}

int tlsc6x_write_burn_space(struct tlsc6x_drv_data *data, u8 *psrc, u16 adr, u16 len)
{
	int rsp = 0;
	u16 left = len;
	u32 retry, combChk;
	ctp_test_wr_t m_cmd;
	ctp_test_rd_t m_rsp;

	TLSC_FUNC_ENTER();
	m_cmd.id = 0x30;
	m_cmd.resv = 0x11;

	while (left) {
		len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;
		combChk = tlsc6x_checksumEx(psrc, len);

		m_cmd.d0 = adr;
		m_cmd.d1 = len;
		m_cmd.d2 = (u16) combChk;
		m_cmd.s2Pad0 = (u16) (combChk >> 16);

		rsp = -1;	/* avoid dead loop */
		retry = 0;
		while (retry < 3) {
			tlsc6x_write_bytes_u16addr(g_tlsc6x_client,
						   MTK_TXRX_BUF, psrc, len);
			m_rsp.id = 0;
			rsp = tlsc6x_cmd_send(data, &m_cmd, &m_rsp);
			if (rsp < 0) {
				if ((m_rsp.d0 == 0X05) && (m_rsp.cc == 0X09)) {	/* fotal error */
					break;
				}
				retry++;
			} else {
				left -= len;
				adr += len;
				psrc += len;
				break;
			}
		}

		if (rsp < 0) {
			break;
		}
	}

	return (!left) ? 0 : -1;
}

static int is_valid_cfg_data(struct tlsc6x_drv_data *data, u16 *ptcfg)
{
	if (ptcfg == NULL) {
		return 0;
	}
	tlsc_info("ptcfg[53]=0x%x,addr =0x%x\n", ptcfg[53],
		  (data->config->addr));
	if ((u8) ((ptcfg[53] >> 1) & 0x7f) != (u8) (data->config->addr)) {
		return 0;
	}

	if (tlsc6x_checksum_u16(ptcfg, 204)) {
		return 0;
	}

	return 1;
}

static int tlsc6x_tpcfg_ver_comp(struct tlsc6x_drv_data *data, unsigned short *ptcfg)
{
	unsigned int u32tmp;
	unsigned short vnow, vbuild;

	TLSC_FUNC_ENTER();
	if (g_tlsc6x_cfg_ver == 0) {	/* no available version information */
		return 0;
	}

	if (is_valid_cfg_data(data, ptcfg) == 0) {
		return 0;
	}

	u32tmp = ptcfg[1];
	u32tmp = (u32tmp << 16) | ptcfg[0];
	if ((g_tlsc6x_cfg_ver & 0x3ffffff) != (u32tmp & 0x3ffffff)) {
		return 0;
	}

	vnow = (g_tlsc6x_cfg_ver >> 26) & 0x3f;
	vbuild = (u32tmp >> 26) & 0x3f;
	tlsc_info("vnow: 0x%x,vbuild: 0x%x", vnow, vbuild);
	if (vbuild <= vnow) {
		return 0;
	}

	return 1;
}

static int tlsc6x_tpcfg_ver_comp_weak(struct tlsc6x_drv_data *data, unsigned short *ptcfg)
{
	unsigned int u32tmp;

	TLSC_FUNC_ENTER();
	if (g_tlsc6x_cfg_ver == 0) {	/* no available version information */
		return 0;
	}

	if (is_valid_cfg_data(data, ptcfg) == 0) {
		tlsc_err("Tlsc6x:cfg_data is invalid!\n");
		return 0;
	}

	u32tmp = ptcfg[1];
	u32tmp = (u32tmp << 16) | ptcfg[0];
	if ((g_tlsc6x_cfg_ver & 0x3ffffff) != (u32tmp & 0x3ffffff)) {	/*  */
		tlsc_err
		    ("tlsc6x:ptcfg version error,g_tlsc6x_cfg_ver is 0x%x:ptcfg version is 0x%x!\n",
		     g_tlsc6x_cfg_ver, u32tmp);
		return 0;
	}

	if (g_tlsc6x_cfg_ver == u32tmp) {
		return 0;
	}

	return 1;
}

/* 0 error */
/* 0x7f80 no data */
static u16 tlsx6x_find_last_cfg(struct tlsc6x_drv_data *data)
{
	unsigned short addr, check;

	addr = 0x7f80 - 256;
	while (addr > 0x6000) {	/* 0x6080 */
		check = 0;
		if (tlsc6x_read_burn_space(data, (u8 *) &check, addr, 2)) {
			addr = 0;
			goto exit;
		}
		if (check == 0xffff) {
			break;
		}
		addr -= 256;
	}

	addr += 256;

exit:

	return addr;
}

static int find_last_valid_burn_cfg(struct tlsc6x_drv_data *data, u16 *ptcfg)
{
	unsigned short addr;

	if (tlsc6x_download_ramcode(data, fw_burn_bin, sizeof(fw_burn_bin))) {
		return -EPERM;
	}

	addr = tlsx6x_find_last_cfg(data);
	if ((addr == 0) || (addr == 0x7f80)) {
		return -EPERM;
	}

	while (addr <= 0x7e80) {
		if (tlsc6x_read_burn_space(data, (u8 *) ptcfg, addr, 204)) {
			addr = 0x7f80;	/* force error */
			break;
		}
		if (is_valid_cfg_data(data, ptcfg) == 0) {
			addr += 256;
		} else {
			break;
		}
	}

	return (addr <= 0x7e80) ? 0 : -1;

}

#if 0
int tlsx6x_update_fcomp_cfg(u16 *ptcfg)
{
	if (tlsc6x_tpcfg_ver_comp_weak(ptcfg) == 0) {
		tlsc_err("Tlsc6x:update error:version error!\n");
		return -EPERM;
	}

	if (g_tlsc6x_cfg_ver && ((u16) (g_tlsc6x_cfg_ver & 0xffff) != ptcfg[0])) {
		return -EPERM;
	}

	if (tlsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn))) {
		tlsc_err("Tlsc6x:update error:ram-code error!\n");
		return -EPERM;
	}

	if (tlsc6x_write_burn_space((unsigned char *)ptcfg, 0x8000, 204)) {
		tlsc_err("Tlsc6x:update fcomp_cfg fail!\n");
		return -EPERM;
	}

	tlsc_info("Tlsc6x:update fcomp_cfg pass!\n");

	memcpy(tl_target_cfg, ptcfg, 204);
	g_tlsc6x_cfg_ver = (ptcfg[1] << 16) | ptcfg[0];
	tlsc_cfg_version = g_tlsc6x_cfg_ver >> 26;

	return 0;
}

int tlsx6x_update_fcomp_boot(unsigned char *pdata, u16 len)
{
	if (tlsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn))) {
		tlsc_err("Tlsc6x:update error:ram-code error!\n");
		return -EPERM;
	}
	pdata[8] = 0xff;
	if (tlsc6x_write_burn_space((unsigned char *)pdata, 0x00, len)) {
		tlsc_err("Tlsc6x:update fcomp_boot fail!\n");
		return -EPERM;
	}
	pdata[8] = 0x4b;
	if (tlsc6x_write_burn_space((unsigned char *)&pdata[8], 0x08, 1)) {
		tlsc_err("Tlsc6x:update fcomp_boot last sig-byte fail!\n");
		return -EPERM;
	}

	tlsc_info("Tlsc6x:update fcomp_boot pass!\n");
	g_tlsc6x_boot_ver = pdata[5];
	g_tlsc6x_boot_ver = (g_tlsc6x_boot_ver << 8) + pdata[4];
	tlsc_boot_version = g_tlsc6x_boot_ver;
	return 0;
}

int tlsx6x_update_ocomp_boot(unsigned char *pdata, u16 len)
{
	unsigned int oo_tail[4] = { 0x60298bf, 0x15cbf, 0x60301bf, 0x3d43f };

	if (tlsc6x_download_ramcode(fw_burn_bin, sizeof(fw_burn_bin))) {
		tlsc_err("Tlsc6x:update error:ram-code error!\n");
		return -EPERM;
	}

	if (tlsc6x_write_burn_space((unsigned char *)pdata, 0x00, len)) {
		tlsc_err("Tlsc6x:update ocomp_boot fail!\n");
		return -EPERM;
	}

	if (tlsc6x_write_burn_space((unsigned char *)oo_tail, 0x7fec, 16)) {
		tlsc_err("Tlsc6x:update ocomp_boot fail!\n");
		return -EPERM;
	}
	tlsc_info("Tlsc6x:update ocomp_boot pass!\n");

	return 0;
}

int tlsc6x_update_f_combboot(u8 *pdata, u16 len)
{
	int ret = 0;
	unsigned short boot_len;

	boot_len = pdata[7];
	boot_len = (boot_len << 8) + pdata[6];

	printk
	    ("tlsc_fail_debug:Now update bin the len is%d,the boot len is%d.\n",
	     len, boot_len);

	if (g_mccode == 0) {
		if (g_needKeepRamCode == 0) {
			tlsc_err
			    ("Tlsc6x:update_f_combboot error:mccode error!\n");
			return -EPERM;
		}
		if ((len >= boot_len)
		    && ((pdata[2] == 0x35) && (pdata[3] == 0x35))) {
			ret = tlsx6x_update_ocomp_boot(pdata, boot_len);
			pdata += boot_len;
			len = len - boot_len;
		}
		if ((ret == 0) && (len >= 204)) {
			memcpy(tl_buf_tmpcfg, pdata, 204);
			ret = tlsx6x_update_burn_cfg(tl_buf_tmpcfg);
		}
		return ret;
	}

	if ((len >= boot_len) && ((pdata[2] == 0x36) && (pdata[3] == 0x35))) {
		ret = tlsx6x_update_fcomp_boot(pdata, boot_len);
		pdata += boot_len;
		len = len - boot_len;
	}

	printk
	    ("tlsc_fail_debug:Boot bin is finish. Now the len is%d,the boot len is%d.\n",
	     len, boot_len);

	if ((ret == 0) && (len >= 204)) {
		memcpy(tl_buf_tmpcfg, pdata, 204);
		ret = tlsx6x_update_fcomp_cfg(tl_buf_tmpcfg);
	}

	return ret;

}

int tlsx6x_update_burn_cfg(u16 *ptcfg)
{
	u16 addr, check;

	TLSC_FUNC_ENTER();
	if (g_mccode == 1) {
		return tlsx6x_update_fcomp_cfg(ptcfg);
	}

	if (tlsc6x_tpcfg_ver_comp_weak(ptcfg) == 0) {
		tlsc_err("Tlsc6x:update error:version error!\n");
		return -EPERM;
	}

	if (g_tlsc6x_cfg_ver && ((u16) (g_tlsc6x_cfg_ver & 0xffff) != ptcfg[0])) {
		return -EPERM;
	}

	if (tlsc6x_download_ramcode(fw_burn_bin, sizeof(fw_burn_bin))) {
		tlsc_err("Tlsc6x:update error:ram-code error!\n");
		return -EPERM;
	}

	addr = tlsx6x_find_last_cfg();
	if ((addr <= 0x6180) || (addr == 0x7f80)) {
		tlsc_err("Tlsc6x:update error:time limit!\n");
		return -EPERM;
	}

	addr = addr - 256;

	/* pre-check */
	check = 0;
	if (tlsc6x_read_burn_space((unsigned char *)&check, addr - 256, 2)) {
		tlsc_err("Tlsc6x:update error:pre-read error!\n");
		return -EPERM;
	}
	if (check != 0xffff) {
		tlsc_err("Tlsc6x:update error:pre-read limit!\n");
		return -EPERM;
	}

	if (tlsc6x_write_burn_space((unsigned char *)ptcfg, addr, 204)) {
		tlsc_err("Tlsc6x:update fail!\n");
		return -EPERM;
	}

	tlsc_info("Tlsc6x:update pass!\n");

	memcpy(tl_target_cfg, ptcfg, 204);
	g_tlsc6x_cfg_ver = (ptcfg[1] << 16) | ptcfg[0];
	g_tlsc6x_chip_code = (unsigned short)ptcfg[53];

	tlsc_cfg_version = g_tlsc6x_cfg_ver >> 26;

	return 0;
}

/* NOTE:caller guarantee the legitimacy */
/* download tp-cfg. */
int tlsx6x_update_running_cfg(u16 *ptcfg)
{
	unsigned int retry;
	unsigned int tmp[2];

	TLSC_FUNC_ENTER();
	if (is_valid_cfg_data(ptcfg) == 0) {
		return -EPERM;
	}
	if (tlsc6x_set_dd_mode()) {
		return -EPERM;
	}

	if (tlsc6x_bulk_down_check((unsigned char *)ptcfg, 0xd7e0, 204)) {	/* stop mcu */
		goto exit;
	}

	tmp[0] = 0x6798;
	tmp[1] = 0xcd3500ff;

	retry = 0;
	while (++retry < 3) {
		if (tlsc6x_write_bytes_u16addr
		    (g_tlsc6x_client, 0xdf10, (u8 *) &tmp[0], 8)) {
			usleep_range(5000, 5500);
			continue;
		}
		break;
	}

	/* write error? don't care */
	retry = 0;
	while (++retry < 5) {
		usleep_range(10000, 11000);
		tmp[0] = 0;
		tlsc6x_read_bytes_u16addr(g_tlsc6x_client, 0xdf16,
					  (u8 *) &tmp[0], 1);
		if (tmp[0] == 0x30) {
			break;
		}
	}

exit:	tlsc6x_set_nor_mode();
	memcpy(tl_target_cfg, ptcfg, 204);
	return 0;
}

/* return :0->no hw resetting needed */
/* else -> caller do hw resettting */

/* 0:successful */
static int tlsc6x_download_gestlib_fast(u8 *pcode, u16 len)
{
	u8 dwr, retry;
	int ret = -2;

	TLSC_FUNC_ENTER();
	if (tlsc6x_set_dd_mode()) {
		return -EPERM;
	}

	dwr = 0x05;
	if (tlsc6x_bulk_down_check(&dwr, 0x0602, 1) == 0) {	/* stop mcu */
		dwr = 0x00;
		tlsc6x_bulk_down_check(&dwr, 0x0643, 1);	/* disable irq */
	} else {
		return -EPERM;
	}

	if (tlsc6x_bulk_down_check(pcode, 0x8000, 1024) == 0) {
		if (tlsc6x_write_bytes_u16addr(g_tlsc6x_client, 0x8400, pcode + 1024, len - 1024) == 0) {	/*  */
			ret = 0;
		}
	}

	if (ret == 0) {
		dwr = 0x88;
		retry = 0;
		do {
			ret =
			    tlsc6x_write_bytes_u16addr(g_tlsc6x_client, 0x0602,
						       &dwr, 1);
		} while ((++retry < 3) && (ret != 0));
	}

	msleep(40);		/* 30ms */
	if (tlsc6x_get_i2cmode() == DIRECTLY_MODE) {
		ret = tlsc6x_download_ramcode(pcode, len);
	}

	return ret;
}

int tlsc6x_load_gesture_binlib(void)
{
	int ret;

	TLSC_FUNC_ENTER();
	ret = tlsc6x_download_gestlib_fast(tlsc6x_gesture_binlib,
					   sizeof(tlsc6x_gesture_binlib));
	if (ret) {
		tlsc_err("Tlsc6x:load gesture binlib error!\n");
		return -EPERM;
	}
	return 0;
}

void tlsc6x_3536enter_gest(int event_mask)
{
	tlsc_info("tlsc6x::3536 setup gesture: 0x%x!\n", event_mask);

	if (tlsc6x_write_reg(g_tlsc6x_client, 0xd0, 0x01) < 0) {
		tlsc_err("tlsc6x error::3536 setup gesture fail!\n");
		return;
	}
}

void tlsc6x_enter_gest_mode(int event_mask)
{
	if (g_mccode == 1) {
		tlsc6x_3536enter_gest(event_mask);
	} else {
		tlsc6x_load_gesture_binlib();
	}
}


int tlsc6x_load_ext_binlib(u8 *pdata, u16 len)
{
	int ret = 0;
	unsigned short boot_len;

	TLSC_FUNC_ENTER();

	boot_len = pdata[7];
	boot_len = (boot_len << 8) + pdata[6];

	if (len >= boot_len) {
		ret = tlsc6x_download_ramcode(pdata, boot_len);
		pdata += boot_len;
		len = len - boot_len;
		if ((ret == 0) && (len >= 204)) {
			msleep(30);
			ret = tlsx6x_update_running_cfg((u16 *) pdata);
		}
	}

	return 0;
}

#endif
/*
 *get running time tp-cfg.
 *@ptcfg:	data buffer
 *@addr:	real data address for different chips
 *
 *return: 0 SUCESS else FAIL
 * Note: touch chip	MUST work in DD-mode.
 */
static int tlsx6x_comb_get_running_cfg(struct tlsc6x_drv_data *data, u16 *ptcfg, u16 addr)
{
	int retry, err_type;

	TLSC_FUNC_ENTER();
	retry = 0;
	err_type = 0;

	tlsc6x_set_dd_mode(data);

	while (++retry < 5) {
		err_type = 0;
		if (tlsc6x_read_bytes_u16addr
		    (data, addr, (u8 *) ptcfg, 204)) {
			msleep(20);
			err_type = 2;	/* i2c error */
			continue;
		}

		if (is_valid_cfg_data(data, ptcfg) == 0) {
			tlsc6x_set_dd_mode(data);
			err_type = 1;	/* data error or no data */
			msleep(20);
			continue;
		}
		break;
	}

	return err_type;

}

static int tlsx6x_3535get_running_cfg(struct tlsc6x_drv_data *data, unsigned short *ptcfg)
{

	TLSC_FUNC_ENTER();

	return tlsx6x_comb_get_running_cfg(data, ptcfg, 0xd6e0);
}

static int tlsx6x_3536get_running_cfg(struct tlsc6x_drv_data *data, unsigned short *ptcfg)
{

	TLSC_FUNC_ENTER();

	return tlsx6x_comb_get_running_cfg(data, ptcfg, 0x9e00);
}

static int tlsx6x_3536get_os_data(unsigned short *ptdata, int len)
{
	int ret = 0;
	int retryTime;
	u8 writebuf[4];

	TLSC_FUNC_ENTER();

	if (len < 96) {
		tlsc_err("%s(): error size.", __func__);
		return -EPERM;
	}

	msleep(100);

	mutex_lock(&i2c_rw_access);
	//write addr
	writebuf[0] = 0x9F;
	writebuf[1] = 0x20;
	writebuf[2] = 48;
	writebuf[3] = 0xFF;
	ret = tlsc6x_i2c_write_sub(g_tlsc6x_client, writebuf, 4);
	writebuf[0] = 0x9F;
	writebuf[1] = 0x24;
	writebuf[2] = 1;

	ret = tlsc6x_i2c_write_sub(g_tlsc6x_client, writebuf, 3);
	retryTime = 100;
	do {
		ret =
		    tlsc6x_i2c_read_sub(g_tlsc6x_client, writebuf, 2,
					&writebuf[2], 1);
		if (ret < 0) {
			break;
		}
		retryTime--;
		msleep(30);
	} while ((retryTime > 0) && (writebuf[2] == 1));

	if (ret >= 0) {
		writebuf[0] = 0x9F;
		writebuf[1] = 0x26;
		ret =
		    tlsc6x_i2c_read_sub(g_tlsc6x_client, writebuf, 2,
					(char *)ptdata, 96);
	}

	mutex_unlock(&i2c_rw_access);

	return ret;
}

static int tlsx6x_3535find_lastvaild_ver(struct tlsc6x_drv_data *data)
{
	if (find_last_valid_burn_cfg(data, tl_buf_tmpcfg) == 0) {
		g_tlsc6x_cfg_ver = (unsigned int)tl_buf_tmpcfg[1];
		g_tlsc6x_cfg_ver = (g_tlsc6x_cfg_ver << 16)
		    + (unsigned int)tl_buf_tmpcfg[0];

		tlsc_cfg_version = g_tlsc6x_cfg_ver >> 26;
		tlsc_vendor_id = (g_tlsc6x_cfg_ver >> 9) & 0x7F;
	}

	return 0;
}

static int tlsx6x_3536find_lastvaild_ver(struct tlsc6x_drv_data *data)
{
	if (tlsc6x_download_ramcode(data, fw_fcode_burn, sizeof(fw_fcode_burn))) {
		tlsc_err("Tlsc6x:update error:ram-code error!\n");
		return -EPERM;
	}

	if (tlsc6x_read_burn_space(data, (u8 *) tl_buf_tmpcfg, 0xf000, 204)) {
		return -EPERM;
	}

	if (is_valid_cfg_data(data, tl_buf_tmpcfg)) {
		g_tlsc6x_cfg_ver = (unsigned int)tl_buf_tmpcfg[1];
		g_tlsc6x_cfg_ver = (g_tlsc6x_cfg_ver << 16)
		    + (unsigned int)tl_buf_tmpcfg[0];
		g_tlsc6x_cfg_ver = g_tlsc6x_cfg_ver & 0x3ffffff;
		tlsc_cfg_version = g_tlsc6x_cfg_ver >> 26;
		tlsc_vendor_id = (g_tlsc6x_cfg_ver >> 9) & 0x7F;
		tlsc_err("zhanghui1 func=%s,vendor= %d,cfg_version=%d\n",
			 __func__, tlsc_vendor_id, tlsc_cfg_version);
	}
	return 0;
}

int tlsc6x_get_running_cfg(struct tlsc6x_drv_data *data, unsigned short *ptcfg)
{
	if (g_mccode == 0) {
		return tlsx6x_3535get_running_cfg(data, ptcfg);
	}
	return tlsx6x_3536get_running_cfg(data, ptcfg);
}

int tlsc6x_get_os_data(unsigned short *ptdata, int len)
{
	if (g_mccode == 1) {
		return tlsx6x_3536get_os_data(ptdata, len);
	}
	return -EPERM;
}

int tlsc6x_find_ver(struct tlsc6x_drv_data *data)
{
	if (g_mccode == 0) {
		return tlsx6x_3535find_lastvaild_ver(data);
	}
	return tlsx6x_3536find_lastvaild_ver(data);

}

/* NOT a public function, only one caller!!! */
static void tlsc6x_tp_mccode(struct tlsc6x_drv_data *data)
{
	unsigned int tmp[3];

	g_mccode = 0xff;

	if (tlsc6x_read_bytes_u16addr(data, 0x8000, (u8 *) tmp, 12)) {
		return;
	}
	if (tmp[2] == 0x544c4e4b) {	/*  boot code  */
		if (tmp[0] == 0x35368008) {
			g_mccode = 1;
			g_tlsc6x_boot_ver = tmp[1] & 0xffff;
			tlsc_boot_version = g_tlsc6x_boot_ver;
		} else if (tmp[0] == 0x35358008) {
			g_mccode = 0;
			g_tlsc6x_boot_ver = tmp[1] & 0xffff;
			tlsc_boot_version = g_tlsc6x_boot_ver;
		}
		tlsc_info("Tlsc6x:get mccode via boot code!\n");
	} else {		/* none code */
		tlsc_info("Tlsc6x:get mccode via registers!\n");
		tmp[0] = 0;
		if (tlsc6x_read_bytes_u16addr
		    (data, 0x09, (u8 *) tmp, 3)) {
			return;
		}
		if ((tmp[0] == 0x444240) || (tmp[0] == 0x5c5c5c)) {
			g_mccode = 1;
		} else if (tmp[0] == 0x35358008) {
			g_mccode = 0;
		}
	}

	if (g_mccode == 0) {
		MTK_TXRX_BUF = 0x80cc00;
		CMD_ADDR = 0x80b400;
		RSP_ADDR = 0x80b440;
		//chip_op_if.get_running_cfg = tlsx6x_3535get_running_cfg;
		//chip_op_if.find_ver = tlsx6x_3535find_lastvaild_ver;
		//chip_op_if.update_boot = tlsc6x_3535boot_update;
	} else {
		MTK_TXRX_BUF = 0x809000;
		CMD_ADDR = 0x809f00;
		RSP_ADDR = 0x809f40;
		//chip_op_if.get_running_cfg = tlsx6x_3536get_running_cfg;
		//chip_op_if.find_ver = tlsx6x_3536find_lastvaild_ver;
		//chip_op_if.update_boot = tlsc6x_3536boot_update;
	}

}

/* is tlsc6x module ? */
int tlsc6x_tp_dect(struct tlsc6x_drv_data *data)
{
	TLSC_FUNC_ENTER();

	g_mccode = 0;		/* default */

	if (tlsc6x_set_dd_mode(data)) {
		tlsc_info("set_dd_mode return 0\n");
		return 0;
	}

	tlsc6x_tp_mccode(data);	/* MUST: call this function there!!! */
	tlsc_info("g_mccode is 0x%x\n", g_mccode);

	if (g_mccode == 0xff) {
		tlsc_err("get mccode fail\n");
		goto exit;
	}

	/*try to get running time tp-cfg. if fail : wrong boot? wrong rom-cfg? */
	if (tlsc6x_get_running_cfg(data, tl_buf_tmpcfg) == 0) {
		tlsc_info("get_runnig_cfg\n");
		g_tlsc6x_cfg_ver = (unsigned int)tl_buf_tmpcfg[1];
		g_tlsc6x_cfg_ver = (g_tlsc6x_cfg_ver << 16)
		    + (unsigned int)tl_buf_tmpcfg[0];
		g_tlsc6x_chip_code = (unsigned short)tl_buf_tmpcfg[53];

		tlsc_cfg_version = g_tlsc6x_cfg_ver >> 26;
		tlsc_vendor_id = (g_tlsc6x_cfg_ver >> 9) & 0x7F;
		tlsc_info("zhanghui vendor= %d,cfg_version=%d\n", tlsc_vendor_id,
			 tlsc_cfg_version);

	} else {
		tlsc_info("tlsc6x_find_ver\n");
		tlsc6x_find_ver(data);
	}

	if (g_tlsc6x_cfg_ver == 0) {
		tlsc_err("get cfg-ver fail\n");
		goto exit;
	}

exit:
	tlsc6x_set_nor_mode(data);

	return 1;
}
