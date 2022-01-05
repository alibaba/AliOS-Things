#include "mqtt_outbox.h"
#include <stdlib.h>
#include <string.h>
#include "sys/queue.h"
#include "esp_log.h"

#ifndef CONFIG_MQTT_CUSTOM_OUTBOX


static const char *TAG = "OUTBOX";

typedef struct outbox_item {
    char *buffer;
    int len;
    int msg_id;
    int msg_type;
    int msg_qos;
    outbox_tick_t tick;
    int retry_count;
    pending_state_t pending;
    STAILQ_ENTRY(outbox_item) next;
} outbox_item_t;

STAILQ_HEAD(outbox_list_t, outbox_item);


outbox_handle_t outbox_init(void)
{
    outbox_handle_t outbox = calloc(1, sizeof(struct outbox_list_t));
    ESP_MEM_CHECK(TAG, outbox, return NULL);
    STAILQ_INIT(outbox);
    return outbox;
}

outbox_item_handle_t outbox_enqueue(outbox_handle_t outbox, outbox_message_handle_t message, outbox_tick_t tick)
{
    outbox_item_handle_t item = calloc(1, sizeof(outbox_item_t));
    ESP_MEM_CHECK(TAG, item, return NULL);
    item->msg_id = message->msg_id;
    item->msg_type = message->msg_type;
    item->msg_qos = message->msg_qos;
    item->tick = tick;
    item->len =  message->len + message->remaining_len;
    item->pending = QUEUED;
    item->buffer = malloc(message->len + message->remaining_len);
    ESP_MEM_CHECK(TAG, item->buffer, {
        free(item);
        return NULL;
    });
    memcpy(item->buffer, message->data, message->len);
    if (message->remaining_data) {
        memcpy(item->buffer + message->len, message->remaining_data, message->remaining_len);
    }
    STAILQ_INSERT_TAIL(outbox, item, next);
    ESP_LOGD(TAG, "ENQUEUE msgid=%d, msg_type=%d, len=%d, size=%d", message->msg_id, message->msg_type, message->len + message->remaining_len, outbox_get_size(outbox));
    return item;
}

outbox_item_handle_t outbox_get(outbox_handle_t outbox, int msg_id)
{
    outbox_item_handle_t item;
    STAILQ_FOREACH(item, outbox, next) {
        if (item->msg_id == msg_id) {
            return item;
        }
    }
    return NULL;
}

outbox_item_handle_t outbox_dequeue(outbox_handle_t outbox, pending_state_t pending, outbox_tick_t *tick)
{
    outbox_item_handle_t item;
    STAILQ_FOREACH(item, outbox, next) {
        if (item->pending == pending) {
            if (tick) {
                *tick = item->tick;
            }
            return item;
        }
    }
    return NULL;
}

uint8_t *outbox_item_get_data(outbox_item_handle_t item,  size_t *len, uint16_t *msg_id, int *msg_type, int *qos)
{
    if (item) {
        *len = item->len;
        *msg_id = item->msg_id;
        *msg_type = item->msg_type;
        *qos = item->msg_qos;
        return (uint8_t *)item->buffer;
    }
    return NULL;
}

esp_err_t outbox_delete(outbox_handle_t outbox, int msg_id, int msg_type)
{
    outbox_item_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, outbox, next, tmp) {
        if (item->msg_id == msg_id && (0xFF & (item->msg_type)) == msg_type) {
            STAILQ_REMOVE(outbox, item, outbox_item, next);
            free(item->buffer);
            free(item);
            ESP_LOGD(TAG, "DELETED msgid=%d, msg_type=%d, remain size=%d", msg_id, msg_type, outbox_get_size(outbox));
            return ESP_OK;
        }

    }
    return ESP_FAIL;
}
esp_err_t outbox_delete_msgid(outbox_handle_t outbox, int msg_id)
{
    outbox_item_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, outbox, next, tmp) {
        if (item->msg_id == msg_id) {
            STAILQ_REMOVE(outbox, item, outbox_item, next);
            free(item->buffer);
            free(item);
        }

    }
    return ESP_OK;
}
esp_err_t outbox_set_pending(outbox_handle_t outbox, int msg_id, pending_state_t pending)
{
    outbox_item_handle_t item = outbox_get(outbox, msg_id);
    if (item) {
        item->pending = pending;
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t outbox_set_tick(outbox_handle_t outbox, int msg_id, outbox_tick_t tick)
{
    outbox_item_handle_t item = outbox_get(outbox, msg_id);
    if (item) {
        item->tick = tick;
        return ESP_OK;
    }
    return ESP_FAIL; 
}

esp_err_t outbox_delete_msgtype(outbox_handle_t outbox, int msg_type)
{
    outbox_item_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, outbox, next, tmp) {
        if (item->msg_type == msg_type) {
            STAILQ_REMOVE(outbox, item, outbox_item, next);
            free(item->buffer);
            free(item);
        }

    }
    return ESP_OK;
}

int outbox_delete_expired(outbox_handle_t outbox, outbox_tick_t current_tick, outbox_tick_t timeout)
{
    int deleted_items = 0;
    outbox_item_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, outbox, next, tmp) {
        if (current_tick - item->tick > timeout) {
            STAILQ_REMOVE(outbox, item, outbox_item, next);
            free(item->buffer);
            free(item);
            deleted_items ++;
        }

    }
    return deleted_items;
}

int outbox_get_size(outbox_handle_t outbox)
{
    int siz = 0;
    outbox_item_handle_t item;
    STAILQ_FOREACH(item, outbox, next) {
        // Suppressing "use after free" warning as this could happen only if queue is in inconsistent state
        // which never happens if STAILQ interface used
        siz += item->len; // NOLINT(clang-analyzer-unix.Malloc)
    }
    return siz;
}

esp_err_t outbox_cleanup(outbox_handle_t outbox, int max_size)
{
    while (outbox_get_size(outbox) > max_size) {
        outbox_item_handle_t item = outbox_dequeue(outbox, CONFIRMED, NULL);
        if (item == NULL) {
            return ESP_FAIL;
        }
        STAILQ_REMOVE(outbox, item, outbox_item, next);
        free(item->buffer);
        free(item);
    }
    return ESP_OK;
}

void outbox_destroy(outbox_handle_t outbox)
{
    outbox_item_handle_t item, tmp;
    STAILQ_FOREACH_SAFE(item, outbox, next, tmp) {
        STAILQ_REMOVE(outbox, item, outbox_item, next);
        free(item->buffer);
        free(item);
    }
    free(outbox);
}

#endif /* CONFIG_MQTT_CUSTOM_OUTBOX */
