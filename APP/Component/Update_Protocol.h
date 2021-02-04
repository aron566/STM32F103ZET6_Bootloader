/**
 *  @file Update_Protocol.h
 *
 *  @date 2021/2/1
 *
 *  @author aron566
 *
 *  @brief �̼�����Э��
 *  
 *  @version v1.0
 */
#ifndef UPDATE_PROTOCOL_H
#define UPDATE_PROTOCOL_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t*/
#include <stddef.h> /**< need definition of NULL*/
#include <stdbool.h>/**< need definition of BOOL*/
#include <stdio.h>  /**< if need printf*/
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
/*bootload ����״̬*/
typedef enum
{
  WAIT_SIGNAL_MODE = 0,       /**< �ȴ�������*/
  UPDATE_MODE,                /**< ������*/
  COMPLETE,                   /**< �����*/
}BOOTLOAD_RUN_MODE_Typedef_t;

/*bootload ���ݴ���Э��*/
typedef enum
{
  YMODEM_PROTOCOCL = 0,
  CUSTOME_PROTOCOL
}UPDATE_PROTOCOL_Typedef_t;

/*bootload ��ǰ�������� or ����*/
typedef enum
{
  UPGRADE_FIREWARE = 0,
  DEMOTION_FIREWARE
}CURRENT_UPDATE_MODE_Typedef_t;

/*bootload ��ǰ����crc���*/
typedef enum
{ 
  DATA_CRC_ERROR = 0,
  DATA_CRC_OK,
}DATA_CRC_RESULT_Typedef_t;

typedef struct
{
  BOOTLOAD_RUN_MODE_Typedef_t run_state;
  UPDATE_PROTOCOL_Typedef_t protocol;
  CURRENT_UPDATE_MODE_Typedef_t current_update_mode;
  
}UPDATE_Handle_Typedef_t;
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*Э��ջ��ʼ��*/
void Update_Protocol_Init(void);
/*Э��ջ����*/
void Update_Protocol_Start(void);
  
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
