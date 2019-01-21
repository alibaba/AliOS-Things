/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IMPORT_UDP_H__
#define __IMPORT_UDP_H__

/**
 * @brief Establish a UDP connection.
 *
 * @param [in] host: @n Specify the hostname(IP) of the UDP server
 * @param [in] port: @n Specify the UDP port of UDP server
 *
 * @retval  < 0 : Fail.
 * @retval >= 0 : Success, the value is handle of this UDP connection.
 * @see None.
 */
DLL_HAL_API intptr_t HAL_UDP_create(_IN_ char *host, _IN_ unsigned short port);

/**
 * @brief Destroy the specific UDP connection.
 *
 * @param [in] p_socket: @n Specify the UDP connection by handle.
 * @return None.
 * @see None .
 */
DLL_HAL_API void HAL_UDP_close(_IN_ intptr_t p_socket);

/**
 * @brief Write data into the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a connection.
 * @param [in] p_data @n A pointer to a buffer containing the data to be transmitted.
 * @param [in] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.

 * @retval          < 0 : UDP connection error occur.
 * @retval [0,datalen ] : The number of bytes sent.
 * @see None.
 */
DLL_HAL_API int HAL_UDP_write(
            _IN_ intptr_t p_socket,
            _IN_ const unsigned char *p_data,
            _IN_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection by blocked
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [in] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @return
 *
 * @retval < 0 : UDP connect error occur.
 * @retval = 0 : End of file.
 * @retval > 0 : The number of byte read.
 * @see None.
 */
DLL_HAL_API int HAL_UDP_read(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen);

/**
 * @brief Read data from the specific UDP connection with timeout parameter.
 *        The API will return immediately if 'datalen' be received from the specific UDP connection.
 *
 * @param [in] p_socket @n A descriptor identifying a UDP connection.
 * @param [out] p_data @n A pointer to a buffer to receive incoming data.
 * @param [out] datalen @n The length, in bytes, of the data pointed to by the 'p_data' parameter.
 * @param [in] timeouf_ms @n Specify the timeout value in millisecond. In other words, the API block timeout_ms millisecond maximumly.
 *
 * @retval          -4 : UDP connect error occur.
 * @retval          -3 : The  call  was interrupted by a signal before any data was read.
 * @retval          -2 : No any data be received in 'timeout_ms' timeout period.
 * @retval          -1 : Invalid parameter.
 * @retval           0 : End of file.
 * @retval (0,datalen] : The number of byte read.
 * @see None.
 */
DLL_HAL_API int HAL_UDP_readTimeout(
            _IN_ intptr_t p_socket,
            _OU_ unsigned char *p_data,
            _OU_ unsigned int datalen,
            _IN_ unsigned int timeout_ms);

/** @} */ /* end of platform_network */
/** @} */ /* end of platform */


/**
 * @brief   创建一个本地的UDP socket, 但并不发起任何网络交互
 *
 * @param   host : UDP的源地址, 如果不指定地址，设为 NULL
 *          port : UDP的源端口
 *
 * @retval  -1 : 创建失败
 * @retval  其它 : 创建成功, 返回值是UDP socket的句柄
 */
DLL_HAL_API intptr_t HAL_UDP_create_without_connect(_IN_ const char *host, _IN_ unsigned short port);

/**
 * @brief   设置UDP socket的目的地址和目的端口
 *
 * @param   host :  UDP的目的地址
 *          port : UDP的目的端口
 *
 * @retval  -1 : 失败
 * @retval  0 : 设置成功
 */
DLL_HAL_API int HAL_UDP_connect(_IN_ intptr_t sockfd,
                                _IN_ const char *host,
                                _IN_ unsigned short port);


/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 *
 * @param   sockfd : UDP socket的句柄
 * @param   p_remote : 目标网络地址结构体的首地址
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_UDP_sendto(_IN_ intptr_t sockfd,
                               _IN_ const NetworkAddr *p_remote,
                               _IN_ const unsigned char *p_data,
                               _IN_ unsigned int datalen,
                               _IN_ unsigned int timeout_ms);

/**
 * @brief   在指定的UDP socket上发送指定缓冲区的指定长度, 阻塞时间不超过指定时长, 且指定长度若发送完需提前返回
 * @param   sockfd : UDP socket的句柄
 * @param   p_data : 被发送的缓冲区起始地址
 * @param   datalen: 被发送的数据长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 发送过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功发送
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功发送的数据长度, 单位是字节(Byte)
 *
 * @note    调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
 */
DLL_HAL_API int HAL_UDP_send(_IN_ intptr_t sockfd,
                             _IN_ const unsigned char *p_data,
                             _IN_ unsigned int datalen,
                             _IN_ unsigned int timeout_ms);

/**
 * @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
 *
 * @param   fd : UDP socket的句柄
 * @param   p_remote : 存放源网络地址的结构体首地址
 * @param   p_data : 存放被接收数据的缓冲区起始地址
 * @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
 * @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
 *
 * @retval  < 0 : 接收过程中出现错误或异常
 * @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
 * @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
 */
DLL_HAL_API int HAL_UDP_recvfrom(_IN_ intptr_t sockfd,
                                 _OU_ NetworkAddr *p_remote,
                                 _OU_ unsigned char *p_data,
                                 _IN_ unsigned int datalen,
                                 _IN_ unsigned int timeout_ms);

/**
* @brief   从指定的UDP句柄接收指定长度数据到缓冲区, 阻塞时间不超过指定时长, 且指定长度若接收完需提前返回, 源地址保存在出参中
*          调用该接口之前需要调用HAL_UDP_connect设置好目的地址和端口。
* @param   fd : UDP socket的句柄
* @param   p_data : 存放被接收数据的缓冲区起始地址
* @param   datalen : 接收并存放到缓冲区中数据的最大长度, 单位是字节(Byte)
* @param   timeout_ms : 可能阻塞的最大时间长度, 单位是毫秒
*
* @retval  < 0 : 接收过程中出现错误或异常
* @retval  0 : 在指定的'timeout_ms'时间间隔内, 没有任何数据被成功接收
* @retval  (0, len] : 在指定的'timeout_ms'时间间隔内, 被成功接收的数据长度, 单位是字节(Byte)
*/
DLL_HAL_API int HAL_UDP_recv(_IN_ intptr_t sockfd,
                             _OU_ unsigned char *p_data,
                             _IN_ unsigned int datalen,
                             _IN_ unsigned int timeout_ms);


/**
 * @brief   在指定的UDP socket上发送加入组播组的请求
 *
 * @param   sockfd : 指定用来发送组播请求的UDP socket
 * @param   p_group : 指定需要加入的组播组名字
 * @retval  < 0 : 发送过程中出现异常或失败
 * @retval  0 : 发送成功
 */
DLL_HAL_API int HAL_UDP_joinmulticast(_IN_ intptr_t sockfd,
                                      _IN_ char *p_group);

/**
 * @brief   绑定UDP socket到指定接口，只接收来自该接口的数据包
 *
 * @param   fd : 指定用来绑定的UDP socket
 * @param   ifname : 指定用来绑定socket的网络接口名字
 *
 * @retval  < 0 : 绑定异常或失败
 * @retval  0 : 发送成功
 */
DLL_HAL_API int HAL_UDP_bindtodevice(_IN_ intptr_t fd,
                                     _IN_ const char *ifname);

/**
 * @brief   销毁指定的UDP socket, 回收资源
 *
 * @param   sockfd : 将要关闭并销毁的UDP socket
 *
 * @return  操作的结果
 * @retval  < 0 : 操作失败
 * @retval  0 : 操作成功
 */
DLL_HAL_API int HAL_UDP_close_without_connect(_IN_ intptr_t sockfd);

#endif
