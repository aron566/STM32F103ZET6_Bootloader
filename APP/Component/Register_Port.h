/**
 *  @file Register_Port.h
 *
 *  @date 2021/2/22
 *
 *  @author aron566
 *
 *  @brief 设备注册检测
 *  
 *  @version v1.0
 */
#ifndef REGISTER_H
#define REGISTER_H
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
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*设备注册接口初始化*/
void Register_Port_Init(void);
/*设备注册接口启动*/
bool Register_Port_Start(void);

#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
