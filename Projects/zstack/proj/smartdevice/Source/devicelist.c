 /**
 ******************************************************************************
  * @file       devicelist.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      devicelist 源文件
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
 * @defgroup devicelist 模块
 * @{
 */

#if defined ( USE_GIZWITS_MOD )

/* Includes ------------------------------------------------------------------*/
#include "devicelist.h"
#include "OSAL_Memory.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/     
/* Private variables ---------------------------------------------------------*/
static DEVICE_LIST_HEAD head = NULL;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       遍历节点
 * @param       [in/out]  head    链表头
 *	            [in/out]  func    链表访问函数
 *              [in/out]  ctx     上下文
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool list_foreach(void **list_head, visit_func visit, void **ctx)
{
	bool status = false;
	LIST_HEAD list = (LIST_HEAD)list_head;

	while (1)
	{
		status = visit(ctx, (void **)list);

		if (status == true || *list == NULL)
		{
			break;
		}

		list = &((LIST_NODE)(*list))->next;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       增加节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_add(void **ctx, void **list)
{
	bool status = false;

	if (*list == NULL)
	{
		*list = *ctx;
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       删除节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_del(void **ctx, void **list)
{
	bool status = false;
	LIST_NODE node = (LIST_NODE)(*ctx);

	if (*list == *ctx)
	{
		*list = node->next;
		node->next = NULL;
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       获取节点数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool get_node_num(void **crx, void **list)
{
	uint8 *num = (uint8 *)(crx);
	bool status = false;

	if (*list == NULL)
	{
		status = true;
	}
	else
	{
		(*num)++;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       查询节点是否在列表中
 * @param       [in/out]  ctx     上下文
 *			   [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_find(void **ctx, void **list)
{
	bool status = false;

	if (*list == *ctx)
	{
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       查询设备是否存在于设备列表中
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool find_device_inlist(void **ctx, void **list)
{
	bool status = false;
	DEVICE_INFO *list_info = NULL;
	DEVICE_INFO *device_info = (DEVICE_INFO *)ctx;

	if (*list != NULL)
	{
		list_info = ((LIST_NODE)(*list))->data;

		if (list_info->device == device_info->device \
			&& list_info->shortaddr == device_info->shortaddr)
		{
			status = true;
		}
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       获取设备在列表中的节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool get_device_addr(void **ctx, void **list)
{
	bool status = false;
	DEVICE_INFO *list_info = NULL;
	DEVICE_INFO *device_info = (DEVICE_INFO *)*ctx;

	if (*list != NULL)
	{
		list_info = ((LIST_NODE)(*list))->data;

		if (list_info->device == device_info->device \
			&& list_info->shortaddr == device_info->shortaddr)
		{
			*ctx = *list;
			status = true;
		}
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       增加设备心跳数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool add_device_tick(void **ctx, void **list)
{
	bool status = false;
	DEVICE_INFO *device_info = (DEVICE_INFO *)ctx;
	DEVICE_INFO *node_info = NULL;

	if (*list != NULL)
	{
		node_info = ((LIST_NODE)*list)->data;

		if (device_info->device == node_info->device \
			&& device_info->shortaddr == node_info->shortaddr)
		{
			node_info->tick++;
			status = true;
		}
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       清空设备的心跳数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool clr_device_tick(void **ctx, void **list)
{
	bool status = false;
	DEVICE_INFO *device_info = (DEVICE_INFO *)ctx;
	DEVICE_INFO *node_info = NULL;

	if (*list != NULL)
	{
		node_info = ((LIST_NODE)*list)->data;

		if (device_info->device == node_info->device \
			&& device_info->shortaddr == node_info->shortaddr)
		{
			node_info->tick = 0;
			status = true;
		}
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       清空所有设备的心跳数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool clr_all_device_tick(void **ctx, void **list)
{
	bool status = false;
	DEVICE_INFO *node_info = NULL;

	if (*list != NULL)
	{
		node_info = ((LIST_NODE)*list)->data;
		node_info->tick = 0;
	}
	else
	{
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       检测设备是否在设备列表中
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool Check_Device_InList( DEVICE_INFO *info )
{
    return list_foreach((void **)&head, find_device_inlist, (void **)info);
}

/**
 *******************************************************************************
 * @brief       添加设备到设备列表中
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool Add_Device_Forlist(DEVICE_INFO *info)
{
	bool status = false;
	DEVICE_INFO *device_info = NULL;
	NODE *list_node = NULL;

	if (list_foreach((void **)&head, find_device_inlist, (void **)info) == false)
	{
		device_info = (DEVICE_INFO *)osal_mem_alloc(sizeof(DEVICE_INFO));
		list_node = (NODE *)osal_mem_alloc(sizeof(NODE));

		if (device_info != NULL && list_node != NULL)
		{
			device_info->device = info->device;
			device_info->shortaddr = info->shortaddr;
			device_info->tick = 1;

			list_node->data = (void *)device_info;
			list_node->next = NULL;

			status = list_foreach((void **)&head, node_add, (void **)&list_node);
		}
		else
		{
			(device_info != NULL) ? osal_mem_free(device_info), 1 : 0;
			(list_node != NULL)   ? osal_mem_free(list_node),   1 : 0;
 
			status = false;
		}
	}
	else
	{
		status = false;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       删除设备列表中的设备
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
bool Del_Device_ForList(DEVICE_INFO *info)
{
	bool status = false;
	LIST_NODE node = (LIST_NODE)info;

	if (list_foreach((void **)&head, get_device_addr, (void *)&node) == true)
	{
		list_foreach((void **)&head, node_del, (void **)&node);
		osal_mem_free(node->data);
		osal_mem_free(node);
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       清除列表中的一台僵尸设备
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool clr_zombie_device(void **ctx, void **list)
{
	bool status = false;
	LIST_NODE node = (LIST_NODE)*list;
	DEVICE_INFO *device_info = NULL;

	if (node != NULL)
	{
		device_info = node->data;

		if (device_info->tick == 0)
		{
			node_del((void **)&node,list);
			osal_mem_free(node->data);
			osal_mem_free(node);
			
			status = true;
		}
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       清除列表中的所有僵尸设备
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Del_ZombieDevice_ForList(void)
{
	uint8 device_num = 0;
	list_foreach((void **)&head, get_node_num, (void **)&device_num);

	for (uint8 i = 0; i < device_num; i++)
	{
		list_foreach((void **)&head, clr_zombie_device, NULL);
	}
}

/**
 *******************************************************************************
 * @brief       清除列表中所有设备的心跳
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Del_DeviceTickCount( void )
{
    list_foreach((void **)&head, clr_all_device_tick, NULL);
}

/**
 *******************************************************************************
 * @brief       增加设备的心跳计数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Add_DeviceTick_ForList( DEVICE_INFO *info )
{
    list_foreach((void **)&head, add_device_tick, (void **)info);
}

#endif
/** @}*/     /* devicelist 模块 */

/**********************************END OF FILE*********************************/
