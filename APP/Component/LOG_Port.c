/**
 *  @file LOG_Port.c
 *
 *  @date 2020/6/9
 *
 *  @author aron566
 *
 *  @copyright None
 *
 *  @brief Easylogger操作
 *
 *  @details --
 *
 *  @version v1.0
 */
#ifdef __cplusplus ///<use C compiler
extern "C" {
#endif
/** Includes -----------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "LOG_Port.h"
#define LOG_TAG    "LOG.TEST_TAR"
#include <elog.h>
#if USE_EASYFLASH
#include "elog_flash.h"
#endif
/** Private typedef ----------------------------------------------------------*/
/** Private macros -----------------------------------------------------------*/
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
  ******************************************************************
  * @brief   日志功能初始化
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version V1.0
  * @date    2020-6-2
  ******************************************************************
  */
void EasyLog_Init(void)
{
    /* close printf buffer */
    setbuf(stdout, NULL);
    
	elog_init();

	/* set EasyLogger log format */
	/* 断言：输出所有内容 */
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	/* 错误：输出级别、标签和时间 */
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	/* 警告：输出级别、标签和时间 */
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	/* 信息：输出级别、标签和时间 */
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	/* 调试：输出除了方法名之外的所有内容 */
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
	/* 详细：输出除了方法名之外的所有内容 */
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);

	/*Eenbale color*/
	elog_set_text_color_enabled(true);
    
    /* initialize EasyLogger Flash plugin */
#if USE_EASYFLASH
    elog_flash_init();
#endif
    
	/* start EasyLogger */
	elog_start();
}


/**
  ******************************************************************
  * @brief   日志功能测试,带RTOS时在线程内使用
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version V1.0
  * @date    2020-6-2
  ******************************************************************
  */
void Test_LOG_Func(void)
{
    log_a("Hello EasyLogger!");
    log_e("Hello EasyLogger!");
    log_w("Hello EasyLogger!");
    log_i("Hello EasyLogger!");
    log_d("Hello EasyLogger!");
    log_v("Hello EasyLogger!");
//    elog_flush();
}
    
#ifdef __cplusplus ///<end extern c
}
#endif
/******************************** End of file *********************************/
