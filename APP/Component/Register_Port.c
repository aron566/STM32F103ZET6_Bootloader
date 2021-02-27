/**
 *  @file Register_Port.c
 *
 *  @date 2021/2/22
 *
 *  @author aron566
 *
 *  @copyright Copyright (c) 2021 aron566 <aron566@163.com>.
 *
 *  @brief --
 *
 *  @details 设备注册检测
 *
 *  @version v1.0
 */
#ifdef __cplusplus ///< use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Register_Port.h"
#include "User_Main.h"
#include "Bootloader.h"
#include "Utilities_AES.h"
/** Private typedef ----------------------------------------------------------*/
/*AES运行模式*/
typedef enum
{
  ENCRYPT_MODE = 0,                  /**< 加密模式*/
  DECRYPT_MODE,                      /**< 解密模式*/
}AES_RUN_MODE_Typedef_t;         
         
/*AES运行结果*/        
typedef struct         
{        
  uint32_t data_len;                 /**< AES加解密数据长度*/
  uint8_t result_Data[0];            /**< AES加解密数据指针*/
}AES_RUN_RESULT_Typedef_t;

/** Private macros -----------------------------------------------------------*/
#define ENABLE_TEST_AES_ENCRYPT     1/**< 测试UID加密调试打印*/
#define ENABLE_REGISTER_DEBUG       0/**< 开启注册码写入调试打印*/
#define REGISTER_CODE_LEN           (REGISTER_CODE_LEN_MAX)/**< 注册码长度*/  
#define COMPARE_CODE_LEN            12/**< 匹配字段长度*/
#define AES_KEY_SIZE                256
#define FLAG_PARTITION_NAME         FRIMWARE_FLAG_PARTITION_NAME/**< 注册码存放分区*/

#define READ_REGISTER_STATE         "READ REGISTER STATUS"
#define READ_DEVICE_UID_CMD         "READ UID"
#define WRITE_REGISTER_CODE_CMD     "WRITE REGISTER CODE"
#define REGISTER_FRAME_LEN_MIN      (strlen(READ_DEVICE_UID_CMD))
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
/*AES_IV*/
static unsigned char AES_IV[16] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', '0', '1', '2', '3', '4' };
/*AES_KEY*/
static unsigned char AES_KEY[32] = { 'a', 'i', 'd', 'i', 's', 'c', 'i', 't',
    'e', 'c', 'h', '2', '0', '2', '1', '0',
    '2', '2', '2', '@', '@', '=', '?', '@',
    '2', '0', '3', '5', '8', '0', '0', '8' };
/*Device_ID*/
static uint32_t Device_UID[REGISTER_CODE_LEN/4] = {0};
/*串口句柄*/
static Uart_Dev_Handle_t *Uart_Dev_Handle = NULL;
static FRIMWARE_INFO_Typedef_t Register_Code;
/** Private function prototypes ----------------------------------------------*/
/*AES加解密运行结果内存申请*/
static AES_RUN_RESULT_Typedef_t *AES_Run_Result_Malloc(uint32_t data_len);
/*AES加解密运行结果内存释放*/
static void AES_Run_Result_Free(AES_RUN_RESULT_Typedef_t *result_ptr);
/*AES加解密数据*/
static AES_RUN_RESULT_Typedef_t *Encrypt_Decrypt_Data(AES_RUN_MODE_Typedef_t mode, uint8_t *in_data, uint32_t data_len);
/*检测数据一致性*/
static bool Check_Data(const uint8_t *Target_Data, const uint8_t *Read_Data, size_t Compare_Len);
/*获取设备UID*/
static const uint32_t *Get_DeviceUID(void);
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
  * @brief   AES加解密运行结果内存申请
  * @param   [in]data_len 数据长度.
  * @return  AES_RUN_RESULT_Typedef_t 申请成功指针 否则为NULL.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static AES_RUN_RESULT_Typedef_t *AES_Run_Result_Malloc(uint32_t data_len)
{
  return (AES_RUN_RESULT_Typedef_t *)calloc(data_len+sizeof(AES_RUN_RESULT_Typedef_t), sizeof(char));
}

