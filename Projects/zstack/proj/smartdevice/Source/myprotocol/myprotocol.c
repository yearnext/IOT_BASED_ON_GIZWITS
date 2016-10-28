 /**
 ******************************************************************************
  * @file       myprotocol.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myprotocol 源文件
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
 * @defgroup myprotocol模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "myprotocol.h"
#include "hal_uart.h"
#include <string.h>
#include "devicelist.h"
#include "NLMEDE.h"
#include "aps_groups.h"
#include "onBoard.h"

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
#include "gizwits_protocol.h"
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
#include "bsp_light.h"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @name 日志打印宏
 * @{
 */
#define USE_MYPROTOCOL_DEBUG 1

#if USE_MYPROTOCOL_DEBUG
    #define MYPROTOCOL_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
#else 
    #define MYPROTOCOL_LOG(n)
#endif
/**@} */

/**
 * @name 发送数据至WIFI模块宏
 * @{
 */
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
#define MYPROTOCOL_UART_WRITE(data,len)  HalUARTWrite(0,data,len)
#define MYPROTOCOL_PACKET_REPORT(packet) gizwitsReport(packet)
#else
#define MYPROTOCOL_PACKET_REPORT(packet)
#define MYPROTOCOL_UART_WRITE(data,len)
#endif
/**@} */

/**
 * @name Zigbee通讯配置
 * @{
 */

/** 簇的数量 */
#define SamrtDevice_ClustersNum        (1)
#define SmartDevice_Comm_ClustersID    (0x0001)
#define SmartDevice_EndPoint           (20)
#define SmartDevice_DeviceID           (0x0001)
#define SmartDevice_Version            (0x00)
#define SmartDevice_Flags              (0)
#define SmartDevice_ProfileID          (0x0F08)

/**@} */

/** 数据包大小 宏 */
#define MYPROTOCOL_PACKET_SIZE          (sizeof(MYPROTOCOL_FORMAT))

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// 数据缓冲区
//static MYPROTOCOL_FORMAT tx_packet;
//static myprotocol_format rx_packet;

/** 端点描述符 */
static endPointDesc_t SmartDevice_epDesc;

/** 簇表 */
const cId_t SmartDevice_InClusterList[SamrtDevice_ClustersNum] =
{
    SmartDevice_Comm_ClustersID,
};

/** 简单描述符 */
static const SimpleDescriptionFormat_t SmartDevice_SimpleDesc =
{
    SmartDevice_EndPoint, 
    SmartDevice_ProfileID,
    SmartDevice_DeviceID,
    SmartDevice_Version,
    SmartDevice_Flags,                  
    SamrtDevice_ClustersNum,
    (cId_t *)&SmartDevice_InClusterList,
    SamrtDevice_ClustersNum,
    (cId_t *)&SmartDevice_InClusterList,
};

/** 发送ID */
static uint8 SmartDevice_TransID;
/** 设备组别 */
static aps_Group_t SmartDevice_Group;

/** 目标地址 */
//static afAddrType_t MYPROTOCOL_DST_ADDR;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       初始化函数
 * @param       [in/out]  *task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_init( uint8 *task_id )
{
    //memset(&tx_packet,0,sizeof(MYPROTOCOL_FORMAT));
    
    SmartDevice_TransID = 0;
    /** 注册AF层应用对象 */
    SmartDevice_epDesc.endPoint = SmartDevice_EndPoint;
    SmartDevice_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SmartDevice_SimpleDesc;
    SmartDevice_epDesc.task_id = task_id;
    SmartDevice_epDesc.latencyReq = noLatencyReqs;
    afRegister(&SmartDevice_epDesc);
    
//#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
//    memset(&MYPROTOCOL_DST_ADDR.addr.extaddr,0,sizeof(MYPROTOCOL_DST_ADDR.addr));
//    MYPROTOCOL_DST_ADDR.addrMode = afAddr64Bit;
//    MYPROTOCOL_DST_ADDR.endPoint = SmartDevice_EndPoint;
//#else
//    MYPROTOCOL_DST_ADDR.addr.shortAddr= 0x0000;
//    MYPROTOCOL_DST_ADDR.addrMode = afAddr16Bit;
//    MYPROTOCOL_DST_ADDR.endPoint = SmartDevice_EndPoint;
//#endif
    
    /** 创建一个组表 */
    SmartDevice_Group.ID = 0x0001;
    osal_memcpy( SmartDevice_Group.name, "Group 1", 7 );
    aps_AddGroup(SmartDevice_EndPoint, &SmartDevice_Group);
}

