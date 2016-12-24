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
 * @defgroup 电灯配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_light.h"
#include "mcu_timer.h"
#include <string.h>
#include "app_save.h"
#include "app_time.h"
#include "app_timer.h"
#include "Onboard.h"
     
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的亮度与PWM占空比之间转换
#define Light_Brightness_Conversion(n) ( 0xFF - (n) )
//#define Light_Brightness_Conversion(n) ( n )
#define Get_Light_Brightness()         ( Get_TIM4_CH0_Duty() )

#define DEVICE_LIGHT_DATA_SIZE         (calSaveDataSize(light))

// 灯的最大亮度/最小亮度
#define LIGHT_MAX_BRIGHTNESS           (100)
#define LIGHT_MIN_BRIGHTNESS           (0)
// 灯的开启亮度/关闭亮度
#define LIGHT_ON_BRIGHTNESS            (100)  
#define LIGHT_OFF_BRIGHTNESS           (0)
     
// 配置定时器使用数量
#define LIGHT_USE_TIMER_NUM            (2)
    
// 加载掉电前的数据
#define LIGHT_NO_LOAD_SET              (0x00)
#define LIGHT_LOAD_SET                 (0x01)
     
// 写入离线数据
#define lightSaveData() deviceSaveData(DEVICE_CURTAIN_SAVE_ID, DEVICE_CURTAIN_DATA_SIZE, (void *)&curtain )

/* Private typedef -----------------------------------------------------------*/
// 电灯控制命令
typedef enum
{
    RD_LIGHT_BRIGHTNESS   = 0x10,
    WR_LIGHT_BRIGHTNESS   = 0x11,
    RD_LIGHT_SINGLE_TIMER = 0x12,
    WR_LIGHT_SINGLE_TIMER = 0x13,
    RD_LIGHT_CIRCUL_TIMER = 0x14,
    WR_LIGHT_CIRCUL_TIMER = 0x15,
    RD_LIGHT_LOAD_SET     = 0x16,
    WR_LIGHT_LOAD_SET     = 0x17,
}DEVICE_LIGHT_CMD;

/* Private variables ---------------------------------------------------------*/
// 电灯存储数据
static struct _DEVICE_LIGHT_SAVE_DATA_
{
    // 状态数据
    struct
    {
        uint8 now;
        uint8 last;
    }status;
    
    // 定时器数据
    DEVICE_TIMER timer[LIGHT_USE_TIMER_NUM]; 
    
