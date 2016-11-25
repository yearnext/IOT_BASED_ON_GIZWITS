 /**
 ******************************************************************************
  * @file       mcu_adc.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      CC2530 ADC配置源文件
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

/**
 * @defgroup CC2530 ADC配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "mcu_adc.h"

/* Exported macro ------------------------------------------------------------*/
// 启动ADC转换
#define MCU_ADC_START()     ( ADCCON1 |= 0x70 )

// ADC转换完成标识
#define MCU_ADC_FLAG        ( ADCCON1 & 0x80 )

// 使能AD转换中断
#define enable_adc_isr()    (IEN0 |= 0x02)
//失能AD转换中断
#define disable_adc_isr()   (IEN0 &= ~0x02)

// 清除ADC中断标志
#define clr_adc_isr_flag()  (TCON &= ~0x20)
// AD中断标志
#define adc_isr_flag        (TCON & 0x20)

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
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       读取AD转换数值
 * @param       [in/out]  void
 * @return      [in/out]  bool    程序运行状态
 * @note        ADC工作在单次转换模式
 *******************************************************************************
 */
bool Rd_ADC_Data( uint16 *data, uint8 convert_bit )
{
    uint16 temp = (uint16)(ADCH << 8);
    temp |= (uint16)ADCL;
    
    if( !MCU_ADC_FLAG )
    {
        return false;
    }
    
    switch(convert_bit)
    {
        case ADC_CONVERT_7BIT:
            temp >>= 8;
            *data = temp & ~(1 << 8);
            break;
        case ADC_CONVERT_9BIT:
            temp >>= 6;
            *data = temp & ~(1 << 10);
            break;
        case ADC_CONVERT_10BIT:
            temp >>= 4;
            *data = temp & ~(1 << 11);
            break;
        case ADC_CONVERT_12BIT:
            temp >>= 2;
            *data = temp & ~(1 << 13);
            break;
        default:
            return false;
            break;
    }

    return true;
}
  
/**
 *******************************************************************************
 * @brief       ADC初始化函数
 * @param       [in/out]  channel    初始化通道
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_adc_init( uint8 channel )
{
    APCFG |= channel;
    P0SEL |= channel;
    P0DIR &= ~channel;
//    P0INP |= channel;
}

/**
 *******************************************************************************
 * @brief       ADC初始化函数
 * @param       [in/out]  channel    初始化通道
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_adc_deinit( uint8 channel )
{
    APCFG &= ~channel;
    P0SEL &= ~channel;
//    P0INP &= ~channel;
}

/**
 *******************************************************************************
 * @brief       ADC启动单次转换函数
 * @param       [in/out]  channel        转换通道
 * @param       [in/out]  convert_bit    转换数据打小
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_adc_signal_start( uint8 adc_ref, uint8 channel, uint8 convert_bit )
{
    ADCCON3 = adc_ref | channel | convert_bit;
    enable_adc_isr();
    MCU_ADC_START();
}

/**
 *******************************************************************************
 * @brief       ADC中断服务函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
HAL_ISR_FUNCTION(ADC_ISR_HANDLER, ADC_VECTOR)
{
    uint16 brightness;
    
    disable_adc_isr();
    clr_adc_isr_flag();
    Rd_ADC_Data(&brightness, ADC_CONVERT_7BIT);
}
  
/** @}*/     /* CC2530 ADC配置模块 */

/**********************************END OF FILE*********************************/
