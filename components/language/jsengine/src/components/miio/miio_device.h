#ifndef MIIO_DEVICE_H
#define MIIO_DEVICE_H

typedef struct miio_device miio_device_t;

miio_device_t *miio_device_create(const char *host, const char *token);

const char *miio_device_control(miio_device_t *device, const char *method,
                                const char *args, const char *sid);

typedef void (*miio_device_event_callback)(void *priv, const char *event);

void miio_device_set_event_cb(miio_device_t *device,
                              miio_device_event_callback cb, void *priv);

#endif /* MIIO_DEVICE_H */
