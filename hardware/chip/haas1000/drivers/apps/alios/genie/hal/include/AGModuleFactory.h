/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file AGModuleFactory.h
 * @brief module interface
 * @author lifen.lf@alibaba-inc.com
 * @data 2018/04/07
 * @version 1.0
 */

#ifndef AG_MODULE_FACTORY_H
#define AG_MODULE_FACTORY_H

#include <stddef.h>
#include <map>
#include <string>
#include <cstring>
#include <list>
#include "AGAudioOutput.h"

class AGLog;
class AGAudioTrack;
class AGAudioSystem;
class AGAudioDecoder;
class AGLight;
class AGAudioRecord;
class AGLocalSpeechProcessor;
class AGSpeechRecognizer;
class AGVad;
class AGInput;
class AGWifi;
class AGBluetooth;
class AGSigmesh;
class AGGatt;
class AGGattServer;
class AGSystem;

class AGModuleFactory
{
public:
    virtual ~AGModuleFactory(){}

    /**
     * @brief Create AGAudioTrack instance base on subclass implementation
     * @return AGAudioTrack*
     */
    virtual AGAudioTrack* createAGAudioTrack()=0;
    virtual AGAudioTrack* createAGAudioTrack(AudioParameters &audioParam)=0;

    /**
     * @brief Create AGAudioSystem instance base on subclass implementation
     * @return AGAudioSystem*
     */
    virtual AGAudioSystem* createAGAudio(){return NULL;}
    /**
     * @brief Create AGAudioDecoder instance base on subclass implementation
     * @return AGAudioDecoder*
     */
    virtual AGAudioDecoder* createAGAudioDecoder()=0;

    /**
     * @brief Create AGLog instance base on subclass implementation
     * @return AGLog*
     */
    virtual AGLog*  createAGLog()=0;
    /**
     * @brief Create AGLight instance base on subclass implementation
     * @return AGLight*
     */
    virtual AGLight* createAGLight()=0;
    /**
     * @brief Create AGBluetooth instance base on subclass implementation
     * @return AGBluetooth*
     */
    virtual AGBluetooth* createAGBluetooth() {return NULL;}
//[RTOS begin] disable AISpeechAudioRecord
    /**
     * @brief Create AGAudioRecord instance base on subclass implementation
     * @return AGAudioRecord*
     */
    //virtual AGAudioRecord* createAGAudioRecord() = 0;
//[RTOS end] disable AISpeechAudioRecord
    /**
     * @brief Create AGLocalSpeechProcessor instance base on subclass implementation
     * @return AGLocalSpeechProcessor*
     */
    virtual AGLocalSpeechProcessor*  createAGLocalSpeechProcessor()=0;
    /**
     * @brief Create AGSpeechRecognizer instance base on subclass implementation
     * @return AGSpeechRecognizer*
     */
    virtual AGSpeechRecognizer*  createAGSpeechRecognizer()=0;
    /**
     * @brief Create AGVad instance base on subclass implementation
     * @return AGVad*
     */
    virtual AGVad*  createAGVad()=0;
    /**
     * @brief Create AGInput instance base on subclass implementation
     * @return AGInput*
     */
    virtual AGInput* createAGInput()=0;
    /**
     * @brief Create AGWifi instance base on subclass implementation
     * @return AGWifi*
     */
    virtual AGWifi* createAGWifi(){return NULL;}
    /**
     * @brief Create AGSigmesh instance base on subclass implementation
     * @return AGSigmesh*
     */
    virtual AGSigmesh* createAGSigmesh(){return NULL;}
    /**
     * @brief Create AGGatt instance base on subclass implementation
     * @return AGGatt*
     */
    virtual AGGatt* createAGGatt(){return NULL;}
    /**
     * @brief Create AGGattServer instance base on subclass implementation
     * @return AGGattServer*
     */
    virtual AGGattServer* createAGGattServer(){return NULL;}
    /**
     * @brief Create AGSystem instance base on subclass implementation
     * @return AGSystem*
     */
    virtual AGSystem* createAGSystem(){return NULL;}
};

AGModuleFactory* createAGModuleFactory();

