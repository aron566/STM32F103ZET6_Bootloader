/**
 *  @file Shell_Port.h
 *
 *  @date 2021/2/2
 *
 *  @author aron566
 *
 *  @brief shell命令解析
 *  
 *  @version v1.0
 */
#ifndef SHELL_PORT_H
#define SHELL_PORT_H
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
#include "shell.h"
/** Private defines ----------------------------------------------------------*/
/** Exported typedefines -----------------------------------------------------*/
/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*shell初始化*/
void Shell_Port_Init(void);
/*shell启动*/
void Shell_Port_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