    // 初始化载入数据
    uint8 load_set;
}light;

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
static void reportLightBrightnessData( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_LIGHT_BRIGHTNESS;
    user_data.data[0] = light.status.now;
    user_data.len = 1;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报电灯定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportLightTimerData( uint8 timer )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    if( timer == 0 )
    {
        user_data.cmd = RD_LIGHT_SINGLE_TIMER;
    }
    else if( timer == 1 )
    {
        user_data.cmd = RD_LIGHT_CIRCUL_TIMER;
    }
    else
    {
        return;
    }
    
    memcpy(&user_data.data,&light.timer[timer],sizeof(DEVICE_TIMER));
    user_data.len = sizeof(DEVICE_TIMER);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报电灯上电加载数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportLightLoadSetData( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_LIGHT_LOAD_SET;
    memcpy(&user_data.data,&light.load_set,sizeof(light.load_set));
    user_data.len = sizeof(light.load_set);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       存储定时器数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool saveLightTimerData( uint8 timer, uint8 *data )
{
    if( timer >= LIGHT_USE_TIMER_NUM )
    {
        return false;
    }
    
    memcpy(&light.timer[timer],data,sizeof(light.timer[timer]));
    
    lightSaveData(); 

    return true;
}
                  
/**
 *******************************************************************************
 * @brief       存储上电加载参数数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool saveLightLoadSetData( uint8 data )
{
    light.load_set = data;    
    lightSaveData();
    
    return true;
}

/**
 *******************************************************************************
 * @brief       初始化电灯设置数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void rstLightData( void )
{
    memset(&light,0,sizeof(light));
    light.status.now = LIGHT_OFF_BRIGHTNESS;
    light.status.last = LIGHT_ON_BRIGHTNESS;
    
    osal_nv_item_init(DEVICE_LIGHT_SAVE_ID,DEVICE_LIGHT_DATA_SIZE,NULL);
    lightSaveData();
}

/**
 *******************************************************************************
 * @brief       电灯亮度设置函数
 * @param       [in/out]  uint8    电灯的亮度
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void setLightBrightness( uint8 brightness )
{
    uint16 brightness_data = brightness;
    brightness_data <<= 8;
    
    if( brightness_data )
    {
        brightness_data -= 100;    
    }
    
    brightness_data /= 100;
    
    TIM4_CH0_UpdateDuty( Light_Brightness_Conversion(brightness_data) );
}

/**
 *******************************************************************************
 * @brief       读取电灯亮度函数
 * @param       [in/out]  void
 * @return      [in/out]  uint8    电灯的亮度
 * @note        None
 *******************************************************************************
 */
uint8 getLightBrightness( void )
{
    uint16 brightness_data = Light_Brightness_Conversion(Get_Light_Brightness());
    brightness_data *= 100;
    
    if( brightness_data )
    {
        brightness_data += 100;
    }
    
    brightness_data >>= 8;

    return (uint8)(brightness_data);
}

/**
 *******************************************************************************
 * @brief       电灯初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bspLightInit( void )
{
    Timer4_PWM_Init( TIM4_CH0_PORT_P2_0 );
    setLightBrightness(LIGHT_OFF_BRIGHTNESS);
    
    // FLASH 数据初始化
    deviceLoadData(DEVICE_LIGHT_SAVE_ID,DEVICE_LIGHT_DATA_SIZE,(void *)&light,rstLightData);
    
    // 载入掉点前的状态
    if( light.load_set == LIGHT_LOAD_SET )
    {
        setLightBrightness(light.status.now);
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
void lightControlHandler( uint8 brightness )
{
    if( brightness != getLightBrightness() )
    {
        light.status.last = light.status.now;
        light.status.now = brightness;
        
        setLightBrightness( light.status.now );
        
        lightSaveData();
    }
}

/**
 *******************************************************************************
 * @brief       电灯亮度设置函数(定时器专用)
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void lightTimerControlHandler( uint8 brightness )
{
    lightControlHandler(brightness);
    reportLightBrightnessData();
}

/**
 *******************************************************************************
 * @brief       电灯按键处理
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void lightSwitchHandler( void )
{
    uint8 temp = 0;

    if( light.status.now == LIGHT_OFF_BRIGHTNESS )
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
            light.status.now = LIGHT_ON_BRIGHTNESS;
        }
    }
    else
    {
        light.status.last = light.status.now;
        light.status.now = LIGHT_OFF_BRIGHTNESS;
    }
    
    setLightBrightness( light.status.now );
    
    lightSaveData();
}

/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void lightSwitchKeyHandler( key_message_t message )
{
    static uint8 lastStatus = LIGHT_MIN_BRIGHTNESS;
    static bool keyStatus = 0;
    
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
            keyStatus = 0;
			break;
        case KEY_MESSAGE_LONG_PRESS_EDGE:
            lastStatus = light.status.now;
            
            if( light.status.last >= light.status.now )
            {
                keyStatus = 1;
                
                if( light.status.now < LIGHT_MAX_BRIGHTNESS  )
                {
                    setLightBrightness( light.status.now++ );
                }
            }
            else
            {
                keyStatus = 2;
                
                if( light.status.now > LIGHT_MIN_BRIGHTNESS  )
                {
                    setLightBrightness( light.status.now-- );
                }
            }
            break;
        case KEY_MESSAGE_MULTI_PRESS:
            if( keyStatus == 1 )
            {
                if( light.status.now < LIGHT_MAX_BRIGHTNESS  )
                {
                    setLightBrightness( light.status.now++ );
                }
            }
            else if( keyStatus == 2 )
            {
                if( light.status.now > LIGHT_MIN_BRIGHTNESS  )
                {
                    setLightBrightness( light.status.now-- );
                }
            }
            else
            {
                // do nothing!
            }
            break;
        case KEY_MESSAGE_RELEASE_EDGE: 
            if( keyStatus == 0 )
            {
                lightSwitchHandler();
            }
            else
            {
                light.status.last = lastStatus;
            }
            reportLightBrightnessData();
            break;
		default:
			break;
    }
}

/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void lightRstKeyHandler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_LONG_PRESS_EDGE:
            rstLightData();
            Onboard_soft_reset();
			break;
		default:
			break;
    }
}

/**
 *******************************************************************************
 * @brief       电灯工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void lightWorkingHandler( void )
{
    uint8 i;
    
    for( i=0; i<LIGHT_USE_TIMER_NUM; i++ )
    {
        deviceTimerHandler((DEVICE_TIMER*)&light.timer[i],lightTimerControlHandler);
    }
//    static uint8 i = 0;
//    static bool status = false;
//    
//    if( status == false )
//    {
//        if( ++i >= 100 )
//        {
//            status = true;
//        }
//    }
//    else
//    {
//        if( --i <= 0 )
//        {
//            status = false;
//        }
//    }
//    
//    setLightBrightness(i);
}

/**
 *******************************************************************************
 * @brief       解析电灯命令数据
 * @param       [in/out]  *data    控制命令数据
 * @return      [in/out]  bool     解析状态
 * @note        None
 *******************************************************************************
 */
bool lightMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{    
    switch( recPacket->user_data.cmd )
    {
        case MYPROTOCOL_TICK_CMD:
            break;
        case MYPROTOCOL_RESET_CMD:
            rstLightData();
        case MYPROTOCOL_REBOOT_CMD:
            Onboard_soft_reset();
            break;
        case MYPROTOCOL_RD_TIME_CMD:
//            if( recPacket->user_data.data[8] == 1 )
//            {
//                app_time_update((user_time *)&recPacket->user_data.data);
//            }
            app_time_update((user_time *)&recPacket->user_data.data);
            break;
        case RD_LIGHT_BRIGHTNESS:
            reportLightBrightnessData();
            break;
        case WR_LIGHT_BRIGHTNESS:
            lightControlHandler(recPacket->user_data.data[0]);
            reportLightBrightnessData();
            break;
        case RD_LIGHT_SINGLE_TIMER:
            reportLightTimerData(0);
            break;
        case WR_LIGHT_SINGLE_TIMER:
            saveLightTimerData(0,recPacket->user_data.data);
            reportLightTimerData(0);
            break;
        case RD_LIGHT_CIRCUL_TIMER:
            reportLightTimerData(1);
            break;
        case WR_LIGHT_CIRCUL_TIMER:
            saveLightTimerData(1,recPacket->user_data.data);
            reportLightTimerData(1);
            break;
        case RD_LIGHT_LOAD_SET:
            reportLightLoadSetData();
            break;
        case WR_LIGHT_LOAD_SET:
            saveLightLoadSetData(recPacket->user_data.data[0]);
            reportLightLoadSetData();
            break;
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 电灯配置模块 */

/**********************************END OF FILE*********************************/
