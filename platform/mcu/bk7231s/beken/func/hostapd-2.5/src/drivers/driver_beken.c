/*
 * Driver interaction with Linux Host AP driver
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "include.h"
#include "hostapd_intf_pub.h"
#include "uart_pub.h"

#include "includes.h"
#include "common.h"
#include "driver.h"
#include "eloop.h"
#include "driver_beken.h"
#include "param_config.h"

#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "l2_packet/l2_packet.h"
#include "fake_socket.h"
#include "priv_netlink.h"
#include "netlink.h"
#include "hostapd_cfg.h"
#include "mac_common.h"
#include "signal.h"
#include "eloop.h"
#include "rw_pub.h"
#include "wlan_ui_pub.h"

static const u8 rfc1042_header[6] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };

struct hostap_driver_data
{
    struct hostapd_data *hapd;
    struct wpa_supplicant *wpa_s;

    char iface[IFNAMSIZ + 1];
    struct l2_packet_data *sock_xmit;	/* raw packet xmit socket */
    int sock; /* raw packet socket for driver access */
    int ioctl_sock; /* socket for ioctl() use */
    
    struct netlink_data *netlink;
    u8 own_addr[ETH_ALEN];
    u8 vif_index;

    int we_version;

    u8 *generic_ie;
    size_t generic_ie_len;
    u8 *wps_ie;
    size_t wps_ie_len;
};

static int hostapd_ioctl(void *priv, struct prism2_hostapd_param *param,                         int len);
static int hostap_set_iface_flags(void *priv, int dev_up);
extern void wpa_hostapd_queue_poll(uint32_t param);
extern int hapd_intf_add_vif(struct prism2_hostapd_param *param, int len);

static void handle_data(struct hostap_driver_data *drv, u8 *buf, size_t len,
                        u16 stype)
{
    struct ieee80211_hdr *hdr;
    u16 fc, ethertype;
    u8 *pos, *sa;
    size_t left;
    union wpa_event_data event;

    if (len < sizeof(struct ieee80211_hdr))
        return;

    hdr = (struct ieee80211_hdr *) buf;
    fc = le_to_host16(hdr->frame_control);

    if ((fc & (WLAN_FC_FROMDS | WLAN_FC_TODS)) != WLAN_FC_TODS)
    {
        os_printf("Not ToDS data frame (fc=0x%04x)\n", fc);
        return;
    }

    sa = hdr->addr2;
    os_memset(&event, 0, sizeof(event));
    event.rx_from_unknown.bssid = 0; // get_hdr_bssid(hdr, len); wangzhilei0
    event.rx_from_unknown.addr = sa;
    wpa_supplicant_event(drv->hapd, EVENT_RX_FROM_UNKNOWN, &event);

    pos = (u8 *) (hdr + 1);
    left = len - sizeof(*hdr);

    if (left < sizeof(rfc1042_header))
    {
        os_printf("Too short data frame\n");
        return;
    }

    if (memcmp(pos, rfc1042_header, sizeof(rfc1042_header)) != 0)
    {
        os_printf("Data frame with no RFC1042 header\n");
        return;
    }
    pos += sizeof(rfc1042_header);
    left -= sizeof(rfc1042_header);

    if (left < 2)
    {
        os_printf("No ethertype in data frame\n");
        return;
    }

    ethertype = WPA_GET_BE16(pos);
    pos += 2;
    left -= 2;
    switch (ethertype)
    {
    case ETH_P_PAE:
        drv_event_eapol_rx(drv->hapd, sa, pos, left);
        break;

    default:
        os_printf("Unknown ethertype 0x%04x in data frame\n", ethertype);
        break;
    }
}


static void handle_tx_callback(struct hostap_driver_data *drv, u8 *buf,
                               size_t len, int ok)
{
    struct ieee80211_hdr *hdr;
    u16 fc;
    union wpa_event_data event;

    hdr = (struct ieee80211_hdr *) buf;
    fc = le_to_host16(hdr->frame_control);

    os_memset(&event, 0, sizeof(event));
    event.tx_status.type = WLAN_FC_GET_TYPE(fc);
    event.tx_status.stype = WLAN_FC_GET_STYPE(fc);
    event.tx_status.dst = hdr->addr1;
    event.tx_status.data = buf;
    event.tx_status.data_len = len;
    event.tx_status.ack = ok;
    wpa_supplicant_event(drv->hapd, EVENT_TX_STATUS, &event);
}


static void handle_frame(struct hostap_driver_data *drv, u8 *buf, size_t len)
{
    struct ieee80211_hdr *hdr;
    u16 fc, type, stype;
    size_t data_len = len;
    int ver;
    union wpa_event_data event;

    /* PSPOLL is only 16 bytes, but driver does not (at least yet) pass
     * these to user space */
    if (len < 24)
    {
        wpa_printf(MSG_MSGDUMP, "handle_frame: too short (%lu)",
                   (unsigned long) len);
        return;
    }

    hdr = (struct ieee80211_hdr *) buf;
    fc = le_to_host16(hdr->frame_control);
    type = WLAN_FC_GET_TYPE(fc);
    stype = WLAN_FC_GET_STYPE(fc);

    if (type != WLAN_FC_TYPE_MGMT || stype != WLAN_FC_STYPE_BEACON)
    {
        wpa_hexdump(MSG_MSGDUMP, "Received management frame",
                    buf, len);
    }

    ver = fc & WLAN_FC_PVER;

    /* protocol version 2 is reserved for indicating ACKed frame (TX
     * callbacks), and version 1 for indicating failed frame (no ACK, TX
     * callbacks) */
    if (ver == 1 || ver == 2)
    {
        handle_tx_callback(drv, buf, data_len, ver == 2 ? 1 : 0);
        return;
    }
    else if (ver != 0)
    {
        os_printf("unknown protocol version %d\n", ver);
        return;
    }

    switch (type)
    {
    case WLAN_FC_TYPE_MGMT:
        os_memset(&event, 0, sizeof(event));
        event.rx_mgmt.frame = buf;
        event.rx_mgmt.frame_len = data_len;
        wpa_supplicant_event(drv->hapd, EVENT_RX_MGMT, &event);
        break;
    case WLAN_FC_TYPE_CTRL:
        wpa_printf(MSG_DEBUG, "CTRL");
        break;
    case WLAN_FC_TYPE_DATA:
        wpa_printf(MSG_DEBUG, "DATA");
        handle_data(drv, buf, data_len, stype);
        break;
    default:
        wpa_printf(MSG_DEBUG, "unknown frame type %d", type);
        break;
    }
}


