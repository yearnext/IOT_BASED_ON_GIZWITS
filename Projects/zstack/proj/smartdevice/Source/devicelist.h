/**
 ******************************************************************************
  * @file       devicelist.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      devicelist 头文件
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

#if defined ( USE_GIZWITS_MOD )

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_DEVICE_LIST_H__
#define __MY_DEVICE_LIST_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct _USER_NODE_
{
	struct _USER_NODE_ *next;
	void *data;
}NODE, *LIST_NODE, **LIST_HEAD;

typedef enum
{
	DEVICE_COORD = 0x00,
	DEVICE_LIGHT,
	DEVICE_SOCKET,
	DEVICE_CURTAIN,
	//    DEVICE_END,
}DEVICE_TYPE;

typedef struct
{
	DEVICE_TYPE device;
	uint16 shortaddr;
	uint8 tick;
}DEVICE_INFO;

typedef bool(*visit_func)(void **ctx, void **list);

typedef LIST_NODE DEVICE_LIST_HEAD;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void Del_ZombieDevice_ForList(void);

extern void Del_DeviceTickCount( void );

#endif      /* __MY_DEVICE_LIST_H__ */

#endif

/**********************************END OF FILE*********************************/
