#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "be_log.h"
#include "be_port_osal.h"
#include "mbedtls/aes.h"
#include "miio-common.h"
#include "miio-device.h"
#include "utils_md5.h"

#define RECV_TIMEOUT_MS 200
#define MAX_RECV_COUNT 10

struct miio_device {
    int sfd; /* socket file description */
    in_addr_t peer;
    char token[TOKEN_SIZE];
    char key[KEY_IV_SIZE];
    char iv[KEY_IV_SIZE];
    int id;
    unsigned long last; /* TOTO:距上一次交互,2分钟内有效 */
    /* 事件监听相关 */
    miio_device_event_callback cb;
    void *priv;
};

/* 这里假定input和output buffer不会溢出 */
static int miio_encrypt(miio_device_t *device, unsigned char *input,
                        int input_len, char *output, int *output_len) {
#ifdef BE_OS_AOS
    int len;
    char *dest = NULL;
    be_aes_crypt_in_t aes_input;

    aes_input.iv      = device->iv;
    aes_input.key     = device->key;
    aes_input.key_len = sizeof(device->key);
    aes_input.packet  = input;
    aes_input.pkt_len = input_len;
    aes_input.type    = BE_AES_CBC;
    aes_input.padding = BE_SYM_PKCS5_PAD;
    len               = be_osal_aes_encrypt(&aes_input, &dest);
    if (len > 0) {
        *output_len = len;
        memcpy(output, dest, len);
        free(dest);
        return 0;
    }
    return -1;
#else
    int i;
    int padding_len;
    int payload_len;

    /* PKCS#7方式数据填充 */
    if (input_len & 15)
        padding_len = 16 - (input_len & 15);
    else
        padding_len = 16;
    payload_len = input_len + padding_len;
    for (i = 0; i < padding_len; i++)
        input[input_len + i] = (unsigned char)padding_len;

    /* 调用加密库 */
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, device->key, KEY_IV_SIZE * 8);
    unsigned char iv_tmp[KEY_IV_SIZE];
    memcpy(iv_tmp, device->iv, sizeof(iv_tmp));
    int err = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, payload_len,
                                    iv_tmp, input, output);
    mbedtls_aes_free(&aes);
    if (!err) {
        *output_len = payload_len;
        return 0;
    }
    return err;
#endif
}

/* 这里假定input和output buffer不会溢出 */
static int miio_decrypt(miio_device_t *device, unsigned char *input,
                        int input_len, char *output, int *output_len) {
#ifdef BE_OS_AOS
    int len;
    char *dest = NULL;
    be_aes_crypt_in_t aes_input;

    aes_input.iv      = device->iv;
    aes_input.key     = device->key;
    aes_input.key_len = sizeof(device->key);
    aes_input.packet  = input;
    aes_input.pkt_len = input_len;
    aes_input.type    = BE_AES_CBC;
    aes_input.padding = BE_SYM_PKCS5_PAD;
    len               = be_osal_aes_decrypt(&aes_input, &dest);
    if (len > 0) {
        *output_len = len;
        memcpy(output, dest, len);
        free(dest);
        return 0;
    }
    return -1;
#else
    /* 调用解密库 */
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, device->key, KEY_IV_SIZE * 8);
    unsigned char iv_tmp[KEY_IV_SIZE];
    memcpy(iv_tmp, device->iv, sizeof(iv_tmp));
    int err = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, input_len,
                                    iv_tmp, input, output);
    mbedtls_aes_free(&aes);
    if (!err) {
        /* 去除填充字段 */
        int padding_len = output[input_len - 1];
        *output_len     = input_len - padding_len;
    }
    return err;
#endif
}

/* FIXME:发送和接收分开,对于control信令需会话id进行校验,非当前会话要有重发机制
 */
static int miio_transmit(miio_device_t *device, const char *send_data,
                         size_t send_data_len, char *recv_buff,
                         size_t recv_buf_size) {
    struct sockaddr_in peer;

    if (!device->sfd) {
        debug("init socket\n");
        device->sfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (device->sfd == -1) {
            perror("create socket failed\n");
            return -1;
        }
    }

    debug("send data size: %d\n", (int)send_data_len);
    memset(&peer, 0, sizeof(peer));
    peer.sin_family      = AF_INET;
    peer.sin_addr.s_addr = device->peer;
    peer.sin_port        = htons(MIIO_SERVER_PORT);
    if (sendto(device->sfd, send_data, send_data_len, 0,
               (struct sockaddr *)&peer, sizeof(peer)) < 0) {
        perror("sendto failed\n");
        return -2;
    }

    struct timeval tv;
    fd_set readfds;
    int i = 0;
    ssize_t nread;
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen;          /* length of addresses */
    for (i = 0; i < MAX_RECV_COUNT; i++) {
        FD_ZERO(&readfds);
        FD_SET(device->sfd, &readfds);
        tv.tv_sec  = 0;
        tv.tv_usec = RECV_TIMEOUT_MS * 1000;
        select(device->sfd + 1, &readfds, NULL, NULL, &tv);
        if (FD_ISSET(device->sfd, &readfds)) {
            addrlen = sizeof(remaddr);
            nread   = recvfrom(device->sfd, recv_buff, recv_buf_size, 0,
                             (struct sockaddr *)&remaddr, &addrlen);
            debug("receive data size: %d\n", (int)nread);
            if (nread == -1) continue; /* Ignore failed request */
            return nread;
        }
    }

    return -3;
}

/*
 * 以太网上UDP最大有效报文长度为1472字节, 1500(以太网MTU) - 20(IP报文)
 * - 8(UDP报文头) = 1472
 */
static unsigned char recv_buf[1473];

