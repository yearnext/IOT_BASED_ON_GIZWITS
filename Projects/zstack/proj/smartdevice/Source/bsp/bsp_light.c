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
#include "myprotocol_packet.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的亮度与PWM占空比之间转换
#define Light_Brightness_Conversion(n) ( 0xFF - (n) )

/* Private typedef -----------------------------------------------------------*/
// 电灯控制命令
#define LIGHT_TICK_CMD         (0x00)
#define LIGHT_R_BRIGHTNESS_CMD (0x01)
#define LIGHT_W_BRIGHTNESS_CMD (0x02)
#define LIGHT_R_COUNTDOWN_CMD  (0x03)
#define LIGHT_W_COUNTDOWN_CMD  (0x04)
#define LIGHT_R_GENTIMER_CMD   (0x05)
#define LIGHT_W_GENTIMER_CMD   (0x06)

typedef struct
{
    uint8 brightness;
}LIGHT_BRIGHTNESS_CMD_DATA;

/* Private variables ---------------------------------------------------------*/
static DEVICE_LIGHT_SAVE_DATA light;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       上报电灯当前的亮度信息
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void report_brightness_data( void )
{
    MYPROTOCOL_USER_DATA user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA));
    
    user_data.cmd = LIGHT_R_BRIGHTNESS_CMD;
    user_data.data[0] = light.status.now;
    user_data.len = 1;
    MYPROTOCO_D2D_MSG_SEND(create_d2w_wait_packet,&user_data);
}

/**
 *******************************************************************************
 * @brief       上报电灯定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void report_timer_data( uint8 timer )
{
    MYPROTOCOL_USER_DATA user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA));
    
    if( timer == 0 )
    {
        user_data.cmd = LIGHT_R_COUNTDOWN_CMD;
    }
    else if( timer == 1 )
    {
        user_data.cmd = LIGHT_R_GENTIMER_CMD;
    }
    else
    {
        return;
    }
    
    memcpy(&user_data.data,&light.timer[timer],sizeof(DEVICE_TIMER));
    user_data.len = sizeof(DEVICE_TIMER);
    
    MYPROTOCO_D2D_MSG_SEND(create_d2w_wait_packet,&user_data);
}

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
    if( Device_FirstWrite_Check() == false \
        || osal_nv_read(DEVICE_LIGHT_SAVE_ID,0,DEVICE_LIGHT_DATA_SIZE,(void *)&light) != SUCCESS )
    {
        memset(&light,0,sizeof(light));
        light.status.now = Light_OFF_Brightness;
        light.status.last = Light_ON_Brightness;
        
        osal_nv_item_init(DEVICE_LIGHT_SAVE_ID,DEVICE_LIGHT_DATA_SIZE,NULL);
        osal_nv_write(DEVICE_LIGHT_SAVE_ID,0,DEVICE_LIGHT_DATA_SIZE,(void *)&light);
    }
    
    light_brightness_set(light.status.now);
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
    if( brightness != light_brightness_get() )
    {
        light.status.last = light.status.now;
        light.status.now = brightness;
        
        TIM4_CH0_UpdateDuty( Light_Brightness_Conversion(light.status.now) );
        
        osal_nv_write(DEVICE_LIGHT_SAVE_ID,\
                      (uint16)(&((DEVICE_LIGHT_SAVE_DATA *)0)->status),\
                      sizeof(light.status),\
                      (void *)&light.status);
    }
}

/**
 *******************************************************************************
 * @brief       电灯按键处理
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void light_switch_headler( void )
{
    uint8 temp = 0;
    
    if( light.status.now == Light_OFF_Brightness )
    {
        if( light.status.last != light.status.now )
        {
            temp = light.status.now;
            light.status.now = light.status.last;
            light.status.last = temp;
        }
        else
        {
            light.status.last = light.status.now;
            light.status.now = Light_ON_Brightness;
        }
    }
    else
    {
        light.status.last = light.status.now;
        light.status.now = Light_OFF_Brightness;
    }
    
    TIM4_CH0_UpdateDuty( Light_Brightness_Conversion(light.status.now) );
    
    osal_nv_write(DEVICE_LIGHT_SAVE_ID,\
              ((uint16)&light.status - (uint16)&light),\
              sizeof(light.status),\
              (void *)&light.status); 
    
    report_brightness_data();
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
    uint8 i;
    
    for( i=0; i<SIMPLE_DEVICE_TIMER_NUM; i++ )
    {
        device_timer_headler(&light.timer[i],&light_brightness_set);
    }
}

/**
 *******************************************************************************
 * @brief       解析电灯命令数据
 * @param       [in/out]  *data    控制命令数据
 * @return      [in/out]  bool     解析状态
 * @note        None
 *******************************************************************************
 */
bool light_cmd_resolve( MYPROTOCOL_USER_DATA *data )
{    
    switch( data->cmd )
    {
        case LIGHT_TICK_CMD:
            break;
        case LIGHT_R_BRIGHTNESS_CMD:
        {
            report_brightness_data();
            break;
        }
        case LIGHT_W_BRIGHTNESS_CMD:
        {
            light_brightness_set(data->data[0]);
            report_brightness_data();
            break;
        }
        case LIGHT_R_COUNTDOWN_CMD:
        {
            report_timer_data(0);
            break;
        }
        case LIGHT_W_COUNTDOWN_CMD:
        {
            memcpy(&light.timer[0],data->data,sizeof(light.timer[0]));
            report_timer_data(0);
            break;
        }
        case LIGHT_R_GENTIMER_CMD:
        {
            report_timer_data(1);
            break;
        }
        case LIGHT_W_GENTIMER_CMD:
        {
            memcpy(&light.timer[1],data->data,sizeof(light.timer[1]));
            report_timer_data(1);
            break;
        }
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 智能电灯配置模块 */

/**********************************END OF FILE*********************************/