static void handle_read(int sock, void *eloop_ctx, void *sock_ctx)
{
#define TMP_BUF_LEN     512

    int len;
    unsigned char *buf;
    struct hostap_driver_data *drv;

    drv = eloop_ctx;
    buf = os_malloc(TMP_BUF_LEN);
    if(!buf) {
        os_printf("handle_read no mem\r\n");
        goto read_exit;
    }
 //    ASSERT(buf);

    len = fsocket_recv(sock, buf, TMP_BUF_LEN, 0);
    if (len < 0)
    {
        wpa_printf(MSG_ERROR, "recv: %s", strerror(errno));
        goto read_exit;
    }

    handle_frame(drv, buf, len);

read_exit:
    if(buf)
    {
        os_free(buf);
    }

    return;
}

static void handle_eapol(void *ctx, const u8 *src_addr, const u8 *buf, size_t len)
{
    struct hostap_driver_data *drv = ctx;
    drv_event_eapol_rx(drv->hapd, src_addr, buf, len);
}

int hostap_get_ifhwaddr(int sock, const char *ifname, u8 *addr)
{
    return 0;
}

static int hostap_init_sockets(struct hostap_driver_data *drv, u8 *own_addr)
{
    int protocol = ETH_P_ALL;
    protocol += drv->vif_index;

    drv->sock = fsocket_init(PF_PACKET, SOCK_RAW, protocol);
    drv->sock_xmit = l2_packet_init(drv->iface, drv->own_addr, ETH_P_EAPOL,
                                    handle_eapol, drv, 0);

    if (eloop_register_read_sock(drv->sock, handle_read, drv, NULL))
    {
        wpa_printf(MSG_ERROR, "Could not register read socket");
        return -1;
    }

    if (hostap_set_iface_flags(drv, 1))
    {
        return -1;
    }

    return hostap_get_ifhwaddr(drv->sock, drv->iface, own_addr);
}


static int hostap_send_mlme(void *priv, const u8 *msg, size_t len, int noack,
                            unsigned int freq)
{
    struct hostap_driver_data *drv = priv;
    struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) msg;
    int res;
    S_TYPE_PTR type_ptr = os_zalloc(sizeof(S_TYPE_ST));

    type_ptr->type = HOSTAPD_MGMT;
    type_ptr->vif_index = drv->vif_index;


    /* Request TX callback */
    hdr->frame_control |= host_to_le16(BIT(1));
    res = fsocket_send(drv->sock, msg, len, type_ptr);
    hdr->frame_control &= ~host_to_le16(BIT(1));

    return res;
}


static int hostap_send_eapol(void *priv, const u8 *addr, const u8 *data,
                             size_t data_len, int encrypt, const u8 *own_addr,
                             u32 flags)
{
    struct hostap_driver_data *drv = priv;
    int status;

    status = l2_packet_send(drv->sock_xmit, addr, ETH_P_EAPOL, data, data_len);

    return status;
}


static int hostap_sta_set_flags(void *priv, const u8 *addr,
                                unsigned int total_flags, unsigned int flags_or,
                                unsigned int flags_and)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    if (flags_or & WPA_STA_AUTHORIZED)
        flags_or = BIT(5); /* WLAN_STA_AUTHORIZED */
    if (!(flags_and & WPA_STA_AUTHORIZED))
        flags_and = ~ BIT(5);
    else
        flags_and = ~0;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_SET_FLAGS_STA;
    memcpy(param.sta_addr, addr, ETH_ALEN);
    param.u.set_flags_sta.flags_or = flags_or;
    param.u.set_flags_sta.flags_and = flags_and;
    param.vif_idx = drv->vif_index;

    return hostapd_ioctl(drv, &param, sizeof(param));
}


static int hostap_set_iface_flags(void *priv, int dev_up)
{
    return 0;
}


static int hostapd_ioctl(void *priv, struct prism2_hostapd_param *param,
                         int len)
{
    struct hostap_driver_data *drv = priv;
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
    iwr.u.data.pointer = (c_addr_t) param;
    iwr.u.data.length = len;

    if (ioctl_inet(drv->ioctl_sock, drv->vif_index, PRISM2_IOCTL_HOSTAPD, (unsigned long)&iwr) < 0)
    {
        return -1;
    }

    return 0;
}

static int wpa_driver_hostap_set_key(const char *ifname, void *priv,
                                     enum wpa_alg alg, const u8 *addr,
                                     int key_idx, int set_tx,
                                     const u8 *seq, size_t seq_len,
                                     const u8 *key, size_t key_len)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0;

    blen = sizeof(*param) + key_len;
    buf = os_zalloc(blen);
    if (buf == NULL)
        return -1;

    param = (struct prism2_hostapd_param *) buf;
    param->cmd = PRISM2_SET_ENCRYPTION;
    if (addr == NULL)
        memset(param->sta_addr, 0xff, ETH_ALEN);
    else
        memcpy(param->sta_addr, addr, ETH_ALEN);
    
    switch (alg)
    {
    case WPA_ALG_NONE:
        os_strlcpy((char *) param->u.crypt.alg, "NONE",
                   HOSTAP_CRYPT_ALG_NAME_LEN);
        break;
    case WPA_ALG_WEP:
        if(key_len == 13)
        {
            os_strlcpy((char *) param->u.crypt.alg, "WEP104",
                       HOSTAP_CRYPT_ALG_NAME_LEN);
        }
        else
        {
            os_strlcpy((char *) param->u.crypt.alg, "WEP40",
                       HOSTAP_CRYPT_ALG_NAME_LEN);
        }
        break;
    case WPA_ALG_TKIP:
        os_strlcpy((char *) param->u.crypt.alg, "TKIP",
                   HOSTAP_CRYPT_ALG_NAME_LEN);
        break;
    case WPA_ALG_CCMP:
        os_strlcpy((char *) param->u.crypt.alg, "CCMP",
                   HOSTAP_CRYPT_ALG_NAME_LEN);
        break;
    default:
        os_free(buf);
        return -1;
    }
    param->u.crypt.flags = set_tx ? HOSTAP_CRYPT_FLAG_SET_TX_KEY : 0;
    param->u.crypt.idx = key_idx;
    param->u.crypt.key_len = key_len;
    param->vif_idx = drv->vif_index;
    
    memcpy((u8 *) (param + 1), key, key_len);

    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }
    os_free(buf);

    return ret;
}


