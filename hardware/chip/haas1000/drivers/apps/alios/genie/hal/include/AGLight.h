/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGLight.h
 * @brief
 * @author chiliang.cl@alibaba-inc.com
 * @data 2017/12/24
 * @version 1.0
 */

#ifndef AG_LIGHT_H
#define AG_LIGHT_H

enum LightImplementEvent {
    LIGHT_IMPL_FIRSTONLINE = 0,
    LIGHT_IMPL_ACTION_START,
    LIGHT_IMPL_ACTION_SUCCESS,
    LIGHT_IMPL_ACTION_FAIL,
    LIGHT_IMPL_SUCCESS_CLOSE,
    LIGHT_IMPL_FAIL_CLOSE,
    LIGHT_IMPL_STANDBY_BREATHE,
    LIGHT_IMPL_WARNING_BREATHE,
    LIGHT_IMPL_OTASUCCESS_MUTE,
    LIGHT_IMPL_OTASUCCESS_UNMUTE,
    LIGHT_IMPL_NETCONFSTART,
    LIGHT_IMPL_WIFISUCCESS,
    LIGHT_IMPL_WIFIWRONG,
    LIGHT_IMPL_WIFIOFFLINE,
    LIGHT_IMPL_NETCON_FAILED,
    LIGHT_IMPL_SERVERDISTCONT,
    LIGHT_IMPL_ASRSERVERERROR,
    LIGHT_IMPL_SERVERRECONECT,
    LIGHT_IMPL_UNAUTHWARNING,
    LIGHT_IMPL_UNAUTHORIZED,
    LIGHT_IMPL_BTPAIR,
    LIGHT_IMPL_BTCONNECTED,
    LIGHT_IMPL_BTFAILED,
    LIGHT_IMPL_SPEECHWAKEUP,
    LIGHT_IMPL_SPEECHPROCESS,
    LIGHT_IMPL_SPEECHFEEDBACK,
    LIGHT_IMPL_SPEECHWAKEUP_VOIMSG,
    LIGHT_IMPL_VOIMSG_PLAYSTART,
    LIGHT_IMPL_STANDBY,
    LIGHT_IMPL_NORMAL,
    LIGHT_IMPL_SHUTDOWN,
    LIGHT_IMPL_LEDVOLUMEON,
    LIGHT_IMPL_LEDVOLUMEOFF,
    LIGHT_IMPL_FACTORYRST,
    LIGHT_IMPL_NEWMESSAGE,
    LIGHT_IMPL_PLAYONETIME,
    LIGHT_IMPL_SHOWWAKEUPCOLOR,
    LIGHT_IMPL_SHOWMSGCOLOR,
    LIGHT_IMPL_SERVERRSP_CLOSE,
    LIGHT_IMPL_TTSDONE_CLOSE,
    LIGHT_IMPL_FORCE_CLOSE,
    LIGHT_IMPL_SET_WAKEUPCOLOR,
    LIGHT_IMPL_SET_MSGCOLOR,
    LIGHT_IMPL_SCREEN_ON,
    LIGHT_IMPL_SCREEN_OFF
};

class AGLight
{
public:
    virtual ~AGLight() {}
    /**
     * @brief Init AGLight
     * @return void
     */
    virtual void init() = 0;
    /**
     * @brief Start AGLight
     * @return void
     */
    virtual void start() = 0;
    /**
     * @brief Set led config file path
     * @param[in] ledConfigDir Led config file path
     * @return void
     */
    virtual void setLedConfigDir(const char *ledConfigDir) = 0;
    /**
     * @brief Set wake up color for led
     * @param[in] wakeupcolor Wake up color to set
     * @return void
     */
    virtual void setWakeupColor(const char *wakeupcolor) = 0;
    /**
     * @brief Set wake up color for led
     * @param[in] msgcolor Message color to set
     * @return void
     */
    virtual void setMessageColor(const char *msgcolor) = 0;
    /**
     * @brief Set board type
     * @param[in] boardtype Board type to set
     * @return void
     */
    virtual void setBoardType(int boardtype) = 0;
    /**
     * @brief Light event
     * @param[in] event Event type
     * @return void
     */
    virtual void lightImplEvent(int event) = 0;
    /**
     * @brief Light event for mic mute
     * @param[in] isMute Mute status: true or false
     * @param[in] isOtaBoot Ota boot status: true or false
     * @return void
     */
    virtual void lightImplEventMicMute(bool isMute, bool isOtaBoot) = 0;
    /**
     * @brief Light event for volume adjust
     * @param[in] volume
     * @param[in] LedTopFlash Should top led flash: true or false
     * @return void
     */
    virtual void lightImplEventAdjustVolume(int volume, bool LedTopFlash) = 0;
    /**
     * @brief Custom bottom light config
     * @param[in] lightConfig Light config for custom bottom light
     * @return void
     */
    virtual void lightImplEventCustomBottomLight(const char *lightConfig) = 0;
    /**
     * @brief Set led index
     * @param[in] ledIndex Set index for led
     * @return void
     */
    virtual void lightImplEventSetLedIndex(int ledIndex) = 0;
    /**
     * @brief Convert doa to led index
     * @param[in] doa Value of doa
     * @return void
     */
    virtual void lightImplEventDoaToLedIndex(int doa) = 0;
};
#endif
