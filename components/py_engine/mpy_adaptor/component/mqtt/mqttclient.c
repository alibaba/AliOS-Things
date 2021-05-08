#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "MQTTClient.h"




#define REQ_BUF_SIZE 2048
// static char mqtt_req_buf[REQ_BUF_SIZE];

// /* @brief http response buffer */
#define RSP_BUF_SIZE 2048
// static char mqtt_rsp_buf[RSP_BUF_SIZE];


static char mqtt_buf[RSP_BUF_SIZE];
static char mqtt_readbuf[RSP_BUF_SIZE];
unsigned int mqtt_msgid = 0;


extern const mp_obj_type_t mqtt_client_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    Network n ;
    MQTTClient c;
    MQTTPacket_connectData data;

    char *ModuleName;
} mqtt_client_obj_t;



static mp_obj_t global_mqtt_on_subcribe ;


void messageArrived(MessageData* md)
{
	MQTTMessage* message = md->message;

	printf("%.*s\n", md->topicName->lenstring.len, md->topicName->lenstring.data);
	printf("%.*s\n", (int)message->payloadlen, (char*)message->payload);
    
    if(mp_obj_is_fun(global_mqtt_on_subcribe)){
        mp_call_function_2(global_mqtt_on_subcribe,mp_obj_new_str(md->topicName->lenstring.data,md->topicName->lenstring.len),mp_obj_new_str((char*)message->payload,(int)message->payloadlen));
    }
}

void mqtt_client_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mqtt_client_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t mqtt_client_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_E("entern  %s,n_args is %d  ;\r\n", __func__,n_args);
    char clientId[150] = {0};
	char username[65] = {0};
	char password[65] = {0};
    mqtt_client_obj_t* mqtt_client_obj = m_new_obj(mqtt_client_obj_t);
    if (!mqtt_client_obj) {
        mp_raise_OSError(ENOMEM);
    }

    mqtt_client_obj->Base.type = &mqtt_client_type;
    mqtt_client_obj->ModuleName = "client";


    Network n  = {0} ;
    MQTTClient c  = { 0} ;

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 3;

    if (n_args >= 1){
        char *cus_client_id = (char *)mp_obj_str_get_str(args[0]) ;
        LOG_D("custom client id is %s \r\n",cus_client_id);
        data.clientID.cstring = cus_client_id;
    }
    else
	    data.clientID.cstring = clientId;
	data.username.cstring = username;
	data.password.cstring = password;
	data.keepAliveInterval = 60;
	data.cleansession = 1;

    mqtt_client_obj->n =  n ;
    mqtt_client_obj->c =  c ;
    mqtt_client_obj->data =  data ;

    return MP_OBJ_FROM_PTR(mqtt_client_obj);
}


STATIC mp_obj_t mqtt_connect(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int rc = -1;
    void* instance = NULL;


    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;
    }


    char *host = (char *)mp_obj_str_get_str(args[1]);
    short port = mp_obj_get_int(args[2]);
    int interval = mp_obj_get_int(args[3]);

    mqtt_client_obj->data.keepAliveInterval = interval ;

    NetworkInit(&mqtt_client_obj->n);

	rc = NetworkConnect(&mqtt_client_obj->n, host, port);
	if(rc != 0){
		printf("Network Connect failed:%d\n", rc);
		return;
	}else{
		printf("Network Connect Success!\n");
	}

	/* init mqtt client */
	MQTTClientInit(&mqtt_client_obj->c, &mqtt_client_obj->n, 1000, mqtt_buf, sizeof(mqtt_buf), mqtt_readbuf, sizeof(mqtt_readbuf));
    
    /* set the default message handler */
	mqtt_client_obj->c.defaultMessageHandler = messageArrived;


    printf("client id is %s \r\n",mqtt_client_obj->data.clientID);
    printf("host is %s \r\n",host);
    printf("port is %d \r\n",port);


    /* set mqtt connect parameter */
	rc = MQTTConnect(&mqtt_client_obj->c, &mqtt_client_obj->data);
	if(rc != 0){
		printf("MQTT Connect server failed:%d\n", rc);
	}else{
		printf("MQTT Connect Success!\n");
	}
    return mp_obj_new_int(rc);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_connect, 4, mqtt_connect);