static int hostap_get_seqnum(const char *ifname, void *priv, const u8 *addr,
                             int idx, u8 *seq)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0;

    blen = sizeof(*param) + 32;
    buf = os_zalloc(blen);
    if (buf == NULL)
        return -1;

    param = (struct prism2_hostapd_param *) buf;
    param->cmd = PRISM2_GET_ENCRYPTION;
    if (addr == NULL)
        os_memset(param->sta_addr, 0xff, ETH_ALEN);
    else
        os_memcpy(param->sta_addr, addr, ETH_ALEN);
    param->u.crypt.idx = idx;

    if (hostapd_ioctl(drv, param, blen))
    {
        os_printf("Failed to get encryption.\n");
        ret = -1;
    }
    else
    {
        os_memcpy(seq, param->u.crypt.seq, 8);
    }
    os_free(buf);

    return ret;
}


static int hostap_ioctl_prism2param(void *priv, int param, int value)
{
    struct hostap_driver_data *drv = priv;
    struct iwreq iwr;
    int *i;

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
    i = (int *) iwr.u.name;
    *i++ = param;
    *i++ = value;

    if (ioctl_inet(drv->ioctl_sock, drv->vif_index, PRISM2_IOCTL_PRISM2_PARAM, (unsigned long)&iwr) < 0)
    {
        return -1;
    }

    return 0;
}


static int hostap_set_ieee8021x(void *priv, struct wpa_bss_params *params)
{
    struct hostap_driver_data *drv = priv;
    int enabled = params->enabled;

    /* enable kernel driver support for IEEE 802.1X */
    if (hostap_ioctl_prism2param(drv, PRISM2_PARAM_IEEE_802_1X, enabled))
    {
        os_printf("Could not setup IEEE 802.1X support in kernel driver."
                  "\n");
        return -1;
    }

    if (!enabled)
        return 0;

    /* use host driver implementation of encryption to allow
     * individual keys and passing plaintext EAPOL frames */
    if (hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOST_DECRYPT, 1) ||
            hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOST_ENCRYPT, 1))
    {
        os_printf("Could not setup host-based encryption in kernel "
                  "driver.\n");
        return -1;
    }

    return 0;
}


static int hostap_set_privacy(void *priv, int enabled)
{
    struct hostap_drvier_data *drv = priv;

    return hostap_ioctl_prism2param(drv, PRISM2_PARAM_PRIVACY_INVOKED,
                                    enabled);
}


static int hostap_set_ssid(void *priv, const u8 *buf, int len)
{
    struct hostap_driver_data *drv = priv;
    struct iwreq iwr;

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
    iwr.u.essid.flags = 1; /* SSID active */
    iwr.u.essid.pointer = (c_addr_t) buf;
    iwr.u.essid.length = len + 1;

    if (ioctl_inet(drv->ioctl_sock, drv->vif_index, SIOCSIWESSID, (unsigned long)&iwr) < 0)
    {
        return -1;
    }

    return 0;
}


static int hostap_flush(void *priv)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_FLUSH;
    return hostapd_ioctl(drv, &param, sizeof(param));
}


static int hostap_read_sta_data(void *priv,
                                struct hostap_sta_driver_data *data,
                                const u8 *addr)
{
#ifdef BERRYWANG
    struct hostap_driver_data *drv = priv;
    char buf[1024], line[128], *pos;
    FILE *f;
    unsigned long val;

    memset(data, 0, sizeof(*data));
    snprintf(buf, sizeof(buf), "/proc/net/hostap/%s/" MACSTR,
             drv->iface, MAC2STR(addr));

    f = fopen(buf, "r");
    if (!f)
        return -1;

    /* Need to read proc file with in one piece, so use large enough
     * buffer. */
    setbuffer(f, buf, sizeof(buf));

    while (fgets(line, sizeof(line), f))
    {
        pos = strchr(line, '=');
        if (!pos)
            continue;
        *pos++ = '\0';
        val = strtoul(pos, NULL, 10);
        if (strcmp(line, "rx_packets") == 0)
            data->rx_packets = val;
        else if (strcmp(line, "tx_packets") == 0)
            data->tx_packets = val;
        else if (strcmp(line, "rx_bytes") == 0)
            data->rx_bytes = val;
        else if (strcmp(line, "tx_bytes") == 0)
            data->tx_bytes = val;
    }

    fclose(f);
#endif

    return 0;
}

static int wpa_driver_hostap_init_vif(void *priv, u32 type)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_WPA_INIT_VIF;
    memcpy(param.sta_addr, drv->own_addr, ETH_ALEN);
    param.u.add_if.indx_ptr = &drv->vif_index;
    param.u.add_if.type = type;

    return hapd_intf_add_vif(&param, sizeof(param));
}

static int wpa_driver_hostap_deinit_vif(void *priv)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_WPA_DEINIT_VIF;
    memcpy(param.sta_addr, drv->own_addr, ETH_ALEN);
    param.vif_idx = drv->vif_index;

    return hostapd_ioctl(drv, &param, sizeof(param));
}

static int wpa_driver_hostap_start_apm(void *priv)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_START_APM;
    memcpy(param.sta_addr, drv->own_addr, ETH_ALEN);
    param.vif_idx = drv->vif_index;

    return hostapd_ioctl(drv, &param, sizeof(param));
}

static int wpa_driver_hostap_stop_apm(void *priv)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_STOP_APM;
    memcpy(param.sta_addr, drv->own_addr, ETH_ALEN);
    param.vif_idx = drv->vif_index;

    return hostapd_ioctl(drv, &param, sizeof(param));
}


static int hostap_sta_add(void *priv, struct hostapd_sta_add_params *params)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;
    int tx_supp_rates = 0;
    size_t i;

#define WLAN_RATE_1M BIT(0)
#define WLAN_RATE_2M BIT(1)
#define WLAN_RATE_5M5 BIT(2)
#define WLAN_RATE_11M BIT(3)

    for (i = 0; i < params->supp_rates_len; i++)
    {
        if ((params->supp_rates[i] & 0x7f) == 2)
            tx_supp_rates |= WLAN_RATE_1M;
        if ((params->supp_rates[i] & 0x7f) == 4)
            tx_supp_rates |= WLAN_RATE_2M;
        if ((params->supp_rates[i] & 0x7f) == 11)
            tx_supp_rates |= WLAN_RATE_5M5;
        if ((params->supp_rates[i] & 0x7f) == 22)
            tx_supp_rates |= WLAN_RATE_11M;
    }

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_ADD_STA;
    memcpy(param.sta_addr, params->addr, ETH_ALEN);
    param.u.add_sta.aid = params->aid;
    param.u.add_sta.capability = params->capability;
    param.u.add_sta.tx_supp_rates = tx_supp_rates;
    param.vif_idx = drv->vif_index;

    return hostapd_ioctl(drv, &param, sizeof(param));
}


