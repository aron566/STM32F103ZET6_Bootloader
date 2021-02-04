/**
 *  @file disk_port.h
 *
 *  @date 2021-01-04
 *
 *  @author aron566
 *
 *  @brief 磁盘接口驱动
 *  
 *  @version V1.0
 */
#ifndef DISK_PORT_H
#define DISK_PORT_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< nedd definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/
#include "main.h"
#include "ff.h"
#include "diskio.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*磁盘状态读取*/
typedef DSTATUS(*pdisk_status_func)(void);
/*磁盘初始化*/
typedef DSTATUS(*pdisk_initialize_func)(void);
/*磁盘读取数据*/
typedef DRESULT(*pdisk_read_func)(
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
);
/*磁盘写入数据*/
typedef DRESULT (*pdisk_write_func)(
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
);
/*磁盘信息*/
typedef DRESULT (*pdisk_ioctl_func)(
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
);

typedef struct disk_drv
{
  pdisk_status_func get_disk_status_port;
  pdisk_initialize_func disk_init_port;
  pdisk_read_func disk_read_port;
  pdisk_write_func disk_write_port;
  pdisk_ioctl_func disk_ioctl_port;
}DISK_DRV_FUNC_MAP_Typedef_t;
/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#define DEV_SPI_FLASH 3
#define DEV_TYPE_MAX DEV_SPI_FLASH+1
/** Exported variables -------------------------------------------------------*/
extern DISK_DRV_FUNC_MAP_Typedef_t disk_drv_array[DEV_TYPE_MAX];/**< 磁盘设备驱动接口*/

extern osMutexDef_t disk_mutex_array[DEV_TYPE_MAX];/**< 磁盘同步锁*/
/** Exported functions prototypes --------------------------------------------*/
DWORD disk_get_rtc_time_s(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
