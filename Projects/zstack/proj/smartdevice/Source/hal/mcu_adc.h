/**
 ******************************************************************************
  * @file       mcu_adc.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      CC2530 ADC配置头文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                          
  * 				 IAR
 ******************************************************************************
  * @note
  * 1.XXXXX                  						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CC2530_MCU_ADC_H__
#define __CC2530_MCU_ADC_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include <ioCC2530.h>
#include "hal_mcu.h"

/* Exported macro ------------------------------------------------------------*/
// AD参考电压配置
#define ADC_SREF_VREF       (0x00)
#define ADC_SREF_AIN7       (0x01)
#define ADC_SREF_AVDD       (0x02)
#define ADC_SREF_AIN6_AIN7  (0x03)

// AD转换精度
#define ADC_CONVERT_7BIT    (0x00)
#define ADC_CONVERT_9BIT    (0x10)
#define ADC_CONVERT_10BIT   (0x20)
#define ADC_CONVERT_12BIT   (0x30)

// AD转换通道
#define ADC_CONVERT_CH0     (0x00)
#define ADC_CONVERT_CH1     (0x01)
#define ADC_CONVERT_CH2     (0x02)
#define ADC_CONVERT_CH3     (0x03)
#define ADC_CONVERT_CH4     (0x04)
#define ADC_CONVERT_CH5     (0x05)
#define ADC_CONVERT_CH6     (0x06)
#define ADC_CONVERT_CH7     (0x07)
// AD转换差分输入信号
#define ADC_CONVERT_CH0_CH1 (0x08)
#define ADC_CONVERT_CH2_CH3 (0x09)
#define ADC_CONVERT_CH4_CH5 (0x0A)
#define ADC_CONVERT_CH6_CH7 (0x0B)
// AD转换其它信号
#define ADC_CONVERT_GND     (0x0C)
#define ADC_CONVERT_VREF    (0x0D)
#define ADC_CONVERT_TEMP    (0x0E)
#define ADC_CONVERT_VDD_3   (0x0F)

// 使能GPIO
#define USE_P0_0_AS_ADC_CH0 (0x01)
#define USE_P0_1_AS_ADC_CH1 (0x02)
#define USE_P0_2_AS_ADC_CH2 (0x04)
#define USE_P0_3_AS_ADC_CH3 (0x08)
#define USE_P0_4_AS_ADC_CH4 (0x10)
#define USE_P0_5_AS_ADC_CH5 (0x20)
#define USE_P0_6_AS_ADC_CH6 (0x40)
#define USE_P0_7_AS_ADC_CH7 (0x80)

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern bool Rd_ADC_Data(uint16*, uint8);
extern void MCU_ADC_CH_Init(uint8);
extern void MCU_ADC_CH_Deinit(uint8);
extern void MCU_ADC_SIGNAL_START(uint8, uint8, uint8);

#endif      /* __CC2530_MCU_ADC_H__ */

/**********************************END OF FILE*********************************/