static int hostap_sta_remove(void *priv, const u8 *addr)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    hostap_sta_set_flags(drv, addr, 0, 0, ~WPA_STA_AUTHORIZED);

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_REMOVE_STA;
    memcpy(param.sta_addr, addr, ETH_ALEN);

    param.vif_idx = drv->vif_index;
    if (hostapd_ioctl(drv, &param, sizeof(param)))
    {
        os_printf("Could not remove station from kernel driver.\n");
        return -1;
    }
    return 0;
}


static int hostap_get_inact_sec(void *priv, const u8 *addr)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_GET_INFO_STA;
    memcpy(param.sta_addr, addr, ETH_ALEN);
    if (hostapd_ioctl(drv, &param, sizeof(param)))
    {
        return -1;
    }

    return param.u.get_info_sta.inactive_sec;
}


static int hostap_sta_clear_stats(void *priv, const u8 *addr)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));
    param.cmd = PRISM2_HOSTAPD_STA_CLEAR_STATS;
    memcpy(param.sta_addr, addr, ETH_ALEN);
    if (hostapd_ioctl(drv, &param, sizeof(param)))
    {
        return -1;
    }

    return 0;
}


static int hostapd_ioctl_set_generic_elem(struct hostap_driver_data *drv)
{
    struct prism2_hostapd_param *param;
    int res;
    size_t blen, elem_len;

    elem_len = drv->generic_ie_len + drv->wps_ie_len;
    blen = PRISM2_HOSTAPD_GENERIC_ELEMENT_HDR_LEN + elem_len;
    if (blen < sizeof(*param))
        blen = sizeof(*param);

    param = os_zalloc(blen);
    if (param == NULL)
        return -1;

    param->cmd = PRISM2_HOSTAPD_SET_GENERIC_ELEMENT;
    param->u.generic_elem.len = elem_len;
    if (drv->generic_ie)
    {
        os_memcpy(param->u.generic_elem.data, drv->generic_ie,
                  drv->generic_ie_len);
    }
    if (drv->wps_ie)
    {
        os_memcpy(&param->u.generic_elem.data[drv->generic_ie_len],
                  drv->wps_ie, drv->wps_ie_len);
    }
    wpa_hexdump(MSG_DEBUG, "hostap: Set generic IE",
                param->u.generic_elem.data, elem_len);
    res = hostapd_ioctl(drv, param, blen);

    os_free(param);

    return res;
}


static int hostap_set_generic_elem(void *priv,
                                   const u8 *elem, size_t elem_len)
{
    struct hostap_driver_data *drv = priv;

    os_free(drv->generic_ie);
    drv->generic_ie = NULL;
    drv->generic_ie_len = 0;
    if (elem)
    {
        drv->generic_ie = os_malloc(elem_len);
        if (drv->generic_ie == NULL)
            return -1;
        os_memcpy(drv->generic_ie, elem, elem_len);
        drv->generic_ie_len = elem_len;
    }

    return hostapd_ioctl_set_generic_elem(drv);
}


static int hostap_set_ap_wps_ie(void *priv, const struct wpabuf *beacon,
                                const struct wpabuf *proberesp,
                                const struct wpabuf *assocresp)
{
    struct hostap_driver_data *drv = priv;

    /*
     * Host AP driver supports only one set of extra IEs, so we need to
     * use the Probe Response IEs also for Beacon frames since they include
     * more information.
     */

    os_free(drv->wps_ie);
    drv->wps_ie = NULL;
    drv->wps_ie_len = 0;
    if (proberesp)
    {
        drv->wps_ie = os_malloc(wpabuf_len(proberesp));
        if (drv->wps_ie == NULL)
            return -1;
        os_memcpy(drv->wps_ie, wpabuf_head(proberesp),
                  wpabuf_len(proberesp));
        drv->wps_ie_len = wpabuf_len(proberesp);
    }

    return hostapd_ioctl_set_generic_elem(drv);
}


static void
hostapd_wireless_event_wireless_custom(struct hostap_driver_data *drv,
                                       char *custom)
{
    wpa_printf(MSG_DEBUG, "Custom wireless event: '%s'", custom);

    if (strncmp(custom, "MLME-MICHAELMICFAILURE.indication", 33) == 0)
    {
        char *pos;
        u8 addr[ETH_ALEN];
        pos = strstr(custom, "addr=");
        if (pos == NULL)
        {
            wpa_printf(MSG_DEBUG,
                       "MLME-MICHAELMICFAILURE.indication "
                       "without sender address ignored");
            return;
        }
        pos += 5;
        if (hwaddr_aton(pos, addr) == 0)
        {
            union wpa_event_data data;
            os_memset(&data, 0, sizeof(data));
            data.michael_mic_failure.unicast = 1;
            data.michael_mic_failure.src = addr;
            wpa_supplicant_event(drv->hapd,
                                 EVENT_MICHAEL_MIC_FAILURE, &data);
        }
        else
        {
            wpa_printf(MSG_DEBUG,
                       "MLME-MICHAELMICFAILURE.indication "
                       "with invalid MAC address");
        }
    }
}


static void hostapd_wireless_event_wireless(struct hostap_driver_data *drv,
        char *data, int len)
{
    struct iw_event iwe_buf, *iwe = &iwe_buf;
    char *pos, *end, *custom, *buf;

    pos = data;
    end = data + len;

    while (pos + IW_EV_LCP_LEN <= end)
    {
        /* Event data may be unaligned, so make a local, aligned copy
         * before processing. */
        memcpy(&iwe_buf, pos, IW_EV_LCP_LEN);
        wpa_printf(MSG_DEBUG, "Wireless event: cmd=0x%x len=%d",
                   iwe->cmd, iwe->len);
        if (iwe->len <= IW_EV_LCP_LEN)
            return;

        custom = pos + IW_EV_POINT_LEN;
        if (drv->we_version > 18 &&
                (iwe->cmd == IWEVMICHAELMICFAILURE ||
                 iwe->cmd == IWEVCUSTOM))
        {
            /* WE-19 removed the pointer from struct iw_point */
            char *dpos = (char *) &iwe_buf.u.data.length;
            int dlen = dpos - (char *) &iwe_buf;
            memcpy(dpos, pos + IW_EV_LCP_LEN,
                   sizeof(struct iw_event) - dlen);
        }
        else
        {
            memcpy(&iwe_buf, pos, sizeof(struct iw_event));
            custom += IW_EV_POINT_OFF;
        }

        switch (iwe->cmd)
        {
        case IWEVCUSTOM:
            if (custom + iwe->u.data.length > end)
                return;
            buf = os_malloc(iwe->u.data.length + 1);
            if (buf == NULL)
                return;
            memcpy(buf, custom, iwe->u.data.length);
            buf[iwe->u.data.length] = '\0';
            hostapd_wireless_event_wireless_custom(drv, buf);

            os_free(buf);
            break;
        }

        pos += iwe->len;
    }
}