/**
 *******************************************************************************
 * @brief       计算数据包的校验和
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  checksum  校验和
 * @note        None
 *******************************************************************************
 */
static uint8 myprotocol_cal_checksum( uint8 *packet )
{
    uint8 i;
    uint8 checksum;

    for( i=0, checksum=0; i<MYPROTOCOL_PACKET_SIZE-1; i++ )
    {
        checksum += packet[i];
    }
    
    return checksum;
}

/**
 *******************************************************************************
 * @brief       检测数据包是否合法
 * @param       [in/out]  data    事件序号
 * @return      [in/out]  bool    数据包合法状态
 * @note        None
 *******************************************************************************
 */
bool myprotocol_packet_check( uint8 *data )
{
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)data;
    
    if( packet->check_sum != myprotocol_cal_checksum(data) )
    {
        return false;
    }
    
    return true;
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   create_packet    创建数据包功能
 *              [in/out]   ctx              上下文
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool MYPROTOCO_D2W_MSG_SEND( packet_func create_packet, void *ctx )
{
    MYPROTOCOL_FORMAT packet;
    afAddrType_t dst_addr;
    
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT));
    memset(&dst_addr,0,sizeof(dst_addr));
    
    create_packet(ctx,&packet);
    
    packet.device.device = SMART_DEVICE_TYPE;
    memcpy(&packet.device.mac,&aExtendedAddress,sizeof(packet.device.mac));
    
    packet.check_sum = myprotocol_cal_checksum((uint8 *)&packet);
    
    MYPROTOCOL_PACKET_REPORT((uint8 *)&packet);
    
    return true;
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   create_packet    创建数据包功能
 *              [in/out]   ctx              上下文
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool MYPROTOCO_S2H_MSG_SEND( packet_func create_packet, void *ctx )
{
    MYPROTOCOL_FORMAT packet;
    afAddrType_t dst_addr;
    
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT));
    memset(&dst_addr,0,sizeof(dst_addr));
    
    create_packet(ctx,&packet);
    
    packet.device.device = SMART_DEVICE_TYPE;
    memcpy(&packet.device.mac,&aExtendedAddress,sizeof(packet.device.mac));
    
    packet.check_sum = myprotocol_cal_checksum((uint8 *)&packet);

    dst_addr.addr.shortAddr = 0x0000;
    dst_addr.addrMode = afAddr16Bit;
    dst_addr.endPoint = SmartDevice_EndPoint;
    
    return AF_DataRequest(&dst_addr,
                           &SmartDevice_epDesc,
                           SmartDevice_Comm_ClustersID,
                           sizeof(MYPROTOCOL_FORMAT),
                           (uint8 *)&packet,
                           &SmartDevice_TransID,
                           AF_DISCV_ROUTE,
                           AF_DEFAULT_RADIUS);
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   create_packet    创建数据包功能
 *              [in/out]   ctx              上下文
 * @return      [in/out]  void
 * @note        本函数只能用于存在完整数据包的情况
 *******************************************************************************
 */
bool MYPROTOCO_H2S_MSG_SEND( MYPROTOCOL_DEVICE_INFO info, packet_func create_packet, void *ctx )
{
    MYPROTOCOL_FORMAT packet;
    afAddrType_t dst_addr;
    
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT));
    memset(&dst_addr,0,sizeof(dst_addr));
    
    create_packet(ctx,&packet);
    
    packet.device.device = info.device;
    memcpy(&packet.device.mac,&info.mac,sizeof(packet.device.mac));
    packet.check_sum = myprotocol_cal_checksum((uint8 *)&packet);

    memcpy(&dst_addr.addr.extAddr,&info.mac,sizeof(dst_addr.addr.extAddr));
    dst_addr.addrMode = afAddr64Bit;
    dst_addr.endPoint = SmartDevice_EndPoint;
    
    return AF_DataRequest(&dst_addr,
                           &SmartDevice_epDesc,
                           SmartDevice_Comm_ClustersID,
                           sizeof(MYPROTOCOL_FORMAT),
                           (uint8 *)&packet,
                           &SmartDevice_TransID,
                           AF_DISCV_ROUTE,
                           AF_DEFAULT_RADIUS);
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   create_packet    创建数据包功能
 *              [in/out]   ctx              上下文
 * @return      [in/out]  void
 * @note        本函数只能用于存在完整数据包的情况
 *******************************************************************************
 */
