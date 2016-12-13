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

/* Includes ------------------------------------------------------------------*/
#include "devicelist.h"
#include "mynode.h"
#include "OSAL_Memory.h"
#include <string.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/     
/* Private variables ---------------------------------------------------------*/
static myList_t listHead = NULL;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       查找含有当前设备信息的节点
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceFindForInfo( void **ctx, void **list, void **expand )
{
  	listInfo_t *node_info = ((myList_t)(*list))->data;
	listInfo_t *device_info = (listInfo_t *)ctx;

	if (*list != NULL)
	{
		if (memcmp(&node_info->device, &device_info->device, sizeof(MYPROTOCOL_DEVICE_INFO_t)) == 0)
		{
            if( expand != NULL )
            {
                *expand = *list;
            }
            
			return true;
		}
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       查找含有当前设备信息的节点，并进行功能操作
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceFindOperaForInfo( void **ctx, void **list, void **expand )
{
  	listInfo_t *node_info = ((myList_t)(*list))->data;
	listInfo_t *device_info = (listInfo_t *)ctx;

	if (*list != NULL)
	{
		if (memcmp(&node_info->device, &device_info->device, sizeof(MYPROTOCOL_DEVICE_INFO_t)) == 0)
		{
            if( expand != NULL )
            {
                ((node_func)((uint16)*expand))(ctx,list,NULL);
            }
            
			return true;
		}
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       查找当前id的节点信息
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceFindForId( void **ctx, void **list, void **expand )
{
  	uint8 *id = (uint8 *)ctx;

	if (*list != NULL)
	{
		if (--(*id))
		{
            if( expand != NULL )
            {
                *expand = ((myList_t)(*list))->data;
            }
            
			return true;
		}
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       查找当前id的节点信息，并进行功能操作
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceFindOperaForId( void **ctx, void **list, void **expand )
{
  	uint8 *id = (uint8 *)ctx;

	if (*list != NULL)
	{
		if (--(*id))
		{
            if( expand != NULL )
            {
                ((node_func)((uint16)*expand))(ctx,list,NULL);
            }
            
			return true;
		}
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       添加设备列表中的设备
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceAdd(void **ctx, void **list, void **expand)
{
	listInfo_t *node_info = ((myList_t)(*list))->data;
	listInfo_t *device_info = (listInfo_t *)*ctx;

	if (*list != NULL)
	{
		if (memcmp(node_info, device_info, sizeof(MYPROTOCOL_DEVICE_INFO_t)) == 0)
		{
			return true;
		}
	}
    else
    {
        listInfo_t *newDevice = (listInfo_t *)osal_mem_alloc(sizeof(listInfo_t));
		myNODE_t *newNode = (myNODE_t *)osal_mem_alloc(sizeof(myNODE_t));

		if (newDevice != NULL && newNode != NULL)
		{
            memcpy(newDevice,device_info,sizeof(listInfo_t));
            newDevice->tick = 1;
            
			newNode->data = (void *)newDevice;
			newNode->next = NULL;

            *list = newNode;
            
			return true;
		}
		else
		{
			if(newDevice != NULL)
            {
                osal_mem_free(newDevice);
            }
            
			if(newNode != NULL)
            {
                osal_mem_free(newNode);
            }
		}
    }

	return false;
}

/**
 *******************************************************************************
 * @brief       删除设备列表中的设备
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceClr(void **ctx, void **list, void **expand)
{
	if (*list != NULL)
	{
        myNODE_t *delNode = (myNODE_t *)(*list);
        *list = delNode->next;
        
        osal_mem_free(delNode->data);
        osal_mem_free(delNode);
        
        return true;
	}
    
	return false;
}

/**
 *******************************************************************************
 * @brief       添加设备心跳计数一次
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceTickAdd( void **ctx, void **list, void **expand )
{
    listInfo_t *info = ((myList_t)(*list))->data;
    
	if (*list != NULL)
	{
        info->tick++;
            
        return true;
	}
	return false;
}

/**
 *******************************************************************************
 * @brief       清除设备心跳计数
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
*/
static bool listDeviceTickClr( void **ctx, void **list, void **expand )
{
	if (*list != NULL)
	{
        ((listInfo_t *)ctx)->tick = 0;
            
        return true;
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       清空所有设备的心跳数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
static bool listAllDeviceTickClear(void **ctx, void **list, void **expand)
{
	listInfo_t *node_info = ((myList_t)*list)->data;
	
    if (*list != NULL)
	{
		node_info->tick = 0;
	}
	else
	{
		return true;
	}

	return false;
}

/**
 *******************************************************************************
 * @brief       清空所有设备的心跳数量
 * @param       [in/out]  ctx     上下文
 *              [in/out]  list    链表
 *              [in/out]  expand  拓展用
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool listZombieDeviceClr( void **ctx, void **list, void **expand )
{
    listInfo_t *info = ((myList_t)(*list))->data;

    if( *list != NULL )
    {
        if( info->tick == 0)
        {
            myNODE_t *delNode = (myNODE_t *)(*list);
            *list = delNode->next;
            
            osal_mem_free(delNode->data);
            osal_mem_free(delNode);
            
            (*((uint8 *)ctx))++;
        }
    }
    
    return false;
}

/**
 *******************************************************************************
 * @brief       从列表中清除添加设备
 * @param       [in/out]  *info   设备信息
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool addDeviceToList( MYPROTOCOL_DEVICE_INFO_t *info )
{
    return nodeTraverse((void **)&listHead, listDeviceAdd, (void **)&info, NULL);
}

/**
 *******************************************************************************
 * @brief       从列表中清除指定设备
 * @param       [in/out]  *info   设备信息
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool clrDeviceInList( MYPROTOCOL_DEVICE_INFO_t *info )
{
    return nodeTraverse((void **)&listHead, listDeviceClr, (void **)&info, NULL);
}

/**
 *******************************************************************************
 * @brief       获取设备列表内的设备数量
 * @param       [in/out]  void
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
uint8 deviceNumGet( void )
{
    uint8 num = 0;
    
    nodeTraverse((void **)&listHead, nodeGetNum, (void **)&num, NULL);
    
    return num;
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
bool deviceIsExists( MYPROTOCOL_DEVICE_INFO_t *info )
{
    return nodeTraverse((void **)&listHead, listDeviceFindForInfo, (void **)info, NULL);
}

/**
 *******************************************************************************
 * @brief       获取设备编号所指定的设备的信息
 * @param       [in/out]  num     设备编号
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool deviceInfoGet( uint8 id, MYPROTOCOL_DEVICE_INFO_t *info )
{
    if( id == 0 )
    {
        return false;
    }
    
    if ( nodeTraverse((void **)&listHead, listDeviceFindForInfo, (void **)&id, (void **)&info) == true )
    {
        return true;
    }
    
    return false;
}

/**
 *******************************************************************************
 * @brief       增加设备心跳计数
 * @param       [in/out]  num     设备编号
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool addDeviceTick( MYPROTOCOL_DEVICE_INFO_t *info )
{
    if( deviceIsExists(info) == true )
    {
        uint16 func = ((uint16)&listDeviceTickAdd);
        return nodeTraverse((void **)&listHead, listDeviceFindOperaForInfo, (void **)&info, (void **)func);
    }
    else
    {
        return nodeTraverse((void **)&listHead, listDeviceAdd, (void **)&info, NULL);
    }
    
//    return false;
}

/**
 *******************************************************************************
 * @brief       清除所有设备的心跳计数
 * @param       [in/out]  void
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool allDeviceTickClr( void )
{
    return nodeTraverse((void **)&listHead, listAllDeviceTickClear, NULL, NULL);
}

/**
 *******************************************************************************
 * @brief       清除所有僵尸设备
 * @param       [in/out]  void
 * @return      [in/out]  status  状态
 * @note        None
 *******************************************************************************
 */
bool allZombieDeviceClr( void )
{
    uint8 num = 0x00;
    nodeTraverse((void **)&listHead, listZombieDeviceClr, (void **)&num, NULL);
    
    return (num == 0) ? (false) : (true);
}

/** @}*/     /* devicelist 模块 */

/**********************************END OF FILE*********************************/
