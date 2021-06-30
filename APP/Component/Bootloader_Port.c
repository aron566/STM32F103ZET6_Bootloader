/**                                                                             
 *  @file Bootloader_Port.c                                                   
 *                                                                              
 *  @date 2021年02月13日 15:20:09 星期六
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief None.
 *
 *  @details None.
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include "rtc.h"
#include "spi.h"
/* Private includes ----------------------------------------------------------*/
#include "Bootloader_Port.h"
#include "Bootloader.h"
#include "Timer_Port.h"
#include "Uart_Port.h"
#include "Flash_Port.h"
#include "Utilities_AES.h"
/** Private typedef ----------------------------------------------------------*/
/*Jump func*/
typedef void (*pJumpFunction)(void);                                                                          

/** Private macros -----------------------------------------------------------*/
#define STACK_ADDR_MASK     STACK_TOP_ADDR_MASK /**< 0消除允许的位*/
#define USE_RTOS_BOOTLOADER 1                   /**< bl是否使用操作系统*/
/** Private constants --------------------------------------------------------*/

/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static BOOTLOADER_Handle_Typedef_t Bl_Handle;
static pJumpFunction JumpToApplication = NULL;
static Uart_Dev_Handle_t *Uart_Dev_Handle = NULL;

/** Private function prototypes ----------------------------------------------*/
                                                                                
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
  * @brief   初始化启动自定义文件传输协议
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static void Customize_Protocol_Stack_Init(void)
{

}

/**
  ******************************************************************
  * @brief   启动自定义文件传输协议
  * @param   [in]None.
  * @return  true 文件传输成功.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static bool Customize_Protocol_Stack_Start(void)
{
  return false;
}

/**
  ******************************************************************
  * @brief   从自定义协议更新固件信息
  * @param   [out]Frimware_Info 信息存储区.
  * @param   [in]Down_Type 下载类型 FACTORY_TYPE 恢复固件类型.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static void Update_Friware_Info(FRIMWARE_INFO_Typedef_t *Frimware_Info, UPDATE_FRIMWARE_TYPE_Typedef_t Down_Type)
{
  UNUSED(Frimware_Info);
  UNUSED(Down_Type);
  /*先读再更新，保留注册码*/
} 

/**
  ******************************************************************
  * @brief   写入指定分区数据
  * @param   [in]name 分区名称
  * @param   [in]data 存储区
  * @param   [in]Offset 写入地址相对分区起始地址偏移
  * @param   [in]Size 写入大小
  * @return  -1失败，写入长度
  * @author  aron566
  * @version V1.0
  * @date    2021-02-13
  ******************************************************************
  */
static int Write_Partition_Data(const char *Partition_Name, const uint8_t *data, uint32_t Offset, uint32_t Size)
{
  return Flash_Port_Write_Partition_Data(Partition_Name, data, Offset, Size);
} 

/**
  ******************************************************************
  * @brief   读取指定分区数据
  * @param   [in]name 分区名称
  * @param   [in]Dest_Buf 存储区
  * @param   [in]Offset 读取地址偏移
  * @param   [in]Read_Size 读取大小
  * @return  -1失败，读取长度
  * @author  aron566
  * @version V1.0
  * @date    2021-02-13
  ******************************************************************
  */
static int Read_Partition_Data(const char *Partition_Name, uint8_t *Dest_Buf, uint32_t Offset, uint32_t Read_Size)
{
  return Flash_Port_Read_Partition_Data(Partition_Name, Dest_Buf, Offset, Read_Size);
}

/**
  ******************************************************************
  * @brief   擦除分区数据
  * @param   [in]分区名
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static void Erase_Partition(const char *Partition_Name)
{
  Flash_Port_Erase_Partition(Partition_Name);
}

/**
  ******************************************************************
  * @brief   获取分区地址
  * @param   [in]分区名
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static uint32_t Get_App_Partition_Address(const char *Partition_Name)
{
  return Flash_Port_Get_Partition_Addr(Partition_Name);
}

/**
  ******************************************************************
  * @brief   获取分区大小
  * @param   [in]分区名
  * @return  分区大小.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static uint32_t Get_Partition_Size(const char *Partition_Name)
{
  return Flash_Port_Get_Partition_Size(Partition_Name);
}

/**
  ******************************************************************
  * @brief   跳转至应用程序分区
  * @param   [in]App_Addr app程序起始地址
  * @param   [in]Stack_Base_Addr 栈底地址
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static void Jump_To_Application(uint32_t App_Addr, uint32_t Stack_Base_Addr)
{
  uint32_t JumpAddress = 0;
  printf("read top address of stack : %08X\r\n", (*(__IO uint32_t*)App_Addr));
  printf("check stack in:%08X\r\n\n", ((*(__IO uint32_t*)App_Addr) & STACK_ADDR_MASK ));
  
  /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
  if(((*(__IO uint32_t*)App_Addr) & STACK_ADDR_MASK ) == Stack_Base_Addr)
  {
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*)(App_Addr + 4);
    JumpToApplication = (pJumpFunction)JumpAddress;
    /* 关闭全局中断 */
    __disable_irq();

    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /* 设置所有时钟到默认状态，使用HSI时钟 */
    HAL_RCC_DeInit();
    
    HAL_DeInit();
    HAL_RTC_MspDeInit(&hrtc);
    HAL_SPI_MspDeInit(&hspi2);
    HAL_SuspendTick();
    HAL_UART_MspDeInit(&huart1);
    HAL_UART_MspDeInit(&huart2);
    
    /* 关闭所有中断，清除所有中断挂起标志 */
    for(int i = 0; i < 8; i++)
    {
      NVIC->ICER[i] = 0xFFFFFFFF;
      NVIC->ICPR[i] = 0xFFFFFFFF;
    }	
    
    /*使能全局中断 */
    __enable_irq();

    /*设置重映射到系统Flash */
