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

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
}DEVICE_LIGHT_CMD;

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
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
void key_switch_handler( key_message_t message )
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

#endif

/** @}*/     /* 窗帘模块 */

/**********************************END OF FILE*********************************/
