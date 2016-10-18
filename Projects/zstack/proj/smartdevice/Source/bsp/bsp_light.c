 /**
 ******************************************************************************
  * @file       bsp_light.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      智能电灯配置源文件
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
 * @defgroup 智能电灯配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_light.h"
#include "timer_config.h"
#include <string.h>
#include "app_save.h"
#include "app_time.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的亮度与PWM占空比之间转换
#define Light_Brightness_Conversion(n) ( 0xFF - (n) )

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static DEVICE_LIGHT_SAVE_DATA device_light;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       电灯初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_light_init( void )
{
    Timer4_PWM_Init( TIM4_CH0_PORT_P2_0 );
    light_brightness_set(Light_OFF_Brightness);
    
    // FLASH 数据初始化
    if( osal_nv_read(DEVICE_LIGHT_SAVE_ID,0,DEVICE_LIGHT_DATA_SIZE,(void *)&device_light) != SUCCESS )
    {
        device_light.key.firstwritekey = 0;
        osal_nv_item_init(DEVICE_LIGHT_SAVE_ID,DEVICE_LIGHT_DATA_SIZE,NULL);
    }
    
    if( device_light.key.firstwritekey != DEVICE_FIRST_WRIYE_KEY )
    {
        memset(&device_light,0,sizeof(device_light));
        
        device_light.key.firstwritekey = DEVICE_FIRST_WRIYE_KEY;
        device_light.data.status = Light_OFF_Brightness;
    }
    else
    {
        light_brightness_set(device_light.data.status);
    }
}

/**
 *******************************************************************************
 * @brief       电灯亮度设置函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void light_brightness_set( uint8 brightness )
{
    TIM4_CH0_UpdateDuty( Light_Brightness_Conversion(brightness) );
}

/**
 *******************************************************************************
 * @brief       读取电灯亮度函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
uint8 light_brightness_get( void )
{
    return Light_Brightness_Conversion( Get_TIM4_CH0_Duty() );
}

/**
 *******************************************************************************
 * @brief       电灯工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void light_working_headler( void )
{
    
}

/** @}*/     /* 智能电灯配置模块 */

/**********************************END OF FILE*********************************/
