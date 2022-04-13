#ifndef SENSOR_CALIBRATE_H
#define SENSOR_CALIBRATE_H
#include "sensor/sensor.h"


int sensor_calibrate_entry(sensor_tag_e tag,int sensor_index,int axis, void *buf);
int sensor_calibrate_entry_default(sensor_tag_e tag,int sensor_index,int axis, void *buf);

#endif
