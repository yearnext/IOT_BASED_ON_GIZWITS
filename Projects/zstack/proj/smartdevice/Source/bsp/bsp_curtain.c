/**
 ******************************************************************************
  * @file       bsp_curtain.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      窗帘配置 源头文件
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
 * @defgroup 窗帘配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_curtain.h"
#include <string.h>
#include "app_save.h"
#include "app_timer.h"
#include "hal_board_cfg.h"
#include "hal_adc.h"
#include "Onboard.h"
#include "hal_ds1302.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @defgroup 电动窗帘硬件抽象层定义
 * @{
 */
// 窗帘正转端口寄存器定义
#define CURTAIN_FORWARD_PORT     P1_2
#define CURTAIN_FORWARD_BV       BV(2)
#define CURTAIN_FORWARD_SEL      P1SEL
#define CURTAIN_FORWARD_DIR      P1DIR
#define CURTAIN_FORWARD_POLARITY ACTIVE_HIGH

// 窗帘反转端口寄存器定义
#define CURTAIN_REVERSE_PORT     P1_3
#define CURTAIN_REVERSE_BV       BV(3)
#define CURTAIN_REVERSE_SEL      P1SEL
#define CURTAIN_REVERSE_DIR      P1DIR
#define CURTAIN_REVERSE_POLARITY ACTIVE_HIGH

// 转速输入端口1
#define CURTAIN_SPD_CH1_PORT     P0_4
#define CURTAIN_SPD_CH1_BV       BV(4)
#define CURTAIN_SPD_CH1_SEL      P0SEL
#define CURTAIN_SPD_CH1_DIR      P0DIR
#define CURTAIN_SPD_CH1_POLARITY ACTIVE_LOW

// 转速输入端口2
#define CURTAIN_SPD_CH2_PORT     P0_5
#define CURTAIN_SPD_CH2_BV       BV(5)
#define CURTAIN_SPD_CH2_SEL      P0SEL
#define CURTAIN_SPD_CH2_DIR      P0DIR
#define CURTAIN_SPD_CH2_POLARITY ACTIVE_LOW

/** 雨滴检测端口 */
#define CURTAIN_RAIN_PORT        P0_6
#define CURTAIN_RAIN_BIT         BV(6)
#define CURTAIN_RAIN_SEL         P0SEL
#define CURTAIN_RAIN_DIR         P0DIR
#define CURTAIN_RAIN_POLARITY    ACTIVE_LOW

// 光照强度检测端口
#define CURTAIN_BRIGHTNESS_ADC_CH HAL_ADC_CHANNEL_7 

// 窗帘正转端口功能配置
#define CURTAIN_FORWARD_WrMode() ( CURTAIN_FORWARD_DIR |=  CURTAIN_FORWARD_BV )
#define CURTAIN_FORWARD_RdMode() ( CURTAIN_FORWARD_DIR &= ~CURTAIN_FORWARD_BV )
#define SET_CURTAIN_FORWARD()    ( CURTAIN_FORWARD_PORT = CURTAIN_FORWARD_POLARITY(1) )
#define CLR_CURTAIN_FORWARD()    ( CURTAIN_FORWARD_PORT = CURTAIN_FORWARD_POLARITY(0) )
#define RD_CURTAIN_FORWARD()     ( CURTAIN_FORWARD_POLARITY(CURTAIN_FORWARD_PORT) )

// 窗帘反转端口功能配置
#define CURTAIN_REVERSE_WrMode() ( CURTAIN_REVERSE_DIR |=  CURTAIN_REVERSE_BV )
#define CURTAIN_REVERSE_RdMode() ( CURTAIN_REVERSE_DIR &= ~CURTAIN_REVERSE_BV )
#define SET_CURTAIN_REVERSE()    ( CURTAIN_REVERSE_PORT = CURTAIN_REVERSE_POLARITY(1) )
#define CLR_CURTAIN_REVERSE()    ( CURTAIN_REVERSE_PORT = CURTAIN_REVERSE_POLARITY(0) )
#define RD_CURTAIN_REVERSE()     ( CURTAIN_REVERSE_POLARITY(CURTAIN_REVERSE_PORT) )

