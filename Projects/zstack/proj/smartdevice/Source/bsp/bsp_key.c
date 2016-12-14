 /**
  ******************************************************************************
  * @file       bsp_key.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      key 源文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                         
  * 		     IAR
  ******************************************************************************
  * @note
  * 1.支持按键按下、按下边沿、松开边沿、松开状态的检测。
  * 2.按键状态通过消息机制传递。
  * 3.按键检测通过状态机流程检测。              						                     
 ******************************************************************************
 */

/**
 * @defgroup 按键模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_key.h"
#include <string.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键检测状态
 * @note        功能未完善
 *******************************************************************************
 */
typedef enum
{
	KEY_DETECT,
/*	KEY_FILTER,*/
	KEY_PRESS,
	KEY_RELEASE,
}key_status_t;

/**
 *******************************************************************************
 * @brief       按键消息包
 * @note        功能未完善
 *******************************************************************************
 */
typedef struct
{
	key_value_t value;
	key_message_t message;
}key_t;

/**
 *******************************************************************************
 * @brief       按键定时器
 * @note        功能未完善
 *******************************************************************************
 */
typedef struct
{
	uint16 status : 1;
	uint16 count : 15;
}key_timer_t;

/* Private variables ---------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键功能变量
 *******************************************************************************
 */
static key_t key;
/*static key_timer_t timer;*/

/**
 *******************************************************************************
 * @brief       按键功能检测数据
 * @note        功能未完善
 *******************************************************************************
 */
static struct
{
	key_timer_t press_long;
    key_timer_t press_multi;
}press_detect;

/* Private functions ---------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键定时器处理函数
 * @param       [in/out]  value   按键键值
 * @return      [in/out]  bool    事件触发标志
 * @note        None
 *******************************************************************************
 */
