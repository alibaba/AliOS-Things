/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifdef AOS_TIMER_SERVICE
#include "cJSON.h"
#include "ccronexpr.h"
#include "stdio.h"
#include "timer_service.h"

static void *check_timer = NULL;

static callback_fun g_timer_service_cb = NULL;
static int utc_week_second_offset = -1;
static uint32_t utc = 0, uptime_on_get_utc= 0, ntp_time_s=0;
static int timezone = DEFAULT_TIMEZONEOFFSET;

static COUNTDOWN_TARGET_INIT();
static countdown_t g_countdown;

static const char** control_targets_list = NULL;
static int g_num_of_tsl_type[NUM_OF_TSL_TYPES];
static int g_control_targets_list_int[NUM_OF_CONTROL_TARGETS];
static double g_control_targets_list_float[NUM_OF_TARGETS_FLOAT];
static char g_control_targets_list_string[NUM_OF_TARGETS_STRING][STRING_MAX_LEN];
static int g_num_control_list = 0;

#ifdef ENABLE_COUNTDOWN_LIST
static const char** countdownlist_target_list = NULL;
static countdown_list_t g_countdown_list[NUM_OF_COUNTDOWNLIST];
static int g_countdown_list_value[NUM_OF_COUNTDOWN_LIST_TARGET];
static int g_num_countdownlist_target = 0;
#endif
#ifdef ENABLE_LOCALTIMER
static const char** localtimer_target_list = NULL;
static local_timer_t g_local_timer[NUM_OF_LOCALTIMER];
static int g_num_localtimer_list = 0;
static int local_timer_arrySize = 0;
#endif

#ifdef ENABLE_PERIOD_TIMER
static period_timer_t g_period_timer[NUM_OF_PERIOD_TIMER];
#endif

#ifdef ENABLE_RANDOM_TIMER
static random_timer_t g_random_timer[NUM_OF_RANDOM_TIMER];
#endif


int get_cjson_int(int *data,cJSON *cjson, const char *prop_name)
{
    cJSON * pItem = cJSON_GetObjectItem(cjson, prop_name);
    if(pItem != NULL && cJSON_IsNumber(pItem)) {
            *data = pItem->valueint;
            return 1;
    }
    return 0;
}

#ifdef ENABLE_LOCALTIMER
void local_timer_parse(const char *timing_string, int save)
{
    cJSON *timing_JSON, *item_JSON, *prop;
    int i,j;
    timing_JSON = cJSON_Parse(timing_string);
    if (timing_JSON == NULL) {
        return;
    }

    local_timer_arrySize = cJSON_GetArraySize(timing_JSON);

    EXAMPLE_TRACE("Timing %s LocalTimer arry size = %d\r\n", timing_string, local_timer_arrySize);

    if (local_timer_arrySize > NUM_OF_LOCALTIMER){
        local_timer_arrySize = NUM_OF_LOCALTIMER;
    }
    memset(g_local_timer, 0, sizeof(g_local_timer));
    for (j = 0; j < local_timer_arrySize; j++) {
        prop = cJSON_GetArrayItem(timing_JSON, j);

        utc_week_second_offset = (utc + ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000) % (86400 * 7);

        item_JSON = cJSON_GetObjectItem(prop, "Timer");
        if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
            const char *err = NULL;
            char cron_str[64];
            cron_expr target;
            cron_str[0] = '*';
            cron_str[1] = ' ';
            // strcpy(&cron_str[2], item_JSON->valuestring);
            strcpy(g_local_timer[j].cron_timer, item_JSON->valuestring);
            memset(cron_str, 0, sizeof(cron_str));
            snprintf(cron_str, sizeof(cron_str), "* %s", item_JSON->valuestring);
            memset(&target, 0, sizeof(cron_expr));
            cron_parse_expr(cron_str, &target, &err);
            // EXAMPLE_TRACE("expr %s", cron_str);
            if (cron_str[strlen(cron_str) - 1] == '*')
                target.days_of_week[0] = 0;
            if (err){
                EXAMPLE_TRACE("parse failed err %s", err);
                cJSON_Delete(timing_JSON);
                aos_kv_del(str_prop_name[LOCAL_TIMER]);
                return;
            }
            int hour, minute, offset;
            for (i = 0; i < HOURS_OF_DAY; i++) {
                if (cron_get_bit(target.hours, i)) {
                    hour = i;
                    break;
                }
            }

            for (i = 0; i < MINUTES_OF_HOUR; i++) {
                if (cron_get_bit(target.minutes, i)) {
                    minute = i;
                    break;
                }
            }

            get_cjson_int(&(g_local_timer[j].enable), prop, "Enable");
            timezone = DEFAULT_TIMEZONEOFFSET * 3600;
            if(get_cjson_int(&(g_local_timer[j].timezone_offset), prop, "TimeZoneOffset")){
                if (g_local_timer[j].enable == 1){
                    timezone = g_local_timer[j].timezone_offset;
                }
            }
            // EXAMPLE_TRACE("localtime enable %d idx=%d", g_local_timer[j].enable, j);
            EXAMPLE_TRACE("week %02x", target.days_of_week[0]);
            g_local_timer[j].repeat = 0;
            for (i = 0; i < DAYS_OF_WEEK; i++) {
                if (cron_get_bit(target.days_of_week, i)) {
                    offset = (i * 24 + hour) * 60 * SECONDS_OF_MINUTE + minute * SECONDS_OF_MINUTE + DEFAULT_TIMEZONEOFFSET * 3600 - timezone;
                    EXAMPLE_TRACE("offset %d  idx=%d, current=%d", offset, g_local_timer[j].repeat, utc_week_second_offset);
                    g_local_timer[j].offset[g_local_timer[j].repeat++] = offset;
                }
            }

            if (g_local_timer[j].repeat == 0) {
                for (i = 0; i <= DAYS_OF_WEEK; i++) {
                    offset = ( (i * 24 + hour) * 60 * SECONDS_OF_MINUTE + minute * SECONDS_OF_MINUTE )%(MINUTES_OF_WEEK * SECONDS_OF_MINUTE) +
                            DEFAULT_TIMEZONEOFFSET * 3600 - timezone;
                    if (offset > utc_week_second_offset) {
                        EXAMPLE_TRACE("offset %d current %d", offset, utc_week_second_offset);
                        g_local_timer[j].offset[0] = offset;
                        break;
                    }
                }
            }
        }

        get_cjson_int(&(g_local_timer[j].is_valid), prop, "IsValid");
        get_cjson_int(&(g_local_timer[j].action), prop, "PowerSwitch");

        item_JSON = cJSON_GetObjectItem(prop, "Targets");
        if (item_JSON != NULL && cJSON_IsNumber(item_JSON) && strlen(prop->valuestring) <= sizeof(g_local_timer[j].targets)) {
            memset(g_local_timer[j].targets, 0, STRING_MAX_LEN);
            memcpy(g_local_timer[j].targets, prop->valuestring, strlen(prop->valuestring));
        }

        for (i = 0; i < g_num_localtimer_list; i++){
            get_cjson_int(&(g_local_timer[j].value_list[i]), prop, localtimer_target_list[i]);
        }
    }
    cJSON_Delete(timing_JSON);
    if (save == 1){
        int ret = aos_kv_set(str_prop_name[LOCAL_TIMER], timing_string, strlen(timing_string), 1);
        EXAMPLE_TRACE("len=%d ret=%d", strlen(timing_string), ret);
    }
}
#endif

int string2time(const char *string)
{
    char hour[3], min[3];
    int offset;
    if (string[0] > '9' || string[1] > '9' || string[3] > '9' || string[4] > '9')
        return -1;
    strncpy(&hour[0], &string[0], 2);
    strncpy(&min[0], &string[3], 2);

    offset = atoi(hour) * 60;
    offset += atoi(min);

    return offset;
}

