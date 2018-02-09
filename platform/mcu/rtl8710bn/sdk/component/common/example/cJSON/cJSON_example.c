#include "cmsis_os.h"
#include <cJSON.h>

#define malloc      pvPortMalloc
#define free        vPortFree

/*  The data structure for this example

{	
	"Motion_Sensor" : "i",
	"Light" : {	
			"Red" : "0",
			"Green" : "0",
			"Blue" : "0",
			}

}

*/
static void gen_json_data(int i, int r, int g, int b)
{


	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);

	
	cJSON *IOTJSObject = NULL, *colorJSObject = NULL;
	char *iot_json = NULL;
	
	if((IOTJSObject = cJSON_CreateObject()) != NULL) {

		cJSON_AddItemToObject(IOTJSObject, "Motion_Sensor", cJSON_CreateNumber(i));
		cJSON_AddItemToObject(IOTJSObject, "Light", colorJSObject = cJSON_CreateObject());

		cJSON_AddItemToObject(colorJSObject, "Red", cJSON_CreateNumber(r));
		cJSON_AddItemToObject(colorJSObject, "Green", cJSON_CreateNumber(g));
		cJSON_AddItemToObject(colorJSObject, "Blue", cJSON_CreateNumber(b));

		iot_json = cJSON_Print(IOTJSObject);
		cJSON_Delete(IOTJSObject);
		
	}

}

static void handle_json_data(char *iot_json)
{
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);


	cJSON *IOTJSObject, *sensorJSObject, *lightJSObject, *redJSObject, *greenJSObject, *blueJSObject;
	int sensor_data, red, green, blue;

	if((IOTJSObject = cJSON_Parse(iot_json)) != NULL) {
		sensorJSObject = cJSON_GetObjectItem(IOTJSObject, "Motion_Sensor");
		if(sensorJSObject) 
			sensor_data = sensorJSObject->valueint;
			
		lightJSObject = cJSON_GetObjectItem(IOTJSObject, "Light");

		if(lightJSObject){
			redJSObject = cJSON_GetObjectItem(lightJSObject, "Red");
			greenJSObject = cJSON_GetObjectItem(lightJSObject, "Green");
			blueJSObject = cJSON_GetObjectItem(lightJSObject, "Blue");

			if(redJSObject)
				red = redJSObject->valueint;
			if(greenJSObject)
				green = greenJSObject->valueint;
			if(blueJSObject)
				blue = blueJSObject->valueint;
		}

		cJSON_Delete(IOTJSObject);
	}
}
