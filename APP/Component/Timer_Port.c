/**
 *  @file Timer_Port.c
 *
 *  @date 2021-01-01
 *
 *  @author aron566
 *
 *  @copyright 爱谛科技研究院.
 *
 *  @brief 定时器接口
 *
 *  @details 1、
 *
 *  @version V1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "Timer_Port.h"
#include "main.h"
/** Private typedef ----------------------------------------------------------*/

/** Private macros -----------------------------------------------------------*/

/** Private constants --------------------------------------------------------*/
/** Public variables ---------------------------------------------------------*/
/** Private variables --------------------------------------------------------*/
static uint32_t Timer_Port_TimeMS = 0;
static uint32_t Timer_Port_TimeSec = 0;
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
  ******************************************************************
  * @brief   上电运行时间
  * @param   [in]None
  * @return  clock_t 运行时间
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
clock_t clock(void) 
{
  return Timer_Port_TimeSec;
}

/**
  ******************************************************************
  * @brief   经过的秒数
  * @param   [in]None
  * @return  time_t 秒数
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
#if _DLIB_TIME_USES_64

time_t __time64(time_t *p) 
{
  return Timer_Port_TimeSec;
}
#else
time_t __time32(time_t *p) 
{
  return Timer_Port_TimeSec;
}
#endif

/**
  ******************************************************************
  * @brief   获取时区
  * @param   [in]None
  * @return  时区
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
const char *__getzone(void)
{
  return ": GMT+8:GMT+9:+0800";
}

/**
  ******************************************************************
  * @brief   设置时间
  * @param   [in]year
  * @param   [in]month
  * @param   [in]day
  * @param   [in]hour
  * @param   [in]min
  * @param   [in]sec
  * @return  None.
  * @author  aron566
  * @version v1.0
  * @date    2021/2/1
  ******************************************************************
  */
void Timer_Port_Set_Time(int year, int month, int day, int hour, int min, int sec)
{
  struct tm set_time;
  set_time.tm_sec = 10;
  set_time.tm_min = 0;
  set_time.tm_hour = 8;
  set_time.tm_mday = 13;
  set_time.tm_mon = 10-1;
  set_time.tm_year = 2017-1900;
  //set_time.tm_wday = 1;
  //set_time.tm_yday = 2;
  set_time.tm_isdst = -1;
  Timer_Port_TimeSec = mktime(&set_time);
}

/**
  ******************************************************************
  * @brief   定时器中断回调
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
void Timer_Port_IRQHandler(void)
{
  Timer_Port_TimeMS++;
  if(Timer_Port_TimeMS == 1000)
  {
    Timer_Port_TimeMS = 0;
    Timer_Port_TimeSec++;
  }
}

/**
  ******************************************************************
  * @brief   初始化定时器
  * @param   [in]None
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
void Timer_Port_Init(void)
{
//  HAL_TIM_Base_Start_IT(&htim3);
}

/**
  ******************************************************************
  * @brief   获取当前运行累计时间
  * @param   [in]time_base 单位
  * @return  None.
  * @author  aron566
  * @version V1.0
  * @date    2021-01-13
  ******************************************************************
  */
uint32_t Timer_Port_Get_Current_Time(TIMER_TIME_UNIT_Typedef_t time_unit)
{
  
  return (time_unit == TIMER_MS)?Timer_Port_TimeMS:Timer_Port_TimeSec;
}

#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
