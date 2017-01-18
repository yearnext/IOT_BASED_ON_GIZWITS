/**
 ******************************************************************************
  * @file       mynode.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      mynode 头文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                          
  * 				 IAR
 ******************************************************************************
  * @note
  * 这是一个通用单链表操作模块能够实现链表的增加、删除等操作。                  						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_NODE_H__
#define __MY_NODE_H__

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @defgroup 定义节点相关类型
 * @{
 */
/** 定义节点类型 */
typedef struct _USER_NODE_
{
	struct _USER_NODE_ *next;
	void *data;
}myNODE_t, *myList_t, **myListHand_t;

/** 定义节点访问函数类型 */
typedef bool(*node_func)(void **, void **, void **);

/**
 * @}
 */

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @defgroup 节点访问相关API
 * @{
 */
extern bool nodeTraverse(void**, node_func, void**, void**);
extern bool nodeAddOpera(void**, void**, void**);
extern bool nodeClrOpera(void**, void**, void**);
extern bool nodeGetNum(void**, void**, void**);
extern bool nodeIsExists(void**, void**, void**);
/**
 * @}
 */
 
#ifdef __cplusplus
}
#endif

#endif      /* __MY_DEVICE_LIST_H__ */

/**********************************END OF FILE*********************************/
