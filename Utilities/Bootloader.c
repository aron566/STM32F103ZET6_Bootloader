/**                                                                             
 *  @file Bootloader.c                                                   
 *                                                                              
 *  @date 2021年02月13日 15:11:16 星期六
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief 引导程序.
 *
 *  @details None.
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Bootloader.h"
#include "Ymodem_Port.h"
#include "utilities.h"
/** Private typedef ----------------------------------------------------------*/

/*倒计时状态设置*/
typedef enum
{
  DOWN_COUNT_RESET = 0, /**< 重置计时*/
  DOWN_COUNT_NORMAL,    /**< 正常引导*/
  DOWN_COUNT_DELAY,     /**< 延时引导*/
}DOWN_COUNT_STATE_SET_Typedef_t;
/** Private macros -----------------------------------------------------------*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static BOOTLOADER_Handle_Typedef_t *Bl_Handle = NULL;

static UPDATE_FRIMWARE_TYPE_Typedef_t Update_Region = DOWN_LOAD_TYPE;
/** Private function prototypes ----------------------------------------------*/
/*擦除下载分区数据*/
static void Erase_Download_Partition(void);
/*擦除恢复分区数据*/
static void Erase_Factory_Partition(void);
/*擦除标识分区数据*/
static void Erase_Flag_Partition(void);
/*擦除APP分区数据*/
static void Erase_App_Partition(void);
/*获取Bootloader分区地址*/
static uint32_t Get_Bl_Partition_Address(void);
/*获取app分区地址*/
static uint32_t Get_App_Partition_Address(void);

/*倒计时引导提示*/
static inline void Print_Count_Down_Time(uint32_t sec);
/*检测引导倒计时*/
static void Check_Jump_DownCount(DOWN_COUNT_STATE_SET_Typedef_t state);
/*引导前检测固件信息*/
static void Update_Check_Friware_Info(void);

/*从ymodem更新固件信息*/
static void Update_Friware_Info_From_Ymodem(FRIMWARE_INFO_Typedef_t *Frimware_Info, UPDATE_FRIMWARE_TYPE_Typedef_t Down_Type);
/*获取固件信息*/
static void Get_Frimware_Info(FRIMWARE_INFO_Typedef_t *Frimware_Info);
/*写入固件信息*/
static void Write_Frimware_Info(FRIMWARE_INFO_Typedef_t *Frimware_Info);

/*启动更新协议*/
static void Start_Bootloader(const char *Protocol_Name, const char *Frimware_Name);
/*拷贝固件到App分区*/
static void App_Friware_Move(FRIMWARE_INFO_Typedef_t *Frimware_Info);
/*恢复固件到App分区*/
static void App_Frimware_Restore(void);
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
  * @brief   启动更新协议
  * @param   [in]Protocol_Name 协议名
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/11
  ******************************************************************
  */
