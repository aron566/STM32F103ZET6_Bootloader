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
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#if USE_EASYFLASH
#include "elog_flash.h"
#endif
#include <string.h>
#include <stdio.h>

#if USE_LINUX_SYSTEM        /**< 使用linux操作系统下的信号量*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/prctl.h>

#include <unistd.h>
#include <pthread.h>

#define ELOG_FILE_SEM_KEY   ((key_t)0x19910614)
#define get_thread_name(name)   prctl(PR_GET_NAME, name)

#ifdef _SEM_SEMUN_UNDEFINED
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};
#endif

static int semid = -1;
static struct sembuf const up = {0, 1, SEM_UNDO};
static struct sembuf const down = {0, -1, SEM_UNDO};

static void linux_lock_init(void);
static int linux_lock_open(void);

#elif USE_FREERTOS_SYSTEM                 /**< 使用FreeRTOS操作系统下的信号量*/
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
osSemaphoreId ELOGTaskBinarySem01Handle;
#endif
static void lock_init(void);
static void lock(void);
static void unlock(void);
static char *Get_Datetime(void);
static const char *Get_Thread_Name(void);

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    /* definition and creation of LOGTaskBinarySem01 */
    lock_init();
    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
    /* add your code here */
     printf("%.*s", size ,log);
    /* output to flash */
#if USE_EASYFLASH
    elog_flash_write(log, size);
#endif
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    lock();
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    unlock();
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    /* add your code here */
    const char *datetime = (const char*)Get_Datetime();
    return datetime;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "Main Func";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    const char *thread_name = (const char *)Get_Thread_Name();
    return thread_name;
}

#if USE_LINUX_SYSTEM
/**
 * initialize the lock 
 */
static void linux_lock_init(void)
{
    int id, rc;
    union semun arg;
    struct sembuf sembuf;

    id = semget(ELOG_FILE_SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0666);
    if(likely(id == -1)) {
        id = linux_lock_open();
        if (id == -1)
            goto __exit;
    } else {
        arg.val = 0;
        rc = semctl(id, 0, SETVAL, arg);
        if (rc == -1)
            goto __exit;

        sembuf.sem_num = 0;
        sembuf.sem_op = 1;
        sembuf.sem_flg = 0;

        rc = semop(id, &sembuf, 1);
        if (rc == -1)
            goto __exit;
    }

    semid = id;
__exit:
    return ;
}

/**
 * gets the lock 
 */
static int linux_lock_open(void)
{
    int id, rc, i;
    union semun arg;
    struct semid_ds ds;

    id = semget(ELOG_FILE_SEM_KEY, 1, 0666);
    if(unlikely(id == -1))
        goto err;

    arg.buf = &ds;

    for (i = 0; i < 10; i++) {
        rc = semctl(id, 0, IPC_STAT, arg);
        if (unlikely(rc == -1))
            goto err;

        if(ds.sem_otime != 0)
            break;

        usleep(10 * 1000);
    }

    if (unlikely(ds.sem_otime == 0))
        goto err;

    return id;
err:
    return -1;
}
#endif

/**
  ******************************************************************
  * @brief   信号量初始化，操作系统下的初始化需放main中
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
    ELOGTaskBinarySem01Handle = osSemaphoreNew(1, 1, NULL);
#elif USE_LINUX_SYSTEM
    linux_lock_init();
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
    osSemaphoreAcquire(ELOGTaskBinarySem01Handle, 0);
#elif USE_LINUX_SYSTEM
    semid == -1 ? -1 : semop(semid, (struct sembuf *)&down, 1);
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
    osSemaphoreRelease(ELOGTaskBinarySem01Handle);
#elif USE_LINUX_SYSTEM
    semid == -1 ? -1 : semop(semid, (struct sembuf *)&up, 1);
#endif
}

/**
  ******************************************************************
  * @brief   获取当前时间
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/26
  ******************************************************************
  */
static char *Get_Datetime(void)
{
    static char datetime[20];
#if USE_FREERTOS_SYSTEM
    sprintf(datetime ,"%d" ,HAL_GetTick());
#elif USE_LINUX_SYSTEM
    time_t timestamp;
	struct tm *p_tm;
	//经过的秒数
	timestamp = time((time_t *) NULL);
	p_tm = localtime(&timestamp);
	sprintf(datetime ,"%d.%d.%d.%d.%d" ,p_tm->tm_year + 1900 ,p_tm->tm_mon + 1,
			p_tm->tm_mday ,p_tm->tm_hour ,p_tm->tm_min);
#endif
    return datetime;
}

/**
  ******************************************************************
  * @brief   获取当前线程名称
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/26
  ******************************************************************
  */
static const char *Get_Thread_Name(void)
{
#if USE_FREERTOS_SYSTEM
    TaskStatus_t TaskStatus;
    osThreadId Current_Thread = osThreadGetId();

    //获取任务信息
    vTaskGetInfo((TaskHandle_t  )Current_Thread,    //任务句柄
                 (TaskStatus_t* )&TaskStatus,       //任务信息结构体
                 (BaseType_t    )pdTRUE,            //允许统计任务堆栈历史最小剩余大小
                 (eTaskState    )eInvalid);         //函数自己获取任务运行壮态

//    printf("任务名:                %s\r\n",TaskStatus.pcTaskName);
//    printf("任务编号:              %d\r\n",(int)TaskStatus.xTaskNumber);
//    printf("任务壮态:              %d\r\n",TaskStatus.eCurrentState);
//    printf("任务当前优先级:        %d\r\n",(int)TaskStatus.uxCurrentPriority);
//    printf("任务基优先级:          %d\r\n",(int)TaskStatus.uxBasePriority);
//    printf("任务堆栈基地址:        %#x\r\n",(int)TaskStatus.pxStackBase);
//    printf("任务堆栈历史剩余最小值: %d\r\n",TaskStatus.usStackHighWaterMark);
    return TaskStatus.pcTaskName;
#elif USE_LINUX_SYSTEM
    static char thread_name[20];
    get_thread_name(thread_name);
    return thread_name;
#else
    return "";
#endif
}