using std::map;
using std::list;
class AGModuleFactoryHelper
{
public:
    /**
     * @brief Get AGModuleFactoryHelper instance
     * @return AGModuleFactoryHelper*
     */
    static AGModuleFactoryHelper* getInstance() { return m_Instance; }
    /**
     * @brief Create AGModuleFactoryHelper instance, who will create AGModuleFactory
     * @param[in] custompath Custom path set when launch smartbox, such as /data/smartbox/
     * @param[in] defaultpath Default path for runtime, such as /usr/lib/smartbox/
     * @return void
     */
    static void createAGModuleFactoryHelper(const char* custompath,const char* defaultpath,const char* requirement);
    /**
     * @brief Get AGModuleFactory instance
     * @return AGModuleFactory*
     */
    AGModuleFactory* getAGModuleFactory() { return mAGModuleFactory; }
    /**
     * @brief Get AGLog instance
     * @return AGLog*
     */
    AGLog* getAGLog() {return mAGLog;}
    /**
     * @brief Get AGAudioSystem instance
     * @return AGAudioSystem*
     */
    AGAudioSystem* getAGAudio() {return mAGAudio;}
    /**
     * @brief Get AGLight instance
     * @return AGLight*
     */
    AGLight* getAGLight() {return mAGLight;}
    /**
     * @brief Get AGLocalSpeechProcessor instance
     * @return AGLocalSpeechProcessor*
     */
    AGLocalSpeechProcessor* getAGLocalSpeechProcessor() {return mLocalSpeechProcessor;}
    /**
     * @brief Get AGSpeechRecognizer instance
     * @return AGSpeechRecognizer*
     */
    AGSpeechRecognizer* getAGSpeechRecognizer() {return mAGSpeechRecognizer;}
    /**
     * @brief Get AGVad instance
     * @return AGVad*
     */
    AGVad* getAGVad() {return mAGVad;}
    /**
     * @brief Get AGInput
     * @return AGInput*
     */
    AGInput* getAGInput(){return mAGInput;}
    /**
     * @brief Get AGWifi instance
     * @return AGWifi*
     */
    AGWifi* getAGWifi(){return mAGWifi;}
    /**
     * @brief Get AGBluetooth instance
     * @return AGBluetooth*
     */
    AGBluetooth* getAGBluetooth(){return mAGBluetooth;}
    /**
     * @brief Get AGSystem instance
     * @return AGSystem*
     */
    AGSystem* getAGSystem(){return mAGSystem;}
    /**
     * @brief Get AGSigmesh instance
     * @return AGSigmesh*
     */
    AGSigmesh* getAGSigmesh(){return mAGSigmesh;}
    /**
     * @brief get AGGatt instance
     * @return AGGatt*
     */
    AGGatt* getAGGatt(){return mAGGatt;}
    /**
     * @brief Get AGGattServer instance
     * @return AGGattServer*
     */
    AGGattServer* getAGGattServer(){ return mAGGattServer;}
private:
    AGModuleFactoryHelper(const char* path,const char* defpath,const char* requirement);
    ~AGModuleFactoryHelper();
    int addLib(const char* name, int id);
    static AGModuleFactoryHelper *m_Instance;

    //void* mLibHandle;
    AGModuleFactory* mAGModuleFactory;
    AGLog* mAGLog;
    AGAudioSystem* mAGAudio;
    AGLight* mAGLight;
    AGLocalSpeechProcessor* mLocalSpeechProcessor;
    AGSpeechRecognizer* mAGSpeechRecognizer;
    AGVad* mAGVad;
    AGInput* mAGInput;
    AGWifi* mAGWifi;
    AGBluetooth* mAGBluetooth;
    AGSystem* mAGSystem;
    AGSigmesh* mAGSigmesh;
    AGGatt* mAGGatt;
    AGGattServer* mAGGattServer;

    map<std::string, int> mSo2FunMap;
    list<void*> mLibHandleList;

    class GC
    {
    public :
        ~GC()
        {
            if (m_Instance != NULL )
            {
                delete m_Instance;
                m_Instance = NULL ;
            }
        }
    };
    //static GC gc;
};
#endif

