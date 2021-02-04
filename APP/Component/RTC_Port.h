/**
 *  @file RTC_Port.h
 *
 *  @date 2021-01-04
 *
 *  @author aron566
 *
 *  @brief rtc时间配置接口
 *  
 *  @version V1.0
 */
#ifndef RTC_PORT_H
#define RTC_PORT_H
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <stdint.h> /**< nedd definition of uint8_t */
#include <stddef.h> /**< need definition of NULL    */
#include <stdbool.h>/**< need definition of BOOL    */
#include <stdio.h>  /**< if need printf             */
#include <stdlib.h>
#include <string.h>
#include <limits.h> /**< need variable max value    */
/** Private includes ---------------------------------------------------------*/

/** Private defines ----------------------------------------------------------*/

/** Exported typedefines -----------------------------------------------------*/
typedef struct DateTime
{
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t WeekDay;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
}DateTime_Typedef;

/** Exported constants -------------------------------------------------------*/

/** Exported macros-----------------------------------------------------------*/
/** Exported variables -------------------------------------------------------*/
/** Exported functions prototypes --------------------------------------------*/
void RTC_DateTime_Init(void);
void RTC_Set_DateTime(uint8_t year ,uint8_t month ,uint8_t date ,uint8_t weekday ,uint8_t hour, uint8_t minute ,uint8_t sec);
void RTC_Set_Date(uint8_t Year ,uint8_t Month ,uint8_t Date ,uint8_t WeekDay);
void RTC_Set_Time(uint8_t Hours ,uint8_t Minutes ,uint8_t Seconds);
int Get_Date_Time(DateTime_Typedef *pDateTime);
uint32_t RTC_Current_Time_S(void);
#ifdef __cplusplus ///<end extern c
}
#endif
#endif
/******************************** End of file *********************************/
