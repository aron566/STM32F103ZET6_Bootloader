/**                                                                             
 *  @file Bootloader_Port.h
 *                                                                              
 *  @date 2021年02月13日 15:19:59 星期六
 *                                                                              
 *  @author aron566                                                             
 *                                                                              
 *  @brief None.                                                                
 *                                                                              
 *  @version V1.0                                                               
 */                                                                             
#ifndef BOOTLOADER_PORT_H                                                          
#define BOOTLOADER_PORT_H                                                          
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
                                                                                
/** Private defines ----------------------------------------------------------*/
                                                                      
/** Exported typedefines -----------------------------------------------------*/
                                                                     
/** Exported constants -------------------------------------------------------*/
                                                                                
/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/

/*bl接口初始化*/
void Bootloader_Port_Init(void);
/*bl接口启动*/
void Bootloader_Port_Start(void);
/*bl接口数据下载写入*/
void Bootloader_Port_Download_Data(const uint8_t *data, uint16_t Packet_Num);

#ifdef __cplusplus ///<end extern c                                             
}                                                                               
#endif                                                                          
#endif                                                                          
/******************************** End of file *********************************/
