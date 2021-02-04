/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for EasyLogger's flash log pulgin.
 * Created on: 2015-07-28
 */

#include "elog_flash.h"
#include <stdio.h>
#if USE_NONE_SYSTEM         /**< 未使用操作系统下的操作锁*/

#elif USE_FREERTOS_SYSTEM   
#include "stm32f1xx.h"
#include "cmsis_os.h"
osSemaphoreId ELOGFLASHTaskBinarySem01Handle;
#endif
static void lock_init(void);
static void lock(void);
static void unlock(void);
/**
 * EasyLogger flash log pulgin port initialize
 *
 * @return result
 */
ElogErrCode elog_flash_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;
    
    /* add your code here */
    lock_init();
    return result;
}

/**
 * output flash saved log port interface
 *
 * @param log flash saved log
 * @param size log size
 */
void elog_flash_port_output(const char *log, size_t size) {
    
    /* add your code here */
    //Flash中的日志被读取后的输出接口
    printf("%.*s", size ,log);
}

/**
 * flash log lock
 */
void elog_flash_port_lock(void) {
    
    /* add your code here */
    lock();
}

/**
 * flash log unlock
 */
void elog_flash_port_unlock(void) {
    
    /* add your code here */
    unlock();
}

/**
  ******************************************************************
  * @brief   信号量初始化
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/26
  ******************************************************************
  */
static void lock_init(void)
{
#if USE_FREERTOS_SYSTEM
    ELOGFLASHTaskBinarySem01Handle = osSemaphoreCreate(osSemaphore(ELOGFLASHTaskBinarySem01ControlBlock), 1);
    xSemaphoreGive(ELOGFLASHTaskBinarySem01Handle);
#elif USE_NONE_SYSTEM

#endif
}

/**
  ******************************************************************
  * @brief   信号量上锁
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/26
  ******************************************************************
  */
static void lock(void)
{
#if USE_FREERTOS_SYSTEM
    osSemaphoreAcquire(ELOGFLASHTaskBinarySem01Handle,(TickType_t)0xffffffUL);
#elif USE_NONE_SYSTEM
    __disable_irq();
#endif
}

/**
  ******************************************************************
  * @brief   信号量解锁
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/26
  ******************************************************************
  */
static void unlock(void)
{
#if USE_FREERTOS_SYSTEM
    osSemaphoreRelease(ELOGFLASHTaskBinarySem01Handle);
#elif USE_NONE_SYSTEM
    __enable_irq();
#endif
}
