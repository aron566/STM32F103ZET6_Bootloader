/**
 *  @file User_Main.h
 *
 *  @date 2021/2/1
 *
 *  @author aron566
 *
 *  @brief 引导任务
 *  
 *  @version v1.0
 */
#ifndef USER_MAIN_H
#define USER_MAIN_H
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
#include <limits.h>/**< if need INT_MAX*/
/** Private includes ---------------------------------------------------------*/
#include "Flash_Port.h"
#include "LED_Port.h"
#include "Timer_Port.h"
#include "UART_Port.h"
#include "CircularQueue.h"
#include "utilities.h"
#include "utilities_crc.h"
#include "Shell_Port.h"
#include "LOG_Port.h"
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*引导任务初始化*/
void User_Main_Init(void);
/*引导任务启动*/
void User_Main_Task_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
