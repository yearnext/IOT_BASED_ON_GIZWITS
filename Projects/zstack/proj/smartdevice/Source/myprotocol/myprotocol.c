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
#include "myprotocol_packet.h"
#include "hal_uart.h"
#include <string.h>
#include "gizwits_protocol.h"
#include "devicelist.h"
#include "NLMEDE.h"
#include "smart_device.h"
#include "aps_groups.h"

/* Exported macro ------------------------------------------------------------*/
/**
* @name 日志打印宏定义
* @{
*/
#define USE_MYPROTOCOL_DEBUG 1

#if USE_MYPROTOCOL_DEBUG
    #define MYPROTOCOL_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
#else 
    #define MYPROTOCOL_LOG(n)
#endif

/**@} */
#define MYPROTOCOL_UART_PORT 0
#define MYPROTOCOL_UART_WRITE(data,len)  HalUARTWrite(GIZWITS_UART_PORT,data,len)

#define SmartDevice_ProfileID (uint16)SMART_DEVICE_TYPE

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MYPROTOCOL_PACKET_SIZE     (sizeof(MYPROTOCOL_FORMAT))
#if defined(USE_GIZWITS_MOD)
#define MYPROTOCOL_PACKET_REPORT() gizwitsReport((uint8 *)&tx_packet)
#else
#define MYPROTOCOL_PACKET_REPORT()
#endif

/** 簇的数量 */
#define SamrtDevice_ClustersNum (sizeof(SmartDevice_InClusterList)/sizeof(SmartDevice_InClusterList[0]))
#define SmartDevice_Comm_ClustersID  (0x0001)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static MYPROTOCOL_FORMAT tx_packet;
//static myprotocol_format rx_packet;
/** 端点描述符 */
static endPointDesc_t SmartDevice_epDesc;

/** 簇表 */
const cId_t SmartDevice_InClusterList[] =
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

/** 周期性发送数据 */
static afAddrType_t SmartDevice_Periodic_DstAddr;
/** 发送ID */
static uint8 SmartDevice_TransID;
/** 设备组别 */
static aps_Group_t SmartDevice_Group;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_init( uint8 endpoint, uint8 *task_id )
{
    memset(&tx_packet,0,sizeof(MYPROTOCOL_FORMAT));
    
    SmartDevice_TransID = 0;
    /** 注册AF层应用对象 */
    SmartDevice_epDesc.endPoint = endpoint;
    SmartDevice_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SmartDevice_SimpleDesc;
    SmartDevice_epDesc.task_id = task_id;
    SmartDevice_epDesc.latencyReq = noLatencyReqs;
    afRegister(&SmartDevice_epDesc);
    
    /** 周期性的发送数据给协调器 */
    SmartDevice_Periodic_DstAddr.addr.shortAddr= 0x0000;
    SmartDevice_Periodic_DstAddr.addrMode = afAddr16Bit;
    SmartDevice_Periodic_DstAddr.endPoint = endpoint;
    
    /** 创建一个组表 */
    SmartDevice_Group.ID = 0x0001;
    osal_memcpy( SmartDevice_Group.name, "Group 1", sizeof("Group 1") );
    aps_AddGroup(endpoint, &SmartDevice_Group);
}

/**
 *******************************************************************************
 * @brief       计算数据包的校验和
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  checksum  校验和
 * @note        None
 *******************************************************************************
 */
static uint8 myprotocol_compute_checksum( uint8 *packet )
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
static bool myprotocol_packet_check( uint8 *data )
{
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)data;
    
    if( packet->check_sum != myprotocol_compute_checksum(data) )
    {
        return false;
    }
    
    if( packet->commtype == MYPROTOCOL_W2D_READ_ACK \
        && packet->commtype == MTPROTOCOL_W2D_WRITE_ACK \
        && packet->commtype == MYPROTOCOL_D2W_READ_WAIT \
        && packet->commtype == MYPROTOCOL_D2W_REPORT_WAIT )
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
static bool MYPROTOCOL_DIR_D2D_SEND_MSG( packet_func create_packet, void *ctx )
{
    MYPROTOCOL_FORMAT packet;
    
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT));
    
    create_packet(ctx,&packet);
    
    packet.check_sum = myprotocol_compute_checksum((uint8 *)&packet);
    
    AF_DataRequest(&SmartDevice_Periodic_DstAddr,
                   &SmartDevice_epDesc,
                   SmartDevice_Comm_ClustersID,
                   sizeof(MYPROTOCOL_FORMAT),
                   (uint8 *)&packet,
                   &SmartDevice_TransID,
                   AF_DISCV_ROUTE,
                   AF_DEFAULT_RADIUS);
    
    return true;
}

