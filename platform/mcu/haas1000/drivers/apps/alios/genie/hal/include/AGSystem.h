/******************************************************************************
 *
 *\brief   some scattered interfaces are defined here
 *\author  jeff
 *\mail    jinfeng.yjf@alibaba-inc.com
 *
 ******************************************************************************/
#ifndef GENIESDK_AGSYSTEM_H
#define GENIESDK_AGSYSTEM_H

#include <string>
#include <mutex>
#include <list>
#include "json-c/json.h"

using namespace std;

#define MAX_STRING_SETTING_LENGTH 128
#define MAX_SETTING_NAME_LENGTH 64


typedef union _configValue
{
    char value_in_str[MAX_STRING_SETTING_LENGTH];
    int value_in_int;
    bool value_in_bool;
} ConfigValue;

typedef enum _configType
{
    CONFIG_TYPE_STRING = 0,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_BOOL,
} ConfigType;

typedef void (*onConfigChange)(const char *name, ConfigType type, const ConfigValue &value);

typedef struct _observerInfo
{
    char observer_name[MAX_SETTING_NAME_LENGTH];
    char config_name[MAX_SETTING_NAME_LENGTH];
    onConfigChange configChangeCallback;
} ObserverInfo;

class AGSystem
{
public:
    AGSystem();
    virtual ~AGSystem();

    /**
     * @brief used to init the hardware module when startup.it's optional.
     * @return void
     */
    virtual void systemInit(){}

    /**
     * @brief Used to read odm name. it's optional.
     * @return string Odm name
     */
    virtual string  getOdmName(){return "";}

    /**
     * @brief Used to write odm name. it's optional.
     * @param[in] st New odm name to set
     * @return int 0 : success; other: fail
     */
    virtual int setOdmName(string st){return -1;}
    virtual string getSystemValue(string name,string defvalue=""){return defvalue;}

    /**
     * @brief Used to read custion boardid ,for TELECOM.
     * @return string Custom Id
     */
    virtual string getCustomID(){return "";}
    /**
     * @brief Used to save custion boardid ,for TELECOM.
     * @param[in] st New custom id to set
     * @return int 0 : success; other: fail
     */
    virtual int  setCustomId(string st){return -1;}

    /**
     * @brief Write settings to config file with string value.
     * @param[in] name Parameter's name
     * @param[in] value Parameter's value in string
     * @return int 0 : success; other: fail
     */
    virtual int writeConfig(string name, string value);
    /**
     * @brief Write settings to config file with int value.
     * @param[in] name Parameter's name
     * @param[in] value Parameter's value in int
     * @return int 0 : success; other: fail
     */
    virtual int writeIntConfig(string name, int value);
    /**
     * @brief Write settings to config file with bool value.
     * @param[in] name Parameter's name
     * @param[in] value Parameter's value in boolean
     * @return int 0 : success; other: fail
     */
    virtual int writeBoolConfig(string name, bool value);

    /**
     * @brief Read string settings from config file.
     * @param[in] name Parameter's name
     * @param[in] defvalue Parameter's default value
     * @return string Config's value in string
     */
    virtual string readConfig(string name, string defvalue);
    /**
     * @brief Read int settings from config file.
     * @param[in] name Parameter's name
     * @param[in] defvalue Parameter's default value
     * @return int Config's value in int
     */
    virtual int readIntConfig(string name, int defvalue);
    /**
     * @brief Read boolean settings from config file.
     * @param[in] name Parameter's name
     * @param[in] defvalue Parameter's default value
     * @return boolean Config's value in boolean
     */
    virtual bool readBoolConfig(string name, bool defvalue);

    /**
     * @brief Register config change observer.
     * @param[in] observer_name Observer's name in string
     * @param[in] config_name Config name which being observed
     * @param[in] callback Observer's callback
     * @return int 0 : success; other: fail
     */
    int registerConfigObserver(const char *observer_name, const char *config_name, onConfigChange callback);
    /**
     * @brief Unregister config change observer.
     *         All observers with same observer_name will be removed if config_name set to NULL in unregisterConfigObserver.
     * @param[in] observer_name Observer's name in string
     * @param[in] config_name Config name which being observed
     * @return int 0 : success; other: fail
     */
    int unregisterConfigObserver(const char *observer_name, const char *config_name);

    /**
     * @brief Enable system perf mode
     * @param[in] ms
     * @return void
     */
    virtual void enablePerfMode(int ms) {}
protected:
    /**
     * @brief Function used to parse json file and get string value for a specific key
     * @param[in] config_data Config to parse
     * @param[in] key Setting's name
     * @param[out] buffer Setting's value in string
     * @param[in] size Buffer size
     * @return int 0 : success; other: fail
     */
    int parseSettingWithStringValue(char *config_data, const char *key, char *buffer, int size);
    /**
     * @brief Function used to parse json file and get int value for a specific key
     * @param[in] config_data Config to parse
     * @param[in] key Setting's name
     * @param[out] value Setting's value in int
     * @return int 0 : success; other: fail
     */
    int parseSettingWithIntValue(char *config_data, const char *key, int *value);
    /**
     * @brief Function used to parse json file and get bool value for a specific key
     * @param[in] config_data Config to parse
     * @param[in] key Setting's name
     * @param[out] value Setting's value in bool
     * @return int 0 : success; other: fail
     */
    int parseSettingWithBoolValue(char *config_data, const char *key, bool *value);

    /**
     * @brief Function used to write specific key and value to  json file
     * @param[in] config_data Config to write
     * @param[in] key Setting's name
     * @param[out] buffer Setting's value in string
     * @return int 0 : success; other: fail
     */
    int writeSettingWithStringValue(char *config_data, const char *key, const char *buffer);
    /**
     * @brief Function used to write specific key and value to  json file
     * @param[in] config_data Config to parse
     * @param[in] key Setting's name
     * @param[out] value Setting's value in int
     * @return int 0 : success; other: fail
     */
    int writeSettingWithIntValue(char *config_data, const char *key, int value);
    /**
     * @brief Function used to write specific key and value to  json file
     * @param[in] config_data Config to parse
     * @param[in] key Setting's name
     * @param[out] value Setting's value in bool
     * @return int 0 : success; other: fail
     */
    int writeSettingWithBoolValue(char *config_data, const char *key, bool value);
    /**
     * @brief Load config file content from file system, must be protected by mutex when called.
     * @param[in] config_file Config file to load
     * @return char* File content
     */
    char* loadConfigFile(const char *config_file);
    /**
     * @brief Store config file to file system with json format, must be protected by mutex when called.
     * @param[in] config_file Config file to load
     * @param[in] param_obj Content in json format
     * @return int 0 : success; other: fail
     */
    int storeConfigFile(const char *config_file, json_object *param_obj);
    /**
     * @brief Notify config change observer
     * @param[in] config_name Config to observe
     * @param[in] type Config type
     * @param[in] value Config value
     * @return void
     */
    virtual void notifyConfigChangeObserver(const char *config_name, ConfigType type, const ConfigValue &value);
    list<ObserverInfo *> mObserverList;
    mutex mLock;
};


#endif
