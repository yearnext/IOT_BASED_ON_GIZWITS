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
#include "mcu_adc.h"

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
#define CURTAIN_FORWARD_POLARITY ACTIVE_LOW

// 窗帘反转端口寄存器定义
#define CURTAIN_REVERSE_PORT     P1_3
#define CURTAIN_REVERSE_BV       BV(3)
#define CURTAIN_REVERSE_SEL      P1SEL
#define CURTAIN_REVERSE_DIR      P1DIR
#define CURTAIN_REVERSE_POLARITY ACTIVE_LOW

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

// 光照强度检测端口
#define CURTAIN_BRIGHTNESS_ADC_CH ADC_CONVERT_CH7 

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
#define SET_SPD_CH1_REVERSE()    ( CURTAIN_SPD_CH1_PORT = CURTAIN_SPD_CH1_POLARITY(1) )
#define CLR_SPD_CH1_REVERSE()    ( CURTAIN_SPD_CH1_PORT = CURTAIN_SPD_CH1_POLARITY(0) )
#define RD_SPD_CH1_REVERSE()     ( CURTAIN_SPD_CH1_POLARITY(CURTAIN_SPD_CH1_PORT) )

// 转速输入端口2
#define CURTAIN_SPD_CH2_WrMode() ( CURTAIN_SPD_CH2_DIR |=  CURTAIN_SPD_CH2_BV )
#define CURTAIN_SPD_CH2_RdMode() ( CURTAIN_SPD_CH2_DIR &= ~CURTAIN_SPD_CH2_BV )
#define SET_SPD_CH2_REVERSE()    ( CURTAIN_SPD_CH2_PORT = CURTAIN_SPD_CH2_POLARITY(1) )
#define CLR_SPD_CH2_REVERSE()    ( CURTAIN_SPD_CH2_PORT = CURTAIN_SPD_CH2_POLARITY(0) )
#define RD_SPD_CH2_REVERSE()     ( CURTAIN_SPD_CH2_POLARITY(CURTAIN_SPD_CH2_PORT) )

// 亮度检测端口
#define Curtain_Brightness_Init() MCU_ADC_CH_Init(CURTAIN_BRIGHTNESS_ADC_CH)

/** @}*/     /* 电动窗帘硬件抽象层定义 */

// 电动窗帘使用定时器数量 
#define CURTAIN_USE_TIMER_NUM    (2)

// 电动窗帘占用FLASH大小
#define DEVICE_CURTAIN_DATA_SIZE (Cal_DataSize(curtain))

// 电动窗帘工作状态
#define CURTAIN_INIT_STATE        (0x00)
#define CURTAIN_OPEN_STATE        (0x01)
#define CURTAIN_OPEN_END_STATE    (0x02)
#define CURTAIN_OPEN_ALL_STATE    (0x03)
#define CURTAIN_CLOSE_STATE       (0x04)
#define CURTAIN_CLOSE_END_STATE   (0x05)
#define CURTAIN_CLOSE_ALL_STATE   (0x06)

// 电动窗帘控制命令
#define CURTAIN_CMD_KEEP          (0x00)
#define CURTAIN_CMD_OPEN          (0x01)
#define CURTAIN_CMD_CLOSE         (0x02)

// 电动窗帘状态
#define CURTAIN_STATE_KEEP        (0x00)
#define CURTAIN_STATE_CLOSE       (0x01)
#define CURTAIN_STATE_OPEN        (0x02)

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
    RD_CURTAIN_RUN_MODE     = 0x16,
    WR_CURTAIN_RUN_MODE     = 0x17,
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
    
    // 定时器数据
    DEVICE_TIMER timer[CURTAIN_USE_TIMER_NUM]; 
    
    // 窗帘智能工作模式
    struct
    {
        uint8 state;
        uint8 brightness;
    }run_mode;
    
    // 下雨警报
    uint8 rain_warning;
    
    // 室内亮度
    uint8 brightness;
    
    uint8 load_set;
}curtain;

// 下雨标志
bool flg_rain = false;
uint8 curtain_cmd = CURTAIN_CMD_KEEP;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       电动窗帘测速端口初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        测速端口状态： CH1、CH2为低电平
 * @note                       CH1、CH2为正弦脉冲                    
 *******************************************************************************
 */