bool MYPROTOCO_H2S_FORWARD_MSG( MYPROTOCOL_FORMAT *packet )
{
    afAddrType_t dst_addr;
    
    memset(&dst_addr,0,sizeof(dst_addr));
    
    memcpy(&dst_addr.addr.extAddr,&packet->device.mac,sizeof(dst_addr.addr.shortAddr));
    dst_addr.addrMode = afAddr64Bit;
    dst_addr.endPoint = SmartDevice_EndPoint;
    
    return AF_DataRequest(&dst_addr,
                           &SmartDevice_epDesc,
                           SmartDevice_Comm_ClustersID,
                           sizeof(MYPROTOCOL_FORMAT),
                           (uint8 *)&packet,
                           &SmartDevice_TransID,
                           AF_DISCV_ROUTE,
                           AF_DEFAULT_RADIUS);
}

/**
 *******************************************************************************
 * @brief       转发数据包函数
 * @param       [in/out]   *user_data       用户数据
 * @return      [in/out]   bool             转发状态
 * @note        None
 *******************************************************************************
 */
bool MYPROTOCOL_FORWARD_PACKET( MYPROTOCOL_DATA_DIR dir, MYPROTOCOL_FORMAT *packet )
{
    switch( dir )
    {
        case MYPROTOCOL_FORWORD_W2D:
            MYPROTOCO_H2S_FORWARD_MSG(packet);
            break;
        case MYPROTOCOL_FORWORD_D2W:
            MYPROTOCOL_PACKET_REPORT((uint8 *)packet);  
            break;
        default:
            return false;
            break;
    }
    
    return true;
}

/**
 *******************************************************************************
 * @brief       MYPROTOCOL 数据发送函数
 * @param       [in/out]   data_dir         数据方向
 *              [in/out]   packet           相关数据包
 *              [in/out]   packet_create    应答数据包创建函数
 *              [in/out]   ctx              参数
 * @return      [out]      bool
 * @note        None
 *******************************************************************************
 */
bool MYPROTOCOL_SEND_MSG( MYPROTOCOL_DATA_DIR dir, MYPROTOCOL_FORMAT *packet, packet_func create_packet, void *param )
{
    switch( dir )
    {
        case MYPROTOCOL_DIR_D2W:
            MYPROTOCO_D2W_MSG_SEND(create_packet,param);
            break;
        case MYPROTOCOL_DIR_S2H:
            MYPROTOCO_S2H_MSG_SEND(create_packet,param);
            break;
        case MYPROTOCOL_DIR_H2S:
            MYPROTOCO_H2S_MSG_SEND(packet->device,create_packet,param);
            break;
        case MYPROTOCOL_FORWORD_D2W:
            MYPROTOCOL_PACKET_REPORT((uint8 *)packet);
            break;
        case MYPROTOCOL_FORWORD_W2D:
            MYPROTOCO_H2S_FORWARD_MSG(packet);
            break;
        default:
            return false;
            break;
    }
    
    return true;
}