uint8_t string2week(const char *string)
{
    int i;
    uint8_t repeat[1] = { 0x00 };

    for (i = 0; i < strlen(string); i++) {
        if (string[i] >= '0' && string[i] <= '7') {
            cron_set_bit(repeat, (string[i] - '0') % 7);
        }
    }
    return repeat[0];
}
#ifdef ENABLE_RANDOM_TIMER
static int Curl_rand()
{
    int r, randseed;

    randseed = (unsigned int)aos_now_ms();
    randseed = randseed * 1103515245 + 12345;
    randseed = randseed * 1103515245 + 12345;
    randseed = randseed * 1103515245 + 12345;
    r = randseed = randseed * 1103515245 + 12345;
    return ( int )( (r << 16) | ((r >> 16) & 0xFFFF) );
}

void randomtimer_paramter_parse(const char *timing_string, int index, const char *tag)
{
    cJSON *arr_item, *item_JSON;
    int i = 0, minute_start, minute_end, idx = 0;

    arr_item = cJSON_Parse(timing_string);
    if (arr_item == NULL) {
        if (tag != NULL)
            aos_kv_del(tag);
        return;
    }
    if (!cJSON_IsObject(arr_item)){
        if (tag != NULL)
            aos_kv_del(tag);
        goto exit;
    }

    if (get_cjson_int(&(g_random_timer[index].enable), arr_item, "Enable") == 0)
        goto save;
    timezone = DEFAULT_TIMEZONEOFFSET * 3600;

    if (get_cjson_int(&(g_random_timer[index].timezoneOffset), arr_item, "TimeZoneOffset")){
        if (g_random_timer[index].enable == 1){
            timezone = g_random_timer[index].timezoneOffset /(SECONDS_OF_MINUTE * MINUTES_OF_HOUR);
        }
    }
    utc_week_second_offset = (utc + ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000) % (86400 * 7);

    item_JSON = cJSON_GetObjectItem(arr_item, "StartTime");
    if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
        if(strlen(item_JSON->valuestring) > 5){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        strcpy(g_random_timer[index].start, item_JSON->valuestring);
        int ret = string2time(item_JSON->valuestring);
        if (ret < 0){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        g_random_timer[index].offset_start[0] = ret;
        minute_start = g_random_timer[index].offset_start[0] + DEFAULT_TIMEZONEOFFSET * MINUTES_OF_HOUR - timezone * MINUTES_OF_HOUR;
        // EXAMPLE_TRACE("string %s", item_JSON->valuestring);
    } else  {
        memset(g_random_timer[index].start, 0, sizeof(g_random_timer[index].start));
        goto save;
    }

    item_JSON = cJSON_GetObjectItem(arr_item, "EndTime");
    if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
        if(strlen(item_JSON->valuestring) > 5){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        strcpy(g_random_timer[index].end, item_JSON->valuestring);
        int ret = string2time(item_JSON->valuestring);
        if (ret < 0){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        g_random_timer[index].offset_end[0] = ret;
        if ( g_random_timer[index].offset_end[0] < g_random_timer[index].offset_start[0])
            g_random_timer[index].offset_end[0] += 24 * 60;
        minute_end = g_random_timer[index].offset_end[0] + DEFAULT_TIMEZONEOFFSET * MINUTES_OF_HOUR - timezone * MINUTES_OF_HOUR;
        EXAMPLE_TRACE("string %s", item_JSON->valuestring);
    } else 
        goto save;

    item_JSON = cJSON_GetObjectItem(arr_item, "Repeat");
    if (item_JSON != NULL && cJSON_IsString(item_JSON)) {
        if(strlen(item_JSON->valuestring) > 16){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        memset(g_random_timer[index].repeat_raw, 0, sizeof(g_random_timer[index].repeat_raw));
        strcpy(g_random_timer[index].repeat_raw, item_JSON->valuestring);
        if (!strlen(item_JSON->valuestring))
            g_random_timer[index].repeat = 0;
        else
            g_random_timer[index].repeat = string2week(item_JSON->valuestring);
        // EXAMPLE_TRACE("string %s", item_JSON->valuestring);
    }
    int random_delay = 0;
    random_delay = Curl_rand()%RANDOM_MINUTE_LIMIT;
    EXAMPLE_TRACE("Curl_rand= %d", random_delay);
    if (g_random_timer[index].repeat != 0){
        for (i = 0; i < DAYS_OF_WEEK; i++) {
            if (cron_get_bit(&g_random_timer[index].repeat, i)) {
                g_random_timer[index].offset_start[idx] = ((i * 24) * 60 + minute_start + random_delay) * SECONDS_OF_MINUTE;
                g_random_timer[index].offset_end[idx] = ((i * 24) * 60 + minute_end + random_delay) * SECONDS_OF_MINUTE;
                EXAMPLE_TRACE("offset start=%d end=%d idx=%d", g_random_timer[index].offset_start[idx], g_random_timer[index].offset_end[idx], idx);
                ++idx;
            }
        }
    } else {
        for (i = 0; i <= DAYS_OF_WEEK; i++) {
            if ( (i * 24 * 60 + minute_start) * SECONDS_OF_MINUTE > utc_week_second_offset) {
                if ( (i * 24 * 60 + minute_start) * SECONDS_OF_MINUTE + random_delay * SECONDS_OF_MINUTE < utc_week_second_offset)
                    random_delay = -1 * random_delay;
                g_random_timer[index].offset_start[0] = ((i * 24) * 60 + minute_start + random_delay) * SECONDS_OF_MINUTE;
                g_random_timer[index].offset_end[0] = ((i * 24) * 60 + minute_end + random_delay) * SECONDS_OF_MINUTE;
                break;
            }
        }
    }
    
save:
    if (tag != NULL && g_random_timer[index].repeat != 0){
        int ret = aos_kv_set(tag, timing_string, strlen(timing_string), 1);
        EXAMPLE_TRACE("%s save len=%d ret=%d",tag, strlen(timing_string), ret);
    } else if (tag != NULL)
        aos_kv_del(tag);

exit:
    cJSON_Delete(arr_item);
}
#endif

#ifdef ENABLE_PERIOD_TIMER
void periodtimer_paramter_parse(const char *timing_string, int index, const char *tag)
{
    cJSON *arr_item, *item_JSON;
    int i = 0, minute_start, minute_end, idx = 0;

    arr_item = cJSON_Parse(timing_string);
    if (arr_item == NULL) {
        if (tag != NULL)
            aos_kv_del(tag);
        return;
    }
    if (!cJSON_IsObject(arr_item)){
        if (tag != NULL)
            aos_kv_del(tag);
        goto exit;
    }

    if (get_cjson_int(&(g_period_timer[index].enable), arr_item, "Enable") == 0)
        goto save;

    timezone = DEFAULT_TIMEZONEOFFSET * 3600;
    if (get_cjson_int(&(g_period_timer[index].timezoneOffset), arr_item, "TimeZoneOffset")){
        if (g_period_timer[index].enable == 1){
            timezone = g_period_timer[index].timezoneOffset /(SECONDS_OF_MINUTE * MINUTES_OF_HOUR);
        }
    }
    utc_week_second_offset = (utc + ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000) % (86400 * 7);
    item_JSON = cJSON_GetObjectItem(arr_item, "StartTime");
    if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
        if(strlen(item_JSON->valuestring) > 5){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        strcpy(g_period_timer[index].start, item_JSON->valuestring);
        int ret = string2time(item_JSON->valuestring);
        if (ret < 0){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        g_period_timer[index].offset_start[0] = ret;
        minute_start = g_period_timer[index].offset_start[0] + DEFAULT_TIMEZONEOFFSET * MINUTES_OF_HOUR - timezone * MINUTES_OF_HOUR;
        EXAMPLE_TRACE("string %s", item_JSON->valuestring);
    } else {
        memset(g_period_timer[index].start, 0, sizeof(g_period_timer[index].start));
        goto save;
    }

    item_JSON = cJSON_GetObjectItem(arr_item, "EndTime");
    if (item_JSON != NULL && strlen(item_JSON->valuestring)) {
        if(strlen(item_JSON->valuestring) > 5){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        strcpy(g_period_timer[index].end, item_JSON->valuestring);
        int ret = string2time(item_JSON->valuestring);
        if (ret < 0){
            if (tag != NULL)
                aos_kv_del(tag);
            goto exit;
        }
        g_period_timer[index].offset_end[0] = ret;
        if ( g_period_timer[index].offset_end[0] < g_period_timer[index].offset_start[0])
            g_period_timer[index].offset_end[0] += 24 * 60;
        minute_end = g_period_timer[index].offset_end[0] + DEFAULT_TIMEZONEOFFSET * MINUTES_OF_HOUR - timezone * MINUTES_OF_HOUR;
        EXAMPLE_TRACE("string %s", item_JSON->valuestring);
    } else 
        goto save;

    if (get_cjson_int(&(g_period_timer[index].run_time), arr_item, "RunTime") == 0)
        goto save;
    else
        g_period_timer[index].run_time *= SECONDS_OF_MINUTE;

    if (get_cjson_int(&(g_period_timer[index].sleep_time), arr_item, "SleepTime") == 0)
        goto save;
    else
        g_period_timer[index].sleep_time *= SECONDS_OF_MINUTE;

    item_JSON = cJSON_GetObjectItem(arr_item, "Repeat");
    if (item_JSON != NULL && cJSON_IsString(item_JSON)) {
        if(strlen(item_JSON->valuestring) > 16)
            goto exit;
        memset(g_period_timer[index].repeat_raw, 0, sizeof(g_period_timer[index].repeat_raw));
        strcpy(g_period_timer[index].repeat_raw, item_JSON->valuestring);
        if (!strlen(item_JSON->valuestring))
            g_period_timer[index].repeat = 0;
        else
            g_period_timer[index].repeat = string2week(item_JSON->valuestring);
        EXAMPLE_TRACE("repeat %x", g_period_timer[index].repeat);
    }

    if (g_period_timer[index].repeat != 0){
        for (i = 0; i < DAYS_OF_WEEK; i++) {
            if (cron_get_bit(&g_period_timer[index].repeat, i)) {
                g_period_timer[index].offset_start[idx] = ((i * 24) * 60 + minute_start) * SECONDS_OF_MINUTE;
                g_period_timer[index].offset_end[idx] = ((i * 24) * 60 + minute_end) * SECONDS_OF_MINUTE;
                EXAMPLE_TRACE("offset start=%d end=%d idx=%d", g_period_timer[index].offset_start[idx], g_period_timer[index].offset_end[idx], idx);
                ++idx;
            }
        }
    } else {
        for (i = 0; i <= DAYS_OF_WEEK; i++) {
            if ( (i * 24 * 60 + minute_start) * SECONDS_OF_MINUTE > utc_week_second_offset) {
                g_period_timer[index].offset_start[0] = ((i * 24) * 60 + minute_start) * SECONDS_OF_MINUTE;
                g_period_timer[index].offset_end[0] = ((i * 24) * 60 + minute_end) * SECONDS_OF_MINUTE;
                break;
            }
        }
    }

save:
    if (tag != NULL && g_period_timer[index].repeat != 0){
        int ret = aos_kv_set(tag, timing_string, strlen(timing_string), 1);
        EXAMPLE_TRACE("%s save len=%d ret=%d",tag, strlen(timing_string), ret);
    } else if (tag != NULL)
        aos_kv_del(tag);
exit:
    cJSON_Delete(arr_item);
}
#endif

void countdown_parse(char *data){
    cJSON *prop, *root;
    int i;
    // EXAMPLE_TRACE("countdown_parse, payload: \"%s\"", data);
    utc_week_second_offset = (utc + ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000) % (86400 * 7);
    root = cJSON_Parse(data);
    if (root != NULL && cJSON_IsObject(root)) {
        if (get_cjson_int(&(g_countdown.duration), prop, "TimeLeft")){
            g_countdown.duration = g_countdown.time_left + utc_week_second_offset;
        }

        get_cjson_int(&(g_countdown.is_running), prop, "IsRunning");
        prop = cJSON_GetObjectItem(root, "Timestamp");
        if (prop != NULL && cJSON_IsString(prop)){
            if (strlen(prop->valuestring) <= sizeof(g_countdown.timeStamp)){
                memset(g_countdown.timeStamp, 0, sizeof(g_countdown.timeStamp));
                memcpy(g_countdown.timeStamp, prop->valuestring, strlen(prop->valuestring));
                // EXAMPLE_TRACE("countdown_target_list: Timestamp=\"%s\"", g_countdown.timeStamp);
            }
        }
        for (i = 0; i < NUM_OF_COUNTDOWN_TARGET; i++){
            get_cjson_int(&(g_countdown.value_list[i]), prop, countdown_target_list[i]);
        }
    }
    cJSON_Delete(root);
}

#ifdef ENABLE_COUNTDOWN_LIST
void paraseContents(char *target, char *contents)
{
    char *ptr0, *ptr1, *ptr2;
    int i = 0, target_flag = 0;
    char tag[20];
    EXAMPLE_TRACE("target %s  contents %s", target, contents);
    utc_week_second_offset = (utc + ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000) % (86400 * 7);
    for (i = 0; i < g_num_countdownlist_target; i++){
        if (strcmp(target, countdownlist_target_list[i]) == 0)
            target_flag = 1;
        else
            continue;
        snprintf(tag, sizeof(tag), "%s%s", countdownlist_target_list[i], "-");
        ptr0 = strstr(contents, tag);
        if (ptr0 == NULL){ // DEL TARGET BY APP
            g_countdown_list[i].is_running = 0;
            memset(g_countdown_list[i].timeStamp, 0, sizeof(g_countdown_list[i].timeStamp));
            return;
        }
        EXAMPLE_TRACE("tag %s  ptr0=%s", tag, ptr0);
        if ((ptr1 = strchr(ptr0, '-')) != NULL) {
            ++ptr1;
            g_countdown_list[i].action = *ptr1 - '0';

            ptr1 += 2;
            g_countdown_list[i].is_running = *ptr1 - '0';

            ptr1 += 2;
            if (target_flag == 1){
                g_countdown_list[i].time_left = atoi(ptr1) + utc_week_second_offset;
                g_countdown_list[i].duration = atoi(ptr1);
            }
            ptr1 = strchr(ptr1, '-');
            ++ptr1;
            ptr2 = strchr(ptr1, ',');
            memset(g_countdown_list[i].timeStamp, 0, sizeof(g_countdown_list[i].timeStamp));
            if (ptr2 != NULL){
                memcpy(g_countdown_list[i].timeStamp, ptr1, ptr2 -ptr1);
            } else 
                memcpy(g_countdown_list[i].timeStamp, ptr1, strlen(ptr1));
        }
    }
}

void countdown_list_parse(char *data){
    cJSON *prop;
    int i;
    prop = cJSON_Parse(data);
    if (prop == NULL) {
        return;
    }
    if ( !cJSON_IsObject(prop))
        goto exit;

    cJSON *target = cJSON_GetObjectItem(prop, "target");
    if (target == NULL || !cJSON_IsString(target)) {
        goto exit;
    }
    cJSON *contents = cJSON_GetObjectItem(prop, "contents");
    if (contents != NULL && cJSON_IsString(contents)) {
        if (strlen(contents->valuestring) == 0) {
            /* clean timer settings */ 
            for (i = 0; i < g_num_countdownlist_target; i++){
                g_countdown_list[i].is_running = 0;
            }
        } else {
            paraseContents(target->valuestring, contents->valuestring);
        }
    }
exit:
    cJSON_Delete(prop);
}
#endif
char *timer_service_property_get(const char *request){
    int i,j, index;

    /* Parse Request */
    cJSON *request_root = cJSON_Parse(request);
    if (request_root == NULL || !cJSON_IsArray(request_root)) {
        EXAMPLE_TRACE("JSON Parse Error");
        return NULL;
    }
    
    cJSON *property_JSON = cJSON_CreateObject();
    if (property_JSON == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(request_root);
        return NULL;
    }
    cJSON *item_propertyid = NULL;
    for (index = 0; index < cJSON_GetArraySize(request_root); index++) {
        item_propertyid = cJSON_GetArrayItem(request_root, index);
        if (item_propertyid == NULL || !cJSON_IsString(item_propertyid)) {
            EXAMPLE_TRACE("JSON Parse Error");
            cJSON_Delete(request_root);
            cJSON_Delete(property_JSON);
            return NULL;
        }

        EXAMPLE_TRACE("Property ID, index: %d, Value: %s", index, item_propertyid->valuestring);
        if (strcmp("CountDown", item_propertyid->valuestring) == 0) {
            /* CountDown start */
            cJSON *list = cJSON_CreateObject();
            if (list == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(request_root);
                cJSON_Delete(property_JSON);
                return NULL;
            }
            if (g_countdown.duration > 0){
                cJSON_AddNumberToObject(list, "TimeLeft", g_countdown.duration);
                // cJSON_AddNumberToObject(list, "PowerSwitch", g_countdown.power_switch);
                cJSON_AddNumberToObject(list, "IsRunning", g_countdown.is_running);
                cJSON_AddStringToObject(list, "Timestamp", g_countdown.timeStamp);
                for (j = 0; j < NUM_OF_COUNTDOWN_TARGET; j++){
                    cJSON_AddNumberToObject(list, countdown_target_list[j], g_countdown.value_list[j]);
                    cJSON_AddNumberToObject(property_JSON, countdown_target_list[j], g_countdown.value_list[j]);
                }
            }
            cJSON_AddItemToObject(property_JSON, "CountDown", list);
            /* CountDown end */
        }
#ifdef ENABLE_COUNTDOWN_LIST
        else if (strcmp(str_prop_name[COUNT_DOWN_LIST], item_propertyid->valuestring) == 0) {
            /* CountDownList start */
            cJSON *list = cJSON_CreateObject();
            if (list == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(request_root);
                cJSON_Delete(property_JSON);
                return NULL;
            }
            cJSON_AddStringToObject(list, "Target", countdownlist_target_list[0]);
            char tmp[64], str[512];
            int content_idx = 0;
            memset(str, 0, sizeof(str));

            for (i = 0; i < g_num_countdownlist_target; i++){
                memset(tmp, 0, sizeof(tmp));
                if (strlen(g_countdown_list[i].timeStamp)){
                    snprintf(tmp, sizeof(tmp), "%s-%d-%d-%d-%s", countdownlist_target_list[i], g_countdown_list[i].action, 
                            g_countdown_list[i].is_running, g_countdown_list[i].duration, g_countdown_list[i].timeStamp);
                    content_idx = strlen(str);
                    if (content_idx == 0)
                        snprintf(&str[content_idx], strlen(tmp)+1, "%s", tmp);
                    else {
                        snprintf(&str[content_idx], strlen(tmp)+2, ",%s", tmp);
                    }
                    // EXAMPLE_TRACE("%s %s  %d",str, tmp, content_idx);
                    content_idx++;
                }
                cJSON_AddNumberToObject(list, countdownlist_target_list[i], g_countdown_list_value[i]);
            }
            cJSON_AddStringToObject(list, "Contents", str);
            cJSON_AddItemToObject(property_JSON, str_prop_name[COUNT_DOWN_LIST], list);
            /* CountDownList end */
        } 
#endif
#ifdef ENABLE_LOCALTIMER
        else if (strcmp(str_prop_name[LOCAL_TIMER], item_propertyid->valuestring) == 0) {
            /* LocalTimer start */
            cJSON *items[NUM_OF_LOCALTIMER];
            cJSON *array_localtimer = cJSON_CreateArray();
            if (array_localtimer == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(request_root);
                cJSON_Delete(property_JSON);
                return NULL;
            }
            for (i = 0; i < local_timer_arrySize; i++){
                items[i] = cJSON_CreateObject();
                if (items[i] == NULL) {
                    EXAMPLE_TRACE("No Enough Memory");
                    cJSON_Delete(request_root);
                    cJSON_Delete(property_JSON);
                    return NULL;
                }
                cJSON_AddStringToObject(items[i], "Timer", g_local_timer[i].cron_timer);
                cJSON_AddNumberToObject(items[i], "Enable", g_local_timer[i].enable);
                cJSON_AddNumberToObject(items[i], "IsValid", g_local_timer[i].is_valid);
                cJSON_AddStringToObject(items[i], "Targets", g_local_timer[i].targets);
                for (j = 0; j < g_num_localtimer_list; j++){
                    cJSON_AddNumberToObject(items[i], localtimer_target_list[j], g_local_timer[i].value_list[j]);
                }
                cJSON_AddItemToArray(array_localtimer, items[i]);
            }
            cJSON_AddItemToObject(property_JSON, str_prop_name[LOCAL_TIMER], array_localtimer);
            /* LocalTimer end*/
        } 
#endif
#ifdef ENABLE_PERIOD_TIMER
        else if (strstr(item_propertyid->valuestring, str_prop_name[PERIOD_TIMER]) != NULL){
            cJSON *period = cJSON_CreateObject();
            if (period == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(request_root);
                cJSON_Delete(property_JSON);
                return NULL;
            }
            char timer_name[16] = {0};
            for (i = 0; i < NUM_OF_PERIOD_TIMER; i++){
                memset(timer_name, 0, sizeof(timer_name));
                if(i == 0){
                    strcpy(timer_name, str_prop_name[PERIOD_TIMER]);
                } else
                    sprintf(timer_name, "%s_%d", str_prop_name[PERIOD_TIMER], i);
                if (strcmp(timer_name, item_propertyid->valuestring) == 0 && strlen(g_period_timer[i].start) > 0){
                    cJSON_AddStringToObject(period, "StartTime", g_period_timer[i].start);
                    cJSON_AddStringToObject(period, "EndTime", g_period_timer[i].end);
                    cJSON_AddNumberToObject(period, "TimeZoneOffset", g_period_timer[i].timezoneOffset);
                    cJSON_AddStringToObject(period, "Repeat", g_period_timer[i].repeat_raw);
                    cJSON_AddNumberToObject(period, "RunTime", g_period_timer[i].run_time/SECONDS_OF_MINUTE);
                    cJSON_AddNumberToObject(period, "SleepTime", g_period_timer[i].sleep_time/SECONDS_OF_MINUTE);
                    cJSON_AddNumberToObject(period, "Enable", g_period_timer[i].enable);
                    cJSON_AddNumberToObject(property_JSON, control_targets_list[0], g_control_targets_list_int[0]);
                    cJSON_AddItemToObject(property_JSON, timer_name, period);
                    break;
                } else {
                    cJSON_AddItemToObject(property_JSON, timer_name, period);
                }
            }
        }
#endif
#ifdef ENABLE_RANDOM_TIMER
        else if (strstr(item_propertyid->valuestring, str_prop_name[RANDOM_TIMER]) != NULL){
            cJSON *list_random = cJSON_CreateObject();
            if (list_random == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(request_root);
                cJSON_Delete(property_JSON);
                return NULL;
            }
            char timer_name_random[16] = {0};
            for (i = 0; i < NUM_OF_RANDOM_TIMER; i++){
                memset(timer_name_random, 0, sizeof(timer_name_random));
                if(i == 0){
                    strcpy(timer_name_random, str_prop_name[RANDOM_TIMER]);
                } else
                    sprintf(timer_name_random, "%s_%d", str_prop_name[RANDOM_TIMER], i);
                if (strcmp(timer_name_random, item_propertyid->valuestring) == 0 && strlen(g_random_timer[i].start) > 0){
                    cJSON_AddStringToObject(list_random, "StartTime", g_random_timer[i].start);
                    cJSON_AddStringToObject(list_random, "EndTime", g_random_timer[i].end);
                    cJSON_AddNumberToObject(list_random, "TimeZoneOffset", g_random_timer[i].timezoneOffset);
                    cJSON_AddStringToObject(list_random, "Repeat", g_random_timer[i].repeat_raw);
                    cJSON_AddNumberToObject(list_random, "Enable", g_random_timer[i].enable);
                    cJSON_AddNumberToObject(property_JSON, control_targets_list[0], g_control_targets_list_int[0]);
                    cJSON_AddItemToObject(property_JSON, timer_name_random, list_random);
                    break;
                } else {
                    cJSON_AddItemToObject(property_JSON, timer_name_random, list_random);
                }
            }
        }
#endif
        else {
            for (i = 0; i < g_num_control_list; i++){
                if (strcmp(control_targets_list[i], item_propertyid->valuestring) == 0){
                    if (i < g_num_of_tsl_type[0]){ /* int/enum/bool  */
                        cJSON_AddNumberToObject(property_JSON, control_targets_list[i], g_control_targets_list_int[i]);
                    }
                    else if (i < g_num_of_tsl_type[0]+g_num_of_tsl_type[1]){ /* float/double  */
                        cJSON_AddNumberToObject(property_JSON, control_targets_list[i], g_control_targets_list_float[i - g_num_of_tsl_type[0]]);
                    }
                    else { /* string/date  */
                        cJSON_AddStringToObject(property_JSON, control_targets_list[i], g_control_targets_list_string[i - g_num_of_tsl_type[0] - g_num_of_tsl_type[1]]);
                    }
                    break;
                }
            }
        }
    }
    cJSON_Delete(request_root);
    char *property = cJSON_PrintUnformatted(property_JSON);
    if (property == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(property_JSON);
        return NULL;
    }
    cJSON_Delete(property_JSON);
    EXAMPLE_TRACE("property_post:%s!", property);
    return property;
}
char *property_post(timer_service_type_t type, int idx){
    int i, j, k;
    char timer_name[16];
    cJSON *property_JSON = cJSON_CreateObject();
    if (property_JSON == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        return NULL;
    }
    if (type == COUNT_DOWN) {
        cJSON *list = cJSON_CreateObject();
        if (list == NULL) {
            EXAMPLE_TRACE("No Enough Memory");
            cJSON_Delete(property_JSON);
            return NULL;
        }
        if (g_countdown.duration > 0){
            cJSON_AddNumberToObject(list, "TimeLeft", g_countdown.duration);
            // cJSON_AddNumberToObject(list, "PowerSwitch", g_countdown.power_switch);
            cJSON_AddNumberToObject(list, "IsRunning", g_countdown.is_running);
            cJSON_AddStringToObject(list, "Timestamp", g_countdown.timeStamp);
            for (j = 0; j < NUM_OF_COUNTDOWN_TARGET; j++){
                cJSON_AddNumberToObject(list, countdown_target_list[j], g_countdown.value_list[j]);
                cJSON_AddNumberToObject(property_JSON, countdown_target_list[j], g_countdown.value_list[j]);
            }
            cJSON_AddItemToObject(property_JSON, "CountDown", list);
        }
    }
#ifdef ENABLE_COUNTDOWN_LIST
    else if (type == COUNT_DOWN_LIST){
        cJSON *list = cJSON_CreateObject();
        if (list == NULL) {
            EXAMPLE_TRACE("No Enough Memory");
            cJSON_Delete(property_JSON);
            return NULL;
        }
        cJSON_AddStringToObject(list, "Target", countdownlist_target_list[idx]);
        char tmp[64], str[512];
        int content_idx = 0;
        memset(str, 0, sizeof(str));

        for (i = 0; i < g_num_countdownlist_target; i++){
            memset(tmp, 0, sizeof(tmp));
            if (strlen(g_countdown_list[i].timeStamp) && g_countdown_list[i].is_running == 1){
                snprintf(tmp, sizeof(tmp), "%s-%d-%d-%d-%s", countdownlist_target_list[i], g_countdown_list[i].action, 
                        g_countdown_list[i].is_running, g_countdown_list[i].duration, g_countdown_list[i].timeStamp);
                content_idx = strlen(str);
                
                if (content_idx == 0)
                    snprintf(&str[content_idx], strlen(tmp)+1, "%s", tmp);
                else {
                    snprintf(&str[content_idx], strlen(tmp)+2, ",%s", tmp);
                }
                // EXAMPLE_TRACE("%s %s  %d",str, tmp, content_idx);
                content_idx++;
            }
            cJSON_AddNumberToObject(list, countdownlist_target_list[i], g_countdown_list_value[i]);

            for ( k = 0; k < g_num_control_list; k++){
                for (j=0; j < g_num_countdownlist_target; j++){
                    if (strcmp(control_targets_list[k], countdownlist_target_list[j]) == 0){
                        cJSON_AddNumberToObject(property_JSON, countdownlist_target_list[j], g_control_targets_list_int[j]);
                        break;
                    }
                }
            }
        }
        cJSON_AddStringToObject(list, "Contents", str);
        cJSON_AddItemToObject(property_JSON, str_prop_name[COUNT_DOWN_LIST], list);
    }
#endif
#ifdef ENABLE_LOCALTIMER
    else if(type == LOCAL_TIMER) {
        cJSON *items[NUM_OF_LOCALTIMER];
        cJSON *array_localtimer = cJSON_CreateArray();
        if (array_localtimer == NULL) {
            EXAMPLE_TRACE("No Enough Memory");
            cJSON_Delete(property_JSON);
            return NULL;
        }
        for (i = 0; i < local_timer_arrySize; i++){
            items[i] = cJSON_CreateObject();
            if (items[i] == NULL) {
                EXAMPLE_TRACE("No Enough Memory");
                cJSON_Delete(property_JSON);
                cJSON_Delete(array_localtimer);
                return NULL;
            }
            cJSON_AddStringToObject(items[i], "Timer", g_local_timer[i].cron_timer);
            cJSON_AddNumberToObject(items[i], "Enable", g_local_timer[i].enable);
            cJSON_AddNumberToObject(items[i], "IsValid", g_local_timer[i].is_valid);
            /* cJSON_AddStringToObject(items[i], "Targets", g_local_timer[i].targets); */
            for (j = 0; j < g_num_localtimer_list; j++){
                cJSON_AddNumberToObject(items[i], localtimer_target_list[j], g_local_timer[i].value_list[j]);
            }
            cJSON_AddItemToArray(array_localtimer, items[i]);
        }
        for (i = 0; i < g_num_control_list; i++){
            for (j = 0; j < g_num_localtimer_list; j++){
                if (strcmp(control_targets_list[i], localtimer_target_list[j]) == 0){
                    cJSON_AddNumberToObject(property_JSON, control_targets_list[i], g_control_targets_list_int[i]);
                    continue;
                }
            }
        }
        cJSON_AddItemToObject(property_JSON, str_prop_name[LOCAL_TIMER], array_localtimer);
    }
#endif 
#ifdef ENABLE_PERIOD_TIMER
    if (type == PERIOD_TIMER) {
        cJSON *list = cJSON_CreateObject();
        if (list == NULL) {
            EXAMPLE_TRACE("No Enough Memory");
            cJSON_Delete(property_JSON);
            return NULL;
        }
        if (strlen(g_period_timer[idx].start) > 0){
            memset(timer_name, 0, sizeof(timer_name));
            if(idx == 0){
                strcpy(timer_name, str_prop_name[PERIOD_TIMER]);
            } else
                sprintf(timer_name, "%s_%d", str_prop_name[PERIOD_TIMER], idx);
            cJSON_AddStringToObject(list, "StartTime", g_period_timer[idx].start);
            cJSON_AddStringToObject(list, "EndTime", g_period_timer[idx].end);
            cJSON_AddNumberToObject(list, "TimeZoneOffset", g_period_timer[idx].timezoneOffset);
            cJSON_AddStringToObject(list, "Repeat", g_period_timer[idx].repeat_raw);
            cJSON_AddNumberToObject(list, "RunTime", g_period_timer[idx].run_time/SECONDS_OF_MINUTE);
            cJSON_AddNumberToObject(list, "SleepTime", g_period_timer[idx].sleep_time/SECONDS_OF_MINUTE);
            cJSON_AddNumberToObject(list, "Enable", g_period_timer[idx].enable);
            cJSON_AddNumberToObject(property_JSON, control_targets_list[0], g_control_targets_list_int[0]);
            cJSON_AddItemToObject(property_JSON, timer_name, list);
        }
    }
#endif
#ifdef ENABLE_RANDOM_TIMER
    if (type == RANDOM_TIMER) {
        cJSON *list = cJSON_CreateObject();
        if (list == NULL) {
            EXAMPLE_TRACE("No Enough Memory");
            cJSON_Delete(property_JSON);
            return NULL;
        }
        if (strlen(g_random_timer[idx].start) > 0){
            memset(timer_name, 0, sizeof(timer_name));
            if(idx == 0){
                strcpy(timer_name, str_prop_name[RANDOM_TIMER]);
            } else
                sprintf(timer_name, "%s_%d", str_prop_name[RANDOM_TIMER], idx);
            cJSON_AddStringToObject(list, "StartTime", g_random_timer[idx].start);
            cJSON_AddStringToObject(list, "EndTime", g_random_timer[idx].end);
            cJSON_AddNumberToObject(list, "TimeZoneOffset", g_random_timer[idx].timezoneOffset);
            cJSON_AddStringToObject(list, "Repeat", g_random_timer[idx].repeat_raw);
            cJSON_AddNumberToObject(list, "Enable", g_random_timer[idx].enable);
            cJSON_AddNumberToObject(property_JSON, control_targets_list[0], g_control_targets_list_int[0]);
            cJSON_AddItemToObject(property_JSON, timer_name, list);
        }
    }
#endif

    char *property = cJSON_PrintUnformatted(property_JSON);
    if (property == NULL) {
        EXAMPLE_TRACE("No Enough Memory");
        cJSON_Delete(property_JSON);
        return NULL;
    }
    cJSON_Delete(property_JSON);
    EXAMPLE_TRACE("property_post:%s!", property);
    return property;
}
static int g_need_read_kv = 0;
void timer_service_ntp_update(const char *str)
{   
    if (strlen(str) < 4)
        return;
    char str_time[16] = {0};
    int str_len = strlen(str);
    memset(str_time, 0, sizeof(str_time));
    memcpy(str_time, str, str_len);
    str_time[str_len - 3] = 0;
    str_time[str_len - 2] = 0;
    str_time[str_len - 1] = 0;

    ntp_time_s = (uint32_t)atoi(str_time);

    utc = (ntp_time_s - SUNDAY - DEFAULT_TIMEZONEOFFSET * 3600) % (86400 * 7); // change from minute to second / 60;
    uptime_on_get_utc = (uint32_t)aos_now_ms();
    EXAMPLE_TRACE("utc=%s,ntp_time_s=%d  utc_week_offset=%d \n", str, ntp_time_s, utc);

    int i, ret;
#ifdef ENABLE_PERIOD_TIMER
    if ( g_need_read_kv == 0) {
        memset(&g_period_timer, 0, sizeof(g_period_timer));
        {
            char tag[16] = {0};
            char str[128] = {0};
            int len = sizeof(str);
            for (i = 0; i < NUM_OF_PERIOD_TIMER; i++){
                if (i == 0){
                    snprintf(tag, sizeof(tag), "%s", str_prop_name[PERIOD_TIMER]);
                } else {
                    snprintf(tag, sizeof(tag), "%s_%d", str_prop_name[PERIOD_TIMER], i);
                }
                ret = aos_kv_get(tag, &str, &len);
                EXAMPLE_TRACE("get kv %s, ret=%d!", tag, ret); 
                if (ret == 0){
                    periodtimer_paramter_parse(str, i, NULL);
                }
            }
        }
    }
#endif
#ifdef ENABLE_RANDOM_TIMER
    if ( g_need_read_kv == 0 || g_need_read_kv > 1) {
        memset(&g_random_timer, 0, sizeof(g_random_timer));
        {
            char tag[16] = {0};
            char str[128] = {0};
            int len = sizeof(str);
            for (i = 0; i < NUM_OF_RANDOM_TIMER; i++){
                if (i == 0){
                    snprintf(tag, sizeof(tag), "%s", str_prop_name[RANDOM_TIMER]);
                } else {
                    snprintf(tag, sizeof(tag), "%s_%d", str_prop_name[RANDOM_TIMER], i);
                }
                ret = aos_kv_get(tag, &str, &len);
                EXAMPLE_TRACE("get kv %s, ret=%d!", tag, ret); 
                if (ret == 0){
                    randomtimer_paramter_parse(str, i, NULL);
                }
            }
        }
    }
#endif

#ifdef ENABLE_LOCALTIMER
    if ( g_need_read_kv == 0) {
        memset(&g_local_timer, 0, sizeof(g_local_timer));  
        {
            int len = 150*NUM_OF_LOCALTIMER;
            char *str = (char *)HAL_Malloc(len);
            if (str == NULL) {
                EXAMPLE_TRACE("get kv localtimer, HAL_Malloc failed!"); 
                return;
            }
            memset(str, 0, len);
            ret = aos_kv_get(str_prop_name[LOCAL_TIMER], str, &len);
            EXAMPLE_TRACE("get kv localtimer, ret=%d!", ret); 
            if (ret == 0){
                local_timer_parse(str, 0);
            }
            HAL_Free(str);
        }
    }
#endif
    g_need_read_kv = 1;
}

static int _sec_next_event = -1;
static void timer_service_event_check()
{
    int i = 0, j,k, current_time;
    uint32_t ticket = ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000;
    if (ntp_time_s == 0 || ticket < 0 || (ticket > SECONDS_OF_DAY/4 && 
        (_sec_next_event == -1 || _sec_next_event - utc_week_second_offset > 10))) {
        if (ntp_time_s == 0 || ticket < 0){
            EXAMPLE_TRACE("ERR: must sync ntp time. ticket=%d, %d now=%d!\n", ticket, uptime_on_get_utc, aos_now_ms());
            linkkit_ntp_time_request(timer_service_ntp_update);
            return;
        }
        linkkit_ntp_time_request(timer_service_ntp_update);
    }
    _sec_next_event = -1;
    utc_week_second_offset = (utc + ticket) % (SECONDS_OF_DAY * 7);
    EXAMPLE_TRACE("offset = %d  uptimems=%d uptime_on_get_utc=%d", utc_week_second_offset, ticket, uptime_on_get_utc);
    
    if (g_countdown.is_running && g_countdown.time_left > utc_week_second_offset)
        _sec_next_event = g_countdown.time_left;
    if (g_countdown.is_running && g_countdown.time_left == utc_week_second_offset){
        g_countdown.is_running = 0;
        char *post_str = property_post(COUNT_DOWN, i);
        if (post_str != NULL){
            for (j = 0; j < NUM_OF_COUNTDOWN_TARGET; j++){
                g_control_targets_list_int[i] = g_countdown.value_list[i];
            }
            g_timer_service_cb(post_str, control_targets_list[0], g_control_targets_list_int[0], 0.0f, NULL, 0);
            HAL_Free(post_str);
        }
    }
    
#ifdef ENABLE_COUNTDOWN_LIST
    for (i = 0; i < g_num_countdownlist_target; i++){
        if (g_countdown_list[i].is_running && (_sec_next_event == -1 || g_countdown_list[i].time_left < _sec_next_event) && g_countdown_list[i].time_left > utc_week_second_offset)
            _sec_next_event = g_countdown_list[i].time_left;
        if (g_countdown_list[i].is_running && g_countdown_list[i].time_left == utc_week_second_offset){
            g_countdown_list[i].is_running = 0;
            for (k = 0; k < g_num_control_list; k++){
                if (strcmp(control_targets_list[k], countdownlist_target_list[i]) == 0){
                    g_control_targets_list_int[k] = g_countdown_list[i].action;
                    break;
                }
            }
            g_countdown_list_value[i] = g_countdown_list[i].action;
            char *post_str = property_post(COUNT_DOWN_LIST, i);
            if (post_str != NULL){
                g_timer_service_cb(post_str, countdownlist_target_list[i], g_control_targets_list_int[i], 0.0f, NULL, k);
                HAL_Free(post_str);
            }
        }
    }
#endif
#ifdef ENABLE_LOCALTIMER
    for (i = 0; i < local_timer_arrySize; i++){
        if (g_local_timer[i].enable == 1){
            int val_idx = 0;
            for ( j = 0; j < DAYS_OF_WEEK; j++){
                if ((_sec_next_event == -1 || g_local_timer[i].offset[j] < _sec_next_event) && g_local_timer[i].offset[j] > utc_week_second_offset)
                    _sec_next_event = g_local_timer[i].offset[j];
                if (g_local_timer[i].offset[j] == utc_week_second_offset){
                    if (g_local_timer[i].repeat == 0)
                        g_local_timer[i].enable = 0;
                    for ( k = 0; k < g_num_control_list; k++){
                        for (val_idx = 0; val_idx < g_num_localtimer_list; val_idx++){
                            if (strcmp(control_targets_list[k], localtimer_target_list[val_idx]) == 0){
                                g_control_targets_list_int[k] = g_local_timer[i].value_list[val_idx];
                                char *post_str = property_post(LOCAL_TIMER, i);
                                if (post_str != NULL){
                                    // EXAMPLE_TRACE("name = %s  val = %d\n",control_targets_list[k], g_control_targets_list_int[k]);
                                    g_timer_service_cb(post_str, control_targets_list[k], g_control_targets_list_int[k], 0.0f, NULL, k);
                                    HAL_Free(post_str);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
#endif
#ifdef ENABLE_PERIOD_TIMER
    int n_run_and_sleep = 0;
    int mod_run_and_sleep = 0;
    int tmp = 0;
    for (i = 0; i < NUM_OF_PERIOD_TIMER; i++){
        if (g_period_timer[i].enable){
            for (j = 0; j < DAYS_OF_WEEK; j++){
                if ((_sec_next_event == -1 || g_period_timer[i].offset_start[j] < _sec_next_event) && g_period_timer[i].offset_start[j] > utc_week_second_offset)
                    _sec_next_event = g_period_timer[i].offset_start[j];

                if (g_period_timer[i].offset_start[j] <= utc_week_second_offset && g_period_timer[i].offset_end[j] >= utc_week_second_offset){
                    EXAMPLE_TRACE("period offset_start = %d  offset_end = %d\n", g_period_timer[i].offset_start[j], g_period_timer[i].offset_end[j]);
                    n_run_and_sleep = (utc_week_second_offset - g_period_timer[i].offset_start[j])/(g_period_timer[i].run_time + g_period_timer[i].sleep_time);
                    mod_run_and_sleep = (utc_week_second_offset - g_period_timer[i].offset_start[j])%(g_period_timer[i].run_time + g_period_timer[i].sleep_time);

                    if (mod_run_and_sleep < g_period_timer[i].run_time ){
                        tmp = g_period_timer[i].offset_start[j] + (g_period_timer[i].run_time + g_period_timer[i].sleep_time) * n_run_and_sleep + g_period_timer[i].run_time;
                        if (tmp < _sec_next_event || _sec_next_event == -1)
                            _sec_next_event = tmp;
                    }
                    else{
                        tmp = g_period_timer[i].offset_start[j] + (g_period_timer[i].run_time + g_period_timer[i].sleep_time) * (n_run_and_sleep + 1);
                        if (tmp < _sec_next_event || _sec_next_event == -1)
                            _sec_next_event = tmp;
                    }

                    current_time = (utc_week_second_offset - g_period_timer[i].offset_start[j]) % (g_period_timer[i].run_time + g_period_timer[i].sleep_time);
                    if (current_time == 0){
                        g_control_targets_list_int[0] = 1;
                        char *post_str = property_post(PERIOD_TIMER, i);
                        if (post_str != NULL){
                            g_timer_service_cb(post_str, control_targets_list[0], g_control_targets_list_int[0], 0.0f, NULL, 1);
                            HAL_Free(post_str);
                        }
                    }
                    else if (current_time == g_period_timer[i].run_time){
                        g_control_targets_list_int[0] = 0;
                        char *post_str = property_post(PERIOD_TIMER, i);
                        if (post_str != NULL){
                            g_timer_service_cb(post_str, control_targets_list[0], g_control_targets_list_int[0], 0.0f, NULL, 1);
                            HAL_Free(post_str);
                        }
                    }
                }
            }
        }
    }
#endif

#ifdef ENABLE_RANDOM_TIMER
    for (i = 0; i < NUM_OF_RANDOM_TIMER; i++){
        if (g_random_timer[i].enable){
            for ( j = 0; j < DAYS_OF_WEEK; j++){
                if ((_sec_next_event == -1 || g_random_timer[i].offset_start[j] < _sec_next_event) && g_random_timer[i].offset_start[j] > utc_week_second_offset)
                    _sec_next_event = g_random_timer[i].offset_start[j];
                if ((_sec_next_event == -1 || g_random_timer[i].offset_end[j] < _sec_next_event) && g_random_timer[i].offset_end[j] > utc_week_second_offset)
                    _sec_next_event = g_random_timer[i].offset_end[j];

                if (g_random_timer[i].offset_start[j] == utc_week_second_offset){
                    g_control_targets_list_int[0] = 1;
                    char *post_str = property_post(RANDOM_TIMER, i);
                    if (post_str != NULL){
                        g_timer_service_cb(post_str, control_targets_list[0], g_control_targets_list_int[0], 0.0f, NULL, 1);
                        HAL_Free(post_str);
                    }
                }
                else if (g_random_timer[i].offset_end[j] == utc_week_second_offset){
                    g_control_targets_list_int[0] = 0;
                    g_need_read_kv++;
                    char *post_str = property_post(RANDOM_TIMER, i);
                    if (post_str != NULL){
                        g_timer_service_cb(post_str, control_targets_list[0], g_control_targets_list_int[0], 0.0f, NULL, 1);
                        HAL_Free(post_str);
                    }
                }
            }
        }
    }
#endif

    ticket = ((uint32_t)aos_now_ms() - uptime_on_get_utc)/1000;
    utc_week_second_offset = (utc + ticket) % (86400 * 7);
    int delay_sec = _sec_next_event - utc_week_second_offset-1;
    EXAMPLE_TRACE("next=%d", delay_sec);
    if (delay_sec > 2 && delay_sec < 5*SECONDS_OF_MINUTE){
        HAL_Timer_Stop(check_timer);
        HAL_Timer_Start(check_timer, delay_sec * 1000);
    } else if (delay_sec >= 0 && delay_sec <= 2) {
        aos_msleep(200);
        timer_service_event_check();
    } else {
        HAL_Timer_Stop(check_timer);
        HAL_Timer_Start(check_timer, 5*SECONDS_OF_MINUTE * 1000);
    }
}

int timer_service_init(const char **control_list, uint8_t num_control_list, \
                                    const char **countdownlist_target, uint8_t num_countdownlist_target, \
                                    const char **localtimer_list, uint8_t num_localtimer_list, \
                                    callback_fun timer_service_cb, int *num_of_tsl_type){
    int i, ret = linkkit_ntp_time_request(timer_service_ntp_update);
    if (check_timer == NULL) {
        check_timer = HAL_Timer_Create("timer_service", (void (*)(void *))timer_service_event_check, NULL);
    }
    if (check_timer == NULL) {
        EXAMPLE_TRACE("error: create timer failed!"); 
        return -1;
    }
    HAL_Timer_Stop(check_timer);
    HAL_Timer_Start(check_timer, 2000);

    if (ret != 0) {
        EXAMPLE_TRACE("ERR:ntp_time_request failed=%d!", ret);
        return -1;
    }
    if (timer_service_cb == NULL){
        EXAMPLE_TRACE("ERR: callback is NULL!");
        return -1;
    }
    if (num_control_list == 0){
        EXAMPLE_TRACE("ERR: num_control_list is invaild!");
        return -1;
    }
    int check_tsl_num = 0;
    if (num_of_tsl_type != NULL){
        for ( i = 0; i < NUM_OF_TSL_TYPES; i++){
            g_num_of_tsl_type[i] = num_of_tsl_type[i];
            check_tsl_num += g_num_of_tsl_type[i];
            EXAMPLE_TRACE("g_num_of_tsl_type[%d]=%d\n", i, g_num_of_tsl_type[i]);
        }
        // EXAMPLE_TRACE("max=%d num_control_list=%d check_tsl_num=%d\n", NUM_OF_CONTROL_TARGETS, num_control_list, check_tsl_num);
        if (check_tsl_num != num_control_list || num_control_list > NUM_OF_CONTROL_TARGETS){
            EXAMPLE_TRACE("ERR: check_tsl_num error! max=%d num=%d check=%d", NUM_OF_CONTROL_TARGETS, num_control_list, check_tsl_num);
            return -1;
        }
    }
    else {
        EXAMPLE_TRACE("ERR: num_of_tsl_type is invaild!");
        return -1;
    }
    control_targets_list = control_list;

    memset(&g_countdown, 0, sizeof(g_countdown));

    if (num_control_list > NUM_OF_CONTROL_TARGETS)
        g_num_control_list = NUM_OF_CONTROL_TARGETS;
    else
        g_num_control_list = num_control_list;
#ifdef ENABLE_COUNTDOWN_LIST
    countdownlist_target_list = countdownlist_target;
    if (num_countdownlist_target > NUM_OF_COUNTDOWN_LIST_TARGET)
        g_num_countdownlist_target = NUM_OF_COUNTDOWN_LIST_TARGET;
    else
        g_num_countdownlist_target = num_countdownlist_target;
    memset(&g_countdown_list, 0, sizeof(g_countdown_list));
#endif

#ifdef ENABLE_LOCALTIMER
    localtimer_target_list = localtimer_list;
    if (num_localtimer_list > NUM_OF_LOCAL_TIMER_TARGET)
        g_num_localtimer_list = NUM_OF_LOCAL_TIMER_TARGET;
    else
        g_num_localtimer_list = num_localtimer_list;
#endif

       
    EXAMPLE_TRACE("control=%d,countdownlist=%d,localtimer=%d", g_num_control_list,num_countdownlist_target,num_localtimer_list);
    g_timer_service_cb = timer_service_cb; 

    return 0;
}

int timer_service_property_set(const char* data)
{
    int ret = -1, i;
    int powerSwitch = 0;
    cJSON *root, *prop;

    root = cJSON_Parse(data);
    if (root == NULL) {
        EXAMPLE_TRACE("property set payload is not JSON format");
        return ret;
    }
    for (i = 0; i < g_num_control_list; i++){
        prop = cJSON_GetObjectItem(root, control_targets_list[i]);
        if (prop != NULL && cJSON_IsNumber(prop)) {
            if (i < g_num_of_tsl_type[0]){
                g_control_targets_list_int[i] = prop->valueint;
                EXAMPLE_TRACE("property set: %s:%d", control_targets_list[i], g_control_targets_list_int[i]);
                g_timer_service_cb(NULL, control_targets_list[i], g_control_targets_list_int[i], 0.0f, NULL, i);
                goto exit;
            }
            else{
                g_control_targets_list_float[i - g_num_of_tsl_type[0]] = prop->valuedouble;
                EXAMPLE_TRACE("property set: %s:%f", control_targets_list[i], prop->valuedouble);
                g_timer_service_cb(NULL, control_targets_list[i], 0, prop->valuedouble, NULL, i);
                goto exit;
                // EXAMPLE_TRACE("property set idx=%d: %s:%f int=%d", i, control_targets_list[i], g_control_targets_list_float[i - g_num_of_tsl_type[0]], prop->valueint);
            }
        }
        else if (prop != NULL && cJSON_IsString(prop)) {
            if (strlen(prop->valuestring) > STRING_MAX_LEN)
                goto exit;
            memset(g_control_targets_list_string[i - g_num_of_tsl_type[0] - g_num_of_tsl_type[1]], 0, STRING_MAX_LEN);
            memcpy(g_control_targets_list_string[i - g_num_of_tsl_type[0] - g_num_of_tsl_type[1]], prop->valuestring, strlen(prop->valuestring));
            g_timer_service_cb(NULL, control_targets_list[i], 0, 0.0f, prop->valuestring, i);
        }
    }

    prop = cJSON_GetObjectItem(root, "CountDown");
    if (prop != NULL && cJSON_IsObject(prop)) {
        char *timing_string = cJSON_PrintUnformatted(prop);
        if (timing_string != NULL) {
            countdown_parse( timing_string );
            HAL_Free(timing_string);
        }
    }

#ifdef ENABLE_COUNTDOWN_LIST
    prop = cJSON_GetObjectItem(root, str_prop_name[COUNT_DOWN_LIST]);
    if (prop != NULL && cJSON_IsObject(prop)) {
        char *timing_string = cJSON_PrintUnformatted(prop);
        if (timing_string != NULL) {
            countdown_list_parse( timing_string );
            HAL_Free(timing_string);
        }
    }
#endif
#ifdef ENABLE_LOCALTIMER
    prop = cJSON_GetObjectItem(root, str_prop_name[LOCAL_TIMER]);
    if (prop != NULL && cJSON_IsArray(prop)) {
        char *timing_string = cJSON_PrintUnformatted(prop);
        if (timing_string != NULL) {
            local_timer_parse(timing_string, 1);
            HAL_Free(timing_string);
        }
    }
#endif
#ifdef ENABLE_RANDOM_TIMER
    for (i = 0; i < NUM_OF_RANDOM_TIMER; i++){
        char tag[16] = {0};
        if (i == 0){
            snprintf(tag, sizeof(tag), "%s", str_prop_name[RANDOM_TIMER]);
        } else {
            snprintf(tag, sizeof(tag), "%s_%d", str_prop_name[RANDOM_TIMER], i);
        }
        prop = cJSON_GetObjectItem(root, tag);
        if (prop != NULL && cJSON_IsObject(prop)) {
            char *timing_string = cJSON_PrintUnformatted(prop);
            if (timing_string != NULL) {
                randomtimer_paramter_parse(timing_string, i, tag);
                HAL_Free(timing_string);
            }
            break;
        }
    }
#endif
#ifdef ENABLE_PERIOD_TIMER
    for (i = 0; i < NUM_OF_PERIOD_TIMER; i++){
        char tag[16] = {0};
        if (i == 0){
            snprintf(tag, sizeof(tag), "%s", str_prop_name[PERIOD_TIMER]);
        } else {
            snprintf(tag, sizeof(tag), "%s_%d", str_prop_name[PERIOD_TIMER], i);
        }
        prop = cJSON_GetObjectItem(root, tag);
        if (prop != NULL && cJSON_IsObject(prop)) {
            char *timing_string = cJSON_PrintUnformatted(prop);
            if (timing_string != NULL) {
                periodtimer_paramter_parse(timing_string, i, tag);
                HAL_Free(timing_string);
            }
            break;
        }
    }
#endif    
    timer_service_event_check();
exit:
    cJSON_Delete(root);
}

#endif