static void Start_Bootloader(const char *Protocol_Name, const char *Frimware_Name)
{
  if(Protocol_Name == NULL || Frimware_Name == NULL)
  {
    printf("set protocol faild.\r\n");
    return;
  }
  if(strcmp(Protocol_Name, "ymodem") == 0)
  {
    printf("set protocol ymodem.\r\n");
    if(strcmp(Frimware_Name, FRIMWARE_DOWNLOAD_PARTITION_NAME) == 0)
    {
      Erase_Download_Partition();
      Update_Region = DOWN_LOAD_TYPE;
    }
    else if(strcmp(Frimware_Name, FRIMWARE_FACTORY_PARTITION_NAME) == 0)
    {
      Erase_Factory_Partition();
      Update_Region = FACTORY_TYPE;
    }
    else
    {
      printf("unknow par!\r\n");
      return;
    }

    /*启动ymodem协议栈*/
    Ymodem_Port_Init();
    if(Ymodem_Port_Start() == true)
    {
      printf("download successful.\r\n");
      Update_Friware_Info_From_Ymodem(&Bl_Handle->Frimware_Info, Update_Region);
      Bl_Handle->Frimware_Info.Flag = (Update_Region == DOWN_LOAD_TYPE)?DOWNLOAD_COMPLETE:Bl_Handle->Frimware_Info.Flag;
      Write_Frimware_Info(&Bl_Handle->Frimware_Info);
      Update_Check_Friware_Info();
      return;
    }
  }
  else if(strcmp(Protocol_Name, "customize") == 0)
  {
    printf("set protocol customize.\r\n");
    if(strcmp(Frimware_Name, FRIMWARE_DOWNLOAD_PARTITION_NAME) == 0)
    {
      Erase_Download_Partition();
      Update_Region = DOWN_LOAD_TYPE;
    }
    else if(strcmp(Frimware_Name, FRIMWARE_FACTORY_PARTITION_NAME) == 0)
    {
      Erase_Factory_Partition();
      Update_Region = FACTORY_TYPE;
    }
    else
    {
      printf("unknow par!\r\n");
      return;
    }

    /*启动自定义协议栈*/
    Bl_Handle->p_Customize_Protocol_Stack_Init_func();
    if(Bl_Handle->p_Customize_Protocol_Stack_Start_func() == true)
    {
      printf("download successful.\r\n");
      Bl_Handle->p_Update_Friware_Info_func(&Bl_Handle->Frimware_Info, Update_Region);
      Bl_Handle->Frimware_Info.Flag = (Update_Region == DOWN_LOAD_TYPE)?DOWNLOAD_COMPLETE:Bl_Handle->Frimware_Info.Flag;
      Write_Frimware_Info(&Bl_Handle->Frimware_Info);
      Update_Check_Friware_Info();
      return;
    }
  }
  else
  {
    printf("unknow par!\r\n");
    return;
  }
}

/**
  ******************************************************************
  * @brief   引导前检测固件信息
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/11
  ******************************************************************
  */
static void Update_Check_Friware_Info(void)
{
  Get_Frimware_Info(&Bl_Handle->Frimware_Info);
  switch(Bl_Handle->Frimware_Info.Flag)
  {
    case UPDATE_SUCESSFUL:
      /*TODO*/
      Bl_Handle->pJump_func(Get_App_Partition_Address(), APP_STACK_BASE_ADDR);
      /*restore*/
      App_Frimware_Restore();
      Update_Check_Friware_Info();
      break;
    case UPDATE_WAITTING:   ///<! 写入APP分区成功，一直为标识可能程序有问题
      /*Wait cnt acc, if cnt more than songthing number recover factory frimware*/
      Bl_Handle->Frimware_Info.Retry_Cnt++;
      if(Bl_Handle->Frimware_Info.Retry_Cnt >= RETRY_UPDATE_NUM_MAX)
      {
        /*restore*/
        App_Frimware_Restore();
        Update_Check_Friware_Info();
        return;
      }
      Write_Frimware_Info(&Bl_Handle->Frimware_Info);
      /*Try Jump To Application*/
      Bl_Handle->pJump_func(Get_App_Partition_Address(), APP_STACK_BASE_ADDR);
      break;
    case DOWNLOAD_COMPLETE:///<! 下载成功，固件迁移
      App_Friware_Move(&Bl_Handle->Frimware_Info);
      Update_Check_Friware_Info();
      break;
    default:
      /*Invaild Frimware*/
      /*First Try Jump To Application*/
      Bl_Handle->pJump_func(Get_App_Partition_Address(), APP_STACK_BASE_ADDR);
      Bl_Handle->pJump_func(Get_Bl_Partition_Address(), BL_STACK_BASE_ADDR);
      break;
  }
}

/**
  ******************************************************************
  * @brief   拷贝固件到App分区
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/12
  ******************************************************************
  */
