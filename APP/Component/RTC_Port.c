/**
 *  @file RTC_Port.c
 *
 *  @date 2021-01-01
 *
 *  @author aron566
 *
 *  @copyright None.
 *
 *  @brief rtc配置接口
 *
 *  @details None.
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
#include <time.h>

/* Private includes ----------------------------------------------------------*/
#include "RTC_Port.h"
#include "main.h"
#include "rtc.h"
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/
#define RTC_SET_CONFIG_FLAG	0x6066/**< RTC设置完成标志*/
/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/

/** Private function prototypes ----------------------------------------------*/

/** Private user code --------------------------------------------------------*/
/** Private application code -------------------------------------------------*/
/*******************************************************************************
*
*       Static code
*
********************************************************************************
*/

/** Public application code --------------------------------------------------*/
/*******************************************************************************
*
*       Public code
*
********************************************************************************
*/
/**
  * @fn RTC_Set_Time
  * @brief 设置RTC时间
  * @details 
  * @param[in] 时 分 秒
  * @return None
  * @note None
  * @attention None
  */
void RTC_Set_Time(uint8_t Hours ,uint8_t Minutes ,uint8_t Seconds)
{
    RTC_TimeTypeDef ConfigTime;
    ConfigTime.Hours = Hours;            
    ConfigTime.Minutes = Minutes;          
    ConfigTime.Seconds = Seconds; 
    if(HAL_RTC_SetTime(&hrtc, &ConfigTime, RTC_FORMAT_BIN) != HAL_OK)
    {
    Error_Handler();
    }
}

/**
  * @fn RTC_Set_Date
  * @brief 设置RTC日期
  * @details 
  * @param[in] 年 月 日 周
  * @return None
  * @note None
  * @attention None
  */
void RTC_Set_Date(uint8_t Year ,uint8_t Month ,uint8_t Date ,uint8_t WeekDay)
{
    RTC_DateTypeDef ConfigDate;     
    ConfigDate.Year = Year; 
    ConfigDate.Month = Month;
    ConfigDate.Date = Date; 
    ConfigDate.WeekDay = WeekDay;  
    if(HAL_RTC_SetDate(&hrtc, &ConfigDate, RTC_FORMAT_BIN) != HAL_OK)
    {
    Error_Handler();
    }
}

/**
  * @fn RTC_DateTime_Init
  * @brief 初始化RTC时间
  * @details 
  * @param[in] 时 分 秒
  * @return None
  * @note None
  * @attention None
  */
void RTC_DateTime_Init(void)
{
    /*判断是否第一次配置*/
    if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1) != RTC_SET_CONFIG_FLAG)
    {
        /*设置时间为：2021-1-28-周四，1点7分0秒*/
        RTC_Set_DateTime(21 ,1 ,28 ,4 ,1 ,7 ,0);
        /*设置已初始化标志*/
        /*取消备份区域写保护*/
        HAL_PWR_EnableBkUpAccess();                       
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,RTC_SET_CONFIG_FLAG);
    }
}

/**
  * @fn RTC_Set_DateTime
  * @brief 设置RTC日期时间
  * @details 
  * @param[in] None
  * @return None
  * @note None
  * @attention None
  */
void RTC_Set_DateTime(uint8_t year ,uint8_t month, uint8_t date, uint8_t weekday, uint8_t hour, uint8_t minute, uint8_t sec)
{
    RTC_Set_Date(year ,month ,date ,weekday);
    RTC_Set_Time(hour ,minute ,sec);
}

/**
  * @fn Get_CurrentTime
  * @brief 获取当前时间
  * @details None
  * @param None
  * @return -1：获取时间失败 0：成功
  * @note None
  * @attention None
  */
int Get_Date_Time(DateTime_Typedef *pDateTime)
{
    RTC_DateTypeDef CurrentDate;
    RTC_TimeTypeDef CurrentTime;

    /*先获取时间，再获取日期，否则有时延*/
    if((HAL_OK == HAL_RTC_GetTime(&hrtc, &CurrentTime, RTC_FORMAT_BIN))
     && HAL_OK == HAL_RTC_GetDate(&hrtc, &CurrentDate, RTC_FORMAT_BIN))
    {
      /*获取年月日*/
      pDateTime->Year     = CurrentDate.Year + 2000;
      pDateTime->Month    = CurrentDate.Month;
      pDateTime->Date     = CurrentDate.Date;
      pDateTime->WeekDay  = CurrentDate.WeekDay;
      pDateTime->Hours    = CurrentTime.Hours;
      pDateTime->Minutes  = CurrentTime.Minutes; 
      pDateTime->Seconds  = CurrentTime.Seconds;
      return 0 ;
    }
    return -1 ;
}

/**
  ******************************************************************
  * @brief   获取当前时间秒数
  * @param   [in]None
  * @return  s
  * @author  aron566
  * @version V1.0
  * @date    2020-01-04
  ******************************************************************
  */
uint32_t RTC_Current_Time_S(void)
{
    struct tm tm_data;
    DateTime_Typedef current_date_time;
    Get_Date_Time(&current_date_time);
    tm_data.tm_year = (int)current_date_time.Year + 2000 - 1900;  
    tm_data.tm_mon  = (int)current_date_time.Month - 1;  
    tm_data.tm_mday = (int)current_date_time.Date;  
    tm_data.tm_hour = (int)current_date_time.Hours;  
    tm_data.tm_min  = (int)current_date_time.Minutes;  
    tm_data.tm_sec  = (int)current_date_time.Seconds;

    return (uint32_t)mktime(&tm_data);
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
