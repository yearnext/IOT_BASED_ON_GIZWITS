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

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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

#define CURTAIN_USE_TIMER_NUM    (2)

#define DEVICE_CURTAIN_DATA_SIZE (Cal_DataSize(curtain))

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
}DEVICE_CURTAIN_CMD;

/* Private variables ---------------------------------------------------------*/
// 电灯存储数据
static struct _DEVICE_CURTAIN_SAVE_DATA_
{
    // 状态数据
    struct
    {
        uint8 now;
        uint8 last;
    }status;
    
    // 定时器数据
    DEVICE_TIMER timer[CURTAIN_USE_TIMER_NUM]; 
    
    // 初始化载入数据
//    enum
//    {
//        LIGHT_NO_LOAD_SET = 0x00,
//        LIGHT_LOAD_SET    = 0x01,
//    }load_set;
    uint8 load_set;
}curtain;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
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
    // 初始化端口输出模块
    CURTAIN_FORWARD_WrMode();
    CLR_CURTAIN_FORWARD();
    
    CURTAIN_REVERSE_WrMode();
    CLR_CURTAIN_REVERSE();
    
    // FLASH 数据初始化
    Device_Load_LastData(DEVICE_CURTAIN_SAVE_ID,DEVICE_CURTAIN_DATA_SIZE,(void *)&curtain,curtain_rst_set);
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
		case KEY_MESSAGE_PRESS_EDGE:
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