/**
 *******************************************************************************
 * @brief       myprotocol 创建并发送一个数据包
 * @param       [in/out]  packet        接收到的数据包
 *              [in/out]  packet_type   创建数据包的类型
 *              [in/out]  data          需要用到的数据   
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool MYPROTOCOL_DIR_D2W_SEND_MSG( MYPROTOCOL_FORMAT *packet, packet_func packet_create, void *ctx )
{
    bool status = false;
    
    if( packet_create == NULL )
    {
        return false;
    }
    
    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
    
    status = packet_create(ctx, &tx_packet);
    
    tx_packet.sn = packet->sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
    
    MYPROTOCOL_PACKET_REPORT();
    
    return status;
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
bool MYPROTOCOL_SEND_MSG( MYPROTOCOL_DATA_DIR data_dir, MYPROTOCOL_FORMAT *packet, packet_func packet_create, void *ctx )
{
    bool status = false;
    
    if( packet_create == NULL )
    {
        return false;
    }
    
    if( data_dir == MYPROTOCOL_DIR_D2W )
    {
        status = MYPROTOCOL_DIR_D2W_SEND_MSG(packet, packet_create, ctx);
    }
    else if( data_dir == MYPROTOCOL_DIR_D2D )
    {
        status = MYPROTOCOL_DIR_D2D_SEND_MSG(packet_create, ctx);
    }
    else
    {
        return false;
    }
    
    return status;
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

    if( packet->check_sum != myprotocol_compute_checksum(pkt->cmd.Data) )
    {
        return;
    }
    
    switch( packet->commtype )
    {
        case MYPROTOCOL_COMM_TICK:
        {
#if defined ( USE_GIZWITS_MOD )
            // 如果为父设备
            DEVICE_INFO *device_info = (DEVICE_INFO *)packet->user_data.data;

            // 添加设备
            if( Add_Device_Forlist(device_info) == false )
            {
                // 增加设备心跳计数
                Add_DeviceTick_ForList(device_info);
            }
            
            // 心跳应答
            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2D,pkt->srcAddr,create_acktick_packet,&device_info->device);
            MYPROTOCOL_LOG("Coord get one end device tick packet!\n");
#endif
            break;
        }
        case MYPROTOCOL_W2D_READ_WAIT:
            break;
        case MYPROTOCOL_W2D_WRITE_WAIT:
            break;
        case MYPROTOCOL_D2W_READ_ACK:
            break;
        case MYPROTOCOL_D2W_REPORT_ACK:
            break;
        default:
            break;
    }
}

#if defined ( USE_GIZWITS_MOD )       
 
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
        MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_errcode_packet,NULL);
        return;
    }
    else 
    {
        switch( packet->commtype )
        {
            // WIFI模组读取设备数据等待应答
            case MYPROTOCOL_W2D_READ_WAIT:
                if( packet->device.device == MYPROTOCOL_DEVICE_COORD )
                {
                    switch( packet->user_data.cmd )
                    {
                        case W2D_GET_DEVICE_NUM_CMD:
                        {
                            uint8 num = Get_DeviceNum_ForList();
                            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_devicenum_packet,(void *)&num);
                        }
                            break;
                        case W2D_GET_DEVICE_INFO_CMD:
                        {
                            MYPROTOCOL_DEVCICE_ACK device;
                            device.id = packet->user_data.data[0];
                            device.info = (MYPROTOCOL_DEVICE_INFO *)Get_DeviceInfo_InList(device.id);
                            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_deviceinfo_packet,(void *)&device);
                        }
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_readack_packet,NULL);
                }
                    break;
            // WIFI模组写入设备数据等待应答
            case MYPROTOCOL_W2D_WRITE_WAIT:
                MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_writeack_packet,NULL);
                    break;
            // 设备读取WIFI模组信息应答
            case MYPROTOCOL_D2W_READ_ACK:
                MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_commend_packet,NULL);
                    break;
            // 设备上报数据应答
            case MYPROTOCOL_D2W_REPORT_ACK:
                MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,packet,create_commend_packet,NULL);
                    break;
            // 通讯错误
            case MYPROTOCOL_COMM_ERROR:
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_COMM_END:
                break;
            default:
                break;
        }
    }
}

#endif

/** @}*/     /* myprotocol模块 */

/**********************************END OF FILE*********************************/
