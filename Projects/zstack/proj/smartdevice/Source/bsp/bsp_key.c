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
  * 				 IAR
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
	KEY_PRESS_FILTER,
	KEY_PRESS_EDGE,
	KEY_PRESS,
	KEY_RELEASE_FILTER,
	KEY_RELEASE_EDGE,
	KEY_RELEASE,
}key_status_t;

/**
 *******************************************************************************
 * @brief       按键检测功能函数指针
 *******************************************************************************
 */
typedef bool(*hal_key_func)(void);

/* Private variables ---------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键功能变量
 *******************************************************************************
 */
static key_status_t KEY_STATUS[KEY_MAX];
static key_message_t KEY_MESSAGE[KEY_MAX];

/* Private functions ---------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       按键1初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  true
 * @note        None
 *******************************************************************************
 */
static bool KEY1_PORT_INIT( void )
{
    KEY1_PORT_SEL &= ~KEY1_PORT_BIT; 
    KEY1_PORT_DIR &= ~KEY1_PORT_BIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       按键1检测函数
 * @param       [in/out]  void
 * @return      [in/out]  按键按下状态
 * @note        None
 *******************************************************************************
 */
static bool KEY1_DETECT( void )
{
    return ((KEY1_READ())?(true):(false));
}

/**
 *******************************************************************************
 * @brief       按键2初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  true
 * @note        None
 *******************************************************************************
 */
static bool KEY2_PORT_INIT( void )
{
    KEY2_PORT_SEL &= ~KEY2_PORT_BIT; 
    KEY2_PORT_DIR &= ~KEY2_PORT_BIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       按键2检测函数
 * @param       [in/out]  void
 * @return      [in/out]  按键按下状态
 * @note        None
 *******************************************************************************
 */
static bool KEY2_DETECT( void )
{
    return ((KEY2_READ())?(true):(false));
}

/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       读取按键消息
 * @param       [in/out]  type      按键功能
 *              [in/out]  message   按键消息
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void key_message_send(uint8 type, key_message_t message)
{
	KEY_MESSAGE[type] = message;
}

/**
 *******************************************************************************
 * @brief       读取按键消息
 * @param       [in/out]  按键类型
 * @return      [in/out]  按键消息
 * @note        None
 *******************************************************************************
 */
key_message_t key_message_read(uint8 type)
{
	key_message_t message = KEY_MESSAGE[type];

	KEY_MESSAGE[type] = KEY_MESSAGE_NONE;

	return message;
}

/**
 *******************************************************************************
 * @brief       遍历按键函数
 * @param       [in/out]  按键功能
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void key_foreach(bsp_key_func func)
{
	uint8 i;

	for (i = 0; i < KEY_MAX; i++)
	{
		func(i);
	}
}

/**
 *******************************************************************************
 * @brief       按键初始化函数
 * @param       [in/out]  按键类型
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void hal_key_init(uint8 type)
{
    hal_key_func init[] = { KEY1_PORT_INIT, KEY2_PORT_INIT };
    
    init[type]();
    
	KEY_STATUS[type] = KEY_DETECT;
	KEY_MESSAGE[type] = KEY_MESSAGE_NONE;
}

/**
 *******************************************************************************
 * @brief       按键检测函数
 * @param       [in/out]  按键类型
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void hal_key_scan(uint8 type)
{
	hal_key_func detect[] = { KEY1_DETECT, KEY2_DETECT };
	bool status = detect[type]();

	switch (KEY_STATUS[type])
	{
        case KEY_DETECT:
            if (status == true)
            {
                KEY_STATUS[type] = KEY_PRESS_FILTER;
            }
            else
            {
                KEY_STATUS[type] = KEY_RELEASE_FILTER;
            }
            break;
            /** 按键按下事件 */
        case KEY_PRESS_FILTER:
            if (status == true)
            {
                KEY_STATUS[type] = KEY_PRESS_EDGE;
                key_message_send(type, KEY_MESSAGE_PRESS_EDGE);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
        case KEY_PRESS_EDGE:
            if (status == true)
            {
                KEY_STATUS[type] = KEY_PRESS;
                key_message_send(type, KEY_MESSAGE_PRESS);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
        case KEY_PRESS:
            if (status == true)
            {
                KEY_STATUS[type] = KEY_PRESS;
                key_message_send(type, KEY_MESSAGE_PRESS);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
            /** 按键松开事件 */
        case KEY_RELEASE_FILTER:
            if (status == false)
            {
                KEY_STATUS[type] = KEY_RELEASE_EDGE;
                key_message_send(type, KEY_MESSAGE_RELEASE_EDGE);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
        case KEY_RELEASE_EDGE:
            if (status == false)
            {
                KEY_STATUS[type] = KEY_RELEASE;
                key_message_send(type, KEY_MESSAGE_RELEASE);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
        case KEY_RELEASE:
            if (status == false)
            {
                KEY_STATUS[type] = KEY_RELEASE;
                key_message_send(type, KEY_MESSAGE_RELEASE);
            }
            else
            {
                KEY_STATUS[type] = KEY_DETECT;
            }
            break;
        default:
            break;
	}
}

/**
 *******************************************************************************
 * @brief       按键处理函数
 * @param       [in/out]  按键类型
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void key_handler(uint8 type)
{
	key_message_t message = key_message_read(type);

	switch (message)
	{
		case KEY_1:
            key1_message_handler(message);
			break;
		case KEY_2:
            //key2_message_handler(message);
			break;
		default:
			break;
	}
}

/** @}*/     /* 按键模块 */

/**********************************END OF FILE*********************************/
