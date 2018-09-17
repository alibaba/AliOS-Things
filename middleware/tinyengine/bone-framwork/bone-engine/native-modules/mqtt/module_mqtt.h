//
//  module_mqtt.h
//  bone
//
//  Created by howie on 06/12/2017.
//  Copyright © 2017 alibaba. All rights reserved.
//

#ifndef module_mqtt_h
#define module_mqtt_h

/*
 * JS MQTT Class
 *
 * API:
 * MQTT.start(config, callback)
 * MQTT.subscribe("reply", function(message){});
 * MQTT.unsubscribe("reply");
 * MQTT.publish(json);
 *
 * Event:
 */
void module_mqtt_load(void);

#endif /* module_mqtt_h */