STATIC mp_obj_t mqtt_subscribe(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int rc = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;

    }
    char *subtopic = (char *)mp_obj_str_get_str(args[1]);
    int qos = mp_obj_get_int(args[2]);
    rc = MQTTSubscribe(&mqtt_client_obj->c, subtopic, qos, messageArrived);

    return mp_obj_new_int(rc);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_subscribe, 3, mqtt_subscribe);


STATIC mp_obj_t mqtt_username_pw_set(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int rc = -1;

    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;
    }
    char *username = (char *)mp_obj_str_get_str(args[1]);
    char *pwd = (char *)mp_obj_str_get_str(args[2]);
    mqtt_client_obj->data.username.cstring = username ;
    mqtt_client_obj->data.password.cstring = pwd ;
    LOG_D("username is %s , pwd is %s \r\n",username,pwd);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_username_pw_set, 3, mqtt_username_pw_set);


STATIC mp_obj_t mqtt_publish(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    int rc = -1;

    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;
    }
    char *topic = (char *)mp_obj_str_get_str(args[1]);
    char *payload = (char *)mp_obj_str_get_str(args[2]);
    int qos  = mp_obj_get_int(args[3]);

    MQTTMessage msg = {0};
    msg.qos = qos ;
    msg.payload = payload ;
    msg.payloadlen = strlen(payload);
    msg.id = ++mqtt_msgid;
    rc = MQTTPublish(&mqtt_client_obj->c, topic, &msg);

    return mp_obj_new_int(rc);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_publish, 4, mqtt_publish);


STATIC mp_obj_t mqtt_loop(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;
    }
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    int timeout = mp_obj_get_int(args[1]);
	MQTTYield(&mqtt_client_obj->c, timeout);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_loop, 2, mqtt_loop);


STATIC mp_obj_t mqtt_disconnect(size_t n_args, const mp_obj_t *args)
{
    LOG_D("enter  %s; n_args = %d;\n", __func__, n_args);
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mqtt_client_obj_t* mqtt_client_obj = (mqtt_client_obj_t *)self;
    if (mqtt_client_obj == NULL)
    {
        LOG_E("mqtt_client_obj  is NULL\n");
        return mp_const_none;
    }
    MQTTDisconnect(&mqtt_client_obj->c);
	NetworkDisconnect(&mqtt_client_obj->n);
    return mp_obj_new_int(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mqtt_client_disconnect, 1, mqtt_disconnect);


STATIC mp_obj_t mqtt_on_subcribe(mp_obj_t id,mp_obj_t func) {

    global_mqtt_on_subcribe = func ;
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(mqtt_client_on_subcribe, mqtt_on_subcribe);

STATIC const mp_rom_map_elem_t mqtt_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_http)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_on_subcribe), MP_ROM_PTR(&mqtt_client_on_subcribe)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_username_pw_set), MP_ROM_PTR(&mqtt_client_username_pw_set)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&mqtt_client_connect)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_publish), MP_ROM_PTR(&mqtt_client_publish)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_subscribe), MP_ROM_PTR(&mqtt_client_subscribe)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_loop), MP_ROM_PTR(&mqtt_client_loop)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&mqtt_client_disconnect)},
};

STATIC MP_DEFINE_CONST_DICT(mqtt_module_globals, mqtt_module_globals_table);



const mp_obj_type_t mqtt_client_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_client,
    .print = mqtt_client_print,
    .make_new = mqtt_client_new,
    .locals_dict = (mp_obj_dict_t *)&mqtt_module_globals,
};
