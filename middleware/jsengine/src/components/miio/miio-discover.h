#ifndef MIIO_DISCOVER_H
#define MIIO_DISCOVER_H

typedef void (*miio_discover_callback)(void *priv, char *host, long device_id);

void miio_device_discover(int timeout, void *priv, miio_discover_callback cb);

#endif /* MIIO_DISCOVER_H */