static void App_Friware_Move(FRIMWARE_INFO_Typedef_t *Frimware_Info)
{
  uint8_t temp_buf[32] = {0};
  if(Frimware_Info->Flag == DOWNLOAD_COMPLETE)
  {
    if(Bl_Handle->p_Get_Partition_Size_func(FRIMWARE_APP_PARTITION_NAME) < Frimware_Info->FrimwareSize)
    {
      printf("error: the app size is too large!\r\n");
      return;
    }
    /*擦除App分区数据*/
    Erase_App_Partition();
    uint32_t bytes = 0, Write_Max = 0;
    printf("move app frimware, size:%u Bytes...\r\n", Frimware_Info->FrimwareSize);
    printf_progress_bar(1, 1, SHARP_CHAR_STYLE, true);
    uint32_t Partition_Size = Bl_Handle->p_Get_Partition_Size_func(FRIMWARE_APP_PARTITION_NAME);
    Write_Max = ((Frimware_Info->FrimwareSize/32)*32) < Frimware_Info->FrimwareSize?((Frimware_Info->FrimwareSize/32)+1)*32:(Frimware_Info->FrimwareSize/32)*32;
    Write_Max = Write_Max < Partition_Size?Write_Max:Partition_Size;
    
    for(bytes = 0; bytes < Write_Max; bytes += 32)
    {
      if(Bl_Handle->p_Read_Partition_Data_func(FRIMWARE_DOWNLOAD_PARTITION_NAME, temp_buf, bytes, 32) == -1)
      {
        return;
      }
      if(Bl_Handle->p_Write_Partition_Data_func(FRIMWARE_APP_PARTITION_NAME, temp_buf, bytes, 32) == -1)
      {
        return;
      }
      printf_progress_bar(bytes, Write_Max, SHARP_CHAR_STYLE, false);
    }
    printf_progress_bar(bytes, Write_Max, SHARP_CHAR_STYLE, false);
    printf("\r\n");
    Frimware_Info->Flag = UPDATE_WAITTING;
    Frimware_Info->Retry_Cnt = 0;
    Write_Frimware_Info(Frimware_Info);
  }
}

/**
  ******************************************************************
  * @brief   恢复固件到App分区
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/12
  ******************************************************************
  */
static void App_Frimware_Restore(void)
{
  uint8_t temp_buf[32] = {0};
  
  /*擦除App分区数据*/
  Erase_App_Partition();
  uint32_t bytes = 0, Write_Max = 0;
  
  printf_progress_bar(1, 1, SHARP_CHAR_STYLE, true);
  uint32_t Partition_Size = Bl_Handle->p_Get_Partition_Size_func(FRIMWARE_APP_PARTITION_NAME);
  Partition_Size = (Partition_Size < Bl_Handle->Frimware_Info.BackFrimwareSize)?Partition_Size:Bl_Handle->Frimware_Info.BackFrimwareSize;
  Partition_Size = Partition_Size < Bl_Handle->p_Get_Partition_Size_func(FRIMWARE_FACTORY_PARTITION_NAME)?Partition_Size:Bl_Handle->p_Get_Partition_Size_func(FRIMWARE_FACTORY_PARTITION_NAME);
  
  printf("Restore_Factory_Friware, size:%u Bytes...\r\n", Partition_Size);
  Write_Max = ((Partition_Size/32)*32) < Partition_Size?((Partition_Size/32)+1)*32:(Partition_Size/32)*32;
  
  for(bytes = 0; bytes < Write_Max; bytes += 32)
  {
    if(Bl_Handle->p_Read_Partition_Data_func(FRIMWARE_FACTORY_PARTITION_NAME, temp_buf, bytes, 32) == -1)
    {
      return;
    }
    if(Bl_Handle->p_Write_Partition_Data_func(FRIMWARE_APP_PARTITION_NAME, temp_buf, bytes, 32) == -1)
    {
      return;
    }
    printf_progress_bar(bytes, Write_Max, SHARP_CHAR_STYLE, false);
  }
  printf_progress_bar(bytes, Write_Max, SHARP_CHAR_STYLE, false);
  printf("\r\n");
  Bl_Handle->Frimware_Info.Flag = UPDATE_WAITTING;
  Bl_Handle->Frimware_Info.Retry_Cnt = 0;
  Bl_Handle->Frimware_Info.FrimwareSize = Partition_Size;
  strncopy(Bl_Handle->Frimware_Info.FrimwareName, Bl_Handle->Frimware_Info.BackFrimwareName, FRIMWARE_NAME_LEN_MAX);
  Write_Frimware_Info(&Bl_Handle->Frimware_Info);
}

