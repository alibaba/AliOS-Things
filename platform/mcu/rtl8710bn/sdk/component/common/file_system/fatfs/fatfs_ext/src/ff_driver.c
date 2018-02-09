#include <fatfs_ext/inc/ff_driver.h>

ff_disk_drv  disk = {0};

// return drv_num assigned
int FATFS_RegisterDiskDriver(ll_diskio_drv *drv){
	unsigned char drv_num = -1;

	if(disk.nbr < _VOLUMES)
	{
	  drv->drv_num = disk.nbr;	// record driver number for a specific disk
	  disk.drv[disk.nbr] = drv;
	  disk.nbr++;
	  drv_num = drv->drv_num;
	}
	return drv_num;
}

int FATFS_UnRegisterDiskDriver(unsigned char drv_num){
	int index;

	if(disk.nbr >= 1)
	{
		for(index=0;index<disk.nbr;index++){
			if(disk.drv[index]->drv_num == drv_num){
				disk.drv[index] = 0;
		  		disk.nbr--;
				return 0;
			}
		}
		return -1; // fail
	}
	return -1; // no disk driver registered
}


/**
  * @brief  Gets number of linked drivers to the FatFs module.
  * @param  None
  * @retval Number of attached drivers.
  */
int FATFS_getDrivernum(unsigned char* TAG)
{
	ll_diskio_drv *drv;
	int index;

	for(index=0;index<disk.nbr;index++){
		drv = disk.drv[index];
		if(!strcmp(drv->TAG, TAG)){
			return drv->drv_num;
		}
	}
  	return -1;
}
