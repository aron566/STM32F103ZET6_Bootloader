/**
 *  @file disk_port.c
 *
 *  @date 2021-01-04
 *
 *  @author aron566
 *
 *  @copyright None.
 *
 *  @brief 磁盘接口驱动映射
 *
 *  @details None.
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "disk_port.h"
#include "RTC_Port.h"
/** Privated typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/

/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/
/**
 * @defgroup SPI FLASH DRIVE FUNC
 * @{
 */
static DSTATUS spi_flash_disk_status(void);
static DSTATUS spi_flash_disk_initialize(void);
static DRESULT spi_flash_disk_read(BYTE *buff, LBA_t sector, UINT count);
static DRESULT spi_flash_disk_write(const BYTE *buff, LBA_t sector, UINT count);
static DRESULT spi_flash_disk_ioctl(BYTE cmd, void *buff);
/** @}*/
/** Public variables ---------------------------------------------------------*/
DISK_DRV_FUNC_MAP_Typedef_t disk_drv_array[DEV_TYPE_MAX] = 
{
  [DEV_SPI_FLASH] = {
                      .get_disk_status_port = spi_flash_disk_status,
                      .disk_init_port       = spi_flash_disk_initialize,
                      .disk_read_port       = spi_flash_disk_read,
                      .disk_write_port      = spi_flash_disk_write,
                      .disk_ioctl_port      = spi_flash_disk_ioctl
                    },
};

osMutexDef_t disk_mutex_array[DEV_TYPE_MAX] = {0};
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/**
  ******************************************************************
  * @brief   spi磁盘状态读取
  * @param   [in]None
  * @return  1 Drive not initialized 2 No medium in the drive 4 Write protected
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
static DSTATUS spi_flash_disk_status(void)
{
    sfud_flash *flash = sfud_get_device(SFUD_W25Q16BV_DEVICE_INDEX);
    if(flash == NULL)
    {
        return STA_NOINIT;
    }
    uint8_t status;
    sfud_read_status(flash, &status);
	return (DSTATUS)status;
}

/**
  ******************************************************************
  * @brief   spi磁盘初始化
  * @param   [in]None
  * @return  0 OK 1 Drive not initialized 2 No medium in the drive 4 Write protected
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
static DSTATUS spi_flash_disk_initialize(void)
{
    sfud_flash *flash = sfud_get_device(SFUD_W25Q16BV_DEVICE_INDEX);
    if(flash == NULL)
    {
        return STA_NOINIT;
    }
    sfud_err result = sfud_device_init(flash);
    if(result != SFUD_SUCCESS)
    {
        return STA_NOINIT;
    }
	return 0;
}

/**
  ******************************************************************
  * @brief   spi磁盘读取指定扇区数的数据到缓冲区
  * @param   [out]buff 缓冲区
  * @param   [in]sector 起始扇区号
  * @param   [in]count 扇区数
  * @return  0: Successful 1: R/W Error 2: Write Protected 3: Not Ready 4: Invalid Parameter
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
static DRESULT spi_flash_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    sfud_flash *flash = sfud_get_device(SFUD_W25Q16BV_DEVICE_INDEX);
    if(flash == NULL)
    {
        return RES_PARERR;
    }
    sfud_err result = sfud_read(flash, sector*512, 512*count, (uint8_t *)buff);
    if(result != SFUD_SUCCESS)
    {
        return RES_ERROR;
    }
	return RES_OK;
}

/**
  ******************************************************************
  * @brief   spi磁盘写入数据到指定扇区数
  * @param   [in]buff
  * @param   [in]sector
  * @param   [in]count
  * @return  0: Successful 1: R/W Error 2: Write Protected 3: Not Ready 4: Invalid Parameter
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
static DRESULT spi_flash_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    sfud_flash *flash = sfud_get_device(SFUD_W25Q16BV_DEVICE_INDEX);
    if(flash == NULL)
    {
        return RES_PARERR;
    }
    sfud_err result = sfud_erase_write(flash, sector*512, 512*count, (const uint8_t *)buff);
    if(result != SFUD_SUCCESS)
    {
        return RES_ERROR;
    }
	return RES_OK;
}


/**
  ******************************************************************
  * @brief   spi磁盘特殊操作
  * @param   [in]cmd
  * @param   [in]buff
  * @return  0: Successful 1: R/W Error 2: Write Protected 3: Not Ready 4: Invalid Parameter
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
static DRESULT spi_flash_disk_ioctl(BYTE cmd, void *buff)
{
//#define CTRL_SYNC			0	/* Complete pending write process (needed at FF_FS_READONLY == 0) */
//#define GET_SECTOR_COUNT	1	/* Get media size (needed at FF_USE_MKFS == 1) */
//#define GET_SECTOR_SIZE		2	/* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
//#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at FF_USE_MKFS == 1) */
//#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */
    switch(cmd)
    {
        case CTRL_SYNC:
            
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = 4096;/**< PAGE SIZE:256,SECTOR SIZE:4KB,BLOCK SIZE:32/64KB,W25Q16BV TOTOAL SIZE:512*4KB*/
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(DWORD*)buff = FF_MAX_SS;/**< if use 512Byte then erase size is 2 PAGE SIZE, if use 4KB then erase size is 1 SECTOR SIZE.*/
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;/**< 擦除扇区的最小个数(1*512Byte = 2 PAGE SIZE),per 32K/64K block size to erase*/
            return RES_OK;
        case CTRL_TRIM:
            return RES_PARERR;/**< 通知扇区数据不使用--未开启功能*/
        default:
            break;
    }
	return RES_PARERR;
}

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   获取当前时间秒数
  * @param   [in]None
  * @return  s
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
DWORD disk_get_rtc_time_s(void)
{
  return RTC_Current_Time_S();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
