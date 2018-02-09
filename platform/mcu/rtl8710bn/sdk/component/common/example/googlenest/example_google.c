#include "cmsis_os.h"
#include "diag.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "google/google_nest.h"

#include <googlenest/example_google.h>
#include "cJSON.h"

osThreadId google_thread_id;
#define malloc      pvPortMalloc
#define free        vPortFree

void google_data_retrieve(char *response_buf);

void google_data_retrieve(char *response_buf) {
	//printf("\r\n\r\n\r\nResponse_buf:\r\n%s\r\n", response_buf);
	char *event = NULL;
	char *delims = "\n";
	char *data = NULL, *backup = NULL;
	char *info = NULL;
	cJSON_Hooks memoryHook;

	event = strtok_r(response_buf, delims, &backup);
	data = strtok_r( NULL, delims, &backup );

	if (!strncmp(data, "data: ", strlen("data: "))){
		info = data + strlen("data: ");
		if(info != NULL){
			memoryHook.malloc_fn = malloc;
			memoryHook.free_fn = free;
			cJSON_InitHooks(&memoryHook);

			cJSON *infoJSObject, *pathJSObject, *dataJSObject;
			cJSON *redJSObject, *greenJSObject, *blueJSObject;
			char *red, *green, *blue;


			if((infoJSObject = cJSON_Parse(info)) != NULL) {
				pathJSObject = cJSON_GetObjectItem(infoJSObject, "path");
				dataJSObject = cJSON_GetObjectItem(infoJSObject, "data");
				if(dataJSObject != NULL) {
					
					redJSObject = cJSON_GetObjectItem(dataJSObject, "Red");
					greenJSObject = cJSON_GetObjectItem(dataJSObject, "Green");
					blueJSObject = cJSON_GetObjectItem(dataJSObject, "Blue");

					if(redJSObject) 
						red = redJSObject->valuestring;

					if(greenJSObject)
						green = greenJSObject->valuestring;

					if(blueJSObject)
						blue = blueJSObject->valuestring;

					printf("\n\rThe latest RGB information: RGB(%s, %s, %s)\n\r", red, green, blue);

					cJSON_Delete(dataJSObject);
				}
				cJSON_Delete(infoJSObject);
			}
			else
				printf("\r\nCannot parse the message to JSON!\r\n");
			
		}
		else
			printf("\r\n This is the keep alive message or cannot get the information!\r\n");
	}
	else
		printf("\r\nData structure may wrong!\r\n");
}


void gn_todevice_start(void) {

	googlenest_context googlenest;
	char *googlenest_host = HOST_ADDR;
	char *googlenest_uri = "light.json";

	printf("\r\nStart connecting to Google Nest Server...\r\n");
	memset(&googlenest, 0, sizeof(googlenest_context));

reconnect:
	if(gn_connect(&googlenest, googlenest_host, GN_PORT) == 0) {
		printf("\r\n Connection is OK!\r\n");
		
		google_retrieve_data_hook_callback(google_data_retrieve);
		if(gn_stream(&googlenest, googlenest_uri) != 0){
			printf("\r\n Connection is fail! \r\n Start Reconnecting...\r\n");
			goto reconnect;
		}
			
		gn_close(&googlenest);
	
	}
	else{ 
		printf("\r\n Connection is fail! \r\n\r\n\r\n\r\nStart Reconnecting...\r\n");
		goto reconnect;
	}

}

