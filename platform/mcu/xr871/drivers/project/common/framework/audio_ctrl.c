/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "audio_ctrl.h"
#include "audio/manager/audio_manager.h"

#if PRJCONF_AUDIO_CTRL_EN

#define	_AU_WARN			1
#define	_AU_ERROR			1
#define	_AU_INFO			0

#define AU_LOG(flags, fmt, arg...)		\
		do {							\
			if (flags)					\
				printf(fmt, ##arg); 	\
		} while (0)

#define AU_WARN(fmt, arg...)	\
		AU_LOG(_AU_WARN, "[AU WRN] %s():%d "fmt, __func__, __LINE__, ##arg)

#define AU_ERROR(fmt, arg...)	\
		AU_LOG(_AU_ERROR, "[AU ERR] %s():%d "fmt, __func__, __LINE__, ##arg)

#define AU_INFO(fmt, arg...)	\
		AU_LOG(_AU_INFO, "[AU INF]  "fmt, ##arg)

#ifdef __CONFIG_XIP_SECTION_FUNC_LEVEL

#define AU_IT_LOG(flags, fmt, arg...)                   \
    do {                                                \
        if (flags) {                                    \
            __nonxip_data static char __fmt[] = fmt;    \
            printf(__fmt, ##arg);                       \
        }                                               \
    } while (0)

#define AU_IT_INFO(fmt, arg...)    AU_IT_LOG(_AU_INFO, "[AU INF] "fmt, ##arg)
#define AU_IT_WARN(fmt, arg...)    AU_IT_LOG(_AU_WARN, "[AU WRN] "fmt, ##arg)
#define AU_IT_ERROR(fmt, arg...)   AU_IT_LOG(_AU_WARN, "[AU ERR] "fmt, ##arg)

#else /* __CONFIG_XIP_SECTION_FUNC_LEVEL */

#define AU_IT_INFO      AU_INFO
#define AU_IT_WARN      AU_WARN
#define AU_IT_ERROR     AU_ERROR

#endif /* __CONFIG_XIP_SECTION_FUNC_LEVEL */


static void audio_ctrl_msg_process(uint32_t event, uint32_t data, void *arg)
{
	switch (EVENT_SUBTYPE(event)) {
		case AUDIO_CTRL_MSG_LINEIN_INSERT:
			AU_INFO("start LINEIN\n");
			aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_IN_DEV_LINEIN, 1);
			break;
		case AUDIO_CTRL_MSG_LINEIN_REMOVE:
			AU_INFO("end LINEIN\n");
			aud_mgr_handler(AUDIO_DEVICE_MANAGER_PATH, AUDIO_IN_DEV_LINEIN, 0);
			break;
		default:
			break;
	}
}

/*
 * @brief create the sys_ctrl events.
 * @param
 *        @arg none
 * @retval  0 if create success or other if failed.
 */
int audio_ctrl_init(void)
{
	observer_base *base = sys_callback_observer_create(CTRL_MSG_TYPE_AUDIO,
	                                                   ALL_SUBTYPE,
	                                                   audio_ctrl_msg_process,
	                                                   NULL);
	if (base == NULL) {
		AU_ERROR("create fail\n");
		return -1;
	}
	if (sys_ctrl_attach(base) != 0) {
		AU_ERROR("attach fail\n");
		return -1;
	}

	return 0;
}

/*
 * @brief LINEIN detect callback.
 * @param present:
 *        @arg present 1-LINEIN insert, 0-LINEIN remove
 * @retval  none
 */
__nonxip_text
void audio_detect_callback(uint32_t present)
{
	int ret;
	uint16_t subtype;

	if (present) {
		AU_IT_INFO("LINEIN insert\n");
		subtype = AUDIO_CTRL_MSG_LINEIN_INSERT;
	} else {
		AU_IT_INFO("LINEIN remove\n");
		subtype = AUDIO_CTRL_MSG_LINEIN_REMOVE;
	}

	ret = sys_event_send(CTRL_MSG_TYPE_AUDIO, subtype, 0, 0);
	if (ret != 0) {
		AU_IT_ERROR("linein detect send event fail\n");
	}
}

#endif /* PRJCONF_AUDIO_CTRL_EN */