// 转速输入端口1
#define CURTAIN_SPD_CH1_WrMode() ( CURTAIN_SPD_CH1_DIR |=  CURTAIN_SPD_CH1_BV )
#define CURTAIN_SPD_CH1_RdMode() ( CURTAIN_SPD_CH1_DIR &= ~CURTAIN_SPD_CH1_BV )
#define SET_CURTAIN_SPD_CH1()    ( CURTAIN_SPD_CH1_PORT = CURTAIN_SPD_CH1_POLARITY(1) )
#define CLR_CURTAIN_SPD_CH1()    ( CURTAIN_SPD_CH1_PORT = CURTAIN_SPD_CH1_POLARITY(0) )
#define RD_CURTAIN_SPD_CH1()     ( CURTAIN_SPD_CH1_POLARITY(CURTAIN_SPD_CH1_PORT) )

// 转速输入端口2
#define CURTAIN_SPD_CH2_WrMode() ( CURTAIN_SPD_CH2_DIR |=  CURTAIN_SPD_CH2_BV )
#define CURTAIN_SPD_CH2_RdMode() ( CURTAIN_SPD_CH2_DIR &= ~CURTAIN_SPD_CH2_BV )
#define SET_CURTAIN_SPD_CH2()    ( CURTAIN_SPD_CH2_PORT = CURTAIN_SPD_CH2_POLARITY(1) )
#define CLR_CURTAIN_SPD_CH2()    ( CURTAIN_SPD_CH2_PORT = CURTAIN_SPD_CH2_POLARITY(0) )
#define RD_CURTAIN_SPD_CH2()     ( CURTAIN_SPD_CH2_POLARITY(CURTAIN_SPD_CH2_PORT) )

// 雨滴检测端口
#define CURTAIN_RAIN_WrMode() ( CURTAIN_RAIN_DIR |=  CURTAIN_RAIN_BV )
#define CURTAIN_RAIN_RdMode() ( CURTAIN_RAIN_DIR &= ~CURTAIN_RAIN_BV )
#define SET_CURTAIN_RAIN()    ( CURTAIN_RAIN_PORT = CURTAIN_RAIN_POLARITY(1) )
#define CLR_CURTAIN_RAIN()    ( CURTAIN_RAIN_PORT = CURTAIN_RAIN_POLARITY(0) )
#define RD_CURTAIN_RAIN()     ( CURTAIN_RAIN_POLARITY(CURTAIN_RAIN_PORT) )

// 亮度检测端口
#define curtainBrightnessInit()  ( P0SEL |= 0x80, P0DIR &= ~0x80 )

/** @}*/     /* 电动窗帘硬件抽象层定义 */

// 电动窗帘使用定时器数量 
#define CURTAIN_USE_TIMER_NUM    (2)

// 电动窗帘占用FLASH大小
#define DEVICE_CURTAIN_DATA_SIZE (calSaveDataSize(curtain))

// 电动窗帘工作状态
#define CURTAIN_INIT_STATE        (0x00)
#define CURTAIN_STOP_STATE        (0x00)
#define CURTAIN_OPEN_STATE        (0x01)
#define CURTAIN_OPEN_END_STATE    (0x02)
#define CURTAIN_OPEN_ALL_STATE    (0x03)
#define CURTAIN_CLOSE_STATE       (0x04)
#define CURTAIN_CLOSE_END_STATE   (0x05)
#define CURTAIN_CLOSE_ALL_STATE   (0x06)

// 电动窗帘控制命令
#define CURTAIN_STOP_CMD          (0x00)
#define CURTAIN_OPEN_CMD          (0x01)
#define CURTAIN_CLOSE_CMD         (0x02)

// 电动窗帘功能使能宏
#define CURTAIN_DISABLE_FUNC      (0x00)
#define CURTAIN_ENABLE_FUNC       (0x01)

