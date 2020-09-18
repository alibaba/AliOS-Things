/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGModuleConfig.h
 * @brief Config the name of so which being loaded
 * @author jinfeng.yjf@alibaba-inc.com
 * @data 2017/09/10
 * @version 1.0
 */

#ifndef AGMODULE_CONFIG_H
#define AGMODULE_CONFIG_H

#include <string>

using namespace std;

#define SO_CONFIG_FILE "3rd_so_config.json"

typedef enum _ConfigPathType
{
    CONFIG_PATH_TYPE_DATA = 0,
    CONFIG_PATH_TYPE_ROOT,
    CONFIG_PATH_TYPE_EXTRA
} ConfigPathType;

class AGModuleConfig
{
public:
    /**
     * @brief Constructor of AGModuleConfig
     * @param[in] path Custom path set when launch smartbox, such as /data/smartbox/
     * @param[in] defpath Default path for runtime, such as /usr/lib/smartbox/
     * @return void
     */
    AGModuleConfig(const char * path,const char* defpath);
    ~AGModuleConfig();
    /**
     * @brief Get Log implement library path
     * @return char*
     */
    const char *getAgLogPath();
    /**
     * @brief Get Audio implement library path
     * @return char*
     */
    const char *getAgAudioPath();
    /**
     * @brief Get Light implement library path
     * @return char*
     */
    const char *getAgLightPath();
    /**
     * @brief Get Input implement library path
     * @return char*
     */
    const char *getAgInputPath();
    /**
     * @brief Get AGLocalSpeechProcessor implement library path
     * @return char*
     */
    const char *getAgAspPath();
    /**
     * @brief Get AGWakeupDetect implement library path
     * @return char*
     */
    const char *getAgWakeupDetectPath();
    /**
     * @brief Get AGSpeechRecognizer implement library path
     * @return char*
     */
    const char *getAgSpeechRecognizerPath();
    /**
     * @brief Get AgCfg implement library path
     * @return char*
     */
    const char *getAgCfgPath();
    /**
     * @brief Get Agffmpeg implement library path
     * @return char*
     */
    const char *getAgffmpegPath();
    /**
     * @brief Get AgWifi implement library path
     * @return char*
     */
    const char * getAgWifiPath();
    /**
     * @brief Get AgBluetooth implement library path
     * @return char*
     */
    const char * getAgBluetoothPath();
    /**
     * @brief Get AgSigmesh implement library path
     * @return char*
     */
    const char * getAgSigmeshPath();
    /**
     * @brief Get AgGatt implement library path
     * @return char*
     */
    const char * getAgGattPath();
    /**
     * @brief Get AgSystem implement library path
     * @return char*
     */
    const char * getAgSystemPath();
    /**
     * @brief Get AgGattServer implement library path
     * @return char*
     */
    const char * getAgGattServerPath();
    /**
    * @brief Update config path for different type
    * @param[in] configpath New config path to set
    * @param[in] type ConfigPathType: data, root, extra
    * @return int 0 : success; other: fail
    */
    static int updateConfigPath(const char *configpath, ConfigPathType type);
    /**
     * @brief Get config path in data partition
     * @return string Custom path set when launch smartbox, such as /data/smartbox/
     */
    inline static string getDataConfigPath() { return mConfigPath; }
    /**
     * @brief Get config path in root partition
     * @return string Path for runtime, such as /usr/lib/smartbox/
     */
    inline static string getRootConfigPath() { return mDefConfigPath; }
    /**
     * @brief Get extra config path
     * @return string Extra path, maybe for a writable partition, used by Gaode now.
     */
    inline static string getExtraConfigPath() { return mExtraConfigPath; }
    /**
     * @brief Set config in int
     * @return int 0 : success; other: fail
     */
    static int setConfigInt(int value) { return 0; }
    /**
     * @brief Get config in int
     * @param[in] defaultValue Default config value in int
     * @return int Config value
     */
    static int getConfigInt(int defaultValue) { return 0; }
    /**
     * @brief Get Product secret config path, who is used to do authorization with gateway
     * @param[in] fullpath Buffer to store path
     * @param[in] size Bufer size
     * @return int 0 : success; other: fail
     */
    static int getProdSecretPath(char *fullpath, int size);

private:
    int loadConfig();
    string mAgLogPath;
    string mAgAudioPath;
    string mAgLightPath;
    string mAgInputPath;
    string mAgAspPath;
    string mAgWakeupDetectPath;
    string mAgSpeechRecognizerPath;
    string mAgCfgPath;
    string mAgffmpegPath;
    string mAgWifiPath;
    string mAgBluetoothPath;
    string mAgSystemPath;
    string mApkCfgRootPath;
    string mAgSigmeshPath;
    string mAgGattPath;
    string mAgGattServerPath;
    static string mConfigPath;
    static string mDefConfigPath;
    /**
     * @brief Extra config path, used to store a config path for backup
     *                  In Gaode project, writable config path is different with readable config path,
     *                  so a extra path is needed.
     */
    static string mExtraConfigPath;

};


#endif



