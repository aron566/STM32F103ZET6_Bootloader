/**
 *  @file Shell_Port.c
 *
 *  @date 2021/2/2
 *
 *  @author aron566
 *
 *  @copyright None.
 *
 *  @brief shell库接口
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
#include "Shell_Port.h"
#include "User_Main.h"
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static Shell shell;/**< shell对象*/
static char shellBuffer[512];
static Uart_Dev_Handle_t *Uart_Dev_Handle = NULL;
/** Private function prototypes ----------------------------------------------*/
static void Shell_Port_Write(char data);
static signed char Shell_Port_Read(char *data);
/** Private user code --------------------------------------------------------*/

/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/
/**
 * @brief shell写
 * 
 * @param data 数据
 */
static void Shell_Port_Write(char data)
{
  Uart_Port_Transmit_Data(Uart_Dev_Handle, (uint8_t *)&data, 1);
}

/**
 * @brief shell读
 * 
 * @param data 数据
 * @return char 状态
 */
static signed char Shell_Port_Read(char *data)
{
  uint32_t len = CQ_getLength(Uart_Dev_Handle->cb);
  if(len == 0)
  {
    return -1;
  }
  CQ_getData(Uart_Dev_Handle->cb, (uint8_t *)data, 1);
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
 * @brief shell启动
 * 
 * @param data 数据
 * @return char 状态
 */
void Shell_Port_Start(void)
{
  shellTask(&shell);
}

/**
 * @brief shell初始化
 * 
 * @param data 数据
 * @return char 状态
 */
void Shell_Port_Init(void)
{
  Uart_Dev_Handle = Uart_Port_Get_Handle(UART_NUM_1);
  if(Uart_Dev_Handle == NULL)
  {
    return;
  }
  shell.write = Shell_Port_Write;
  shell.read = Shell_Port_Read;
  shellInit(&shell, shellBuffer, 512);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