/**
  ******************************************************************
  * @brief   AES加解密运行结果内存释放
  * @param   [in]result_ptr 结果内存指针.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static void AES_Run_Result_Free(AES_RUN_RESULT_Typedef_t *result_ptr)
{
  if(result_ptr == NULL)
  {
    return;
  }
  free(result_ptr);
}

/**
  ******************************************************************
  * @brief   AES加解密数据
  * @param   [in]mode 运行模式.
  * @param   [in]in_data 源数据.
  * @param   [in]data_len 数据长度.
  * @return  AES_RUN_RESULT_Typedef_t* 输出运行结果指针，需进行手动释放
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static AES_RUN_RESULT_Typedef_t *Encrypt_Decrypt_Data(AES_RUN_MODE_Typedef_t mode, uint8_t *in_data, uint32_t data_len)
{
  uint32_t rest_len = data_len % AES_BLOCK_SIZE;
  uint32_t padding_len = ((ENCRYPT_MODE == mode) ? (AES_BLOCK_SIZE - rest_len) : 0);
  uint32_t src_len = data_len + padding_len;
#if ENABLE_REGISTER_DEBUG 
  printf("\r\nsrc_len size : %s\r\n", calculateSize(src_len));
#endif
  uint8_t *input = (uint8_t *)calloc((src_len+1), sizeof(char)); // Enough memory for file + \0
  
  memcpy(input, in_data, data_len);
  
  /*填充*/
  if(padding_len > 0) 
  {
    memset(input + data_len, (uint8_t)padding_len, padding_len);
  }
  
  AES_RUN_RESULT_Typedef_t *result = AES_Run_Result_Malloc(src_len+1);
  if(result == NULL)
  {
    free(input);
    AES_Run_Result_Free(result);
    return NULL;
  }
  
  /*set key & iv*/
  uint32_t key_schedule[AES_BLOCK_SIZE * 4] = { 0 };
  aes_key_setup(AES_KEY, key_schedule, AES_KEY_SIZE);
  
  if(mode == ENCRYPT_MODE)
  {
    aes_encrypt_cbc(input, src_len, result->result_Data, key_schedule, AES_KEY_SIZE, AES_IV);
  } 
  else 
  {
    aes_decrypt_cbc(input, src_len, result->result_Data, key_schedule, AES_KEY_SIZE, AES_IV);
  }
  result->data_len = src_len+1;
  free(input);
  return result;
}

/**
  ******************************************************************
  * @brief   获取设备UID
  * @param   [in]None.
  * @return  96位ID.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static const uint32_t *Get_DeviceUID(void)
{
  Device_UID[0] = HAL_GetUIDw0();
  Device_UID[1] = HAL_GetUIDw1();
  Device_UID[2] = HAL_GetUIDw2();
#if ENABLE_TEST_AES_ENCRYPT  
  /*原始数据*/
  printf("origin data:");
  debug_print((uint8_t *)Device_UID, REGISTER_CODE_LEN);
  
  /*加密*/
  AES_RUN_RESULT_Typedef_t *p_ENResult = Encrypt_Decrypt_Data(ENCRYPT_MODE, (uint8_t *)Device_UID, REGISTER_CODE_LEN);
  if(p_ENResult == NULL)
  {
    printf("run aes encrypt faild.\r\n");
    return NULL;
  }
  printf("\r\nencrypt data:");
  debug_print(p_ENResult->result_Data, p_ENResult->data_len-1);
  
  /*解密*/
  AES_RUN_RESULT_Typedef_t *p_DEResult = Encrypt_Decrypt_Data(DECRYPT_MODE, (uint8_t *)p_ENResult->result_Data, p_ENResult->data_len-1);
  if(p_DEResult == NULL)
  {
    printf("run aes decrypt faild.\r\n");
    return NULL;
  }
  printf("\r\ndecrypt data:");
  debug_print(p_DEResult->result_Data, p_DEResult->data_len-1);
  
  AES_Run_Result_Free(p_ENResult);
  AES_Run_Result_Free(p_DEResult);
#endif
  return Device_UID;
}

/**
  ******************************************************************
  * @brief   注册码写入
  * @param   [in]code 注册码地址.
  * @return  < 0 失败.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/23
  ******************************************************************
  */