void gn_fromdevice_start(int type) {
	googlenest_context googlenest;
	char *googlenest_uri_1 = "MotionSensor.json";
	char *googlenest_uri_2 = ".json";
	cJSON_Hooks memoryHook;
	int j = 0;
	char a[3],b[3],c[3];

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);
	printf("\r\nStart connecting to Google Nest Server to update data!\r\n");
	
	while(1) {
		memset(&googlenest, 0, sizeof(googlenest_context));
		if(gn_connect(&googlenest, HOST_ADDR, GN_PORT) == 0) {
			char *data;
			//For the example "From Device"
			if(type == 1){
				cJSON *MSJSObject;
				if((MSJSObject = cJSON_CreateObject()) != NULL) {
					cJSON_AddItemToObject(MSJSObject, "MotionSensor", cJSON_CreateNumber(j++));
					data = cJSON_Print(MSJSObject);
					cJSON_Delete(MSJSObject);
				}

				if(gn_put(&googlenest, googlenest_uri_1, data) == 0)
					printf("\n\rUpdate the Motion Sensor's data to %d\n\r", (j-1));
			}
			//For the example "BOTH"
			else if(type == 2){
				
				cJSON *dataJSObject, *lightJSObject, *MSJSObject;
				if((j+4)>256)
					j = 0;

				sprintf(a,"%d", j);
				sprintf(b,"%d", (j+2));
				sprintf(c,"%d", (j+4));
				
				if((dataJSObject = cJSON_CreateObject()) != NULL) {

					if((lightJSObject = cJSON_CreateObject()) != NULL) {
						cJSON_AddItemToObject(dataJSObject, "light", lightJSObject);
						cJSON_AddItemToObject(lightJSObject, "Red", cJSON_CreateString(a));
						cJSON_AddItemToObject(lightJSObject, "Green", cJSON_CreateString(b));
						cJSON_AddItemToObject(lightJSObject, "Blue", cJSON_CreateString(c));
						
					}
					
					if((MSJSObject = cJSON_CreateObject()) != NULL) {
						cJSON_AddItemToObject(dataJSObject, "MotionSensor", MSJSObject);
						cJSON_AddItemToObject(MSJSObject, "MotionSensor", cJSON_CreateNumber(j++));
					}
					data = cJSON_Print(dataJSObject);
					cJSON_Delete(dataJSObject);
				}
				
				if(gn_put(&googlenest, googlenest_uri_2, data) == 0)
					printf("\n\rUpdate data OK\r\n");

			}
				
			free(data);		
			gn_close(&googlenest);
		}
		else{
			printf("\n\rConnection failed!\n\r");
			break;
		}

		vTaskDelay(5 * configTICK_RATE_HZ);
	}
}

void gn_fromdevice_task(void *arg) {
	int i, j = 1;

	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 2 mins to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	for (i = 0; i<120; i++) 
		vTaskDelay(1000 / portTICK_PERIOD_MS);

	gn_fromdevice_start(j);
}

void gn_todevice_task(void *arg) {
	int i;

	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 2 mins to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	for (i = 0; i<120; i++)
		vTaskDelay(1000 / portTICK_PERIOD_MS);

	gn_todevice_start();
}

void gn_both_fromdevice_task(void *arg) {
	int i, j = 2;

	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 2 mins to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	
	for (i = 0; i<120; i++)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	printf("\n\r\n\r\n\r\n\r<<<<<<Starting update data to Google Nest Server>>>>>>>\n\r\n\r\n\r\n\r");
	gn_fromdevice_start(j);
}

void gn_both_todevice_task(void *arg) {
	int i;
	
	for (i = 0; i<150; i++)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	
	printf("\n\r\n\r\n\r\n\r<<<<<<Starting retrieving data from Google Nest Server>>>>>>>\n\r\n\r\n\r\n\r");
	gn_todevice_start();
}

void example_google(char *type) {


	if(strcmp(type, "FromDevice") == 0){
		if(xTaskCreate(gn_fromdevice_task, ((const char*)"gn_fromdevice_task"), 768, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}
	else if(strcmp(type, "ToDevice") == 0){
		if(xTaskCreate(gn_todevice_task, ((const char*)"gn_todevice_task"), 1000, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}
	else if(strcmp(type, "BOTH") == 0){              
		if(xTaskCreate(gn_both_fromdevice_task, ((const char*)"gn_both_fromdevice_task"), 768, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
		if(xTaskCreate(gn_both_todevice_task, ((const char*)"gn_both_todevice_task"), 1000, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
			printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}

}

