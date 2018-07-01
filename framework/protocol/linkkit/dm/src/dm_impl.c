#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "interface/thing_abstract.h"
#include "interface/log_abstract.h"
#include "interface/list_abstract.h"
#include "interface/thing_manager_abstract.h"
#include "dm_thing_manager.h"
#include "dm_logger.h"
#include "dm_thing.h"
#include "dm_slist.h"
#include "iot_export_dm.h"
#include "iot_import.h"
#include "lite-utils.h"
#include "class_interface.h"

#include "dm_import.h"

static const char string_dm_impl_class_name[] __DM_READ_ONLY__ = "dm_impl_cls";
static const char string_dm_impl_log_object_name[] __DM_READ_ONLY__ = "dm_logger";
static const char string_dm_impl_thing_manager_object_name[] __DM_READ_ONLY__ = "dm thing manager";

static void *dm_impl_ctor(void *_self, va_list *params)
{
    dm_impl_t *self = _self;
    log_t **logger;
    void *linkkit_callback_fp;

    self->_name = va_arg(*params, char *);
    self->_get_tsl_from_cloud = va_arg(*params, int);
    self->_log_level = va_arg(*params, int);
    linkkit_callback_fp = va_arg(*params, void *);
    self->_domain_type = va_arg(*params, int);

    if (self->_log_level > IOT_LOG_DEBUG) {
        self->_log_level = IOT_LOG_DEBUG;
    }

    self->_logger = new_object(DM_LOGGER_CLASS, string_dm_impl_log_object_name, 0);
    logger = self->_logger;
    (*logger)->open(logger, "dm");
    (*logger)->set_log_level(logger, self->_log_level);

    /* use sh as default domain. */
    if (self->_domain_type >= dm_cloud_domain_max) {
        self->_domain_type = dm_cloud_domain_shanghai;
    }

    self->_thing_manager = new_object(DM_THING_MANAGER_CLASS, string_dm_impl_thing_manager_object_name,
                                      self->_get_tsl_from_cloud, linkkit_callback_fp, self->_domain_type);

    return self;
}

static void *dm_impl_dtor(void *_self)
{
    dm_impl_t *self = _self;

    delete_object(self->_thing_manager);
    delete_object(self->_logger);

    self->_logger = NULL;

    return self;
}

static void *dm_impl_generate_new_thing(void *_self, const char *tsl, int tsl_len)
{
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;
    thing_t **thing = NULL;

    assert(thing_manager && tsl && tsl_len > 0);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    thing = (thing_t **)(*thing_manager)->generate_new_local_thing(thing_manager, tsl, tsl_len);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return thing;
}

static int dm_impl_set_property_value(void *_self, const void *thing_id, const void *identifier, const void *value,
                                      const char *value_str)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->set_thing_property_value && thing_id && identifier &&
           (value || value_str));

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->set_thing_property_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_set_event_output_value(void *_self, const void *thing_id, const void *identifier, const void *value,
                                          const char *value_str)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->set_thing_event_output_value && thing_id && identifier &&
           (value || value_str));
    if (strcmp(identifier, "post") == 0) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->set_thing_event_output_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_set_service_output_value(void *_self, const void *thing_id, const void *identifier,
                                            const void *value, const char *value_str)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->set_thing_service_output_value && thing_id && identifier &&
           (value || value_str));
    if (strcmp(identifier, "set") == 0 || strcmp(identifier, "get") == 0) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->set_thing_service_output_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_get_property_value(const void *_self, const void *thing_id, const void *identifier, void *value,
                                      char **value_str)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->get_thing_property_value && thing_id && identifier &&
           (value || value_str));

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->get_thing_property_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_get_service_input_value(const void *_self, const void *thing_id, const void *identifier, void *value,
                                           char **value_str)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->get_thing_service_input_value && thing_id && identifier &&
           (value || value_str));
    if (strcmp(identifier, "set") == 0 || strcmp(identifier, "get") == 0) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->get_thing_service_input_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_get_service_output_value(const void *_self, const void *thing_id, const void *identifier,
                                            void *value, char **value_str)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->get_thing_service_output_value && thing_id && identifier &&
           (value || value_str));
    if (strcmp(identifier, "set") == 0 || strcmp(identifier, "get") == 0) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->get_thing_service_output_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_get_event_output_value(const void *_self, const void *thing_id, const void *identifier, void *value,
                                          char **value_str)
{
    int res = 0;
    const dm_impl_t *self = _self;
    const thing_manager_t **thing_manager = (const thing_manager_t **)self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->get_thing_event_output_value && thing_id && identifier &&
           (value || value_str));
    if (strcmp(identifier, "post") == 0) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->get_thing_event_output_value(thing_manager, thing_id, identifier, value, value_str);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_install_callback_function(void *_self, handle_dm_callback_fp_t linkkit_callback_fp)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    if (!linkkit_callback_fp) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->add_callback_function(thing_manager, linkkit_callback_fp);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_trigger_event(const void *_self, const void *thing_id, const void *event_identifier,
                                 const char *property_identifier)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->trigger_event && thing_id && event_identifier);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->trigger_event(thing_manager, thing_id, event_identifier, property_identifier);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}