// 电机工作函数
//#define OPEN_CURTAIN_OPERA()      {CLR_CURTAIN_REVERSE();Onboard_wait(10);SET_CURTAIN_FORWARD();Onboard_wait(10);}
//#define CLOSE_CURTAIN_OPERA()     {CLR_CURTAIN_FORWARD();Onboard_wait(10);SET_CURTAIN_REVERSE();Onboard_wait(10);}
//#define INIT_CURTAIN_OPERA()      {CLR_CURTAIN_REVERSE();Onboard_wait(10);CLR_CURTAIN_FORWARD();Onboard_wait(10);}
     
static void OPEN_CURTAIN_OPERA( void )      
{
    CLR_CURTAIN_REVERSE();
    Onboard_wait(10);
    SET_CURTAIN_FORWARD();
    Onboard_wait(10);
}

static void CLOSE_CURTAIN_OPERA( void )
{
    CLR_CURTAIN_FORWARD();
    Onboard_wait(10);
    SET_CURTAIN_REVERSE();
    Onboard_wait(10);
}

static void INIT_CURTAIN_OPERA( void )      
{
    CLR_CURTAIN_REVERSE();
    Onboard_wait(10);
    CLR_CURTAIN_FORWARD();
    Onboard_wait(10);
}

// 电动窗帘定时器标号
#define CURTAIN_SIGNAL_TIMER      (0x01)
#define CURTAIN_CIRCUL_TIMER      (0x02)

// 电机限位标志
#define CURTAIN_LIMIT_NONE        (0x00)
#define CURTAIN_LIMIT_LEFT        (0x01)
#define CURTAIN_LIMIT_RIGHT       (0x02)

// 写入离线数据
#define curtainSaveData() deviceSaveData(DEVICE_CURTAIN_SAVE_ID, DEVICE_CURTAIN_DATA_SIZE, (void *)&curtain )

/* Private typedef -----------------------------------------------------------*/
// 窗帘控制命令
typedef enum
{
    RD_CURTAIN_STATE        = 0x10,
    WR_CURTAIN_OPERA        = 0x11,
    RD_CURTAIN_SINGLE_TIMER = 0x12,
    WR_CURTAIN_SINGLE_TIMER = 0x13,
    RD_CURTAIN_CIRCUL_TIMER = 0x14,
    WR_CURTAIN_CIRCUL_TIMER = 0x15,
    RD_CURTAIN_SMART_MODE   = 0x16,
    WR_CURTAIN_SMART_MODE   = 0x17,
    RD_CURTAIN_RAIN_WARNING = 0x18,
    RD_CURTAIN_BRIGHTNESS   = 0x19,
    RD_CURTAIN_LOAD_MODE    = 0x1A,
    WR_CURTAIN_LOAD_MODE    = 0x1B,
}DEVICE_CURTAIN_CMD;

/* Private variables ---------------------------------------------------------*/
// 电动窗帘存储数据
static struct _DEVICE_CURTAIN_SAVE_DATA_
{
    // 状态数据
    uint8 status;
    
    struct
    {
        uint8 status;
        uint8 last_status;
    }metor;
    
    // 定时器数据
    struct
    {
        DEVICE_TIMER signal;
        DEVICE_TIMER circul;
    }timer;
 
    // 窗帘智能工作模式
    struct
    {
        uint8 state;
        uint8 brightness;    
    }mode;
    
    // 下雨报警标志
    struct
    {
        uint8 status;
        uint8 last_status;
    }rain;
    
    // 室内亮度
    uint8 brightness;
    
    // 上电设置
    uint8 load_set;
    