const char *miio_device_control(miio_device_t *device, const char *method,
                                const char *args, const char *sid) {
    unsigned char pkt[1472];
    size_t recv_data_len;
    time_t start = time(NULL);
    debug("in\n");
    if ((recv_data_len = miio_transmit(
             device, hello_packet, sizeof(hello_packet), pkt, sizeof(pkt))) <
        0) {
        perror("handshake failed\n");
        goto failed;
    }

    /* 暂时借用recv_buf */
    if (!sid)
        snprintf(recv_buf, sizeof(recv_buf),
                 "{\"id\":%d,\"method\":\"%s\",\"params\":%s}", device->id++,
                 method, args);
    else
        snprintf(recv_buf, sizeof(recv_buf),
                 "{\"id\":%d,\"method\":\"%s\",\"params\":%s,\"sid\":\"%s\"}",
                 device->id++, method, args, sid);
    debug("info: %s\n", recv_buf);

    /* 加密数据并填充length字段 */
    int len = 0;
    miio_encrypt(device, recv_buf, strlen(recv_buf), pkt + 32, &len);
    len += 32;
    pkt[2] = (unsigned char)((unsigned int)len >> 8);
    pkt[3] = (unsigned char)len;

    /* 更新时间戳 */
    unsigned long timestamp =
        pkt[12] << 24 | pkt[13] << 16 | pkt[14] << 8 | pkt[15];
    timestamp += time(NULL) - start;
    *((unsigned long *)(pkt + 12)) = htonl(timestamp);

    /* 计算md5值并填充到[16, 31] */
    iot_md5_context ctx;
    utils_md5_init(&ctx);
    utils_md5_starts(&ctx);
    utils_md5_update(&ctx, pkt, 16);
    utils_md5_update(&ctx, device->token, sizeof(device->token));
    utils_md5_update(&ctx, pkt + 32, len - 32);
    utils_md5_finish(&ctx, pkt + 16); /* 填充到[16,31] */
    utils_md5_free(&ctx);

    /* 传输并接收 */
    if ((recv_data_len =
             miio_transmit(device, pkt, len, recv_buf, sizeof(recv_buf))) < 0) {
        perror("miio_transmit failed\n");
        goto failed;
    }
    debug("receive data size: %d\n", (int)recv_data_len);

    /* 解密回复报文 */
    int decrypted_data_len = 0;
    if (!miio_decrypt(device, recv_buf + 32, recv_data_len - 32, pkt,
                      &decrypted_data_len)) {
        memcpy(recv_buf, pkt, decrypted_data_len);
        recv_buf[decrypted_data_len] = 0;
        debug("response %s\n", recv_buf);
        return recv_buf;
    }

failed:
    return NULL;
}

#define MULTICAST_ADDRESS "224.0.0.50"
#define SERVER_PORT 9898
#define MSGBUFSIZE 1473

static void *event_receive(void *arg) {
    struct sockaddr_in addr;
    socklen_t addrlen;
    int fd;
    ssize_t nbytes;
    struct ip_mreq mreq;
    miio_device_t *device = (miio_device_t *)arg;
    char msgbuf[MSGBUFSIZE];
    u_int yes = 1; /*** MODIFICATION TO ORIGINAL */

    /* create what looks like an ordinary UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }
    /*** END OF MODIFICATION TO ORIGINAL */

    /* set up destination address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
    addr.sin_port        = htons(SERVER_PORT);

    /* bind to receive address */
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) <
        0) {
        perror("setsockopt");
        exit(1);
    }

    /* now just enter a read-print loop */
    while (1) {
        addrlen = sizeof(addr);
        if ((nbytes =
                 recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *)&addr,
                          (socklen_t *)&addrlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        msgbuf[nbytes] = '\0';
        device->cb(device->priv, msgbuf);
    }

    return NULL;
}

void miio_device_set_event_cb(miio_device_t *device,
                              miio_device_event_callback cb, void *priv) {
    debug("in\n");

    /* 创建接收事件线程 */
    int ret = be_osal_create_task("miio event receive task", event_receive,
                                  device, 4096, ADDON_TSK_PRIORRITY, NULL);
    if (ret == 0) {
        device->cb   = cb;
        device->priv = priv;
    } else {
        perror("create receive thread failed\n");
    }
}

miio_device_t *miio_device_create(const char *host, const char *token) {
    int i;
    int j;

    debug("in\n");
    miio_device_t *device = calloc(1, sizeof(*device));
    if (!device) {
        perror("miio device create failed\n");
        return NULL;
    }

    device->peer = inet_addr(host);

    /* 转为16进制  */
    for (j = 0, i = 0; i < 32; i += 2, j++) {
        unsigned char ch = token[i];
        if (ch >= '0' && ch <= '9') {
            device->token[j] = (ch - '0') << 4;
        } else if (ch >= 'a' && ch <= 'f') {
            device->token[j] = (ch - 'a' + 10) << 4;
        } else if (ch >= 'A' && ch <= 'F') {
            device->token[j] = (ch - 'A' + 10) << 4;
        }
        ch = token[i + 1];
        if (ch >= '0' && ch <= '9') {
            device->token[j] |= (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            device->token[j] |= (ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            device->token[j] |= (ch - 'A' + 10);
        }
    }

    device->id = 1;

    /* 生成key */
    utils_md5(device->token, sizeof(device->token), device->key);

    /* 生成iv */
    iot_md5_context ctx;
    utils_md5_init(&ctx);
    utils_md5_starts(&ctx);
    utils_md5_update(&ctx, device->key, sizeof(device->key));
    utils_md5_update(&ctx, device->token, sizeof(device->token));
    utils_md5_finish(&ctx, device->iv);
    utils_md5_free(&ctx);

    debug("out\n");
    return device;
}
