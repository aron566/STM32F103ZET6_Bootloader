/**
 *  @file User_Main.c
 *
 *  @date 2021/2/1
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 引导任务
 *
 *  @details 固件更新将依据shell设置的更新协议进行更新指定分区的固件
 *
 *  @version v1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "User_Main.h"
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
int func(int argc, char *argv[])
{
    printf("%dparameter(s)\r\n", argc);
    for (char i = 1; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }
    return 0;
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
  * @brief   引导任务启动
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void User_Main_Task_Start(void)
{
  /*启动shell*/
  Shell_Port_Start();
}

/**
  ******************************************************************
  * @brief   引导任务初始化
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void User_Main_Init(void)
{
  /*初始化串口设备*/
  Uart_Port_Init();
   
  /*Flash操作初始化*/
  Flash_Port_Init();
  
  /*Easylogger初始化*/
  EasyLog_Init();
  Test_LOG_Func();
  
  /*Shell初始化*/
  Shell_Port_Init();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), func, func, test);
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
