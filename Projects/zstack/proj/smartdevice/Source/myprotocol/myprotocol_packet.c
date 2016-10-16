 /**
 ******************************************************************************
  * @file       myprotocol_packet.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myprotocol_packet 源文件
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
 * @defgroup myprotocol packet模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "myprotocol.h"
#include "myprotocol_packet.h"
#include <string.h>
#include "smart_device.h"
#include "NLMEDE.h"

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
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   ctx              上下文
 *              [in/out]   create_packet    创建数据包功能
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_tick_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    MYPROTOCOL_DEVICE_INFO device_info;
    uint8 *mac_addr = NULL;
    
    packet->commtype = MYPROTOCOL_COMM_TICK;
    packet->sn = 0;
    
    mac_addr = NLME_GetExtAddr();
    memcpy(&device_info.mac,mac_addr,sizeof(device_info.mac));
    device_info.device = SMART_DEVICE_TYPE;
    memcpy(&packet->user_data.data,&device_info,sizeof(MYPROTOCOL_DEVICE_INFO));
    
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = sizeof(MYPROTOCOL_DEVICE_INFO);
    
    return true;
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   ctx              上下文
 *              [in/out]   create_packet    创建数据包功能
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_acktick_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    MYPROTOCOL_DEVICE_INFO *device_info = (MYPROTOCOL_DEVICE_INFO *)ctx;
    
    packet->commtype = MYPROTOCOL_COMM_TICK;
    packet->sn = 0;
    packet->device.device = device_info->device;
    memcpy(&packet->device.mac,device_info->mac,sizeof(packet->device.mac));
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = sizeof(packet->user_data.cmd);
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建错误代码数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_errcode_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_COMM_ERROR;
    return true;
}

/**
 *******************************************************************************
 * @brief       创建结束通讯数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_commend_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_COMM_END;
    return true;
}

/**
 *******************************************************************************
 * @brief       创建写入设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_writeack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MTPROTOCOL_W2D_WRITE_ACK;
    return true;
}

/**
 *******************************************************************************
 * @brief       创建读取设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_readack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    return true;
}

/**
 *******************************************************************************
 * @brief       创建读取设备数量应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_devicenum_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{ 
    uint8 *num = (uint8 *)ctx;
    
    if( ctx == NULL )
    {
        return false;
    }
    
    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    packet->user_data.cmd = W2D_GET_DEVICE_NUM_CMD;
    packet->user_data.data[0] = *num;
    packet->user_data.len = sizeof(uint8);
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建读取设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_deviceinfo_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{ 
    MYPROTOCOL_DEVCICE_ACK *device_info = (MYPROTOCOL_DEVCICE_ACK *)ctx;
    
    if( ctx == NULL )
    {
        return false;
    }

    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    packet->user_data.cmd = W2D_GET_DEVICE_NUM_CMD;
    packet->user_data.data[0] = device_info->id;
    packet->user_data.data[1] = device_info->info->device;
    memcpy(&packet->user_data.data[2],&device_info->info->mac,sizeof(device_info->info->mac));
    
    packet->user_data.len = sizeof(uint8);
    
    return true;
}

/** @}*/     /* myprotocol packet模块 */

/**********************************END OF FILE*********************************/
