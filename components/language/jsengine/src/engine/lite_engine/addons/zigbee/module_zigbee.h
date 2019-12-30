/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef MODULE_ZIGBEE_H
#define MODULE_ZIGBEE_H

/*
 * JS Zigbee Class
 *
 * API:
 * Zigbee.start(config)
 * Zigbee.on("reply", function(message){});
 * Zigbee.publish(json);
 *
 * Event:
 */

void module_zigbee_load(void);

#endif /* MODULE_ZIGBEE_H */
