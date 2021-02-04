/**
 *  @file Update_Protocol.c
 *
 *  @date 2021/2/1
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 固件更新协议
 *
 *  @details 
 *
 *  @version v1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Update_Protocol.h"
#include "User_Main.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
#define FIREWARE_DOWNLOAD_PARTITION_NAME    "download"/**< 固件下载分区名*/
#define FIREWARE_FACTORY_PARTITION_NAME     "factory"/**< 工厂固件分区名*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static uint8_t ReceiveBuf[CQ_BUF_2KB] = {0};
static CQ_handleTypeDef CQ_Update_Handle;
/** Private function prototypes ----------------------------------------------*/
/*擦除下载分区数据*/
static void Erase_Download_Partition(void);
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
  ******************************************************************
  * @brief   获取更新协议
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
static UPDATE_PROTOCOL_Typedef_t Get_Update_Protocol(void)
{
  /*read bluetooth key state*/
  if(1)
  {
    /*the bluetooth device is connected*/
    return CUSTOME_PROTOCOL;
  }
  else  
  {
    return YMODEM_PROTOCOCL;
  }
}



/**
  ******************************************************************
  * @brief   删除下载分区
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
static void Erase_Download_Partition(void)
{
  Flash_Port_Erase_Partition(FIREWARE_DOWNLOAD_PARTITION_NAME);
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
  * @brief   协议栈启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void Update_Protocol_Start(void)
{
  for(;;)
  {

  }
}

/**
  ******************************************************************
  * @brief   协议栈初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void Update_Protocol_Init(void)
{
  CQ_init(&CQ_Update_Handle ,ReceiveBuf ,CQ_BUF_2KB);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