static void hostapd_wireless_event_rtm_newlink(void *ctx,
        struct ifinfomsg *ifi,
        u8 *buf, size_t len)
{
    struct hostap_driver_data *drv = ctx;
    int attrlen, rta_len;
    struct rtattr *attr;

    /* TODO: use ifi->ifi_index to filter out wireless events from other
     * interfaces */

    attrlen = len;
    attr = (struct rtattr *) buf;

    rta_len = RTA_ALIGN(sizeof(struct rtattr));
    while (RTA_OK(attr, attrlen))
    {
        if (attr->rta_type == IFLA_WIRELESS)
        {
            hostapd_wireless_event_wireless(
                drv, ((char *) attr) + rta_len,
                attr->rta_len - rta_len);
        }
        attr = RTA_NEXT(attr, attrlen);
    }
}


static int hostap_get_we_version(struct hostap_driver_data *drv)
{
    struct iw_range *range;
    struct iwreq iwr;
    int minlen;
    size_t buflen;

    drv->we_version = 0;

    /*
     * Use larger buffer than struct iw_range in order to allow the
     * structure to grow in the future.
     */
    buflen = sizeof(struct iw_range) + 500;
    range = os_zalloc(buflen);
    if (range == NULL)
        return -1;

    memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
    iwr.u.data.pointer = (c_addr_t) range;
    iwr.u.data.length = buflen;

    minlen = ((char *) &range->enc_capa) - (char *) range +
             sizeof(range->enc_capa);

    if (ioctl_inet(drv->ioctl_sock, drv->vif_index, SIOCGIWRANGE, (unsigned long)&iwr) < 0)
    {
        os_free(range);
        return -1;
    }
    else if (iwr.u.data.length >= minlen &&
             range->we_version_compiled >= 18)
    {
        drv->we_version = range->we_version_compiled;
    }

    os_free(range);
    return 0;
}


static int hostap_wireless_event_init(struct hostap_driver_data *drv)
{
    struct netlink_config *cfg;

    hostap_get_we_version(drv);

    cfg = os_zalloc(sizeof(*cfg));
    if (cfg == NULL)
        return -1;
    cfg->ctx = drv;
    cfg->newlink_cb = hostapd_wireless_event_rtm_newlink;
    drv->netlink = 0;
    if (drv->netlink == NULL)
    {
        os_free(cfg);
    }

    return 0;
}


static void *hostap_init(struct hostapd_data *hapd,
                         struct wpa_init_params *params)
{
    struct hostap_driver_data *drv;
    int ret;

    drv = os_zalloc(sizeof(struct hostap_driver_data));
    if (drv == NULL)
    {
        os_printf("Could not allocate memory for hostapd driver data\n");
        return NULL;
    }

    drv->hapd = hapd;
    drv->ioctl_sock = -1;
    drv->sock = -1;
    os_memcpy(drv->iface, params->ifname, sizeof(drv->iface));
    os_memcpy(drv->own_addr, params->own_addr, ETH_ALEN);

    ret = wpa_driver_hostap_init_vif(drv, NL80211_IFTYPE_AP);
    if(ret || (drv->vif_index == 0xff)) {
        os_printf("Could not found vif indix: %d\n", drv->vif_index);
        os_free(drv);
        return NULL;
    }

    drv->ioctl_sock = fsocket_init(PF_INET, SOCK_DGRAM, drv->vif_index);

    if (hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOSTAPD, 1))
    {
        wpa_printf(MSG_ERROR,
                   "Could not enable hostapd mode for interface %s",
                   drv->iface);
        fsocket_close(drv->ioctl_sock);
        os_free(drv);
        return NULL;
    }

    ret = wpa_driver_hostap_start_apm(drv);
    if(ret) {
        os_printf("wpa_driver_hostap_start_apm failed\n");
        return NULL;
    }

    if (hostap_init_sockets(drv, params->own_addr) ||
            hostap_wireless_event_init(drv))
    {
        fsocket_close(drv->ioctl_sock);
        os_free(drv);
        return NULL;
    }

    return drv;
}


static void hostap_driver_deinit(void *priv)
{
    struct hostap_driver_data *drv = priv;

#ifdef BERRYWANG
    netlink_deinit(drv->netlink);
#endif

    (void) hostap_set_iface_flags(drv, 0);
    (void) hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOSTAPD, 0);
    (void) hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOSTAPD_STA, 0);

	if(wpa_driver_hostap_stop_apm(drv))	
		os_printf("Could not stop apm vif: %d\n", drv->vif_index);		

    if(wpa_driver_hostap_deinit_vif(drv))
        os_printf("Could not remove vif: %d\n", drv->vif_index);

    if (drv->ioctl_sock >= 0)
        fsocket_close(drv->ioctl_sock);

    if (drv->sock >= 0) {
        fsocket_close(drv->sock);
        eloop_unregister_read_sock(drv->sock);
    }

	l2_packet_deinit(drv->sock_xmit);

    os_free(drv->generic_ie);
    os_free(drv->wps_ie);

    os_free(drv);
}


static int hostap_sta_deauth(void *priv, const u8 *own_addr, const u8 *addr,
                             int reason)
{
    struct hostap_driver_data *drv = priv;
    struct ieee80211_mgmt mgmt;

    if (is_broadcast_ether_addr(addr))
    {
        /*
         * New Prism2.5/3 STA firmware versions seem to have issues
         * with this broadcast deauth frame. This gets the firmware in
         * odd state where nothing works correctly, so let's skip
         * sending this for the hostap driver.
         */
        return 0;
    }

    memset(&mgmt, 0, sizeof(mgmt));
    mgmt.frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
                                      WLAN_FC_STYPE_DEAUTH);
    memcpy(mgmt.da, addr, ETH_ALEN);
    memcpy(mgmt.sa, own_addr, ETH_ALEN);
    memcpy(mgmt.bssid, own_addr, ETH_ALEN);
    mgmt.u.deauth.reason_code = host_to_le16(reason);
    return hostap_send_mlme(drv, (u8 *) &mgmt, IEEE80211_HDRLEN +
                            sizeof(mgmt.u.deauth), 0, 0);
}


