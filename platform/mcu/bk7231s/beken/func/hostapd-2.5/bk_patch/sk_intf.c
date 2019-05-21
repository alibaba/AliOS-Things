#include "include.h"
#include "sk_intf.h"
#include "main_none.h"
#include "eloop.h"

#include "param_config.h"
#include "uart_pub.h"
#include "rw_pub.h"

extern void wpa_hostapd_queue_poll(uint32_t param);

SOCKET mgmt_get_socket_num(u8 vif_idx)
{
	return (SK_INTF_MGMT_SOCKET_NUM + vif_idx);
}

SOCKET ioctl_get_socket_num(u8 vif_idx)
{
	return (SK_INTF_IOCTL_SOCKET_NUM + vif_idx);
}

SOCKET data_get_socket_num(u8 vif_idx)
{
	return (SK_INTF_DATA_SOCKET_NUM + vif_idx);
}

int ke_mgmt_packet_tx(unsigned char *buf, int len, int flag)
{
	int ret;
	SOCKET sk = mgmt_get_socket_num(flag);

	ret = ke_sk_send(sk, buf, len, flag);

    wpa_hostapd_queue_poll((uint32_t)flag);
	
	return ret;
}

int ke_mgmt_packet_rx(unsigned char *buf, int len, int flag)
{
	SOCKET sk = mgmt_get_socket_num(flag);
	
	return ke_sk_recv(sk, buf, len, flag);
}

int ke_mgmt_peek_txed_next_payload_size(int flag)
{
	SOCKET sk = mgmt_get_socket_num(flag);

	return ke_sk_send_peek_next_payload_size(sk);
}

int ke_mgmt_peek_rxed_next_payload_size(int flag)
{
	SOCKET sk = mgmt_get_socket_num(flag);

	return ke_sk_recv_peek_next_payload_size(sk);
}

int ke_l2_packet_tx(unsigned char *buf, int len, int flag)
{
	int ret;
	SOCKET sk = data_get_socket_num(flag);
	
	ret = ke_sk_send(sk, buf, len, flag);

    wpa_hostapd_queue_poll((uint32_t)flag);
	
	return ret;
}

int ke_l2_packet_rx(unsigned char *buf, int len, int flag)
{
	SOCKET sk = data_get_socket_num(flag);
	
	return ke_sk_recv(sk, buf, len, flag);
}

int ke_data_peek_txed_next_payload_size(int flag)
{
	SOCKET sk = data_get_socket_num(flag);

	return ke_sk_send_peek_next_payload_size(sk);
}

int ke_data_peek_rxed_next_payload_size(int flag)
{
	SOCKET sk = data_get_socket_num(flag);

	return ke_sk_recv_peek_next_payload_size(sk);
}

int ws_mgmt_peek_rxed_next_payload_size(int flag)
{
	SOCKET sk = mgmt_get_socket_num(flag);

	return fsocket_peek_recv_next_payload_size(sk);
}

int ws_get_mgmt_packet(unsigned char *buf, int len, int flag)
{
	SOCKET sk;

	sk = mgmt_get_socket_num(flag);
	
	return fsocket_recv(sk, buf, len, flag);
}

int ws_data_peek_rxed_next_payload_size(int flag)
{
	SOCKET sk = data_get_socket_num(flag);

	return fsocket_peek_recv_next_payload_size(sk);
}

int ws_get_data_packet(unsigned char *buf, int len, int flag)
{
	SOCKET sk;

	sk = data_get_socket_num(flag);
	
	return fsocket_recv(sk, buf, len, flag);
}

// eof