static void Curtain_Speed_Port_Init( void )
{
	CURTAIN_SPD_CH1_RdMode();
	CURTAIN_SPD_CH2_RdMode();	
}

/**
 *******************************************************************************
 * @brief       电动窗帘状态检测
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None                  
 *******************************************************************************
 */
static uint8 Curtain_Speed_Scan( void )
{
	static enum
    {
        CURTAIN_WORKING_STATE_LOW,
        CURTAIN_WORKING_STATE_PULSE,
    }CH1_STATE, CH2_STATE; 
	
	return CURTAIN_STATE_KEEP;
}

/**
 *******************************************************************************
 * @brief       电动窗帘电机控制端口初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        电机正反转控制                  
 *******************************************************************************
 */
static void Curtain_Metor_Port_Init( void )
{
	CURTAIN_FORWARD_WrMode();
    CLR_CURTAIN_FORWARD();
    
    CURTAIN_REVERSE_WrMode();
    CLR_CURTAIN_REVERSE();
}

/**
 *******************************************************************************
 * @brief       初始化窗帘设置数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void curtain_rst_set( void )
{
    memset(&curtain,0,sizeof(curtain));
    
    osal_nv_item_init(DEVICE_CURTAIN_SAVE_ID,DEVICE_CURTAIN_DATA_SIZE,NULL);
    osal_nv_write(DEVICE_CURTAIN_SAVE_ID,0,DEVICE_CURTAIN_DATA_SIZE,(void *)&curtain);
}

/**
 *******************************************************************************
 * @brief       初始化窗帘
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_curtain_init(void)
{
    // 电动窗帘驱动模块初始化
	Curtain_Metor_Port_Init();
	
	// 电动窗帘测速模块初始化
	Curtain_Speed_Port_Init();
	
	// 亮度检测端口初始化
    Curtain_Brightness_Init();
	
    // FLASH 数据初始化
    Device_Load_LastData(DEVICE_CURTAIN_SAVE_ID,DEVICE_CURTAIN_DATA_SIZE,(void *)&curtain,curtain_rst_set);
}
 
/**
 *******************************************************************************
 * @brief       电动窗帘控制命令
 * @param       [in/out]  cmd     控制命令
 * @return      [in/out]  bool    执行状态
 * @note        None
 *******************************************************************************
 */
bool bsp_curtain_cmd( uint8 cmd )
{
    if( cmd == CURTAIN_CMD_OPEN )
    {
        SET_CURTAIN_FORWARD();
        CLR_CURTAIN_REVERSE();
    }
    else if( cmd == CURTAIN_CMD_CLOSE )
    {
        CLR_CURTAIN_REVERSE();
        SET_CURTAIN_FORWARD(); 
    }
    else
    {
        CLR_CURTAIN_REVERSE();
        CLR_CURTAIN_REVERSE();
    }
    
    return true;
}

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void curtain_switch_handler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
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
void curtain_rain_handler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_LONG_PRESS:
            flg_rain = true;
			break;
		default:
			break;
    }
}

#endif

/**
 *******************************************************************************
 * @brief       解析命令数据
 * @param       [in/out]  *data    控制命令数据
 * @return      [in/out]  bool     解析状态
 * @note        None
 *******************************************************************************
 */
bool curtain_cmd_resolve( MYPROTOCOL_USER_DATA *data )
{    
    switch( data->cmd )
    {
        case DEVICE_TICK:
            break;
        case DEVICE_RESET:
            break;
        case DEVICE_REBOOT:
            break;
        case RD_CURTAIN_STATE:
            break;
        case WR_CURTAIN_OPERA:
            break;
        case RD_CURTAIN_SINGLE_TIMER:
            break;
        case RD_CURTAIN_CIRCUL_TIMER:
            break;
        case WR_CURTAIN_CIRCUL_TIMER:
            break;
        case RD_CURTAIN_RUN_MODE:
            break;
        case WR_CURTAIN_RUN_MODE:
            break;
        case RD_CURTAIN_RAIN_WARNING:
            break;
        case RD_CURTAIN_BRIGHTNESS:
            break;
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 窗帘模块 */

/**********************************END OF FILE*********************************/
