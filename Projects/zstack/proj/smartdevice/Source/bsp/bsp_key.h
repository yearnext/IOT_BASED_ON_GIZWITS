/**
 ******************************************************************************
  * @file       bsp_key.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      key 头文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                         
  * 				 IAR
 ******************************************************************************
  * @note
  * 1.支持按键按下、按下边沿、松开边沿、松开状态的检测。
  * 2.按键状态通过消息机制传递。
  * 3.按键检测通过状态机流程检测。                 						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include <ioCC2530.h>
#include "myprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键模块配置层
 *******************************************************************************
 */
// 按键扫描时间
#define KEY_SCAN_TIME (10)
#define KEY_TIME2COUNT(n) ((n)/KEY_SCAN_TIME)

#define KEY_LONG_RRESS_TIME KEY_TIME2COUNT(300)

// 按键动作
#define KEY_ACTIVE_LOW  !
#define KEY_ACTIVE_HIGH !!

// 按键端口配置
/** KEY1 P0_0 */
#define KEY1_PORT        P0_0
#define KEY1_PORT_SEL    P0SEL
#define KEY1_PORT_DIR    P0DIR
#define KEY1_PORT_BIT    BV(0)
#define KEY1_PORT_INIT() st(KEY1_PORT_SEL &= ~KEY1_PORT_BIT; KEY1_PORT_DIR &= ~KEY1_PORT_BIT;)
#define KEY1_READ()      KEY_ACTIVE_LOW(KEY1_PORT)
/** KEY2 P1_2 */
#define KEY2_PORT        P1_2
#define KEY2_PORT_SEL    P1SEL
#define KEY2_PORT_DIR    P1DIR
#define KEY2_PORT_BIT    BV(2)
#define KEY2_PORT_INIT() st(KEY2_PORT_SEL &= ~KEY2_PORT_BIT; KEY2_PORT_DIR &= ~KEY2_PORT_BIT;)
#define KEY2_READ()      KEY_ACTIVE_LOW(KEY2_PORT)

/* Exported types ------------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       配置按键键值
 * @note        用户可编辑
 *******************************************************************************
 */
typedef enum
{
	KEY_VALUE_NOP = 0x00,
	KEY_VALUE_SWITCH,
	KEY_VALUE_RESET,
}key_value_t;

/**
 *******************************************************************************
 * @brief       按键消息
 * @note        功能未完善
 *******************************************************************************
 */
typedef enum
{
	KEY_MESSAGE_NONE,
	KEY_MESSAGE_PRESS_EDGE,
	KEY_MESSAGE_PRESS,
	KEY_MESSAGE_LONG_PRESS,
	KEY_MESSAGE_RELEASE_EDGE,
	KEY_MESSAGE_RELEASE,
}key_message_t;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern bool key_message_send(key_value_t, key_message_t);
extern bool key_message_read(key_value_t*, key_message_t*);
extern void hal_key_init(void);
extern key_value_t get_key_value(void);
extern void key_scan(void);
extern void key_handler(void);

extern void key_switch_handler(key_message_t);

#endif      /* __BSP_KEY_H__ */

/**********************************END OF FILE*********************************/
