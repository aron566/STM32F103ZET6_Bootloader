/**
 *  @file Flash_Port.c
 *
 *  @date 2021-01-13
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief FLASH操作接口
 *
 *  @details BANK1 RANGE:0x0800 0000- 0x080F FFFF TOTAL_SIZE:128Kb*8
 *           BANK2 RANGE:0x0810 0000- 0x081F FFFF TOTAL_SIZE:128Kb*8
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Flash_Port.h"
#include "User_Main.h"
#include "fal.h"
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   擦除指定分区
  * @param   [in]name 分区名称
  * @return  true 操作成功
  * @author  aron566
  * @version V1.0
  * @date    2021-01-31
  ******************************************************************
  */
bool Flash_Port_Erase_Partition(const char *Partition_Name)
{
  const struct fal_partition *part = fal_partition_find(Partition_Name);
  if(part == NULL)
  {
    return false;
  }
  if(fal_partition_erase_all(part) < 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
  ******************************************************************
  * @brief   Flash操作初始化
  * @param   [in]None
  * @return  None
  * @author  aron566
  * @version V1.0
  * @date    2021-01-31
  ******************************************************************
  */
void Flash_Port_Init(void)
{
  /*万能驱动初始化*/
  sfud_init();
  
  /*flash分区初始化*/
  fal_init();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
