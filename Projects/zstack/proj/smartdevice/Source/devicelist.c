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
                [in/out]  func    链表访问函数
                [in/out]  ctx     上下文
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool list_foreach(void **list_head, visit_func visit, void *ctx)
{
	LIST_HEAD list = (LIST_HEAD)list_head;
	LIST_NODE node = *list_head;
	bool status = visit(ctx,(void **)list);

	while ( status != true && node != NULL )
	{
		list = (LIST_HEAD)node;
		node = node->next;

		status = visit(ctx, (void **)list);
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       增加节点
 * @param       [in/out]  ctx     上下文
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_add(void *ctx, void **list)
{
	bool status = false;

	if ( *list == NULL )
	{
		*list = ctx;
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       删除节点
 * @param       [in/out]  ctx     上下文
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_del(void *ctx, void **list)
{
	bool status = false;
	LIST_NODE node = (LIST_NODE)ctx;

	if ( *list == ctx )
	{
		*list = node->next;
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       获取节点数量
 * @param       [in/out]  ctx     上下文
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool get_node_num(void *crx, void **list)
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
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool node_find(void *ctx, void **list)
{
	bool status = false;

	if (*list == ctx)
	{
		status = true;
	}

	return status;
}

/**
 *******************************************************************************
 * @brief       查询设备是否存在于设备列表中
 * @param       [in/out]  ctx     上下文
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool find_device_inlist( void *ctx, void **list )
{
    bool status = false;
    NODE *node = *list; 
    DEVICE_INFO *list_info = node->data;
    DEVICE_INFO *device_info = ctx;
    
    if( *list != NULL )
    {
        if( list_info->device == device_info->device \
            && list_info->shortaddr == device_info->shortaddr )
        {
            status = true;
        }
    }
    
    return status;
}

/**
 *******************************************************************************
 * @brief       添加设备到设备列表中
 * @param       [in/out]  ctx     上下文
                [in/out]  list    链表
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool Add_Device_Forlist( DEVICE_INFO info )
{
    bool status = false;
    DEVICE_INFO *device_info = NULL;
    NODE *list_node = NULL;
    
    if( list_foreach((void **)&head,find_device_inlist,(void *)&info) == false )
    {
        device_info = (DEVICE_INFO *)osal_mem_alloc(sizeof(DEVICE_INFO));
        list_node   = (NODE *)osal_mem_alloc(sizeof(NODE));
        
        if( device_info != NULL && list_node != NULL )
        {
            device_info->device = info.device;
            device_info->shortaddr = info.shortaddr;
            
            list_node->data = (void *)device_info;
            list_node->next = NULL;
            
            list_foreach((void **)&head,node_add,(void *)list_node);
            
            status = true;
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
        status = true;
    }
    
    return status;
}

#endif
/** @}*/     /* devicelist 模块 */

/**********************************END OF FILE*********************************/
