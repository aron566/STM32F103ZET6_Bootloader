/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
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
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);
/************************ 外部接口 ********************************/
#include "main.h"

extern SPI_HandleTypeDef hspi2;
#if USE_FREERTOS_SYSTEM /**< 使用FreeRTOS操作系统下的信号量，则操作flash需在线程下执行，且变量不可超出线程栈大小！！！*/
#include "cmsis_os.h"
osSemaphoreId SPI2TaskBinarySem01Handle;
#endif
static uint32_t SPI_userData = 0;
/**
  ******************************************************************
  * @brief   使能Flash
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void HAL_W25QXX_CS_ENABLE(void)
{
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin, GPIO_PIN_RESET);
}

/**
  ******************************************************************
  * @brief   失能Flash
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void HAL_W25QXX_CS_DISABLE(void)
{
	HAL_GPIO_WritePin(SPI2_CS_GPIO_Port,SPI2_CS_Pin, GPIO_PIN_SET);
}

/**
  ******************************************************************
  * @brief   SPI发送一个字节
  * @param   [in]byte
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void HAL_SPI_Send_Byte(uint8_t byte)
{
	HAL_SPI_Transmit(&hspi2,&byte,1,0xFFFF);
}

/**
  ******************************************************************
  * @brief   SPI接收一个字节
  * @param   [in]None
  * @retval  字节
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static uint8_t HAL_SPI_Receive_Byte(void)
{
	uint8_t data = 0xFF;
	HAL_SPI_Receive(&hspi2,&data,1,0xFFFF);
	return data;
}

/**
  ******************************************************************
  * @brief   SPI多字节发送
  * @param   [in]pbyte 发送字节数
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void HAL_Flash_Page_Write(const uint8_t* pbuffer, uint16_t num_byte_to_write)
{
    /* select the flash: chip select low */
    HAL_W25QXX_CS_ENABLE();

    /* while there is data to be written on the flash */
    while(num_byte_to_write--){
        /* send the current byte */
        HAL_SPI_Send_Byte(*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }
}

/**
  ******************************************************************
  * @brief   SPI多字节接收
  * @param   [in]pbyte 接收字节数
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void HAL_Flash_Buffer_Read(uint8_t* pbuffer, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    HAL_W25QXX_CS_ENABLE();

    /* while there is data to be read */
    while(num_byte_to_read--){
        /* read a byte from the flash */
        *pbuffer = HAL_SPI_Receive_Byte();
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }
}

/**
  ******************************************************************
  * @brief   SPI信号量初始化
  * @param   [in]None
  * @retval  None
  * @author  aron566
  * @version v1.0
  * @date    2020/6/9
  ******************************************************************
  */
static void SPI_Lock_Init(void)
{
#if USE_FREERTOS_SYSTEM
	SPI2TaskBinarySem01Handle = osSemaphoreNew(1, 1, NULL);
#endif
}
/* lock SPI bus */
static void SPI_lock(const struct __sfud_spi *spi)
{
#if USE_FREERTOS_SYSTEM
	osSemaphoreAcquire(SPI2TaskBinarySem01Handle, 0);
#endif
}
/* unlock SPI bus */
static void SPI_unlock(const struct __sfud_spi *spi)
{
#if USE_FREERTOS_SYSTEM
	osSemaphoreRelease(SPI2TaskBinarySem01Handle);
#endif
}

static void SPI_Delay(void)
{
#if USE_FREERTOS_SYSTEM
   osDelay(10);
#else
   uint32_t delay = 200;
   while(delay--);
#endif
}
/********************** END 外部接口 ******************************/

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your spi write and read code
     */
    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }
    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }
    HAL_W25QXX_CS_ENABLE();
	HAL_Flash_Page_Write(write_buf ,write_size);
	HAL_Flash_Buffer_Read(read_buf ,read_size);
    HAL_W25QXX_CS_DISABLE();
    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    SPI_Lock_Init();
    flash->spi.wr = spi_write_read;
    flash->spi.lock = SPI_lock;
    flash->spi.unlock = SPI_unlock;
    flash->spi.user_data = &SPI_userData;
    flash->retry.delay = SPI_Delay;
    flash->retry.times = 60*10000; //60 Sec
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