int hostap_channel_switch(void *priv, struct csa_settings *settings)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    
    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        return -1;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_CHANNEL_SWITCH;
    param->vif_idx = drv->vif_index;
    param->u.chan_switch.freq = settings->freq_params.freq;
    param->u.chan_switch.csa_cnt = settings->cs_count;
    param->u.chan_switch.settings = settings;
    
    if (hostapd_ioctl(drv, param, blen))
    {
    	os_free(buf);
        return -1;
    }

	eloop_register_signal(SIGCSA, wpa_driver_csa_sig_handler, drv->hapd);
	
    param = (struct prism2_hostapd_param *)buf;
    param->vif_idx = drv->vif_index;
    param->cmd = PRISM2_HOSTAPD_REG_CSA_CALLBACK;
    param->u.reg_csa_event.cb = wpa_handler_signal;
    param->u.reg_csa_event.arg = (void *)SIGCSA;
    if (hostapd_ioctl(drv, param, blen))
    {
    	os_free(buf);
        return -1;
    }
    
    os_free(buf);

    return 0;
}

static int hostap_set_freq(void *priv, struct hostapd_freq_params *freq)
{
    struct hostap_driver_data *drv = priv;
    struct iwreq iwr;

    os_memset(&iwr, 0, sizeof(iwr));
    os_strlcpy(iwr.ifr_name, drv->iface, IFNAMSIZ);
    iwr.u.freq.m = freq->channel;
    iwr.u.freq.e = 0;

    if (ioctl_inet(drv->ioctl_sock, drv->vif_index, SIOCSIWFREQ, (unsigned long)&iwr) < 0)
    {
        wpa_printf(MSG_ERROR, "ioctl[SIOCSIWFREQ]: %s",
                   strerror(errno));
        return -1;
    }

    return 0;
}

static int hostap_sta_disassoc(void *priv, const u8 *own_addr, const u8 *addr,
                               int reason)
{
    struct hostap_driver_data *drv = priv;
    struct ieee80211_mgmt mgmt;

    memset(&mgmt, 0, sizeof(mgmt));
    mgmt.frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
                                      WLAN_FC_STYPE_DISASSOC);
    memcpy(mgmt.da, addr, ETH_ALEN);
    memcpy(mgmt.sa, own_addr, ETH_ALEN);
    memcpy(mgmt.bssid, own_addr, ETH_ALEN);
    mgmt.u.disassoc.reason_code = host_to_le16(reason);
    
    return  hostap_send_mlme(drv, (u8 *) &mgmt, IEEE80211_HDRLEN +
                             sizeof(mgmt.u.disassoc), 0, 0);
}

static struct hostapd_hw_modes *hostap_get_hw_feature_data(void *priv,
        u16 *num_modes,
        u16 *flags)
{
    struct hostapd_hw_modes *mode;
    int i, clen, rlen;
    const short chan2freq[14] =
    {
        2412, 2417, 2422, 2427, 2432, 2437, 2442,
        2447, 2452, 2457, 2462, 2467, 2472, 2484
    };

    mode = os_zalloc(sizeof(struct hostapd_hw_modes));
    if (mode == NULL)
        return NULL;

    *num_modes = 1;
    *flags = 0;

    mode->num_channels = 14;

#if CFG_SUPPORT_80211G
    mode->mode = HOSTAPD_MODE_IEEE80211G;
    mode->num_rates    = 12;
#else
    mode->mode = HOSTAPD_MODE_IEEE80211B;
    mode->num_rates    = 4;
#endif

    clen = mode->num_channels *sizeof(struct hostapd_channel_data);
    rlen = mode->num_rates *sizeof(int);

    mode->channels = os_zalloc(clen);
    mode->rates = os_zalloc(rlen);
    if (mode->channels == NULL || mode->rates == NULL)
    {
        os_free(mode->channels);
        os_free(mode->rates);
        os_free(mode);
        return NULL;
    }

    for (i = 0; i < 14; i++)
    {
        mode->channels[i].chan = i + 1;
        mode->channels[i].freq = chan2freq[i];
    }

    mode->rates[0] = 10;
    mode->rates[1] = 20;
    mode->rates[2] = 55;
    mode->rates[3] = 110;

    mode->rates[4] = 60;
    mode->rates[5] = 90;
    mode->rates[6] = 120;
    mode->rates[7] = 180;
    mode->rates[8] = 240;
    mode->rates[9] = 360;
    mode->rates[10] = 480;
    mode->rates[11] = 540;

    return mode;
}


int hostap_set_ap(void *priv, struct wpa_driver_ap_params *params)
{
    struct hostap_driver_data *drv = priv;
    char *pos;
    char *beacon;
    int bcn_len;
    int ret;
    struct prism2_hostapd_param param;

    memset(&param, 0, sizeof(param));

    bcn_len = params->head_len + params->tail_len + WLAN_EID_TIM_LEN;
    beacon = (char *)os_malloc(bcn_len);
    if(0 == beacon)
    {
        return 0;
    }

    pos = beacon;
    os_memcpy(pos, params->head, params->head_len);
    pos = pos + params->head_len;

    *pos++ = WLAN_EID_TIM;
    *pos++ = 4;
    *pos++ = 0;
    *pos++ = 2;
    *pos++ = 0;
    *pos++ = 0;

    os_memcpy(pos, params->tail, params->tail_len);
    pos = pos + params->tail_len;

    param.cmd = PRISM2_HOSTAPD_SET_AP_BCN;
    param.u.bcn_change.beacon = beacon;
    param.u.bcn_change.bcn_len = bcn_len;
    param.u.bcn_change.head_len = params->head_len;
    param.u.bcn_change.tim_len = 6;
    param.vif_idx = drv->vif_index;

    ret = hostapd_ioctl(drv, &param, sizeof(param));

    os_free(beacon);
    return ret;
}

static void wpa_driver_hostap_poll_client(void *priv, const u8 *own_addr,
        const u8 *addr, int qos)
{
    struct ieee80211_hdr hdr;

    os_memset(&hdr, 0, sizeof(hdr));

    /*
     * WLAN_FC_STYPE_NULLFUNC would be more appropriate,
     * but it is apparently not retried so TX Exc events
     * are not received for it.
     * This is the reason the driver overrides the default
     * handling.
     */
    hdr.frame_control = IEEE80211_FC(WLAN_FC_TYPE_DATA,
                                     WLAN_FC_STYPE_DATA);

    hdr.frame_control |= host_to_le16(WLAN_FC_FROMDS);

    os_memcpy(hdr.IEEE80211_DA_FROMDS, addr, ETH_ALEN);
    os_memcpy(hdr.IEEE80211_BSSID_FROMDS, own_addr, ETH_ALEN);
    os_memcpy(hdr.IEEE80211_SA_FROMDS, own_addr, ETH_ALEN);

    hostap_send_mlme(priv, (u8 *)&hdr, sizeof(hdr), 0, 0);
}

void wpa_driver_scan_sig_handler(int sig, void *signal_ctx)
{
    os_printf("wpa_driver_scan_cb\r\n");
    wpa_supplicant_event_sta(signal_ctx, EVENT_SCAN_RESULTS, NULL);
}

