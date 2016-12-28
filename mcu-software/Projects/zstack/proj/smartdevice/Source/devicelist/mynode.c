 /**
 ******************************************************************************
  * @file       mynode.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      mynode 源文件
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

/**
 * @defgroup mynode 模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "mynode.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/     
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       遍历节点
 * @param       [in/out]  head    链表头
 *	            [in/out]  func    链表访问函数
 *              [in/out]  ctx     上下文
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool nodeTraverse(void **list_head, node_func visit, void **ctx, void **expand)
{
	bool status = false;
	myListHand_t list = (myListHand_t)list_head;

	while(1)
	{
		status = visit(ctx, (void **)list, expand);

		if (status == true || *list == NULL)
		{
			break;
		}

		list = &((myList_t)(*list))->next;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       增加节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool nodeAddOpera(void **ctx, void **list, void **expand)
{
    if(*list == NULL)
	{
		*list = *ctx;
		return true;
	}
	
	return false;
}

/**
 *******************************************************************************
 * @brief       删除节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool nodeClrOpera(void **ctx, void **list, void **expand)
{
	myList_t node = (myList_t)(*ctx);

	if (*list == *ctx)
	{
		*list = node->next;
		node->next = NULL;
		return true;
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       获取节点数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool nodeGetNum(void **crx, void **list, void **expand)
{
	uint8 *num = (uint8 *)(crx);

	if (*list == NULL)
	{
		return true;
	}
	else
	{
		(*num)++;
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       查询节点是否在列表中
 * @param       [in/out]  ctx     上下文
 *			    [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool nodeIsExists(void **ctx, void **list, void **expand)
{
    return (*list == *ctx) ? (true) : (false);
}

/** @}*/     /* devicelist 模块 */

/**********************************END OF FILE*********************************/
