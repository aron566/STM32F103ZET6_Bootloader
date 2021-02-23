/**
 *  @file Ymodem_Port.c
 *
 *  @date 2021-02-11
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief YMODEM协议接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Ymodem_Port.h"
#include "User_Main.h"
#include "Bootloader.h"
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static YMODEM_HANDLE_Typedef_t Ymodem_Handle;
static Uart_Dev_Handle_t *Uart_Dev_Handle = NULL;
//static uint8_t Rec_Data[133] = {0};
static uint16_t Packet_Num_Cnt = 0;
static YMODEM_MODE_Typedef_t Ymodem_Mode = YMODEM_REC_MODE;
static FRIMWARE_INFO_Typedef_t Frimware_Info;
/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/********************************************************************
 * @brief Ymodem接收数据回调
 * @param data 数据
 * @param size 数据长度
 * @param packet_num 第几包数据
 * @return 接收数据大小
 */
static uint32_t Ymodem_Rec_Data_Port(const uint8_t *Data, uint32_t Size, uint8_t Packet_Num)
{
  Packet_Num_Cnt++;
  Bootloader_Port_Download_Data(Data, Packet_Num_Cnt);
  return Size;
}

/********************************************************************
 * @brief Ymodem数据发送回调
 * @param dest_buf 数据存储区
 * @param size 存储区大小
 * @param packet_num 填充本次数据包号
 * @return 本次填充数据大小
 */
static uint32_t Ymodem_Send_Data_Port(uint8_t *Dest_Buf, uint32_t Size, uint8_t *Packet_Num)
{
  Packet_Num_Cnt++;
  /*依据size填充数据和本次的包号进行发送*/
  Flash_Port_Read_Partition_Data(FRIMWARE_APP_PARTITION_NAME, 
                              (uint8_t *)Dest_Buf, (Packet_Num_Cnt-1)*Size, Size);
  *Packet_Num = (uint8_t)Packet_Num_Cnt;
  uint32_t fill_size = Packet_Num_Cnt*Size >= Frimware_Info.FrimwareSize?Size-((Packet_Num_Cnt*Size)-Frimware_Info.FrimwareSize):Size;
  return fill_size;
}

/********************************************************************
 * @brief Ymodem答复接口回调
 * @param data 数据
 * @param size 数据长度
 * @return None
 */
static void Ymodem_Reply_Data_Port(const uint8_t *data, uint32_t size)
{
  Uart_Port_Transmit_Data(Uart_Dev_Handle, (uint8_t *)data, (uint16_t)size);
}

/********************************************************************
 * @brief SHELL 上传固件
 * @param None.
 * @return None
 */
static void Upload_Frimware(const char* Protocol_Name)
{
  if(strcmp(Protocol_Name, "ymodem") == 0)
  {
    Ymodem_Mode = YMODEM_SEND_MODE;
    Ymodem_Port_Init();
    Flash_Port_Read_Partition_Data(FRIMWARE_FLAG_PARTITION_NAME, 
                              (uint8_t *)&Frimware_Info, 0, sizeof(FRIMWARE_INFO_Typedef_t));
    Ymodem_Set_Send_File_Info(Frimware_Info.FrimwareName, Frimware_Info.FrimwareSize);
    if(Ymodem_Port_Start() == true)
    {
      printf("Upload Frimware OK.\r\n");
    }
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
  * @brief   Ymodem协议接口获取文件名
  * @param   [in]None.
  * @return  文件名.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-12
  ******************************************************************
  */
const char *Ymodem_Port_Get_File_Name(void)
{
  return Ymodem_Handle.file_name;
}

/**
  ******************************************************************
  * @brief   Ymodem协议接口获取文件大小
  * @param   [in]None.
  * @return  文件大小字节数.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-12
  ******************************************************************
  */
uint32_t Ymodem_Port_Get_File_Size(void)
{
  return Ymodem_Handle.file_size;
}

/********************************************************************
 * @brief Ymodem设置发送文件信息
 * @param file_name 文件名
 * @param file_size 文件大小
 * @return None
 */
void Ymodem_Set_Send_File_Info(const char *file_name, uint32_t file_size)
{
  ymodem_set_send_file_info(&Ymodem_Handle, file_name, file_size);
}

/**
  ******************************************************************
  * @brief   Ymodem协议接口启动
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-11
  ******************************************************************
  */
bool Ymodem_Port_Start(void)
{
  YMODEM_RUN_RESULT_Typedef_t ret = YMODEM_SUCCESSFUL;
  uint32_t len = 0;
  uint8_t Rec_Data[133] = {0};
  for(;;)
  {
    /*获取输入*/
    len = CQ_getLength(Uart_Dev_Handle->cb);
    len = (len > 133)?133:len;
    
    CQ_ManualGetData(Uart_Dev_Handle->cb, Rec_Data, len);
    ret = ymodem_task_run(&Ymodem_Handle, Rec_Data, len);
    switch(ret)
    {
      case YMODEM_SUCCESSFUL:
        CQ_emptyData(Uart_Dev_Handle->cb);
        Ymodem_Mode = YMODEM_REC_MODE;
        return true;
      case YMODEM_IS_RUNNING:
        CQ_ManualOffsetInc(Uart_Dev_Handle->cb, len);
        break;
      case YMODEM_FRAME_NOT_MATCH:
        CQ_ManualOffsetInc(Uart_Dev_Handle->cb, 1);
        break;
      case YMODEM_FRAME_NOT_FULL:
        break;
      default:
        CQ_emptyData(Uart_Dev_Handle->cb);
        Ymodem_Mode = YMODEM_REC_MODE;
        return false;
    }
    HAL_Delay(20);
  }
}

/**
  ******************************************************************
  * @brief   Ymodem协议接口初始化
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-02-11
  ******************************************************************
  */
void Ymodem_Port_Init(void)
{
  /*获取输入输出接口*/
  Uart_Dev_Handle = Uart_Port_Get_Handle(UART_NUM_1);
  if(Uart_Dev_Handle == NULL)
  {
    return;
  }
  if(Ymodem_Mode == YMODEM_REC_MODE)
  {
    ymodem_init(&Ymodem_Handle, YMODEM_REC_MODE, Ymodem_Rec_Data_Port
                  ,Ymodem_Send_Data_Port, Ymodem_Reply_Data_Port, 60);
  }
  else
  {
    ymodem_init(&Ymodem_Handle, YMODEM_SEND_MODE, Ymodem_Rec_Data_Port
                  ,Ymodem_Send_Data_Port, Ymodem_Reply_Data_Port, 60);
  }
  Packet_Num_Cnt = 0;
}

/**
 * @name SHELL 命令导出
 * @{
 */

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), upload, Upload_Frimware, upload <protocol>);

/** @}*/

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
