/**
 *  @file Flash_Port.h
 *
 *  @date 2021-01-13
 *
 *  @author aron566
 *
 *  @brief FLASH操作接口
 *  
 *  @version V1.0
 */
#ifndef FLASH_PORT_H
#define FLASH_PORT_H
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
#include "sfud.h"
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*Flash操作初始化*/
void Flash_Port_Init(void);
/*Flash擦除指定分区*/
bool Flash_Port_Erase_Partition(const char *Partition_Name);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