void wpa_driver_assoc_sig_handler(int sig, void *signal_ctx)
{
    os_printf("wpa_driver_assoc_cb\r\n");
    wpa_supplicant_event_sta(signal_ctx, EVENT_ASSOC, NULL);
}

void wpa_driver_disassoc_sig_handler(int sig, void *signal_ctx)
{
    os_printf("wpa_driver_deassoc_cb\r\n");
    wpa_supplicant_event_sta(signal_ctx, EVENT_DISASSOC, NULL);
}

void wpa_driver_deauth_sig_handler(int sig, void *signal_ctx)
{
    os_printf("wpa_driver_deauth_cb\r\n");
    wpa_supplicant_event_sta(signal_ctx, EVENT_DEAUTH, NULL);
}

void wpa_driver_csa_sig_handler(int sig, void *signal_ctx)
{
	union wpa_event_data data;
    
    data.ch_switch.freq = bk_wlan_ap_get_frequency();
    data.ch_switch.ht_enabled = 1;
    data.ch_switch.ch_offset = 0;
    data.ch_switch.ch_width = 20;
    data.ch_switch.cf1 = bk_wlan_ap_get_frequency();
    data.ch_switch.cf2 = 0;
    
    wpa_supplicant_event(signal_ctx, EVENT_CH_SWITCH, &data);
}

static void *wpa_driver_init(void *ctx, const char *ifname)
{
    struct hostap_driver_data *drv;
    int ret;

    drv = os_zalloc(sizeof(struct hostap_driver_data));
    if (drv == NULL)
    {
        return NULL;
    }

    drv->wpa_s = ctx;
    os_memcpy(drv->iface, ifname, sizeof(drv->iface));
	wifi_get_mac_address((char *)drv->own_addr, CONFIG_ROLE_STA);

    ret = wpa_driver_hostap_init_vif(drv, NL80211_IFTYPE_STATION);
    if(ret || (drv->vif_index == 0xff)) {
        os_printf("Could not found vif indix: %d\n", drv->vif_index);
        os_free(drv);
        return NULL;
    }
    
    drv->ioctl_sock = fsocket_init(PF_INET, SOCK_DGRAM, drv->vif_index);

    return drv;
}

static void wpa_driver_deinit(void *priv)
{
    struct hostap_driver_data *drv = priv;

    (void) hostap_set_iface_flags(drv, 0);
    (void) hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOSTAPD, 0);
    (void) hostap_ioctl_prism2param(drv, PRISM2_PARAM_HOSTAPD_STA, 0);

    if(wpa_driver_hostap_deinit_vif(drv))
        os_printf("Could not remove vif: %d\n", drv->vif_index);

    if (drv->ioctl_sock >= 0)
        fsocket_close(drv->ioctl_sock);

    if (drv->sock >= 0)
        fsocket_close(drv->sock);

    os_free(drv->generic_ie);
    os_free(drv->wps_ie);

    os_free(drv);
}

void wpa_handler_signal(void *arg, u8 vif_idx)
{
	int sig = (int)arg;
	
	eloop_handle_signal(sig);

    wpa_hostapd_queue_poll((uint32_t)vif_idx);
}

int wpa_driver_scan2(void *priv, struct wpa_driver_scan_params *params)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0, i;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
        return -1;

	eloop_register_signal(SIGSCAN, wpa_driver_scan_sig_handler, drv->wpa_s);
	
    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_REG_SCAN_CALLBACK;
    param->vif_idx = drv->vif_index;
    param->u.reg_scan_cfm.cb = wpa_handler_signal;
    param->u.reg_scan_cfm.arg = (void *)SIGSCAN;
    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_SCAN_REQ;
    param->u.scan_req.ssids_num = MIN(SCAN_SSID_MAX, params->num_ssids);
    param->vif_idx = drv->vif_index;

    for(i = 0; i < param->u.scan_req.ssids_num; i++)
    {
        param->u.scan_req.ssids[i].ssid_len = params->ssids[i].ssid_len;
        os_memcpy(param->u.scan_req.ssids[i].ssid, params->ssids[i].ssid, param->u.scan_req.ssids[i].ssid_len);
    }

    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    os_free(buf);

    return ret;
}

struct wpa_scan_results *wpa_driver_get_scan_results2(void *priv)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    struct wpa_scan_results *results = NULL;
    u8 *buf;
    size_t blen;
    int ret = 0;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        goto fail_result;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_GET_SCAN_RESULT;
    param->vif_idx = drv->vif_index;

    results = os_zalloc(sizeof(*results));
    if(results == NULL)
    {
        goto fail_result;
    }
    results->res = os_calloc(MAX_BSS_LIST, sizeof(struct wpa_scan_res *));
    if (results->res == NULL)
    {
        goto fail_result;
    }
	
    param->u.scan_rst = results;

    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    if(!ret)
    {
    	os_free(buf);
        return results;
    }

fail_result:
	if(results && results->res)
	{
		os_free(results->res);
		results->res = NULL;
	}

	if(results)
	{
		os_free(results);
		results = NULL;
	}

	if(buf)
	{
		os_free(buf);
		buf = NULL;
	}
	
    return NULL;
}

int wpa_driver_associate(void *priv, struct wpa_driver_associate_params *params)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        return -1;
    }
	
	eloop_register_signal(SIGASSOC, wpa_driver_assoc_sig_handler, drv->wpa_s);
	
    param = (struct prism2_hostapd_param *)buf;
    param->vif_idx = drv->vif_index;
    param->cmd = PRISM2_HOSTAPD_REG_ASSOC_CALLBACK;
    param->u.reg_assoc_cfm.cb = wpa_handler_signal;
    param->u.reg_assoc_cfm.arg = (void *)SIGASSOC;
    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    os_printf("wpa_driver_associate\r\n");
    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_ASSOC_REQ;
    param->vif_idx = drv->vif_index;
	if(params->auth_alg & WPA_AUTH_ALG_OPEN)
	{
		param->u.assoc_req.auth_alg = HOSTAP_AUTH_OPEN;
	}
	else
	{
		param->u.assoc_req.auth_alg = HOSTAP_AUTH_SHARED;
	}
    os_memcpy(param->u.assoc_req.bssid, params->bssid, ETH_ALEN);
    param->u.assoc_req.ssid_len = params->ssid_len;
    os_memcpy(param->u.assoc_req.ssid, params->ssid, param->u.assoc_req.ssid_len);
    param->u.assoc_req.proto = params->wpa_proto;
    param->u.assoc_req.ie_len = params->wpa_ie_len;
    os_memcpy((u8 *)param->u.assoc_req.ie_buf, params->wpa_ie, param->u.assoc_req.ie_len);

    if(hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }
    else
    {
        param->cmd = PRISM2_HOSTAPD_ASSOC_ACK;
        param->vif_idx = drv->vif_index;
        hostapd_ioctl(drv, param, blen);
    }

	eloop_register_signal(SIGDISASSOC, wpa_driver_disassoc_sig_handler, drv->wpa_s);

    param->cmd = PRISM2_HOSTAPD_REG_DISASSOC_CALLBACK;
    param->vif_idx = drv->vif_index;
    param->u.reg_disassoc_evt.cb = wpa_handler_signal;
    param->u.reg_disassoc_evt.arg = (void *)SIGDISASSOC;
    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

	eloop_register_signal(SIGDEAUTH, wpa_driver_deauth_sig_handler, drv->wpa_s);

    param->cmd = PRISM2_HOSTAPD_REG_DEAUTH_CALLBACK;
    param->vif_idx = drv->vif_index;
    param->u.reg_deauth_evt.cb = wpa_handler_signal;
    param->u.reg_deauth_evt.arg = (void *)SIGDEAUTH;
    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    os_free(buf);

    return ret;
}

