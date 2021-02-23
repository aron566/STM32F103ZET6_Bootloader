/**                                                                             
 *  @file Bootloader.h                                                    
 *                                                                              
 *  @date 2021年02月13日 15:11:33 星期六
 *                                                                              
 *  @author aron566                                                             
 *                                                                              
 *  @brief None.                                                                
 *                                                                              
 *  @version V1.0                                                               
 */                                                                             
#ifndef BOOTLOADER_H                                                          
#define BOOTLOADER_H                                                          
#ifdef __cplusplus ///< use C compiler                                           
extern "C" {                                                                   
#endif                                                                          
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */                          
#include <stddef.h> /**< need definition of NULL    */                          
#include <stdbool.h>/**< need definition of BOOL    */                        
#include <stdio.h>  /**< if need printf             */                          
#include <stdlib.h>                                                             
#include <string.h>                                                             
#include <limits.h> /**< need variable max value    */                          
/** Private includes ---------------------------------------------------------*/
#include "Shell_Port.h"
/** Private defines ----------------------------------------------------------*/
#define FRIMWARE_BL_PARTITION_NAME          "bl"      /**< bootloader分区名*/
#define FRIMWARE_DOWNLOAD_PARTITION_NAME    "download"/**< 固件下载分区名*/
#define FRIMWARE_FACTORY_PARTITION_NAME     "factory" /**< 工厂固件分区名*/
#define FRIMWARE_APP_PARTITION_NAME         "app"     /**< 应用固件分区名*/
#define FRIMWARE_FLAG_PARTITION_NAME        "fdb_kvdb1"/**< 升级标识分区名*/
#define COUNT_DONW_TIME_SEC                 3U        /**< 倒计时时间*/
#define DOUNT_DOWN_DELAY_TIME               (COUNT_DONW_TIME_SEC*10)
#define RETRY_UPDATE_NUM_MAX                3U        /**< 更新重试次数*/
#define FRIMWARE_NAME_LEN_MAX               64U       /**< 固件名称长度*/
#define REGISTER_CODE_LEN_MAX               (4*8U)    /**< 注册码长度Byte*/
/** Exported typedefines -----------------------------------------------------*/

/*bootload 当前数据crc结果*/
typedef enum
{ 
  DATA_CRC_ERROR = 0,
  DATA_CRC_OK,
}DATA_CRC_RESULT_Typedef_t;

/*升级标识*/
typedef enum
{
  UPDATE_SUCESSFUL  = 0xAD8888,                       /**< 更新成功*/
  UPDATE_WAITTING   = 0xAD6666,                       /**< 等待更新*/
  DOWNLOAD_COMPLETE = 0xAD5555,                       /**< 下载完成*/
}FRIMWARE_UPDATE_FLAG_Typedef_t;      
      
/*更新区域类型*/      
typedef enum      
{     
  DOWN_LOAD_TYPE = 0,     
  FACTORY_TYPE,     
}UPDATE_FRIMWARE_TYPE_Typedef_t;      
      
/*固件信息*/      
typedef struct      
{     
  char FrimwareName[FRIMWARE_NAME_LEN_MAX];           /**< 固件名称*/
  uint32_t FrimwareSize;                              /**< 固件大小*/
  char BackFrimwareName[FRIMWARE_NAME_LEN_MAX];       /**< facotry固件名称*/
  uint32_t BackFrimwareSize;                          /**< facotry固件大小*/
  FRIMWARE_UPDATE_FLAG_Typedef_t Flag;                /**< 升级标识*/
  uint32_t Retry_Cnt;                                 /**< 等待更新次数累计*/
  uint32_t Register_Code[REGISTER_CODE_LEN_MAX/4];    /**< 注册码*/
}FRIMWARE_INFO_Typedef_t;

/*Bootloader 任务句柄*/
typedef void (*JUMP_TO_ADDR_CALLBACK)(uint32_t App_Addr);
typedef uint32_t (*GET_PARTITION_ADDR_CALLBACK)(const char *Partition_Name);
typedef void (*ERASE_PARTITION_CALLBACK)(const char *Partition_Name);
typedef int (*WRITE_PARTITION_DATA_CALLBACK)(const char *Partition_Name, const uint8_t *data, uint32_t Offset, uint32_t Size);
typedef int (*READ_PARTITION_DATA_CALLBACK)(const char *Partition_Name, uint8_t *Dest_Buf, uint32_t Offset, uint32_t Read_Size);
typedef uint32_t (*GET_PARTITION_SIZE)(const char *Partition_Name);
typedef void (*CUSTOMIZE_PROTOCOL_STACK_INIT_CALLBACK)(void);
typedef bool (*CUSTOMIZE_PROTOCOL_STACK_START_CALLBACK)(void);
typedef void (*CUSTOMIZE_PROTOCOL_STACK_UPDATE_FRIMWARE_INFO)(FRIMWARE_INFO_Typedef_t *Frimware_Info, UPDATE_FRIMWARE_TYPE_Typedef_t Down_Type);
typedef uint32_t (*GET_CURRENT_TIME_SEC_CALLBACK)(void);
typedef bool (*IS_PAUSE_STATE)(void);
typedef void (*PRINT_COUNT_DOWN_TIME_CALLBACK)(uint32_t sec);
typedef struct
{
  FRIMWARE_INFO_Typedef_t Frimware_Info;                                        /**< 固件信息*/
  JUMP_TO_ADDR_CALLBACK pJump_func;                                             /**< 跳转到指定地址*/
  GET_PARTITION_ADDR_CALLBACK pGet_Partition_Addr_func;                         /**< 获取指定分区地址*/
  ERASE_PARTITION_CALLBACK p_Erase_Partition_func;                              /**< 擦除指定分区数据*/
  WRITE_PARTITION_DATA_CALLBACK p_Write_Partition_Data_func;                    /**< 写入数据到分区*/
  READ_PARTITION_DATA_CALLBACK p_Read_Partition_Data_func;                      /**< 写入数据到分区*/
  GET_PARTITION_SIZE p_Get_Partition_Size_func;                                 /**< 获取分区大小*/

  CUSTOMIZE_PROTOCOL_STACK_INIT_CALLBACK p_Customize_Protocol_Stack_Init_func;  /**< 自定义文件传输协议栈初始化*/
  CUSTOMIZE_PROTOCOL_STACK_START_CALLBACK p_Customize_Protocol_Stack_Start_func;/**< 自定义文件传输协议栈启动*/
  CUSTOMIZE_PROTOCOL_STACK_UPDATE_FRIMWARE_INFO p_Update_Friware_Info_func;     /**< 自定义文件传输协议栈更新固件信息*/

  GET_CURRENT_TIME_SEC_CALLBACK p_Get_Current_Time_Sec_func;                    /**< 获取当前时间*/
  IS_PAUSE_STATE p_Is_Pause_State_func;                                         /**< 检测是否暂停引导*/
  PRINT_COUNT_DOWN_TIME_CALLBACK p_Print_Count_Down_Str_func;                   /**< 打印自定义倒计时字符*/
}BOOTLOADER_Handle_Typedef_t;
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*bl初始化*/
void Bootloader_Init(BOOTLOADER_Handle_Typedef_t *p_Bl_Handle);
/*更新协议栈启动*/
void Bootloader_Start(void);
/*数据下载写入*/
void Bootloader_Download_Data(const uint8_t *data, uint16_t Packet_Num);
  
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
