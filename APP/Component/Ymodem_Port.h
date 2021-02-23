/**
 *  @file Ymodem_Port.h
 *
 *  @date 2021-02-11
 *
 *  @author aron566
 *
 *  @brief YMODEM协议接口
 *  
 *  @version V1.0
 */
#ifndef YMODEM_PORT_H
#define YMODEM_PORT_H
#ifdef __cplusplus ///<use C compiler
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

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*Ymodem协议接口初始化*/
void Ymodem_Port_Init(void);
/*Ymodem协议接口启动*/
bool Ymodem_Port_Start(void);
/*Ymodem协议接口获取文件名*/
const char *Ymodem_Port_Get_File_Name(void);
/*Ymodem协议接口获取文件大小*/
uint32_t Ymodem_Port_Get_File_Size(void);
/*Ymodem设置发送文件信息*/
void Ymodem_Set_Send_File_Info(const char *file_name, uint32_t file_size);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