int wpa_driver_get_bssid(void *priv, u8 *bssid)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        return -1;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_GET_BSS_INFO;
    param->vif_idx = drv->vif_index;
    if (hostapd_ioctl(drv, param, blen))
    {
    	os_free(buf);
        return -1;
    }
    os_memcpy(bssid, param->u.bss_info.bssid, ETH_ALEN);
    os_free(buf);

    return 0;
}

int wpa_driver_get_ssid(void *priv, u8 *ssid)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf, len;
    size_t blen;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        return -1;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_GET_BSS_INFO;
    param->vif_idx = drv->vif_index;
    if (hostapd_ioctl(drv, param, blen))
    {
    	os_free(buf);
        return -1;
    }

	len = MIN(SSID_MAX_LEN, os_strlen(param->u.bss_info.ssid));
    os_memcpy(ssid, param->u.bss_info.ssid, len);
    os_free(buf);

    return len;
}

const u8 *wpa_driver_get_mac(void *priv)
{
    struct hostap_driver_data *drv = priv;

    return drv->own_addr;
}

int wpa_driver_get_capa(void *priv, struct wpa_driver_capa *capa)
{
    os_memset(capa, 0, sizeof(*capa));

    capa->max_scan_ssids = 1;
    capa->enc = WPA_DRIVER_CAPA_ENC_WEP40
                | WPA_DRIVER_CAPA_ENC_WEP104
                | WPA_DRIVER_CAPA_ENC_TKIP
                | WPA_DRIVER_CAPA_ENC_CCMP;
    capa->flags = WPA_DRIVER_FLAGS_AP_CSA;

    return 0;
}

int wpa_driver_set_operstate(void *priv, int state)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0;

    if(state == 0)
        return 0;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
        return -1;

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_REG_APP_START;
    param->vif_idx = drv->vif_index;
    if(hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }

    os_free(buf);

    return ret;
}

int wpa_driver_deauthenticate(void *priv, const u8 *addr, int reason_code)
{
    struct hostap_driver_data *drv = priv;
    struct prism2_hostapd_param *param;
    u8 *buf;
    size_t blen;
    int ret = 0;

    blen = sizeof(*param);
    buf = os_zalloc(blen);
    if(buf == NULL)
    {
        return -1;
    }

    param = (struct prism2_hostapd_param *)buf;
    param->cmd = PRISM2_HOSTAPD_DISCONN_REQ;
    param->vif_idx = drv->vif_index;
    param->u.disconnect_req.reason = reason_code;
    if (hostapd_ioctl(drv, param, blen))
    {
        ret = -1;
    }
	
    os_free(buf);

    return ret;
}

const struct wpa_driver_ops wpa_driver_hostap_ops =
{
    .name = "hostap_beken",
    .desc = "Host AP driver",
    
    .hapd_init = hostap_init,
    .hapd_deinit = hostap_driver_deinit,
    .set_ieee8021x = hostap_set_ieee8021x,
    .set_privacy = hostap_set_privacy,
    .get_seqnum = hostap_get_seqnum,
    .flush = hostap_flush,
    .set_generic_elem = hostap_set_generic_elem,
    .read_sta_data = hostap_read_sta_data,
    .hapd_send_eapol = hostap_send_eapol,
    .sta_set_flags = hostap_sta_set_flags,
    .sta_deauth = hostap_sta_deauth,
    .sta_disassoc = hostap_sta_disassoc,
    .sta_remove = hostap_sta_remove,
    .hapd_set_ssid = hostap_set_ssid,
    .send_mlme = hostap_send_mlme,
    .sta_add = hostap_sta_add,
    .get_inact_sec = hostap_get_inact_sec,
    .sta_clear_stats = hostap_sta_clear_stats,
    .get_hw_feature_data = hostap_get_hw_feature_data,
    .set_ap_wps_ie = hostap_set_ap_wps_ie,
    .set_freq = hostap_set_freq,
    .switch_channel = hostap_channel_switch,

    .set_key = wpa_driver_hostap_set_key,
    .poll_client = wpa_driver_hostap_poll_client,
    .set_ap = hostap_set_ap,
    .init = wpa_driver_init,
    .deinit = wpa_driver_deinit,
    .scan2 = wpa_driver_scan2,
    .get_scan_results2 = wpa_driver_get_scan_results2,
    .deauthenticate = wpa_driver_deauthenticate,
    .associate = wpa_driver_associate,
    .get_bssid = wpa_driver_get_bssid,
    .get_ssid = wpa_driver_get_ssid,
    .get_mac_addr = wpa_driver_get_mac,
    .get_capa = wpa_driver_get_capa,
    .set_operstate = wpa_driver_set_operstate,    
};

#if CFG_ENABLE_WPA_LOG
void wpa_dbg(void *ctx, int level, const char *fmt, ...)
{
	va_list ap;
	char *buf;
	int buflen;
	int len;

	va_start(ap, fmt);
	buflen = vsnprintf(NULL, 0, fmt, ap) + 1;
	va_end(ap);

	buf = os_malloc(buflen);
	if (buf == NULL) {
		bk_printf("wpa_msg: Failed to allocate message "
			   "buffer");
		return;
	}
	va_start(ap, fmt);

	len = vsnprintf(buf, buflen, fmt, ap);
	va_end(ap);
	bk_send_string(1, buf);
	os_free(buf);
	
	bk_printf("\r\n");	
}
#endif // CFG_ENABLE_WPA_LOG

