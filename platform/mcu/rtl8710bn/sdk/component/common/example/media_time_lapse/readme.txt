
THis is an example for USB Video Capture specifically for motion-jpeg capturing. 

Please MAKE SURE to reserve enough heap size for UVC by raising configTOTAL_HEAP_SIZE in freeRTOSconfig.h & turning off some functions (e.g. WPS, JDSMART, ATcmd for internal and system) since image frame storing could consume quite large memory space.

You may switch to UVC workspace to run the example for all settings has been made for you to run example well.