//    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
    
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*)App_Addr);
#if USE_RTOS_BOOTLOADER
    /*设置特权模式使用MSP*/
    __set_CONTROL(0);
#endif
    JumpToApplication();
  }
  printf("Try Jump Faild.\r\n");
}                                                                      

/**
  ******************************************************************
  * @brief   获取当前时间
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */   
static uint32_t Get_Current_Time_Sec(void)
{
  return Timer_Port_Get_Current_Time(TIMER_SEC);
}   

/**
  ******************************************************************
  * @brief   倒计时引导提示
  * @param   [in]sec 显示的倒计时秒
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static void Print_Count_Down_Time(uint32_t sec)
{
  const char * Tips_Str = "aidi:/$ Please press any key to stop bootloader, when %u S later app jump.\r";
  if(sec <= COUNT_DONW_TIME_SEC)
  {
    if(sec == COUNT_DONW_TIME_SEC)
    {
      printf(Tips_Str+8, sec);
    }
    else
    {
      printf(Tips_Str, sec);
    }
#if !defined (__CC_ARM)
    fflush(stdout);
#endif
  }
}

/**
  ******************************************************************
  * @brief   检查是否暂停引导
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
static bool Is_Puase_State(void)
{
  uint32_t len = CQ_getLength(Uart_Dev_Handle->cb);
  if(len == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
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
  * @brief   bl接口数据下载写入
  * @param   [in]data 数据
  * @param   [in]Packet_Num 包号
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
void Bootloader_Port_Download_Data(const uint8_t *data, uint16_t Packet_Num)
{
  Bootloader_Download_Data(data, Packet_Num);
}

/**
  ******************************************************************
  * @brief   bl接口启动
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
void Bootloader_Port_Start(void)
{
  Bootloader_Start();
}

/**
  ******************************************************************
  * @brief   bl接口初始化
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/13
  ******************************************************************
  */
void Bootloader_Port_Init(void)
{
  /*获取输入输出接口*/
  Uart_Dev_Handle = Uart_Port_Get_Handle(UART_NUM_1);
  if(Uart_Dev_Handle == NULL)
  {
    return;
  }
  Bl_Handle.p_Is_Pause_State_func                 = Is_Puase_State;
  Bl_Handle.p_Get_Current_Time_Sec_func           = Get_Current_Time_Sec;
  Bl_Handle.p_Print_Count_Down_Str_func           = Print_Count_Down_Time;
          
  Bl_Handle.p_Write_Partition_Data_func           = Write_Partition_Data;
  Bl_Handle.p_Read_Partition_Data_func            = Read_Partition_Data;
  Bl_Handle.pGet_Partition_Addr_func              = Get_App_Partition_Address;
  Bl_Handle.p_Get_Partition_Size_func             = Get_Partition_Size;
  Bl_Handle.p_Erase_Partition_func                = Erase_Partition;

  Bl_Handle.p_Customize_Protocol_Stack_Init_func  = Customize_Protocol_Stack_Init;
  Bl_Handle.p_Customize_Protocol_Stack_Start_func = Customize_Protocol_Stack_Start;
  Bl_Handle.p_Update_Friware_Info_func            = Update_Friware_Info;

  Bl_Handle.pJump_func                            = Jump_To_Application;

  Bootloader_Init(&Bl_Handle);
}

/**
 * @name SHELL 命令导出
 * @{
 */

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), call, Jump_To_Application, call <app address>);

/** @}*/

#ifdef __cplusplus ///< end extern c                                             
}                                                                               
#endif                                                                          
/******************************** End of file *********************************/
