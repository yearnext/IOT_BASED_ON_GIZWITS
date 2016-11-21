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

#define KEY_LONG_RRESS_TIME  KEY_TIME2COUNT(3000)
#define KEY_MULTI_RRESS_TIME KEY_TIME2COUNT(300)

// 按键动作
#define KEY_ACTIVE_LOW  !
#define KEY_ACTIVE_HIGH !!

// 网关按键端口配置
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
/** KEY1 P0_4 */
#define KEY1_PORT        P0_4
#define KEY1_PORT_SEL    P0SEL
#define KEY1_PORT_DIR    P0DIR
#define KEY1_PORT_BIT    BV(4)
#define KEY1_PORT_INIT() st(KEY1_PORT_SEL &= ~KEY1_PORT_BIT; KEY1_PORT_DIR &= ~KEY1_PORT_BIT;)
#define KEY1_READ()      KEY_ACTIVE_LOW(KEY1_PORT)
/** KEY2 P0_5 */
#define KEY2_PORT        P0_5
#define KEY2_PORT_SEL    P0SEL
#define KEY2_PORT_DIR    P0DIR
#define KEY2_PORT_BIT    BV(5)
#define KEY2_PORT_INIT() st(KEY2_PORT_SEL &= ~KEY2_PORT_BIT; KEY2_PORT_DIR &= ~KEY2_PORT_BIT;)
#define KEY2_READ()      KEY_ACTIVE_LOW(KEY2_PORT)
// 电灯灯按键端口配置
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
/** KEY1 P0_4 */
#define KEY1_PORT        P0_4
#define KEY1_PORT_SEL    P0SEL
#define KEY1_PORT_DIR    P0DIR
#define KEY1_PORT_BIT    BV(4)
#define KEY1_PORT_INIT() st(KEY1_PORT_SEL &= ~KEY1_PORT_BIT; KEY1_PORT_DIR &= ~KEY1_PORT_BIT;)
#define KEY1_READ()      KEY_ACTIVE_LOW(KEY1_PORT)
/** KEY2 P0_5 */
#define KEY2_PORT        P0_5
#define KEY2_PORT_SEL    P0SEL
#define KEY2_PORT_DIR    P0DIR
#define KEY2_PORT_BIT    BV(5)
#define KEY2_PORT_INIT() st(KEY2_PORT_SEL &= ~KEY2_PORT_BIT; KEY2_PORT_DIR &= ~KEY2_PORT_BIT;)
#define KEY2_READ()      KEY_ACTIVE_LOW(KEY2_PORT)
// 插座按键端口配置
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_SOCKET)
/** KEY1 P0_4 */
#define KEY1_PORT        P0_4
#define KEY1_PORT_SEL    P0SEL
#define KEY1_PORT_DIR    P0DIR
#define KEY1_PORT_BIT    BV(4)
#define KEY1_PORT_INIT() st(KEY1_PORT_SEL &= ~KEY1_PORT_BIT; KEY1_PORT_DIR &= ~KEY1_PORT_BIT;)
#define KEY1_READ()      KEY_ACTIVE_LOW(KEY1_PORT)
/** KEY2 P0_5 */
#define KEY2_PORT        P0_5
#define KEY2_PORT_SEL    P0SEL
#define KEY2_PORT_DIR    P0DIR
#define KEY2_PORT_BIT    BV(5)
#define KEY2_PORT_INIT() st(KEY2_PORT_SEL &= ~KEY2_PORT_BIT; KEY2_PORT_DIR &= ~KEY2_PORT_BIT;)
#define KEY2_READ()      KEY_ACTIVE_LOW(KEY2_PORT)
// 窗帘按键端口配置
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
/** KEY1 P1_0 */
#define KEY1_PORT        P1_0
#define KEY1_PORT_SEL    P1SEL
#define KEY1_PORT_DIR    P1DIR
#define KEY1_PORT_BIT    BV(0)
#define KEY1_PORT_INIT() st(KEY1_PORT_SEL &= ~KEY1_PORT_BIT; KEY1_PORT_DIR &= ~KEY1_PORT_BIT;)
#define KEY1_READ()      KEY_ACTIVE_LOW(KEY1_PORT)

/** KEY2 P0_6 */
#define KEY2_PORT        P0_6
#define KEY2_PORT_SEL    P0SEL
#define KEY2_PORT_DIR    P0DIR
#define KEY2_PORT_BIT    BV(6)
#define KEY2_PORT_INIT() st(KEY2_PORT_SEL &= ~KEY2_PORT_BIT; KEY2_PORT_DIR &= ~KEY2_PORT_BIT;)
#define KEY2_READ()      KEY_ACTIVE_LOW(KEY2_PORT)

// 温湿度传感器按键端口配置
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_HT_SENSOR)

#else
/** KEY1 */
#define KEY1_PORT        
#define KEY1_PORT_SEL    
#define KEY1_PORT_DIR    
#define KEY1_PORT_BIT    
#define KEY1_PORT_INIT() 
#define KEY1_READ()     
/** KEY2 */
#define KEY2_PORT       
#define KEY2_PORT_SEL    
#define KEY2_PORT_DIR   
#define KEY2_PORT_BIT    
#define KEY2_PORT_INIT()
#define KEY2_READ()      

#endif

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
    KEY_VALUE_RAIN,
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
    KEY_MESSAGE_MULTI_PRESS,
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