    // 窗帘限位标志
    uint8 limit;
}curtain;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       上报窗帘当前状态
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainStatus( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_CURTAIN_STATE;
    user_data.data[0] = curtain.status;
    user_data.len = 1;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报定时器数据
 * @param       [in/out]  timer   定时器标号
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainTimerData( uint8 timer )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    if( timer == CURTAIN_SIGNAL_TIMER )
    {
        user_data.cmd = RD_CURTAIN_SINGLE_TIMER;
        memcpy(&user_data.data,&curtain.timer.signal,sizeof(DEVICE_TIMER));
    }
    else if( timer == CURTAIN_CIRCUL_TIMER )
    {
        user_data.cmd = RD_CURTAIN_CIRCUL_TIMER;
        memcpy(&user_data.data,&curtain.timer.circul,sizeof(DEVICE_TIMER));
    }
    else
    {
        return;
    }
    
    user_data.len = sizeof(DEVICE_TIMER);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报运行模式的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainSmartModeData( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_CURTAIN_SMART_MODE;  
    memcpy(&user_data.data,&curtain.mode,sizeof(curtain.mode));
    user_data.len = sizeof(curtain.mode);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       下雨报警
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainRainStatus( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_CURTAIN_RAIN_WARNING;  
    user_data.data[0] = curtain.rain.status;
    user_data.len = 1;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报当前环境亮度
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainBrightness( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_CURTAIN_BRIGHTNESS;  
    user_data.data[0] = curtain.brightness;
    user_data.len = 1;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报窗帘上电加载数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportCurtainLoadSetData( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_CURTAIN_LOAD_MODE;
    memcpy(&user_data.data,&curtain.load_set,sizeof(curtain.load_set));
    user_data.len = sizeof(curtain.load_set);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       存储定时器数据
 * @param       [in/out]  timer   定时器标号
 * @param       [in/out]  data    保存的数据
 * @return      [in/out]  bool    程序运行状态
 * @note        None
 *******************************************************************************
 */
static bool saveCurtainTimerData( uint8 timer, uint8 *data )
{
    if( timer >= CURTAIN_USE_TIMER_NUM )
    {
        return false;
    }
    
    if( timer == CURTAIN_SIGNAL_TIMER )
    {
        memcpy(&curtain.timer.signal,data,sizeof(curtain.timer.signal));
        curtainSaveData();
        
        return true;
    }
    else if ( timer == CURTAIN_CIRCUL_TIMER )
    {
        memcpy(&curtain.timer.circul,data,sizeof(curtain.timer.circul));
        curtainSaveData(); 
        
        return true;
    }
    else
    {
        return false;
    }

//    return true;
}

/**
 *******************************************************************************
 * @brief       存储定时器数据
 * @param       [in/out]  uint8*    数据
 * @return      [in/out]  bool      程序运行结果
 * @note        None
 *******************************************************************************
 */
static bool saveCurtainSmartModeData( uint8 *data )
{
    memcpy(&curtain.mode,data,sizeof(curtain.mode));
    curtainSaveData(); 

    return true;
}

/**
 *******************************************************************************
 * @brief       存储上电加载参数数据
 * @param       [in/out]  uint8     数据
 * @return      [in/out]  bool      程序运行结果
 * @note        None
 *******************************************************************************
 */
static bool saveCurtainLoadSetData( uint8 data )
{
    curtain.load_set = data;
    curtainSaveData();
    
    return true;
}

/**
 *******************************************************************************
 * @brief       电动窗帘测速端口初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        测速端口状态： CH1、CH2为低电平
 * @note                       CH1、CH2为正弦脉冲                    
 *******************************************************************************
 */
static void CurtainSpeedInit( void )
{
	CURTAIN_SPD_CH1_RdMode();
	CURTAIN_SPD_CH2_RdMode();	
}

/**
 *******************************************************************************
 * @brief       电动窗帘电机控制端口初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        电机正反转控制                  
 *******************************************************************************
 */
static void CurtainMetorInit( void )
{
    // GPIO 初始化
	CURTAIN_FORWARD_WrMode();
    CURTAIN_REVERSE_WrMode();
    
    // 关闭窗帘电机
    INIT_CURTAIN_OPERA();
}

/**
 *******************************************************************************
 * @brief       初始化窗帘设置数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void rstCurtainData( void )
{
    memset(&curtain,0,sizeof(curtain));
    
    osal_nv_item_init(DEVICE_CURTAIN_SAVE_ID,DEVICE_CURTAIN_DATA_SIZE,NULL);
    curtainSaveData();
}

/**
 *******************************************************************************
 * @brief       初始化窗帘
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bspCurtainInit(void)
{
    // 电动窗帘驱动模块初始化
    CurtainMetorInit();
	
    // 电动窗帘测速模块初始化
    CurtainSpeedInit();
    
    // 亮度检测端口初始化
    curtainBrightnessInit();
	
    // FLASH 数据初始化
    deviceLoadData(DEVICE_CURTAIN_SAVE_ID,DEVICE_CURTAIN_DATA_SIZE,(void *)&curtain,rstCurtainData);
    
    memset(&curtain, 0, sizeof(curtain));
    
    curtainControlHandler(curtain.metor.status);
}
 
/**
 *******************************************************************************
 * @brief       电动窗帘控制命令
 * @param       [in/out]  cmd     控制命令
 * @return      [in/out]  bool    执行状态
 * @note        None
 *******************************************************************************
 */
void curtainControlHandler( uint8 cmd )
{
    if( cmd == CURTAIN_OPEN_CMD )
    {
        if( curtain.status != CURTAIN_OPEN_ALL_STATE && curtain.limit != CURTAIN_LIMIT_LEFT )
        {
            INIT_CURTAIN_OPERA();
            OPEN_CURTAIN_OPERA();

            curtain.status = CURTAIN_OPEN_STATE;
            curtain.metor.last_status = curtain.metor.status;
            curtain.metor.status = CURTAIN_OPEN_STATE;
        }
    }
    else if( cmd == CURTAIN_CLOSE_CMD )
    {
        if( curtain.status != CURTAIN_CLOSE_ALL_STATE && curtain.limit != CURTAIN_LIMIT_RIGHT )
        {
            INIT_CURTAIN_OPERA();
            CLOSE_CURTAIN_OPERA();
            
            curtain.status = CURTAIN_CLOSE_STATE;
            curtain.metor.last_status = curtain.metor.status;
            curtain.metor.status = CURTAIN_CLOSE_STATE;
        }
    }
    else
    {
        INIT_CURTAIN_OPERA();
        
        if( curtain.limit == CURTAIN_LIMIT_LEFT )
        {
            curtain.status = CURTAIN_OPEN_ALL_STATE;
        }
        else if( curtain.limit == CURTAIN_LIMIT_RIGHT )
        {
            curtain.status = CURTAIN_CLOSE_ALL_STATE;
        }
        else
        {
            if( curtain.status == CURTAIN_OPEN_STATE )
            {
                curtain.status = CURTAIN_OPEN_END_STATE;
            }
            else if( curtain.status == CURTAIN_CLOSE_STATE )
            {
                curtain.status = CURTAIN_CLOSE_END_STATE;
            }
            else
            {
                // do nothing!
            }
        }
        curtain.metor.last_status = curtain.metor.status;
        curtain.metor.status = CURTAIN_STOP_STATE;
    }
}

/**
 *******************************************************************************
 * @brief       刷新电动窗帘当前状态
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None                  
 *******************************************************************************
 */
void curtainMetorFresh( void )
{
    curtainControlHandler(curtain.metor.status);
}

/**
 *******************************************************************************
 * @brief       电动窗帘状态检测
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None                  
 *******************************************************************************
 */
void curtainSpeedDetection( void )
{
#define CURTAIN_SPEED_COUNT (15)
    
    static uint8 s1Count = 0;
//    static uint8 s2Count = 0;
    static uint8 s1Handler = 0;
//    static uint8 s2Handler = 0;
    static uint8 s1Value = KEY_VALUE_NOP;
//    static uint8 s2Value = KEY_VALUE_NOP;
    uint8 s1NowValue = RD_CURTAIN_SPD_CH1();
//    uint8 s2NowValue = RD_CURTAIN_SPD_CH2();
    
    if( s1Value != s1NowValue )
    {
        s1Value = s1NowValue;
        s1Count = 0;
        s1Handler = 0;
        curtain.limit = CURTAIN_LIMIT_NONE;
    }
    else
    {
        if( !s1Handler )
        {
            if( s1Count >= CURTAIN_SPEED_COUNT )
            {
                if( curtain.status == CURTAIN_OPEN_STATE )
                {
                    curtain.limit = CURTAIN_LIMIT_LEFT;
                    curtainControlHandler(CURTAIN_STOP_CMD);
                }
                else if( curtain.status == CURTAIN_CLOSE_STATE )
                {
                    curtain.limit = CURTAIN_LIMIT_RIGHT;
                    curtainControlHandler(CURTAIN_STOP_CMD);
                }
                
                reportCurtainStatus();
                s1Handler = 1;
                
                return;
            }
            else
            {
                s1Count++;
            }
        }
    }
    
//    if( s2Value != s2NowValue )
//    {
//        s2Value = s2NowValue;
//        s2Count = 0;
//        s2Handler = 0;
//    }
//    else
//    {
//        if( !s2Handler )
//        {
//            if( s2Count >= CURTAIN_SPEED_COUNT )
//            {
//                curtainControlHandler(CURTAIN_STOP_CMD);
//                reportCurtainStatus();
//                s2Handler = 1;
//                
//                return;
//            }
//            else
//            {
//                s2Count++;
//            }
//        }
//    }
}

/**
 *******************************************************************************
 * @brief       电动窗帘下雨状态检测
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None                  
 *******************************************************************************
 */
void curtainRainDetection( void )
{
#define RAIN_SPEED_COUNT (10)
    
    static uint16 rainCount = 0;
    static uint8 rainHandler = 0;
    static uint8 rainValue = KEY_VALUE_NOP;
    uint8 rainNowValue = RD_CURTAIN_RAIN();
    
    if( rainValue != rainNowValue )
    {
        rainValue = rainNowValue;
        rainCount = 0;
        rainHandler = 0;
    }
    else
    {
        if( !rainHandler )
        {
            if( rainCount >= RAIN_SPEED_COUNT )
            {
                if( rainValue )
                {
                    curtain.rain.status = 0;
                    reportCurtainRainStatus();
                    
#if USE_MYPROTOCOL_DEBUG
                MYPROTOCOL_LOG("curtain not check raining! \r\n");
#endif 
                }
                else
                {                             
#if USE_MYPROTOCOL_DEBUG
                MYPROTOCOL_LOG("curtain check raining! \r\n");
#endif 
                    curtain.rain.status = 1;
                    reportCurtainRainStatus();
            
                    curtainControlHandler(CURTAIN_CLOSE_CMD);
                    reportCurtainStatus();
                }
                
                rainHandler = 1;
                
                return;
            }
            else
            {
                rainCount++;
            }
        }
    }
}

/**
 *******************************************************************************
 * @brief       电动窗帘状态检测
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None                  
 *******************************************************************************
 */
void curtainBrightnessHandler( void )
{
   curtain.brightness = (uint8)HalAdcRead(HAL_ADC_CHANNEL_7,HAL_ADC_RESOLUTION_8);
   curtain.brightness = (uint8)((uint16)curtain.brightness * 100 / 255);

   if( curtain.mode.state )
   {
       if( curtain.brightness > (curtain.mode.brightness + 20) )
       {
           curtainControlHandler(CURTAIN_CLOSE_CMD);
       }
       else if( curtain.brightness < (curtain.mode.brightness + 10) )
       {
           curtainControlHandler(CURTAIN_OPEN_CMD);
       }
       else
       {
           return;
       }
   }
}

/**
 *******************************************************************************
 * @brief       电动窗帘工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void curtainWorkingHandler( void )
{
    deviceTimerHandler((DEVICE_TIMER*)& curtain.timer.signal,curtainControlHandler);
    deviceTimerHandler((DEVICE_TIMER*)& curtain.timer.circul,curtainControlHandler);
}

/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void curtainSwitchKeyHandler( key_message_t message )
{
    switch (message)
    {
        case KEY_MESSAGE_PRESS_EDGE:
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("KEY_MESSAGE_PRESS_EDGE handler! \r\n");
#endif
            switch( curtain.status )
            {
                case CURTAIN_STOP_STATE:
                curtainControlHandler(CURTAIN_OPEN_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is open! \r\n");
#endif
                    break;
                case CURTAIN_OPEN_STATE:
                    curtainControlHandler(CURTAIN_STOP_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is stop，old state is open! \r\n");
#endif
                    break;
                case CURTAIN_OPEN_END_STATE:
                    curtainControlHandler(CURTAIN_CLOSE_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is close! \r\n");
#endif
                    break;
                case CURTAIN_OPEN_ALL_STATE:
                    curtainControlHandler(CURTAIN_CLOSE_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is close! \r\n");
#endif
                    break;
                case CURTAIN_CLOSE_STATE:
                    curtainControlHandler(CURTAIN_STOP_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is stop，old state is close! \r\n");
#endif
                    break;
                case CURTAIN_CLOSE_END_STATE:
                    curtainControlHandler(CURTAIN_OPEN_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is open! \r\n");
#endif
                    break;
                case CURTAIN_CLOSE_ALL_STATE:
                    curtainControlHandler(CURTAIN_OPEN_CMD);
#if USE_MYPROTOCOL_DEBUG
                    MYPROTOCOL_LOG("curtain is open! \r\n");
#endif
                    break;
                default:
                    return;
                    break; 
            }
            curtainSaveData();
            reportCurtainStatus();  
			break;
        case KEY_MESSAGE_LONG_PRESS:
//            curtainMetorFresh();
//            rstCurtainData();
//            Onboard_soft_reset();
            break;
		default:
			break;
    }
}

/**
 *******************************************************************************
 * @brief       解析命令数据
 * @param       [in/out]  *data    控制命令数据
 * @return      [in/out]  bool     解析状态
 * @note        None
 *******************************************************************************
 */
bool curtainMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{    
    switch( recPacket->user_data.cmd )
    {
        case MYPROTOCOL_TICK_CMD:
            break;
        case MYPROTOCOL_RESET_CMD:
            rstCurtainData();
        case MYPROTOCOL_REBOOT_CMD:
            Onboard_soft_reset();
            break;
        case MYPROTOCOL_RD_TIME_CMD:
//            if( recPacket->user_data.data[sizeof(user_time)] == 1 )
//            {
//#if USE_MYPROTOCOL_DEBUG
//                MYPROTOCOL_LOG("curtain get net time is not invaild! \r\n");
//#endif 
//                app_time_update((user_time *)&recPacket->user_data.data);
//            }
            app_time_update((user_time *)&recPacket->user_data.data);
            break;
        case RD_CURTAIN_STATE:
            reportCurtainStatus();
            break;
        case WR_CURTAIN_OPERA:
            curtainControlHandler(recPacket->user_data.data[0]);
            reportCurtainStatus();
            break;
        case RD_CURTAIN_SINGLE_TIMER:
            reportCurtainTimerData(CURTAIN_SIGNAL_TIMER);
            break;
        case WR_CURTAIN_SINGLE_TIMER:
            saveCurtainTimerData(CURTAIN_SIGNAL_TIMER,recPacket->user_data.data);
            reportCurtainTimerData(CURTAIN_SIGNAL_TIMER);
            break;
        case RD_CURTAIN_CIRCUL_TIMER:
            reportCurtainTimerData(CURTAIN_CIRCUL_TIMER);
            break;
        case WR_CURTAIN_CIRCUL_TIMER:
            saveCurtainTimerData(CURTAIN_CIRCUL_TIMER,recPacket->user_data.data);
            reportCurtainTimerData(CURTAIN_CIRCUL_TIMER);
            break;
        case RD_CURTAIN_SMART_MODE:
            reportCurtainSmartModeData();
            break;
        case WR_CURTAIN_SMART_MODE:
            saveCurtainSmartModeData(recPacket->user_data.data);
            reportCurtainSmartModeData();
            break;
        case RD_CURTAIN_RAIN_WARNING:
            reportCurtainRainStatus();
            break;
        case RD_CURTAIN_BRIGHTNESS:
            reportCurtainBrightness();
            break;
        case RD_CURTAIN_LOAD_MODE:
            reportCurtainLoadSetData();
            break;
        case WR_CURTAIN_LOAD_MODE:
            saveCurtainLoadSetData(recPacket->user_data.data[0]);
            reportCurtainLoadSetData();
            break;
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 窗帘模块 */

/**********************************END OF FILE*********************************/
