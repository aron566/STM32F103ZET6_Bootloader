/**
 *  @file LOG_Port.h
 *
 *  @date 2020/6/9
 *
 *  @author aron566
 *
 *  @brief None
 *  
 *  @version v1.0
 */
#ifndef LOG_PORT_H
#define LOG_PORT_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< need definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
//#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
/** Private includes ---------------------------------------------------------*/
/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/

/** Exported constants -------------------------------------------------------*/
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
void EasyLog_Init(void);
void Test_LOG_Func(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