static bool key_press_timer_handler(key_value_t value)
{
    // 按键多击检测
    if (press_detect.press_multi.status)
	{
		if (key_message_send(value, KEY_MESSAGE_MULTI_PRESS) == true)
		{
			press_detect.press_multi.status = 0;			
		}
		
		return true;
	} 
	else
	{
		if (++press_detect.press_multi.count >= KEY_MULTI_RRESS_TIME)
		{
			press_detect.press_multi.count = 0;
			
			if (key_message_send(value, KEY_MESSAGE_MULTI_PRESS) == false)
			{
				press_detect.press_multi.status = 1;	
			}
			
			return true;
		}
	}
    
    // 按键长按检测
	if (press_detect.press_long.status)
	{
		key_message_send(value, KEY_MESSAGE_LONG_PRESS);
		return true;
	} 
	else
	{
		if (++press_detect.press_long.count >= KEY_LONG_RRESS_TIME)
        {
            press_detect.press_long.status = 1;	
            key_message_send(value, KEY_MESSAGE_LONG_PRESS_EDGE);
            return true;
        }
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       清除定时器数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void clear_key_press_timer(void)
{
	memset(&press_detect, 0, sizeof(press_detect));
}

/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       发送按键消息
 * @param       [in/out]  value     按键键值
 *              [in/out]  message   按键消息
 * @return      [in/out]  bool      发送结果
 * @note        None
 *******************************************************************************
 */
bool key_message_send(key_value_t value, key_message_t message)
{
	if ( key.value == KEY_VALUE_NOP && key.message == KEY_MESSAGE_NONE )
	{
		key.value = value;
		key.message = message;

		return true;
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       读取按键消息
 * @param       [in/out]  value     按键键值
 *              [in/out]  message   按键消息
 * @return      [in/out]  bool      读取结果
 * @note        None
 *******************************************************************************
 */
bool key_message_read(key_value_t *value, key_message_t *message)
{
	if (key.value == KEY_VALUE_NOP && key.message == KEY_MESSAGE_NONE)
	{
		return false;
	}
	else
	{
		*value = key.value;
		*message = key.message;

		key.value = KEY_VALUE_NOP;
		key.message = KEY_MESSAGE_NONE;
	
		return true;
	}
}

/**
 *******************************************************************************
 * @brief       按键初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bsp_key_init(void)
{
#if MYPROTOCOL_DEVICE_IS_COORD
    /** KEY1 P0_4 */
    KEY1_PORT_INIT();
    /** KEY2 P0_5 */
    KEY2_PORT_INIT();

#elif MYPROTOCOL_DEVICE_IS_LIGHT
    /** KEY1 P0_4 */
    KEY1_PORT_INIT();
    /** KEY2 P0_5 */
    KEY2_PORT_INIT();

#elif MYPROTOCOL_DEVICE_IS_SOCKET
    /** KEY1 P0_4 */
    KEY1_PORT_INIT();
    /** KEY2 P0_5 */
    KEY2_PORT_INIT();

#elif MYPROTOCOL_DEVICE_IS_CURTAIN
    /** KEY1 P1_0 */
    KEY1_PORT_INIT();
    /** KEY2 P0_6 */
    KEY2_PORT_INIT();
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR

#else
    /** KEY1 */ 
    KEY1_PORT_INIT();     
    /** KEY2 */  
    KEY2_PORT_INIT();    
#endif
    
	clear_key_press_timer();

	key.value = KEY_VALUE_NOP;
	key.message = KEY_MESSAGE_NONE;
}

/**
 *******************************************************************************
 * @brief       读取按键键值
 * @param       [in/out]  void
 * @return      [in/out]  读取到的键值
 * @note        None
 *******************************************************************************
 */
key_value_t get_key_value(void)
{
#if MYPROTOCOL_DEVICE_IS_COORD
    /** KEY1 P0_4 */
	if (KEY1_READ())
	{
		return KEY_VALUE_K1;
	}
    
    /** KEY2 P0_5 */
	if (KEY2_READ())
	{
		return KEY_VALUE_K2;
	}
#elif MYPROTOCOL_DEVICE_IS_LIGHT
    /** KEY1 P0_4 */
	if (KEY1_READ())
	{
		return KEY_VALUE_K1;
	}
    
    /** KEY2 P0_5 */
	if (KEY2_READ())
	{
		return KEY_VALUE_K2;
	}  
#elif MYPROTOCOL_DEVICE_IS_SOCKET
    /** KEY1 P0_4 */
	if (KEY1_READ())
	{
		return KEY_VALUE_K1;
	}
    
    /** KEY2 P0_5 */
	if (KEY2_READ())
	{
		return KEY_VALUE_K2;
	}
#elif MYPROTOCOL_DEVICE_IS_CURTAIN
    /** KEY1 P0_4 */
	if (KEY1_READ())
	{
		return KEY_VALUE_K1;
	}
    
    /** KEY1 P0_6 */
	if (KEY2_READ())
	{
		return KEY_VALUE_K2;
	}
    
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR
#else
#endif
    
	return KEY_VALUE_NOP;
}

/**
 *******************************************************************************
 * @brief       按键检测程序
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void key_scan(void)
{
	static key_value_t key_filter_value = KEY_VALUE_NOP;
	static key_status_t key_now_status = KEY_DETECT;

	key_value_t key_now_value = get_key_value();

	switch (key_now_status)
	{
		case KEY_DETECT:
			if (key_now_value == key_filter_value)
			{
				if (key_now_value != KEY_VALUE_NOP)
				{
					clear_key_press_timer();
					key_now_status = KEY_PRESS;
					key_message_send(key_now_value, KEY_MESSAGE_PRESS_EDGE);
				}
				else
				{
					key_now_status = KEY_RELEASE;
				}
			}
			else
			{
				key_filter_value = key_now_value;
				key_now_status = KEY_DETECT;
			}
			break;
		case KEY_PRESS:
			if ( key_now_value == key_filter_value )
			{
				if ( key_press_timer_handler(key_now_value) == false )
				{
					key_message_send(key_now_value, KEY_MESSAGE_PRESS);
				}
			} 
			else
			{
				clear_key_press_timer();
				key_message_send(key_filter_value, KEY_MESSAGE_RELEASE_EDGE);
				
				key_filter_value = key_now_value;
				key_now_status = KEY_DETECT;
			}
			break;
		case KEY_RELEASE:
			if (key_now_value == KEY_VALUE_NOP)
			{

			}
			else
			{
				key_filter_value = key_now_value;
				key_now_status = KEY_DETECT;
			}
			break;
		default:
			break;
	}
}

/**
 *******************************************************************************
 * @brief       按键处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void key_handler(void)
{
	key_t message;
	
	if (key_message_read(&message.value, &message.message) == true)
	{
		switch (message.value)
		{
			case KEY_VALUE_K1:
#if MYPROTOCOL_DEVICE_IS_COORD
                key1_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_LIGHT
                key1_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_SOCKET
                key1_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_CURTAIN
                key1_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR
#else
#endif
				break;
			case KEY_VALUE_K2:
#if MYPROTOCOL_DEVICE_IS_COORD
                key2_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_LIGHT
                key2_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_SOCKET
                key2_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_CURTAIN
                key2_handler(message.message);
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR
#else
#endif
				break;
			default:
				break;
		}
	}
}

/** @}*/     /* 按键模块 */

/**********************************END OF FILE*********************************/
