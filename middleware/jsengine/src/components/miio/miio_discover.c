#include <stdio.h>
#include <string.h>

#include "jse_common.h"
#include "miio_common.h"
#include "miio_discover.h"

struct miio_discover_context {
    int timeout;
    miio_discover_callback cb;
    void *priv;
};

static struct miio_discover_context ctx;

static void discover_routin(void *arg)
{
    int fd;
    if ((fd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        jse_error("socket fail\n");
        return;
    }
    int optval = 1; /* MUST set optval */
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval,
               sizeof(int));
    struct sockaddr_in sid;
    memset(&sid, 0, sizeof(struct sockaddr_in));
    sid.sin_family      = AF_INET;
    sid.sin_addr.s_addr = inet_addr("255.255.255.255");
    sid.sin_port        = htons(MIIO_SERVER_PORT);

    time_t start = time(NULL);
    time_t now;
    struct timeval tv;
    fd_set readfds;
    int i = 0;
    ssize_t nread;
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen;          /* length of addresses */
    unsigned char recv_buf[HELLO_PACKET_SIZE];
    char str[INET_ADDRSTRLEN];
    long device_id;

    do {
        sendto(fd, hello_packet, sizeof(hello_packet), 0,
               (struct sockaddr *)&sid, sizeof(struct sockaddr));

        for (i = 0; i < 100; i++) {
            FD_ZERO(&readfds);
            FD_SET(fd, &readfds);
            tv.tv_sec  = 0;
            tv.tv_usec = 100 * 1000;
            select(fd + 1, &readfds, NULL, NULL, &tv);
            if (FD_ISSET(fd, &readfds)) {
                addrlen = sizeof(remaddr);
                nread   = recvfrom(fd, recv_buf, sizeof(recv_buf), 0,
                                 (struct sockaddr *)&remaddr, &addrlen);
                jse_debug("receive data size: %d\n", (int)nread);
                if (nread == -1) continue; /* Ignore failed request */
                device_id = (recv_buf[8] << 24) | (recv_buf[9] << 16) |
                            (recv_buf[10] << 8) | recv_buf[11];
                memset(str, 0, sizeof(str));
                inet_ntop(AF_INET, &(remaddr.sin_addr), str, INET_ADDRSTRLEN);
                jse_debug("peer address: %s, device_id: %lu\n", str, device_id);
                ctx.cb(ctx.priv, str, device_id);
            }
        }
        now = time(NULL);
    } while ((now - start) > ctx.timeout);

    return;
}

void miio_device_discover(int timeout, void *priv, miio_discover_callback cb)
{
    int ret = jse_osal_create_task("miio discover task", discover_routin, &ctx,
                                  4096, ADDON_TSK_PRIORRITY, NULL);
    if (ret == 0) {
        ctx.cb      = cb;
        ctx.priv    = priv;
        ctx.timeout = timeout;
    } else {
        perror("create thread failed\n");
    }
}