/**
 *******************************************************************************
 * @brief       SmartDevice信息处理
 * @param       [in]   pkt    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void SmartDevice_Message_Headler( afIncomingMSGPacket_t *pkt )
{
    if( pkt->clusterId != SmartDevice_Comm_ClustersID )
    {
        return;
    }
    
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)pkt->cmd.Data;

    if( packet->check_sum != myprotocol_cal_checksum(pkt->cmd.Data) )
    {
        return;
    }
    else
    {
        switch( packet->commtype )
        {
            case MYPROTOCOL_W2D_WAIT:
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
        light_cmd_resolve(&packet->user_data);
#endif
                break;
            case MYPROTOCOL_W2D_ACK:
                break;
            case MYPROTOCOL_S2H_WAIT:
            {
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
            // 如果为父设备
            DEVICE_INFO *device_info = (DEVICE_INFO *)packet->user_data.data;

            // 添加设备
            if( Add_Device_Forlist(device_info) == false )
            {
                // 增加设备心跳计数
                Add_DeviceTick_ForList(device_info);
            }
            
            // 心跳应答
            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_H2S,(void *)&packet->device,create_acktick_packet,&device_info->device);
            MYPROTOCOL_LOG("Coord get one device tick!\n");
#endif
                break;
            }
            case MYPROTOCOL_S2H_ACK:
                break;
            case MYPROTOCOL_COMM_ERROR:
                break;
            default:
                break;
        }
    }
}

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)     
/**
 *******************************************************************************
 * @brief       数据包处理函数
 * @param       [in/out]  data    数据包
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Gizwits_Message_Headler( uint8 *report_data, uint8 *packet_data )
{
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)packet_data;
    
    if( myprotocol_packet_check(packet_data) == false )
    {
        MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_errcode_packet,NULL);
        return;
    }
    else 
    {
        switch( packet->commtype )
        {
            case MYPROTOCOL_W2D_WAIT:
            {
                // 检测数据接收设备，若不是本机则进行数据转发操作
                if( packet->device.device == MYPROTOCOL_DEVICE_COORD )
                {
                    switch( packet->user_data.cmd )
                    {
                        case W2D_GET_DEVICE_NUM_CMD:
                        {
                            uint8 num = Get_DeviceNum_ForList();
                            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_devicenum_packet,(void *)&num);
                        }
                            break;
                        case W2D_GET_DEVICE_INFO_CMD:
                        {
                            MYPROTOCOL_DEVCICE_ACK device;
                            device.id = packet->user_data.data[0];
                            device.info = (MYPROTOCOL_DEVICE_INFO *)Get_DeviceInfo_InList(device.id);
                            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_deviceinfo_packet,&device);
                        }
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    // 转发数据并应答APP
                    MYPROTOCOL_FORWARD_PACKET(MYPROTOCOL_FORWORD_W2D,packet);
                    MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_w2d_ack_packet,&packet->user_data);
                }
                    break;
            }
            case MYPROTOCOL_D2W_ACK:
                break;
            case MYPROTOCOL_COMM_ERROR:
                break;
            default:
                break;
        }
    }
}

#endif

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
    packet->commtype = MYPROTOCOL_S2H_WAIT;
    packet->sn = 0;
        
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = 0;
    
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
    packet->commtype = MYPROTOCOL_S2H_ACK;
    packet->sn = 0;
        
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = 0;
    
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
bool create_w2d_ack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    if( ctx != NULL )
    {
        memcpy(&packet->user_data,ctx,sizeof(MYPROTOCOL_USER_DATA));
    }

    packet->commtype = MYPROTOCOL_W2D_ACK;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建写入设备信息应等待答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_w2d_wait_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    if( ctx != NULL )
    {
        memcpy(&packet->user_data,ctx,sizeof(MYPROTOCOL_USER_DATA));
    }

    packet->commtype = MYPROTOCOL_W2D_WAIT;
    
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
bool create_d2w_ack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    if( ctx != NULL )
    {
        memcpy(&packet->user_data,ctx,sizeof(MYPROTOCOL_USER_DATA));
    }

    packet->commtype = MYPROTOCOL_D2W_ACK;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建写入设备信息应等待答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_d2w_wait_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    if( ctx != NULL )
    {
        memcpy(&packet->user_data,ctx,sizeof(MYPROTOCOL_USER_DATA));
    }

    packet->commtype = MYPROTOCOL_D2W_WAIT;
    
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
    
    packet->commtype = MYPROTOCOL_W2D_ACK;
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

    packet->commtype = MYPROTOCOL_W2D_ACK;
    packet->user_data.cmd = W2D_GET_DEVICE_NUM_CMD;
    packet->user_data.data[0] = device_info->id;
    packet->user_data.data[1] = device_info->info->device;
    memcpy(&packet->user_data.data[2],&device_info->info->mac,sizeof(device_info->info->mac));
    
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
bool create_devicelist_update_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{ 
    packet->commtype = MYPROTOCOL_W2D_WAIT;
    packet->user_data.cmd = W2D_DEVICE_LIST_UPDATE_CMD;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建数据上报数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_report_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    MYPROTOCOL_USER_DATA *data = (MYPROTOCOL_USER_DATA *)ctx;
    
    packet->commtype = MYPROTOCOL_D2W_WAIT;
    memcpy(&packet->user_data, data, sizeof(MYPROTOCOL_USER_DATA));
    
    return true;
}

/** @}*/     /* myprotocol模块 */

/**********************************END OF FILE*********************************/
