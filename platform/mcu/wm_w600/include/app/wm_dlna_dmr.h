/**
 * @file    wm_dlna_dmr.h
 *
 * @brief   DLNA DMR API
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM__DLNA_DMR_H_
#define __WM__DLNA_DMR_H_

enum dmr_control_type
{
	CONTROL_GET = 0,
	CONTROL_SET
};

/**
 * @brief          This Function prototype for action callback functions
 *
 * @param[in]      None    
 *
 * @return         None
 *
 * @note           None
 */
typedef void (*dmr_action_callback)(void);

/**
 * @brief          This Function prototype for control callback functions
 *
 * @param[in]      type       to get or set control
 * @param[in]      channel    the channel to control
 * @param[in]      value      the value of getting or wanting to set
 *
 * @return         None
 *
 * @note           None
 */
typedef void (*dmr_control_callback)(enum dmr_control_type type, const char* channel, int* value);

/**
 * @brief          This Function prototype for getting the playing progress
 *
 * @param[in]      totlen    The total length of the playing media
 *
 * @returns		   the progress(0.0 ~ 1.0) of having been played
 *
 * @note           None
 */
typedef float (*dmr_play_progress_callback)(int totlen);

/**
 * @brief          This Function prototype for downloading media data finis-
                   hed
 *
 * @param[in]      buf       The downloaded data this time 
 * @param[in]      datalen   The length of buffer data 
 *
 * @retval         None
 *
 * @note           None
 */
typedef void (*dmr_download_finish_callback)(char * buf, int datalen);

/**
 * @defgroup APP_APIs APP APIs
 * @brief APP APIs
 */

/**
 * @addtogroup APP_APIs
 * @{
 */

/**
 * @defgroup DLNA_APIs DLNA APIs
 * @brief DLNA DMR APIs
 */

/**
 * @addtogroup DLNA_APIs
 * @{
 */

/**
 * @brief          This function is used to register play callback which would
                   be called while starting to play
 *
 * @param[in]      callback    the play callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_play_callback(dmr_action_callback callback);

/**
 * @brief          This function is used to register stop callback which would
                   be called while to stop
 *
 * @param[in]      callback    the stop callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_stop_callback(dmr_action_callback callback);

/**
 * @brief          This function is used to register seek callback which would
                   be called while to seek
 *
 * @param[in]      callback    the seek callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_seek_callback(dmr_action_callback callback);

/**
 * @brief          This function is used to register pause callback which
                   would be called while to pause
 *
 * @param[in]      callback    the pause callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_pause_callback(dmr_action_callback callback);

/**
 * @brief          This function is used to register play progress callback
                   which would be called while to get play progress
 *
 * @param[in]      callback    the play progress callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_play_progress_callback(dmr_play_progress_callback callback);

/**
 * @brief          This function is used to register mute control callback
                   which would be called while to get or set mute control
 *
 * @param[in]      callback    the mute control callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_mute_callback(dmr_control_callback callback);

/**
 * @brief          This function is used to register volume control callback
                   which would be called while to get or set volume control
 *
 * @param[in]      callback    the volume control callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_volume_callback(dmr_control_callback callback);

/**
 * @brief          This function is used to register volumedb control callb-
                   ack which would be called while to get or set volumedb
                   control
 *
 * @param[in]      callback    the volumedb control callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_volumedb_callback(dmr_control_callback callback);

/**
 * @brief          This function is used to register loudness control callb-
                   ack which would be called while to get or set loudness
                   control
 *
 * @param[in]      callback    the loudness control callback
 *
 * @return         None
 *
 * @note           None
 */
void tls_dmr_set_loudness_callback(dmr_control_callback callback);

/**
 * @brief          This function is used to initialize the media render
 *
 * @param[in]      *friendly_name    friendly name of the media render
 * @param[in]      *uuid             uuid of the media render
 *
 * @retval         0     success
 * @retval         minus failed
 *
 * @note           None
 */
int tls_dmr_init(const char *friendly_name, const char *uuid);

/**
 * @brief          This function is uesd to destroy  the media render
 *
 * @param[in]      None
 *
 * @retval         0     success
 * @retval         minus failed
 *
 * @note           None
 */
int tls_dmr_destroy(void);

/**
 * @brief          This function is used to trigger to download data from media
                   server, the download_callback would be called when
                   downloading finished
 *
 * @param[in]      dest                Destination address to download data
 * @param[in]      downsize            download size of this time
 * @param[in]      download_callback   download finished callback
 *
 * @retval         0     success
 * @retval         minus failed
 *
 * @note           None
 */
int tls_dmr_download_data(char * dest, int downsize, dmr_download_finish_callback download_callback);

/**
 * @}
 */

/**
 * @}
 */

#endif /* __WM__DLNA_DMR_H_ */