#ifdef EXTENDED_INFO_ENABLED
static int dm_impl_trigger_deviceinfo_update(const void *_self, const void *thing_id, const char *params)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->trigger_extended_info_update && thing_id && params);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->trigger_extended_info_update(thing_manager, thing_id, params);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_trigger_deviceinfo_delete(const void *_self, const void *thing_id, const char *params)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->trigger_extended_info_delete && thing_id && params);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->trigger_extended_info_delete(thing_manager, thing_id, params);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}
#endif /* EXTENDED_INFO_ENABLED*/
#ifdef RRPC_ENABLED
static int dm_impl_answer_service(const void *_self, const void *thing_id, const void *identifier, int response_id,
                                  int code, int rrpc)
#else
static int dm_impl_answer_service(const void *_self, const void *thing_id, const void *identifier, int response_id,
                                  int code)
#endif /* RRPC_ENABLED */
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->answer_service && thing_id && identifier);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
#ifdef RRPC_ENABLED
    res = (*thing_manager)->answer_service(thing_manager, thing_id, identifier, response_id, code, rrpc);
#else
    res = (*thing_manager)->answer_service(thing_manager, thing_id, identifier, response_id, code);
#endif /* RRPC_ENABLED */
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_invoke_raw_service(const void *_self, const void *thing_id, void *raw_data, int raw_data_length)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->invoke_raw_service && thing_id);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->invoke_raw_service(thing_manager, thing_id, raw_data, raw_data_length);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_answer_raw_service(const void *_self, const void *thing_id, void *raw_data, int raw_data_length)
{
    int res = 0;
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->invoke_raw_service && thing_id);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->answer_raw_service(thing_manager, thing_id, raw_data, raw_data_length);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}
#ifndef CM_SUPPORT_MULTI_THREAD
static int dm_impl_yield(const void *_self, int timeout_ms)
{
    const dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;

    assert(thing_manager && *thing_manager && (*thing_manager)->yield);

    return (*thing_manager)->yield(thing_manager, timeout_ms);
}
#endif /* CM_SUPPORT_MULTI_THREAD */
void *dm_lite_calloc(size_t nmemb, size_t size)
{
#ifdef CM_SUPPORT_MEMORY_MAGIC
    return LITE_calloc(nmemb, size, MEM_MAGIC, DM_MODULE_NAME);
#else
    return LITE_calloc(nmemb, size);
#endif
}

void *dm_lite_malloc(size_t size)
{
#ifdef CM_SUPPORT_MEMORY_MAGIC
    return LITE_malloc(size, MEM_MAGIC, DM_MODULE_NAME);
#else
    return LITE_malloc(size);
#endif
}

void dm_lite_free_func(void *ptr)
{
    return LITE_free_internal(ptr);
}

void dm_lite_free(const void *ptr)
{
    assert(ptr);

    if (!ptr) {
        dm_printf("dm_lite_free abort\n");
        return;
    }

    LITE_free_internal((char *)ptr);
}