/**
  ******************************************************************
  * @brief   从ymodem更新固件信息
  * @param   [out]Frimware_Info 信息存储区.
  * @param   [in]Down_Type 下载类型 FACTORY_TYPE 恢复固件类型.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/12
  ******************************************************************
  */
static inline void Update_Friware_Info_From_Ymodem(FRIMWARE_INFO_Typedef_t *Frimware_Info, UPDATE_FRIMWARE_TYPE_Typedef_t Down_Type)
{
  /*先读再更新，保留注册码*/
  int ret = Bl_Handle->p_Read_Partition_Data_func(FRIMWARE_FLAG_PARTITION_NAME, 
                              (uint8_t *)Frimware_Info, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  if(ret < 0)
  {
    printf("get frimware info faild.\r\n");
  }
  /*更新信息*/
  if(Down_Type == DOWN_LOAD_TYPE)
  {
    strncopy(Frimware_Info->FrimwareName, Ymodem_Port_Get_File_Name(), 64);
    Frimware_Info->FrimwareSize = Ymodem_Port_Get_File_Size();
  }
  else if(Down_Type == FACTORY_TYPE)
  {
    strncopy(Frimware_Info->BackFrimwareName, Ymodem_Port_Get_File_Name(), 64);
    Frimware_Info->BackFrimwareSize = Ymodem_Port_Get_File_Size();
  }
}

/**
  ******************************************************************
  * @brief   获取固件信息
  * @param   [out]Frimware_Info 信息存储区.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/11
  ******************************************************************
  */
static void Get_Frimware_Info(FRIMWARE_INFO_Typedef_t *Frimware_Info)
{
  int ret = Bl_Handle->p_Read_Partition_Data_func(FRIMWARE_FLAG_PARTITION_NAME, 
                              (uint8_t *)Frimware_Info, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  if(ret < 0)
  {
    printf("get frimware info faild.\r\n");
  }
  Frimware_Info->FrimwareName[FRIMWARE_NAME_LEN_MAX-1] = '\0';
  Frimware_Info->BackFrimwareName[FRIMWARE_NAME_LEN_MAX-1] = '\0';
  printf("\r\nfirmware name: %-.*s, size : %u Bytes, update flag:%08X ", FRIMWARE_NAME_LEN_MAX-1, Frimware_Info->FrimwareName
        ,Frimware_Info->FrimwareSize, Frimware_Info->Flag);
  switch(Frimware_Info->Flag)
  {
    /*更新成功*/
    case UPDATE_SUCESSFUL:
      printf("Update Finsh.\r\n");
      break;
    /*等待更新*/
    case UPDATE_WAITTING:
      printf("Wait Update.\r\n");
      break;
    /*下载完成*/
    case DOWNLOAD_COMPLETE:
      printf("Wait Move Frimware.\r\n");
      break;
    default:
      printf("Unknow flag.\r\n");
      break;
  }
}

/**
  ******************************************************************
  * @brief   写入固件信息
  * @param   [in]Frimware_Info 固件信息.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/12
  ******************************************************************
  */
static void Write_Frimware_Info(FRIMWARE_INFO_Typedef_t *Frimware_Info)
{
  /*擦除信息*/
  printf("erase flag partition ...\r\n");
  Erase_Flag_Partition();
  Bl_Handle->p_Write_Partition_Data_func(FRIMWARE_FLAG_PARTITION_NAME,
                              (uint8_t *)Frimware_Info, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  printf("write flag partition ok.\r\n");
}

/**
  ******************************************************************
  * @brief   倒计时引导提示
  * @param   [in]sec 显示的倒计时秒
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/7
  ******************************************************************
  */
static inline void Print_Count_Down_Time(uint32_t sec)
{
  Bl_Handle->p_Print_Count_Down_Str_func(sec);
}

/**
  ******************************************************************
  * @brief   删除APP分区
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/12
  ******************************************************************
  */
static inline void Erase_App_Partition(void)
{
  Bl_Handle->p_Erase_Partition_func(FRIMWARE_APP_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   擦除下载分区数据
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
static inline void Erase_Download_Partition(void)
{
  Bl_Handle->p_Erase_Partition_func(FRIMWARE_DOWNLOAD_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   擦除恢复分区数据
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
static inline void Erase_Factory_Partition(void)
{
  Bl_Handle->p_Erase_Partition_func(FRIMWARE_FACTORY_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   擦除标识分区数据
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
static inline void Erase_Flag_Partition(void)
{
  Bl_Handle->p_Erase_Partition_func(FRIMWARE_FLAG_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   获取Bootloader分区地址
  * @param   [in]None.
  * @return  地址.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/8
  ******************************************************************
  */
static inline uint32_t Get_Bl_Partition_Address(void)
{
  return Bl_Handle->pGet_Partition_Addr_func(FRIMWARE_BL_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   获取app分区地址
  * @param   [in]None.
  * @return  地址.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/8
  ******************************************************************
  */
static inline uint32_t Get_App_Partition_Address(void)
{
  return Bl_Handle->pGet_Partition_Addr_func(FRIMWARE_APP_PARTITION_NAME);
}

/**
  ******************************************************************
  * @brief   检测引导倒计时
  * @param   [in]state 倒计时状态设置
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/7
  ******************************************************************
  */
static void Check_Jump_DownCount(DOWN_COUNT_STATE_SET_Typedef_t state)
{
  static uint32_t Last_Time = 0;
  static uint32_t Down_Count_Record = COUNT_DONW_TIME_SEC;
  uint32_t Current_Sec = Bl_Handle->p_Get_Current_Time_Sec_func();
  
  /*检测是否进行重置倒计时*/
  if(state == DOWN_COUNT_DELAY)
  {
    Down_Count_Record = DOUNT_DOWN_DELAY_TIME;
    return;
  }
  else if(state == DOWN_COUNT_RESET)
  {
    Down_Count_Record = COUNT_DONW_TIME_SEC;
    return;
  }

  if(Down_Count_Record == 0)
  {
    /*check flag partition*/
    Update_Check_Friware_Info();
  }
  else if(Current_Sec != Last_Time)
  {
    Last_Time = Current_Sec;
    
    /*输出倒数计时表示*/
    Print_Count_Down_Time(Down_Count_Record--);
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
  * @brief   数据下载写入
  * @param   [in]data 数据
  * @param   [in]Packet_Num 包号
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void Bootloader_Download_Data(const uint8_t *data, uint16_t Packet_Num)
{
  int ret = 0;
  if(Update_Region == DOWN_LOAD_TYPE)
  {
    ret = Bl_Handle->p_Write_Partition_Data_func(FRIMWARE_DOWNLOAD_PARTITION_NAME, data, (Packet_Num-1)*128, 128);
  }
  else if(Update_Region == FACTORY_TYPE)
  {
    ret = Bl_Handle->p_Write_Partition_Data_func(FRIMWARE_FACTORY_PARTITION_NAME, data, (Packet_Num-1)*128, 128);
  }
  if(ret < 0)
  {
    printf("write frimware faild.\r\n");
  }
}

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
void Bootloader_Start(void)
{
  /*获取输入*/
  bool pause = Bl_Handle->p_Is_Pause_State_func();
  if(pause == false)
  {
    /*进入引导倒计时*/
    Check_Jump_DownCount(DOWN_COUNT_NORMAL);
  }
  else
  {
    /*启动shell*/
    Shell_Port_Start();
    
    /*延时倒计时*/
    Check_Jump_DownCount(DOWN_COUNT_DELAY);
  }
}

/**
  ******************************************************************
  * @brief   bl初始化
  * @param   [in]p_Bl_Handle bl句柄
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void Bootloader_Init(BOOTLOADER_Handle_Typedef_t *p_Bl_Handle)
{
  if(p_Bl_Handle == NULL)
  {
    return;
  }
  Bl_Handle = p_Bl_Handle;
  printf("\r\n");
}

/**
 * @name SHELL 命令导出
 * @{
 */

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), upgrade, Start_Bootloader, upgrade <protocol> <partition>);

/** @}*/

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