static int Register_Code_Write(uint8_t *code)
{
  /*读取注册码*/
  int ret = Flash_Port_Read_Partition_Data(FLAG_PARTITION_NAME, (uint8_t *)&Register_Code, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  if(ret < 0)
  {
    printf("Read Register Code Faild.\r\n");
    return ret;
  }
  memmove(&Register_Code.Register_Code, code, REGISTER_CODE_LEN);
  
  /*erase partition*/
  Flash_Port_Erase_Partition(FLAG_PARTITION_NAME);
  
  /*write*/
  ret = Flash_Port_Write_Partition_Data(FLAG_PARTITION_NAME, (uint8_t *)&Register_Code, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  return ret;
}

/**
  ******************************************************************
  * @brief   比较数据
  * @param   [in]Target_Data 目标数据.
  * @param   [in]Read_Data 读取的数据.
  * @param   [in]Compare_Len 比较长度.
  * @return  true 匹配.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static bool Check_Data(const uint8_t *Target_Data, const uint8_t *Read_Data, size_t Compare_Len)
{
  if(memcmp(Target_Data, Read_Data, Compare_Len) == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
  ******************************************************************
  * @brief   启动注册任务
  * @param   [in]None.
  * @return  true 注册成功.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
static bool Register_Task_Start(void)
{
  if(Uart_Dev_Handle == NULL)
  {
    return false;
  }
  uint32_t len = CQ_getLength(Uart_Dev_Handle->cb);
  
  /*检查缓冲区数据长度*/
  if(len < REGISTER_FRAME_LEN_MIN)
  {
    return false;
  }
  uint8_t temp_buf[256] = {0};
  len = (len > 256)?256:len;
  CQ_ManualGetData(Uart_Dev_Handle->cb, temp_buf, len);
  
  /*读取UID*/
  if(strncmp((const char *)temp_buf, READ_DEVICE_UID_CMD, REGISTER_FRAME_LEN_MIN) == 0)
  {
    /*发送UID*/
    Uart_Port_Transmit_Data(Uart_Dev_Handle, (uint8_t *)Device_UID, REGISTER_CODE_LEN);
    CQ_ManualOffsetInc(Uart_Dev_Handle->cb, len);
    return false;
  }
  /*读取注册状态*/
  else if(strncmp((const char *)temp_buf, READ_REGISTER_STATE, REGISTER_FRAME_LEN_MIN) == 0)
  {
    if(len < strlen(READ_REGISTER_STATE))
    {
      return false;
    }
    /*发送注册码*/
    Uart_Port_Transmit_Data(Uart_Dev_Handle, (uint8_t *)Register_Code.Register_Code, REGISTER_CODE_LEN);
    CQ_ManualOffsetInc(Uart_Dev_Handle->cb, len);
    return false;
  }
  /*写入REGISTER CODE*/
  else if(strncmp((const char *)temp_buf, WRITE_REGISTER_CODE_CMD, REGISTER_FRAME_LEN_MIN) == 0)
  {
    if(len < strlen(WRITE_REGISTER_CODE_CMD))
    {
      return false;
    }
    char str[REGISTER_CODE_LEN*2+REGISTER_CODE_LEN] = {0};
    int ret = sscanf((const char *)temp_buf, WRITE_REGISTER_CODE_CMD
                     " %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s"
                     , str, str+3, str+6, str+9, str+12, str+15, str+18
                     , str+21, str+24, str+27, str+30, str+33, str+36, str+39
                     , str+42, str+45);
    if(ret == REGISTER_CODE_LEN)
    {
      int index = 0, i = 0;
      for(; index < REGISTER_CODE_LEN*3; index += 3)
      {
        temp_buf[i++] = hextoi(str+index);
      }
      /*注册*/
      int ret = Register_Code_Write(temp_buf);
      if(ret < 0)
      {
        printf("Write Code Faild.\r\n");
        CQ_ManualOffsetInc(Uart_Dev_Handle->cb, len);
        return false;
      }
#if ENABLE_REGISTER_DEBUG       
      printf("\r\nGet Registe HEX Code:");
      debug_print(temp_buf, REGISTER_CODE_LEN);
      for(index = 2; index < REGISTER_CODE_LEN*3-3; index += 3)
      {
        str[index] = str[index] == '\0'?' ':str[index];
      }
      printf("Register Code:%s", str);
#endif
      CQ_ManualOffsetInc(Uart_Dev_Handle->cb, len);
      return true;
    }
    CQ_ManualOffsetInc(Uart_Dev_Handle->cb, 1);
    return false;
  }
  else
  {
    CQ_ManualOffsetInc(Uart_Dev_Handle->cb, 1);
  }
  return false;
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
  * @brief   设备注册接口启动
  * @param   [in]None.
  * @return  true 注册成功
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
bool Register_Port_Start(void)
{  
  /*读取注册码*/
  Flash_Port_Read_Partition_Data(FLAG_PARTITION_NAME, (uint8_t *)&Register_Code, 0, sizeof(FRIMWARE_INFO_Typedef_t));
  
  /*解码成UID*/
  /*解密*/
  AES_RUN_RESULT_Typedef_t *p_DEResult = Encrypt_Decrypt_Data(DECRYPT_MODE, (uint8_t *)&Register_Code.Register_Code, REGISTER_CODE_LEN);
  if(p_DEResult == NULL)
  {
    printf("run aes decrypt faild.\r\n");
    return NULL;
  }
#if ENABLE_REGISTER_DEBUG
  printf("\r\nCheck decrypt data:");
  debug_print(p_DEResult->result_Data, p_DEResult->data_len-1);
#endif 
  /*不匹配则等待注册*/
  if(false == Check_Data((const uint8_t *)Device_UID, (const uint8_t *)p_DEResult->result_Data, COMPARE_CODE_LEN))
  {
    printf("\r\nPlease Write Command: the \"READ UID\" or \"READ REGISTER STATUS\"first and write the \"WRITE REGISTER CODE\" at after.\r\n");
    while(1)
    {
      if(Register_Task_Start() == true)
      {
        break;
      }
      HAL_Delay(10);
    }
    AES_Run_Result_Free(p_DEResult);
    return false;
  }
  printf("\r\nCongratulations Verification PASS!\r\n");
  AES_Run_Result_Free(p_DEResult);
  return true;
}

/**
  ******************************************************************
  * @brief   设备注册接口初始化
  * @param   [in]None.
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/22
  ******************************************************************
  */
void Register_Port_Init(void)
{
  /*获取输入输出接口*/
  Uart_Dev_Handle = Uart_Port_Get_Handle(UART_NUM_1);
  if(Uart_Dev_Handle == NULL)
  {
    printf("Get IO Faild.\r\n");
    return;
  }
  
  /*读取设备UID*/
  Get_DeviceUID();
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