void dm_lltoa(long long n, char *str, int radix)
{
    int i, j;
    long long remain;
    unsigned long long n_abs;
    char tmp_char;

    i = 0;

    if (n < 0) {
        str[i] = '-';
        str++;
        n_abs = -1 * n;
    } else {
        n_abs = n;
    }

    do {
        remain = n_abs % radix;
        if (remain > 9) {
            str[i] = remain  - 10 + 'A';
        } else {
            str[i] = remain + '0';
        }
        i++;
    } while (n_abs /= radix);

    str[i] = '\0';

    for (i-- , j = 0 ; j <= i ; j++ , i--) {
        tmp_char = str[j];
        str[j] = str[i];
        str[i] = tmp_char;
    }
    return;
}

#ifdef SUBDEV_ENABLE
int dm_impl_add_subdev_callback_function(void *_self, handle_dm_subdev_callback_fp_t subdev_callback_func)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    if (!subdev_callback_func) {
        return -1;
    }

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->add_subdev_callback_function(thing_manager, subdev_callback_func);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static void *dm_impl_generate_new_subthing(void *_self, const char *product_key, const char *device_name,
                                           const char *tsl, int tsl_len)
{
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;
    void *thing = NULL;

    assert(thing_manager && *thing_manager && product_key && device_name);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    thing = (*thing_manager)->generate_new_sub_thing(thing_manager, product_key, device_name, tsl, tsl_len);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return thing;
}

static int dm_impl_remove_subthing(void *_self, const void *sub_thing_id)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && sub_thing_id);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->remove_sub_thing(thing_manager, sub_thing_id);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_bind_sub_thing(void *_self, const char *pk, const char *dn, const char *ds)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && pk && dn);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->bind_sub_thing(thing_manager, pk, dn, ds);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_unbind_sub_thing(void *_self, const char *pk, const char *dn)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && pk && dn);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->unbind_sub_thing(thing_manager, pk, dn);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_login_sub_thing(void *_self, const void *sub_thing_id, const char *ds)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && sub_thing_id && ds);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->login_sub_thing(thing_manager, sub_thing_id, ds);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_logout_sub_thing(void *_self, const void *sub_thing_id)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && sub_thing_id);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->logout_sub_thing(thing_manager, sub_thing_id);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}

static int dm_impl_topo_delete_subthing(void *_self, const void *sub_thing_id)
{
    int res = 0;
    dm_impl_t *self = _self;
    thing_manager_t **thing_manager = self->_thing_manager;
    dm_thing_manager_t *thing_manager_object = self->_thing_manager;

    assert(thing_manager && *thing_manager && sub_thing_id);

    HAL_MutexLock(thing_manager_object->_usercall_mutex);
    res = (*thing_manager)->topo_delete_sub_thing(thing_manager, sub_thing_id);
    HAL_MutexUnlock(thing_manager_object->_usercall_mutex);

    return res;
}
#endif

static dm_t _dm_impl_class = {
    sizeof(dm_impl_t),
    string_dm_impl_class_name,
    dm_impl_ctor,
    dm_impl_dtor,
    dm_impl_generate_new_thing,
#ifdef SUBDEV_ENABLE
    dm_impl_generate_new_subthing,
    dm_impl_remove_subthing,
    dm_impl_bind_sub_thing,
    dm_impl_unbind_sub_thing,
    dm_impl_login_sub_thing,
    dm_impl_logout_sub_thing,
    dm_impl_topo_delete_subthing,
    dm_impl_add_subdev_callback_function,
#endif
    dm_impl_set_property_value,
    dm_impl_set_event_output_value,
    dm_impl_set_service_output_value,
    dm_impl_get_property_value,
    dm_impl_get_service_input_value,
    dm_impl_get_service_output_value,
    dm_impl_get_event_output_value,
    dm_impl_install_callback_function,
    dm_impl_trigger_event,
#ifdef EXTENDED_INFO_ENABLED
    dm_impl_trigger_deviceinfo_update,
    dm_impl_trigger_deviceinfo_delete,
#endif /* EXTENDED_INFO_ENABLED*/
    dm_impl_answer_service,
    dm_impl_invoke_raw_service,
    dm_impl_answer_raw_service,
#ifndef CM_SUPPORT_MULTI_THREAD
    dm_impl_yield,
#endif /* CM_SUPPORT_MULTI_THREAD */
};

const void *get_dm_impl_class()
{
    return &_dm_impl_class;
}
